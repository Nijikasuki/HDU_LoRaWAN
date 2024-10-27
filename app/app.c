#include <string.h>
#include "app.h"
#include "usart.h"
#include "gpio.h"
#include "lorawan_node_driver.h"
#include "hdc1000.h"
#include "sensors_test.h"
#include "math.h"


#define TEMP_DATA_SIZE ((sizeof(TempData) + 7) / 8 * 8)  // 保证是8字节对齐
#define LED_BLINK_DELAY 500
#define FLASH_USER_START_ADDR ((uint32_t)0x080e1000)  // 存储的起始地址
#define ADDR_FLASH_PAGE_511   ((uint32_t)0x080ff800) /* Base @ of Page 511, 2 Kbytes */
#define FLASH_USER_END_ADDR     ADDR_FLASH_PAGE_511 + FLASH_PAGE_SIZE - 1   //存储终止页

#define total_hour 0.05  //一整天则设为24，一小时设为1

typedef struct {
    char time_str[20];   // 存储时间戳
    float temperature;   // 存储温度
} TempData;

extern DEVICE_MODE_T device_mode;
extern DEVICE_MODE_T *Device_Mode_str;
down_list_t *pphead = NULL;

TempData temp_data;  // 用于存储每分钟的数据
float save_temp[10000];
float max_value = 0;
float min_value = 100;
float average = 0;
int i = 0;
int day = 0;
int minute_counter = 0;  // 记录已经过了多少分钟
double temperature, last_temperature = 0, total_variance = 0, temp_diff = 0, max_diff = 0;
float mean = 0.0;
float M2 = 0.0;  // 用于计算方差的累加值
int count = 0;   // 数据计数器

uint8_t dev_eui_7th_byte = 0xA0;
uint8_t dev_eui_8th_byte = 0xE2;


void update_statistics(float temperature) {
    count++;
    float delta = temperature - mean;      // 新数据与当前平均值的差
    mean += delta / count;                 // 更新平均值
    M2 += delta * (temperature - mean);    // 更新用于方差的累加值
}

float calculate_variance() {
    if (count < 2) {
        return 0.0;  // 至少要有两个数据点才能计算方差
    }
    return M2 / (count - 1);  // 样本方差
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

    // 解锁Flash
    HAL_FLASH_Unlock();

    // 清除任何可能影响擦除的标志
    __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_OPTVERR);

    // 获取起始页和页数
    FirstPage = GetPage(startAddress);
    NbOfPages = GetPage(endAddress) - FirstPage + 1;
    BankNumber = GetBank(startAddress);

    // 配置EraseInitStruct结构
    EraseInitStruct.TypeErase = FLASH_TYPEERASE_PAGES;
    EraseInitStruct.Banks = BankNumber;
    EraseInitStruct.Page = FirstPage;
    EraseInitStruct.NbPages = NbOfPages;
		
    // 执行擦除操作
    HAL_StatusTypeDef status = HAL_FLASHEx_Erase(&EraseInitStruct, &PAGEError);

    // 锁定Flash以保护内存
    HAL_FLASH_Lock();

    return status;  // 返回状态，用于检查是否成功
}


void store_float_in_buffer(float number, uint8_t *buffer, int start_index) {
    // 使用指针访问 float 的字节表示
    uint8_t *byte_ptr = (uint8_t*)&number;

    // 按大端序（Big-Endian）的顺序存储
    buffer[start_index] = byte_ptr[3];
    buffer[start_index + 1] = byte_ptr[2];
    buffer[start_index + 2] = byte_ptr[1];
    buffer[start_index + 3] = byte_ptr[0];
}


void send_data_via_lorawan(float average,float max_value,float min_value,float diff,float variance,float temp_rate) {
    uint8_t buffer[31]; // 通信协议要求的31字节数据

    // 字节1：帧头
    buffer[0] = 0xAA;

    // 字节2-3：DevEui第7和第8字节
    buffer[1] = dev_eui_7th_byte;
    buffer[2] = dev_eui_8th_byte;

    // 字节4：数据类型 (假设传输平均温度)
    buffer[3] = 0x01;

    // 字节5-6：时间戳
    buffer[4] = 0x4A;
    buffer[5] = 0x1B;

    // 字节7-10：平均温度（float型）
		store_float_in_buffer(average, buffer, 6);

    // 字节11-14：最高温度（float型）
		store_float_in_buffer(max_value, buffer, 10);

    // 字节15-18：最低温度（float型）
		store_float_in_buffer(min_value, buffer, 14);

    // 字节19-22：温差（float型）
		store_float_in_buffer(diff, buffer, 18);

    // 字节23-26：方差（float型）
		store_float_in_buffer(variance, buffer, 22);

    // 字节27-30：温度变化率（float型）
		store_float_in_buffer(temp_rate, buffer, 26);

    // 字节31：帧尾
    buffer[30] = 0x0F;

    // 通过 LPUART1 发送数据
    lpusart1_send_data(buffer, 31);
		debug_printf("数据处理结果上传成功\n");
}


uint32_t store_data_to_flash(TempData *data,uint32_t address ) {
    HAL_FLASH_Unlock();  // 解锁Flash

    // 将数据结构逐字节写入Flash，调整步长为8字节（如果使用 DOUBLEWORD 写入）
    for (int j = 0; j < TEMP_DATA_SIZE; j += 8) {
        uint64_t word = *(uint64_t*)((uint8_t*)data + j);  // 每次取8字节
        HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, address, word);
        address += 8;  // 每次递增8字节
    }

    HAL_FLASH_Lock();  // 锁定Flash
		return address;
}

void read_and_send_data_from_flash(void) {
    uint32_t read_address = FLASH_USER_START_ADDR;  // 从 Flash 的起始地址开始读取

    TempData read_temp_data;
    float average, max_value, min_value, diff, variance, temp_rate;

    // 逐一读取每个结构体
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

    // 打印读取到的结果
    debug_printf("从 flash 中读取得到的日平均温度为：%.3f℃ 日最高温度为：%.3f℃ 日最低温度为：%.3f℃ 最大温差为：%.3f℃ 方差为：%.3f 温度变化率为：%.3f\n", 
                 average, max_value, min_value, diff, variance, temp_rate);

    // 将数据通过 LoRaWAN 发送
    send_data_via_lorawan(average, max_value, min_value, diff, variance, temp_rate);
		
		
		//擦除处理数据
		if (Flash_Erase(FLASH_USER_START_ADDR,FLASH_USER_END_ADDR)!= HAL_OK){
	// 如果擦除失败，可以处理错误（例如输出调试信息）
			debug_printf("处理结果擦除失败\n");
			} 
		else {
// 擦除成功，输出成功消息
			debug_printf("成功删除flash中处理结果的备份\n");
		}
}


//-----------------Users application--------------------------
void LoRaWAN_Func_Process(void)
{
    static DEVICE_MODE_T dev_stat = NO_MODE;

    uint16_t temper = 0;

    switch((uint8_t)device_mode)
    {
    /* 指令模式 */
    case CMD_CONFIG_MODE:
    {
        /* 如果不是command Configuration function, 则进入if语句,只执行一次 */
        if(dev_stat != CMD_CONFIG_MODE)
        {
            dev_stat = CMD_CONFIG_MODE;
            debug_printf("\r\n[Command Mode]\r\n");

            nodeGpioConfig(wake, wakeup);
            nodeGpioConfig(mode, command);
        }
        /* 等待usart2产生中断 */
        if(UART_TO_PC_RECEIVE_FLAG)
        {
            UART_TO_PC_RECEIVE_FLAG = 0;
            lpusart1_send_data(UART_TO_PC_RECEIVE_BUFFER,UART_TO_PC_RECEIVE_LENGTH);
        }
        /* 等待lpuart1产生中断 */
        if(UART_TO_LRM_RECEIVE_FLAG)
        {
            UART_TO_LRM_RECEIVE_FLAG = 0;
            usart2_send_data(UART_TO_LRM_RECEIVE_BUFFER,UART_TO_LRM_RECEIVE_LENGTH);
        }
    }
    break;

    /* 透传模式 */
    case DATA_TRANSPORT_MODE:
    {
        /* 如果不是data transport function,则进入if语句,只执行一次 */
        if(dev_stat != DATA_TRANSPORT_MODE)
        {
            dev_stat = DATA_TRANSPORT_MODE;
            debug_printf("\r\n[Transperant Mode]\r\n");

            /* 模块入网判断 */
            if(nodeJoinNet(JOIN_TIME_120_SEC) == false)
            {
                return;
            }

            temper = HDC1000_Read_Temper()/1000;

            nodeDataCommunicate((uint8_t*)&temper,sizeof(temper),&pphead);
        }

        /* 等待usart2产生中断 */
        if(UART_TO_PC_RECEIVE_FLAG && GET_BUSY_LEVEL)  //Ensure BUSY is high before sending data
        {
            UART_TO_PC_RECEIVE_FLAG = 0;
            nodeDataCommunicate((uint8_t*)UART_TO_PC_RECEIVE_BUFFER, UART_TO_PC_RECEIVE_LENGTH, &pphead);
        }

        /* 如果模块正忙, 则发送数据无效，并给出警告信息 */
        else if(UART_TO_PC_RECEIVE_FLAG && (GET_BUSY_LEVEL == 0))
        {
            UART_TO_PC_RECEIVE_FLAG = 0;
            debug_printf("--> Warning: Don't send data now! Module is busy!\r\n");
        }

        /* 等待lpuart1产生中断 */
        if(UART_TO_LRM_RECEIVE_FLAG)
        {
            UART_TO_LRM_RECEIVE_FLAG = 0;
            usart2_send_data(UART_TO_LRM_RECEIVE_BUFFER,UART_TO_LRM_RECEIVE_LENGTH);
        }
    }
    break;

    /*工程模式*/
    case PRO_TRAINING_MODE:
    {
        /* 如果不是Class C云平台数据采集模式, 则进入if语句,只执行一次 */
        if(dev_stat != PRO_TRAINING_MODE)
        {
            dev_stat = PRO_TRAINING_MODE;
            debug_printf("\r\n[Project Mode]\r\n");

        }
				
				/* 你的实验代码位置 */
				float temperature;
				char *time_str = Get_RTC_Timestamp();  // 获取当前时间戳
				temperature = HDC1000_return();
				
				//获取最大最小温度
				if (i > 0) {
    // 更新最大、最小值和温差
					if (temperature > max_value) max_value = temperature;
					if (temperature < min_value) min_value = temperature;

    // 更新统计数据
					update_statistics(temperature);
					}
				
				// 计算温度差异和最大温差
				if (i > 0) 
				{  // 确保至少有前一次数据
					temp_diff = fabs(temperature - last_temperature);  // 计算变化率
					if (temp_diff > max_diff) {
							max_diff = temp_diff;  // 更新最大温差
					}
				}
		
				save_temp[i] = temperature;
				average += temperature;
				
				
				debug_printf("当前时间: %s,当前温度为%.3f℃\n",time_str,save_temp[i]);
				LEDWAKE_ON;               // 打开 LED
        HAL_Delay(LED_BLINK_DELAY); // 延时

        LEDWAKE_OFF;              // 关闭 LED
        HAL_Delay(LED_BLINK_DELAY); // 延时
			
				
				// 每隔一分钟保存数据到Flash
				if (i % 6 == 0 && i!=0) {  // 假设每十秒获取一次数据，6次即为一分钟
						minute_counter = minute_counter +3;
						strcpy(temp_data.time_str, time_str);
            temp_data.temperature = (float)temperature;
            // 存储当前数据到Flash
            store_data_to_flash(&temp_data, FLASH_USER_START_ADDR);
						debug_printf("存入第%d分钟的温度为%.3f℃到flash中\n",minute_counter,temperature);
				}
				
				
				// 当达到一天结束时处理数据
				if (minute_counter == total_hour * 60) {
						// 计算结果
						average = average / (total_hour * 60* 6 + 1);  // 日平均温度
						float variance = calculate_variance();
						float temp_rate = max_diff;  // 最大温度变化率
						float diff = max_value - min_value;

								
						//擦除原始数据
						if (Flash_Erase(FLASH_USER_START_ADDR,FLASH_USER_END_ADDR)!= HAL_OK){
						// 如果擦除失败，可以处理错误（例如输出调试信息）
								debug_printf("擦除失败\n");
						} 
						else {
							// 擦除成功，输出成功消息
							debug_printf("成功擦除原始温度数据\n");
							}
						
						// 创建一个时间戳（假设当前时间作为保存的时间戳）
						char *end_of_day_timestamp = Get_RTC_Timestamp();  // 获取当前时间戳

						// 创建并保存各个数据到TempData结构体中

						// 1. 存储日平均温度
						TempData avg_data;
						uint32_t address_1; //记录存储结束的地址
						strcpy(avg_data.time_str, end_of_day_timestamp);  // 保存时间戳
						avg_data.temperature = (float)average;  // 保存平均温度
						address_1 = store_data_to_flash(&avg_data,FLASH_USER_START_ADDR);
						debug_printf("存入日平均温度为%.3f℃到flash中\n", average);

						// 2. 存储日最高温度
						TempData max_temp_data;
						uint32_t address_2;
						strcpy(max_temp_data.time_str, end_of_day_timestamp);  // 保存时间戳
						max_temp_data.temperature = (float)max_value;  // 保存最高温度
						address_2 = store_data_to_flash(&max_temp_data,address_1);
						debug_printf("存入日最高温度为%.3f℃到flash中\n", max_value);

						// 3. 存储日最低温度
						TempData min_temp_data;
						uint32_t address_3;
						strcpy(min_temp_data.time_str, end_of_day_timestamp);  // 保存时间戳
						min_temp_data.temperature = (float)min_value;  // 保存最低温度
						address_3 = store_data_to_flash(&min_temp_data,address_2);
						debug_printf("存入日最低温度为%.3f℃到flash中\n", min_value);

						// 4. 存储最大温差
						TempData max_diff_data;
						uint32_t address_4;
						strcpy(max_diff_data.time_str, end_of_day_timestamp);  // 保存时间戳
						max_diff_data.temperature = (float)(max_value - min_value);  // 保存最大温差
						address_4 = store_data_to_flash(&max_diff_data,address_3);
						debug_printf("存入最大温差为%.3f℃到flash中\n", max_value - min_value);

						// 5. 存储方差
						TempData variance_data;
						uint32_t address_5;
						strcpy(variance_data.time_str, end_of_day_timestamp);  // 保存时间戳
						variance_data.temperature = (float)variance;  // 保存方差
						address_5 = store_data_to_flash(&variance_data,address_4);
						debug_printf("存入方差为%.3f到flash中\n", variance);

						// 6. 存储温度变化率
						TempData temp_rate_data;
						strcpy(temp_rate_data.time_str, end_of_day_timestamp);  // 保存时间戳
						temp_rate_data.temperature = (float)temp_rate;  // 保存温度变化率
						store_data_to_flash(&temp_rate_data,address_5);
						debug_printf("存入温度变化率为%.3f到flash中\n", temp_rate);
						
						read_and_send_data_from_flash(); //从flash中读取处理结果并发送
						minute_counter = 0;
						day++;
						debug_printf("开始记录第%d天的数据\n",day);
				}
				
				last_temperature = temperature;
				i++;
				HAL_Delay(10000); //每隔10s循环一次
				

    }
    break;

    default:
        break;
    }
}


/**
 * @brief   开发板版本信息和按键使用说明信息打印
 * @details 上电所有灯会短暂亮100ms
 * @param   无
 * @return  无
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




