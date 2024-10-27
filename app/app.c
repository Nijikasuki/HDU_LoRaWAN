#include <string.h>
#include "app.h"
#include "usart.h"
#include "gpio.h"
#include "lorawan_node_driver.h"
#include "hdc1000.h"
#include "sensors_test.h"
#include "math.h"


#define TEMP_DATA_SIZE ((sizeof(TempData) + 7) / 8 * 8)  // ��֤��8�ֽڶ���
#define LED_BLINK_DELAY 500
#define FLASH_USER_START_ADDR ((uint32_t)0x080e1000)  // �洢����ʼ��ַ
#define ADDR_FLASH_PAGE_511   ((uint32_t)0x080ff800) /* Base @ of Page 511, 2 Kbytes */
#define FLASH_USER_END_ADDR     ADDR_FLASH_PAGE_511 + FLASH_PAGE_SIZE - 1   //�洢��ֹҳ

#define total_hour 0.05  //һ��������Ϊ24��һСʱ��Ϊ1

typedef struct {
    char time_str[20];   // �洢ʱ���
    float temperature;   // �洢�¶�
} TempData;

extern DEVICE_MODE_T device_mode;
extern DEVICE_MODE_T *Device_Mode_str;
down_list_t *pphead = NULL;

TempData temp_data;  // ���ڴ洢ÿ���ӵ�����
float save_temp[10000];
float max_value = 0;
float min_value = 100;
float average = 0;
int i = 0;
int day = 0;
int minute_counter = 0;  // ��¼�Ѿ����˶��ٷ���
double temperature, last_temperature = 0, total_variance = 0, temp_diff = 0, max_diff = 0;
float mean = 0.0;
float M2 = 0.0;  // ���ڼ��㷽����ۼ�ֵ
int count = 0;   // ���ݼ�����

uint8_t dev_eui_7th_byte = 0xA0;
uint8_t dev_eui_8th_byte = 0xE2;


void update_statistics(float temperature) {
    count++;
    float delta = temperature - mean;      // �������뵱ǰƽ��ֵ�Ĳ�
    mean += delta / count;                 // ����ƽ��ֵ
    M2 += delta * (temperature - mean);    // �������ڷ�����ۼ�ֵ
}

float calculate_variance() {
    if (count < 2) {
        return 0.0;  // ����Ҫ���������ݵ���ܼ��㷽��
    }
    return M2 / (count - 1);  // ��������
}

static uint32_t GetBank(uint32_t Addr)
{
  uint32_t bank = 0;
  
  if (READ_BIT(SYSCFG->MEMRMP, SYSCFG_MEMRMP_FB_MODE) == 0)
  {
  	/* No Bank swap */
    if (Addr < (FLASH_BASE + FLASH_BANK_SIZE))
    {
      bank = FLASH_BANK_1;
    }
    else
    {
      bank = FLASH_BANK_2;
    }
  }
  else
  {
  	/* Bank swap */
    if (Addr < (FLASH_BASE + FLASH_BANK_SIZE))
    {
      bank = FLASH_BANK_2;
    }
    else
    {
      bank = FLASH_BANK_1;
    }
  }
  
  return bank;
}

static uint32_t GetPage(uint32_t Addr)
{
  uint32_t page = 0;
  
  if (Addr < (FLASH_BASE + FLASH_BANK_SIZE))
  {
    /* Bank 1 */
    page = (Addr - FLASH_BASE) / FLASH_PAGE_SIZE;
  }
  else
  {
    /* Bank 2 */
    page = (Addr - (FLASH_BASE + FLASH_BANK_SIZE)) / FLASH_PAGE_SIZE;
  }
  
  return page;
}

HAL_StatusTypeDef Flash_Erase(uint32_t startAddress, uint32_t endAddress) {
    uint32_t FirstPage = 0, NbOfPages = 0, BankNumber = 0;
    uint32_t PAGEError = 0;
    FLASH_EraseInitTypeDef EraseInitStruct;

    // ����Flash
    HAL_FLASH_Unlock();

    // ����κο���Ӱ������ı�־
    __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_OPTVERR);

    // ��ȡ��ʼҳ��ҳ��
    FirstPage = GetPage(startAddress);
    NbOfPages = GetPage(endAddress) - FirstPage + 1;
    BankNumber = GetBank(startAddress);

    // ����EraseInitStruct�ṹ
    EraseInitStruct.TypeErase = FLASH_TYPEERASE_PAGES;
    EraseInitStruct.Banks = BankNumber;
    EraseInitStruct.Page = FirstPage;
    EraseInitStruct.NbPages = NbOfPages;
		
    // ִ�в�������
    HAL_StatusTypeDef status = HAL_FLASHEx_Erase(&EraseInitStruct, &PAGEError);

    // ����Flash�Ա����ڴ�
    HAL_FLASH_Lock();

    return status;  // ����״̬�����ڼ���Ƿ�ɹ�
}


void store_float_in_buffer(float number, uint8_t *buffer, int start_index) {
    // ʹ��ָ����� float ���ֽڱ�ʾ
    uint8_t *byte_ptr = (uint8_t*)&number;

    // �������Big-Endian����˳��洢
    buffer[start_index] = byte_ptr[3];
    buffer[start_index + 1] = byte_ptr[2];
    buffer[start_index + 2] = byte_ptr[1];
    buffer[start_index + 3] = byte_ptr[0];
}


void send_data_via_lorawan(float average,float max_value,float min_value,float diff,float variance,float temp_rate) {
    uint8_t buffer[31]; // ͨ��Э��Ҫ���31�ֽ�����

    // �ֽ�1��֡ͷ
    buffer[0] = 0xAA;

    // �ֽ�2-3��DevEui��7�͵�8�ֽ�
    buffer[1] = dev_eui_7th_byte;
    buffer[2] = dev_eui_8th_byte;

    // �ֽ�4���������� (���贫��ƽ���¶�)
    buffer[3] = 0x01;

    // �ֽ�5-6��ʱ���
    buffer[4] = 0x4A;
    buffer[5] = 0x1B;

    // �ֽ�7-10��ƽ���¶ȣ�float�ͣ�
		store_float_in_buffer(average, buffer, 6);

    // �ֽ�11-14������¶ȣ�float�ͣ�
		store_float_in_buffer(max_value, buffer, 10);

    // �ֽ�15-18������¶ȣ�float�ͣ�
		store_float_in_buffer(min_value, buffer, 14);

    // �ֽ�19-22���²float�ͣ�
		store_float_in_buffer(diff, buffer, 18);

    // �ֽ�23-26�����float�ͣ�
		store_float_in_buffer(variance, buffer, 22);

    // �ֽ�27-30���¶ȱ仯�ʣ�float�ͣ�
		store_float_in_buffer(temp_rate, buffer, 26);

    // �ֽ�31��֡β
    buffer[30] = 0x0F;

    // ͨ�� LPUART1 ��������
    lpusart1_send_data(buffer, 31);
		debug_printf("���ݴ������ϴ��ɹ�\n");
}


uint32_t store_data_to_flash(TempData *data,uint32_t address ) {
    HAL_FLASH_Unlock();  // ����Flash

    // �����ݽṹ���ֽ�д��Flash����������Ϊ8�ֽڣ����ʹ�� DOUBLEWORD д�룩
    for (int j = 0; j < TEMP_DATA_SIZE; j += 8) {
        uint64_t word = *(uint64_t*)((uint8_t*)data + j);  // ÿ��ȡ8�ֽ�
        HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, address, word);
        address += 8;  // ÿ�ε���8�ֽ�
    }

    HAL_FLASH_Lock();  // ����Flash
		return address;
}

void read_and_send_data_from_flash(void) {
    uint32_t read_address = FLASH_USER_START_ADDR;  // �� Flash ����ʼ��ַ��ʼ��ȡ

    TempData read_temp_data;
    float average, max_value, min_value, diff, variance, temp_rate;

    // ��һ��ȡÿ���ṹ��
    memcpy(&read_temp_data, (uint8_t *)read_address, sizeof(TempData));
    average = read_temp_data.temperature;
    read_address += TEMP_DATA_SIZE;

    memcpy(&read_temp_data, (uint8_t *)read_address, sizeof(TempData));
    max_value = read_temp_data.temperature;
    read_address += TEMP_DATA_SIZE;

    memcpy(&read_temp_data, (uint8_t *)read_address, sizeof(TempData));
    min_value = read_temp_data.temperature;
    read_address += TEMP_DATA_SIZE;

    memcpy(&read_temp_data, (uint8_t *)read_address, sizeof(TempData));
    diff = read_temp_data.temperature;
    read_address += TEMP_DATA_SIZE;

    memcpy(&read_temp_data, (uint8_t *)read_address, sizeof(TempData));
    variance = read_temp_data.temperature;
    read_address += TEMP_DATA_SIZE;

    memcpy(&read_temp_data, (uint8_t *)read_address, sizeof(TempData));
    temp_rate = read_temp_data.temperature;

    // ��ӡ��ȡ���Ľ��
    debug_printf("�� flash �ж�ȡ�õ�����ƽ���¶�Ϊ��%.3f�� ������¶�Ϊ��%.3f�� ������¶�Ϊ��%.3f�� ����²�Ϊ��%.3f�� ����Ϊ��%.3f �¶ȱ仯��Ϊ��%.3f\n", 
                 average, max_value, min_value, diff, variance, temp_rate);

    // ������ͨ�� LoRaWAN ����
    send_data_via_lorawan(average, max_value, min_value, diff, variance, temp_rate);
		
		
		//������������
		if (Flash_Erase(FLASH_USER_START_ADDR,FLASH_USER_END_ADDR)!= HAL_OK){
	// �������ʧ�ܣ����Դ�������������������Ϣ��
			debug_printf("����������ʧ��\n");
			} 
		else {
// �����ɹ�������ɹ���Ϣ
			debug_printf("�ɹ�ɾ��flash�д������ı���\n");
		}
}


//-----------------Users application--------------------------
void LoRaWAN_Func_Process(void)
{
    static DEVICE_MODE_T dev_stat = NO_MODE;

    uint16_t temper = 0;

    switch((uint8_t)device_mode)
    {
    /* ָ��ģʽ */
    case CMD_CONFIG_MODE:
    {
        /* �������command Configuration function, �����if���,ִֻ��һ�� */
        if(dev_stat != CMD_CONFIG_MODE)
        {
            dev_stat = CMD_CONFIG_MODE;
            debug_printf("\r\n[Command Mode]\r\n");

            nodeGpioConfig(wake, wakeup);
            nodeGpioConfig(mode, command);
        }
        /* �ȴ�usart2�����ж� */
        if(UART_TO_PC_RECEIVE_FLAG)
        {
            UART_TO_PC_RECEIVE_FLAG = 0;
            lpusart1_send_data(UART_TO_PC_RECEIVE_BUFFER,UART_TO_PC_RECEIVE_LENGTH);
        }
        /* �ȴ�lpuart1�����ж� */
        if(UART_TO_LRM_RECEIVE_FLAG)
        {
            UART_TO_LRM_RECEIVE_FLAG = 0;
            usart2_send_data(UART_TO_LRM_RECEIVE_BUFFER,UART_TO_LRM_RECEIVE_LENGTH);
        }
    }
    break;

    /* ͸��ģʽ */
    case DATA_TRANSPORT_MODE:
    {
        /* �������data transport function,�����if���,ִֻ��һ�� */
        if(dev_stat != DATA_TRANSPORT_MODE)
        {
            dev_stat = DATA_TRANSPORT_MODE;
            debug_printf("\r\n[Transperant Mode]\r\n");

            /* ģ�������ж� */
            if(nodeJoinNet(JOIN_TIME_120_SEC) == false)
            {
                return;
            }

            temper = HDC1000_Read_Temper()/1000;

            nodeDataCommunicate((uint8_t*)&temper,sizeof(temper),&pphead);
        }

        /* �ȴ�usart2�����ж� */
        if(UART_TO_PC_RECEIVE_FLAG && GET_BUSY_LEVEL)  //Ensure BUSY is high before sending data
        {
            UART_TO_PC_RECEIVE_FLAG = 0;
            nodeDataCommunicate((uint8_t*)UART_TO_PC_RECEIVE_BUFFER, UART_TO_PC_RECEIVE_LENGTH, &pphead);
        }

        /* ���ģ����æ, ����������Ч��������������Ϣ */
        else if(UART_TO_PC_RECEIVE_FLAG && (GET_BUSY_LEVEL == 0))
        {
            UART_TO_PC_RECEIVE_FLAG = 0;
            debug_printf("--> Warning: Don't send data now! Module is busy!\r\n");
        }

        /* �ȴ�lpuart1�����ж� */
        if(UART_TO_LRM_RECEIVE_FLAG)
        {
            UART_TO_LRM_RECEIVE_FLAG = 0;
            usart2_send_data(UART_TO_LRM_RECEIVE_BUFFER,UART_TO_LRM_RECEIVE_LENGTH);
        }
    }
    break;

    /*����ģʽ*/
    case PRO_TRAINING_MODE:
    {
        /* �������Class C��ƽ̨���ݲɼ�ģʽ, �����if���,ִֻ��һ�� */
        if(dev_stat != PRO_TRAINING_MODE)
        {
            dev_stat = PRO_TRAINING_MODE;
            debug_printf("\r\n[Project Mode]\r\n");

        }
				
				/* ���ʵ�����λ�� */
				float temperature;
				char *time_str = Get_RTC_Timestamp();  // ��ȡ��ǰʱ���
				temperature = HDC1000_return();
				
				//��ȡ�����С�¶�
				if (i > 0) {
    // ���������Сֵ���²�
					if (temperature > max_value) max_value = temperature;
					if (temperature < min_value) min_value = temperature;

    // ����ͳ������
					update_statistics(temperature);
					}
				
				// �����¶Ȳ��������²�
				if (i > 0) 
				{  // ȷ��������ǰһ������
					temp_diff = fabs(temperature - last_temperature);  // ����仯��
					if (temp_diff > max_diff) {
							max_diff = temp_diff;  // ��������²�
					}
				}
		
				save_temp[i] = temperature;
				average += temperature;
				
				
				debug_printf("��ǰʱ��: %s,��ǰ�¶�Ϊ%.3f��\n",time_str,save_temp[i]);
				LEDWAKE_ON;               // �� LED
        HAL_Delay(LED_BLINK_DELAY); // ��ʱ

        LEDWAKE_OFF;              // �ر� LED
        HAL_Delay(LED_BLINK_DELAY); // ��ʱ
			
				
				// ÿ��һ���ӱ������ݵ�Flash
				if (i % 6 == 0 && i!=0) {  // ����ÿʮ���ȡһ�����ݣ�6�μ�Ϊһ����
						minute_counter = minute_counter +3;
						strcpy(temp_data.time_str, time_str);
            temp_data.temperature = (float)temperature;
            // �洢��ǰ���ݵ�Flash
            store_data_to_flash(&temp_data, FLASH_USER_START_ADDR);
						debug_printf("�����%d���ӵ��¶�Ϊ%.3f�浽flash��\n",minute_counter,temperature);
				}
				
				
				// ���ﵽһ�����ʱ��������
				if (minute_counter == total_hour * 60) {
						// ������
						average = average / (total_hour * 60* 6 + 1);  // ��ƽ���¶�
						float variance = calculate_variance();
						float temp_rate = max_diff;  // ����¶ȱ仯��
						float diff = max_value - min_value;

								
						//����ԭʼ����
						if (Flash_Erase(FLASH_USER_START_ADDR,FLASH_USER_END_ADDR)!= HAL_OK){
						// �������ʧ�ܣ����Դ�������������������Ϣ��
								debug_printf("����ʧ��\n");
						} 
						else {
							// �����ɹ�������ɹ���Ϣ
							debug_printf("�ɹ�����ԭʼ�¶�����\n");
							}
						
						// ����һ��ʱ��������赱ǰʱ����Ϊ�����ʱ�����
						char *end_of_day_timestamp = Get_RTC_Timestamp();  // ��ȡ��ǰʱ���

						// ����������������ݵ�TempData�ṹ����

						// 1. �洢��ƽ���¶�
						TempData avg_data;
						uint32_t address_1; //��¼�洢�����ĵ�ַ
						strcpy(avg_data.time_str, end_of_day_timestamp);  // ����ʱ���
						avg_data.temperature = (float)average;  // ����ƽ���¶�
						address_1 = store_data_to_flash(&avg_data,FLASH_USER_START_ADDR);
						debug_printf("������ƽ���¶�Ϊ%.3f�浽flash��\n", average);

						// 2. �洢������¶�
						TempData max_temp_data;
						uint32_t address_2;
						strcpy(max_temp_data.time_str, end_of_day_timestamp);  // ����ʱ���
						max_temp_data.temperature = (float)max_value;  // ��������¶�
						address_2 = store_data_to_flash(&max_temp_data,address_1);
						debug_printf("����������¶�Ϊ%.3f�浽flash��\n", max_value);

						// 3. �洢������¶�
						TempData min_temp_data;
						uint32_t address_3;
						strcpy(min_temp_data.time_str, end_of_day_timestamp);  // ����ʱ���
						min_temp_data.temperature = (float)min_value;  // ��������¶�
						address_3 = store_data_to_flash(&min_temp_data,address_2);
						debug_printf("����������¶�Ϊ%.3f�浽flash��\n", min_value);

						// 4. �洢����²�
						TempData max_diff_data;
						uint32_t address_4;
						strcpy(max_diff_data.time_str, end_of_day_timestamp);  // ����ʱ���
						max_diff_data.temperature = (float)(max_value - min_value);  // ��������²�
						address_4 = store_data_to_flash(&max_diff_data,address_3);
						debug_printf("��������²�Ϊ%.3f�浽flash��\n", max_value - min_value);

						// 5. �洢����
						TempData variance_data;
						uint32_t address_5;
						strcpy(variance_data.time_str, end_of_day_timestamp);  // ����ʱ���
						variance_data.temperature = (float)variance;  // ���淽��
						address_5 = store_data_to_flash(&variance_data,address_4);
						debug_printf("���뷽��Ϊ%.3f��flash��\n", variance);

						// 6. �洢�¶ȱ仯��
						TempData temp_rate_data;
						strcpy(temp_rate_data.time_str, end_of_day_timestamp);  // ����ʱ���
						temp_rate_data.temperature = (float)temp_rate;  // �����¶ȱ仯��
						store_data_to_flash(&temp_rate_data,address_5);
						debug_printf("�����¶ȱ仯��Ϊ%.3f��flash��\n", temp_rate);
						
						read_and_send_data_from_flash(); //��flash�ж�ȡ������������
						minute_counter = 0;
						day++;
						debug_printf("��ʼ��¼��%d�������\n",day);
				}
				
				last_temperature = temperature;
				i++;
				HAL_Delay(10000); //ÿ��10sѭ��һ��
				

    }
    break;

    default:
        break;
    }
}


/**
 * @brief   ������汾��Ϣ�Ͱ���ʹ��˵����Ϣ��ӡ
 * @details �ϵ����еƻ������100ms
 * @param   ��
 * @return  ��
 */
void LoRaWAN_Borad_Info_Print(void)
{
    debug_printf("\r\n\r\n");
    PRINT_CODE_VERSION_INFO("%s",CODE_VERSION);
    debug_printf("\r\n");
    debug_printf("--> Press Key1 to: \r\n");
    debug_printf("-->  - Enter command Mode\r\n");
    debug_printf("-->  - Enter Transparent Mode\r\n");
    debug_printf("--> Press Key2 to: \r\n");
    debug_printf("-->  - Enter Project Trainning Mode\r\n");
    LEDALL_ON;
    HAL_Delay(100);
    LEDALL_OFF;
}




