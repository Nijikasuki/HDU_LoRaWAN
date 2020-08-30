#include "str.h"
#include "string.h"

/**
  * @brief  Convert an Integer to a string
  * @param  str: The string
  * @param  intnum: The integer to be converted
  * @retval None
  */
void Int2Str(uint8_t* str, int32_t intnum)
{
  uint32_t i, Div = 1000000000, j = 0, Status = 0;
 
  if(intnum < 0)
  {
	intnum = intnum*(-1);
	str[j++] = '-';
  }
  
  for (i = 0; i < 10; i++)
  {
    str[j++] = (intnum / Div) + 48;	/* '0' */

    intnum = intnum % Div;
    Div /= 10;
    if ((str[j-1] == '0') & (Status == 0))
    {
      j = 0;
    }
    else
    {
      Status++;
    }
  }
}

/*
**	string concat
*/
uint8_t *StringCat(uint8_t *str,const uint8_t *string)
{
	uint8_t *s = str;
	while(*s)
	{
		s++; 
	}
	
	while(*string)
	{
		*s++ = *string++;
	}
	
	*s++ = '\r';
	*s++ = '\n';
	*s = '\0';		
	return str;	
}

uint8_t *StringCat2(uint8_t *str, const uint8_t *string)
{
	uint8_t *s = str;
	while(*s)
	{
		s++;
	}
	
	while(*string)
	{
		*s++ = *string++;
	}
	*s = '\0';
	return str;		
}

int8_t StrToHex(uint8_t temp)
{
	int8_t ret = 0;
	if(temp>=48 && temp<=57)
	{
		ret = temp - 48;
		return ret;
	}

	if(temp>=65 && temp<=70)
	{
		ret = temp - 55;
		return ret;
	}

	if(temp>=97 && temp<=102)
	{
		ret = temp - 87;
		return ret;
	}
	return -1;
}

uint8_t *StringStr(uint8_t *str,uint8_t *dest)
{
    #define STR_BUFF_LEN	0x100
	int i = STR_BUFF_LEN;
	uint8_t *cp = str;
	uint8_t *s1 = NULL, *s2 = NULL;
	
	if (*dest == 0)
	{
		return 0;
	}
	
	while(i--)
	{		
		s1 = cp;
		s2 = dest;
		
		while((*s1 == *s2) && *s1 && *s2)
		{
			s1++;
			s2++;
		}
		if(!*s2)
		{
			return cp;
		}
		else if(*s1)
		{
			cp++;
		}
		else if(!*s1)
		{
			return 0;
		}
	}
	return 0;
}

int8_t StringCmp(const uint8_t *src,const uint8_t *dst)
{
	int ret;
	while (!(ret=*(unsigned char *)src-*(unsigned char *)dst)&& *dst)//ѭ���Ƚ������ַ��Ƿ����
	{
		++src;
		++dst;
	}
	if(ret<0) ret=-1;
	else
	if(ret>0) ret=1;

	return (ret);
}

uint8_t* find_string(uint8_t *s, uint8_t *d)
{
    uint8_t *tmp;
    
    while(0 != *s && 0 != *d)
    {
        tmp = d;
        while(*s == *tmp && *s && *tmp)
        {
            s++;
            tmp++;
        }
        
        if(0 == *tmp)
        {
            return s;
        }
        s++;
    }
    return NULL;
}

void match_string(unsigned char *str, unsigned char *s, unsigned char *e, unsigned char *res)
{
    unsigned char *first_result = NULL;
    unsigned char *f_t = NULL, *tmp = NULL;
    unsigned int i = 0, t_i = 0;
	unsigned char result_flag = 0;

    first_result = find_string(str, s);

    if(NULL != first_result)
    {
        f_t = first_result;
        while(0 != *f_t && 0 != *e)
        {
            tmp = e;
            t_i = 0;
            while(*f_t == *tmp && *f_t && *tmp)
            {
                f_t++;
                tmp++;
                t_i++;
            }

            if(0 == *tmp)
            {
    			result_flag = 1;
                break;
            }
            else if(t_i > 0)
            {
                f_t--;
                t_i--;
            }

            f_t++;
            i += t_i + 1;
        }

        while(i > 0 && result_flag)
        {
            *res = *first_result;
            res ++;
            first_result ++;
            i--;
        }
    }
}

int8_t LowerStrToUpperStr(uint8_t *p_str)
{
	if(!*p_str)
		return -1;
	while(*p_str!='\0') //����һ���ַ����Իس�������
	{
		if(*p_str >= 'a'&& *p_str <= 'z') //������д��ĸʱ��ת����Сд������
        {
            *p_str -= 32;
        }
		p_str ++; //0~25��Ӧa~z
	}
	return 0;
}

int8_t lowstr_num_to_uppstr(uint8_t *p_str,uint8_t length)
{
	if(!*p_str)
		return -1;
	while(length --) //����һ���ַ����Իس�������
	{
		if(*p_str >= 'a' && *p_str <= 'z') //����Сд��ĸʱ��ת���ɴ�д������
        {
            *p_str -= 32;
        }
		p_str ++; //0~25��Ӧa~z
	}
	return 0;
}



unsigned int str2int(const char *str)
{
	unsigned int temp = 0;
	char count = 0;
	const char *ptr = str;  //ptr����str�ַ�����ͷ
	if (*str == '-' || *str == '+')  //�����һ���ַ��������ţ�
	{                      //���Ƶ���һ���ַ�
	   str++;
	}
	while(*str != 0)
	{
	   if ((*str < '0') || (*str > '9'))  //�����ǰ�ַ���������
	   {
		   if(count) break;
		   str++;      //�Ƶ���һ���ַ�
		   continue;
	   }
	   count ++;
	   temp = temp * 10 + (*str - '0'); //�����ǰ�ַ��������������ֵ
	   str++;      //�Ƶ���һ���ַ�
	}
	if (*ptr == '-')     //����ַ������ԡ�-����ͷ����ת�������෴��
	{
	   temp = -temp;
	}
	return temp;
}

/**
* strnstr - Find the first substring in a %NUL terminated string
* @s1: The string to be searched
* @s2: The string to search for
* @pos1: ��s1��ǰpos1�ַ��в���
* add by?zoukaiping*/

char* strnstr(char* s1, char* s2, int pos1)
{
    int l1, l2;
    
    l1 = strlen(s1);
    l2 = strlen(s2);
    if (!l1 || !l2)
        return 0;
    pos1 = (pos1 > l1) ? l1:pos1;
    while (pos1) 
    {
        pos1--;
        if (!memcmp(s1, s2, l2))
            return (char *)s1;
        s1++;
    }
    return NULL;
}





