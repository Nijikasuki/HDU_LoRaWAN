# ifndef __STR_H__
# define __STR_H__
#include "stdint.h"

void Int2Str(uint8_t* str, int32_t intnum);
uint8_t *StringCat(uint8_t *str, const uint8_t *string);
uint8_t *StringCat2(uint8_t *str, const uint8_t *string);
int8_t StrToHex(uint8_t temp);
uint8_t *StringStr(uint8_t *str,uint8_t *dest);
int8_t StringCmp(const uint8_t *src,const uint8_t *dst);
uint8_t* find_string(uint8_t *s, uint8_t *d);
void match_string(uint8_t *str, uint8_t *s, uint8_t *e, uint8_t *res);
int8_t LowerStrToUpperStr(uint8_t *p_str);
int8_t lowstr_num_to_uppstr(uint8_t *p_str,uint8_t length);
unsigned int str2int(const char *str);
char* strnstr(char* s1, char* s2, int pos1);
#endif  //(__SRT_H)







