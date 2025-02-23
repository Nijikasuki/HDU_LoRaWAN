#include "ST7789v.h"
#include "XPT2046.h"
#include "tim.h"

extern uint8_t LCD_EN;

const unsigned char asc2_1608[95][16]= {
    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},/*" ",0*/
    {0x00,0x00,0x00,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x00,0x00,0x18,0x18,0x00,0x00},/*"!",1*/
    {0x00,0x48,0x6C,0x24,0x12,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},/*""",2*/
    {0x00,0x00,0x00,0x24,0x24,0x24,0x7F,0x12,0x12,0x12,0x7F,0x12,0x12,0x12,0x00,0x00},/*"#",3*/
    {0x00,0x00,0x08,0x1C,0x2A,0x2A,0x0A,0x0C,0x18,0x28,0x28,0x2A,0x2A,0x1C,0x08,0x08},/*"$",4*/
    {0x00,0x00,0x00,0x22,0x25,0x15,0x15,0x15,0x2A,0x58,0x54,0x54,0x54,0x22,0x00,0x00},/*"%",5*/
    {0x00,0x00,0x00,0x0C,0x12,0x12,0x12,0x0A,0x76,0x25,0x29,0x11,0x91,0x6E,0x00,0x00},/*"&",6*/
    {0x00,0x06,0x06,0x04,0x03,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},/*"'",7*/
    {0x00,0x40,0x20,0x10,0x10,0x08,0x08,0x08,0x08,0x08,0x08,0x10,0x10,0x20,0x40,0x00},/*"(",8*/
    {0x00,0x02,0x04,0x08,0x08,0x10,0x10,0x10,0x10,0x10,0x10,0x08,0x08,0x04,0x02,0x00},/*")",9*/
    {0x00,0x00,0x00,0x00,0x08,0x08,0x6B,0x1C,0x1C,0x6B,0x08,0x08,0x00,0x00,0x00,0x00},/*"*",10*/
    {0x00,0x00,0x00,0x00,0x08,0x08,0x08,0x08,0x7F,0x08,0x08,0x08,0x08,0x00,0x00,0x00},/*"+",11*/
    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x06,0x06,0x04,0x03},/*",",12*/
    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFE,0x00,0x00,0x00,0x00,0x00,0x00,0x00},/*"-",13*/
    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x06,0x06,0x00,0x00},/*".",14*/
    {0x00,0x00,0x80,0x40,0x40,0x20,0x20,0x10,0x10,0x08,0x08,0x04,0x04,0x02,0x02,0x00},/*"/",15*/
    {0x00,0x00,0x00,0x18,0x24,0x42,0x42,0x42,0x42,0x42,0x42,0x42,0x24,0x18,0x00,0x00},/*"0",16*/
    {0x00,0x00,0x00,0x08,0x0E,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x3E,0x00,0x00},/*"1",17*/
    {0x00,0x00,0x00,0x3C,0x42,0x42,0x42,0x20,0x20,0x10,0x08,0x04,0x42,0x7E,0x00,0x00},/*"2",18*/
    {0x00,0x00,0x00,0x3C,0x42,0x42,0x20,0x18,0x20,0x40,0x40,0x42,0x22,0x1C,0x00,0x00},/*"3",19*/
    {0x00,0x00,0x00,0x20,0x30,0x28,0x24,0x24,0x22,0x22,0x7E,0x20,0x20,0x78,0x00,0x00},/*"4",20*/
    {0x00,0x00,0x00,0x7E,0x02,0x02,0x02,0x1A,0x26,0x40,0x40,0x42,0x22,0x1C,0x00,0x00},/*"5",21*/
    {0x00,0x00,0x00,0x38,0x24,0x02,0x02,0x1A,0x26,0x42,0x42,0x42,0x24,0x18,0x00,0x00},/*"6",22*/
    {0x00,0x00,0x00,0x7E,0x22,0x22,0x10,0x10,0x08,0x08,0x08,0x08,0x08,0x08,0x00,0x00},/*"7",23*/
    {0x00,0x00,0x00,0x3C,0x42,0x42,0x42,0x24,0x18,0x24,0x42,0x42,0x42,0x3C,0x00,0x00},/*"8",24*/
    {0x00,0x00,0x00,0x18,0x24,0x42,0x42,0x42,0x64,0x58,0x40,0x40,0x24,0x1C,0x00,0x00},/*"9",25*/
    {0x00,0x00,0x00,0x00,0x00,0x00,0x18,0x18,0x00,0x00,0x00,0x00,0x18,0x18,0x00,0x00},/*":",26*/
    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x08,0x00,0x00,0x00,0x00,0x00,0x08,0x08,0x04},/*";",27*/
    {0x00,0x00,0x00,0x40,0x20,0x10,0x08,0x04,0x02,0x04,0x08,0x10,0x20,0x40,0x00,0x00},/*"<",28*/
    {0x00,0x00,0x00,0x00,0x00,0x00,0x7F,0x00,0x00,0x00,0x7F,0x00,0x00,0x00,0x00,0x00},/*"=",29*/
    {0x00,0x00,0x00,0x02,0x04,0x08,0x10,0x20,0x40,0x20,0x10,0x08,0x04,0x02,0x00,0x00},/*">",30*/
    {0x00,0x00,0x00,0x3C,0x42,0x42,0x46,0x40,0x20,0x10,0x10,0x00,0x18,0x18,0x00,0x00},/*"?",31*/
    {0x00,0x00,0x00,0x1C,0x22,0x5A,0x55,0x55,0x55,0x55,0x2D,0x42,0x22,0x1C,0x00,0x00},/*"@",32*/
    {0x00,0x00,0x00,0x08,0x08,0x18,0x14,0x14,0x24,0x3C,0x22,0x42,0x42,0xE7,0x00,0x00},/*"A",33*/
    {0x00,0x00,0x00,0x1F,0x22,0x22,0x22,0x1E,0x22,0x42,0x42,0x42,0x22,0x1F,0x00,0x00},/*"B",34*/
    {0x00,0x00,0x00,0x7C,0x42,0x42,0x01,0x01,0x01,0x01,0x01,0x42,0x22,0x1C,0x00,0x00},/*"C",35*/
    {0x00,0x00,0x00,0x1F,0x22,0x42,0x42,0x42,0x42,0x42,0x42,0x42,0x22,0x1F,0x00,0x00},/*"D",36*/
    {0x00,0x00,0x00,0x3F,0x42,0x12,0x12,0x1E,0x12,0x12,0x02,0x42,0x42,0x3F,0x00,0x00},/*"E",37*/
    {0x00,0x00,0x00,0x3F,0x42,0x12,0x12,0x1E,0x12,0x12,0x02,0x02,0x02,0x07,0x00,0x00},/*"F",38*/
    {0x00,0x00,0x00,0x3C,0x22,0x22,0x01,0x01,0x01,0x71,0x21,0x22,0x22,0x1C,0x00,0x00},/*"G",39*/
    {0x00,0x00,0x00,0xE7,0x42,0x42,0x42,0x42,0x7E,0x42,0x42,0x42,0x42,0xE7,0x00,0x00},/*"H",40*/
    {0x00,0x00,0x00,0x3E,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x3E,0x00,0x00},/*"I",41*/
    {0x00,0x00,0x00,0x7C,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x11,0x0F},/*"J",42*/
    {0x00,0x00,0x00,0x77,0x22,0x12,0x0A,0x0E,0x0A,0x12,0x12,0x22,0x22,0x77,0x00,0x00},/*"K",43*/
    {0x00,0x00,0x00,0x07,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x42,0x7F,0x00,0x00},/*"L",44*/
    {0x00,0x00,0x00,0x77,0x36,0x36,0x36,0x36,0x2A,0x2A,0x2A,0x2A,0x2A,0x6B,0x00,0x00},/*"M",45*/
    {0x00,0x00,0x00,0xE3,0x46,0x46,0x4A,0x4A,0x52,0x52,0x52,0x62,0x62,0x47,0x00,0x00},/*"N",46*/
    {0x00,0x00,0x00,0x1C,0x22,0x41,0x41,0x41,0x41,0x41,0x41,0x41,0x22,0x1C,0x00,0x00},/*"O",47*/
    {0x00,0x00,0x00,0x3F,0x42,0x42,0x42,0x42,0x3E,0x02,0x02,0x02,0x02,0x07,0x00,0x00},/*"P",48*/
    {0x00,0x00,0x00,0x1C,0x22,0x41,0x41,0x41,0x41,0x41,0x4D,0x53,0x32,0x1C,0x60,0x00},/*"Q",49*/
    {0x00,0x00,0x00,0x3F,0x42,0x42,0x42,0x3E,0x12,0x12,0x22,0x22,0x42,0xC7,0x00,0x00},/*"R",50*/
    {0x00,0x00,0x00,0x7C,0x42,0x42,0x02,0x04,0x18,0x20,0x40,0x42,0x42,0x3E,0x00,0x00},/*"S",51*/
    {0x00,0x00,0x00,0x7F,0x49,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x1C,0x00,0x00},/*"T",52*/
    {0x00,0x00,0x00,0xE7,0x42,0x42,0x42,0x42,0x42,0x42,0x42,0x42,0x42,0x3C,0x00,0x00},/*"U",53*/
    {0x00,0x00,0x00,0xE7,0x42,0x42,0x22,0x24,0x24,0x14,0x14,0x18,0x08,0x08,0x00,0x00},/*"V",54*/
    {0x00,0x00,0x00,0x6B,0x49,0x49,0x49,0x49,0x55,0x55,0x36,0x22,0x22,0x22,0x00,0x00},/*"W",55*/
    {0x00,0x00,0x00,0xE7,0x42,0x24,0x24,0x18,0x18,0x18,0x24,0x24,0x42,0xE7,0x00,0x00},/*"X",56*/
    {0x00,0x00,0x00,0x77,0x22,0x22,0x14,0x14,0x08,0x08,0x08,0x08,0x08,0x1C,0x00,0x00},/*"Y",57*/
    {0x00,0x00,0x00,0x7E,0x21,0x20,0x10,0x10,0x08,0x04,0x04,0x42,0x42,0x3F,0x00,0x00},/*"Z",58*/
    {0x00,0x78,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x78,0x00},/*"[",59*/
    {0x00,0x00,0x02,0x02,0x04,0x04,0x08,0x08,0x08,0x10,0x10,0x20,0x20,0x20,0x40,0x40},/*"\",60*/
    {0x00,0x1E,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x1E,0x00},/*"]",61*/
    {0x00,0x38,0x44,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},/*"^",62*/
    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF},/*"_",63*/
    {0x00,0x06,0x08,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},/*"`",64*/
    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x3C,0x42,0x78,0x44,0x42,0x42,0xFC,0x00,0x00},/*"a",65*/
    {0x00,0x00,0x00,0x03,0x02,0x02,0x02,0x1A,0x26,0x42,0x42,0x42,0x26,0x1A,0x00,0x00},/*"b",66*/
    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x38,0x44,0x02,0x02,0x02,0x44,0x38,0x00,0x00},/*"c",67*/
    {0x00,0x00,0x00,0x60,0x40,0x40,0x40,0x78,0x44,0x42,0x42,0x42,0x64,0xD8,0x00,0x00},/*"d",68*/
    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x3C,0x42,0x7E,0x02,0x02,0x42,0x3C,0x00,0x00},/*"e",69*/
    {0x00,0x00,0x00,0xF0,0x88,0x08,0x08,0x7E,0x08,0x08,0x08,0x08,0x08,0x3E,0x00,0x00},/*"f",70*/
    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x7C,0x22,0x22,0x1C,0x02,0x3C,0x42,0x42,0x3C},/*"g",71*/
    {0x00,0x00,0x00,0x03,0x02,0x02,0x02,0x3A,0x46,0x42,0x42,0x42,0x42,0xE7,0x00,0x00},/*"h",72*/
    {0x00,0x00,0x00,0x0C,0x0C,0x00,0x00,0x0E,0x08,0x08,0x08,0x08,0x08,0x3E,0x00,0x00},/*"i",73*/
    {0x00,0x00,0x00,0x30,0x30,0x00,0x00,0x38,0x20,0x20,0x20,0x20,0x20,0x20,0x22,0x1E},/*"j",74*/
    {0x00,0x00,0x00,0x03,0x02,0x02,0x02,0x72,0x12,0x0A,0x16,0x12,0x22,0x77,0x00,0x00},/*"k",75*/
    {0x00,0x00,0x00,0x0E,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x3E,0x00,0x00},/*"l",76*/
    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x7F,0x92,0x92,0x92,0x92,0x92,0xB7,0x00,0x00},/*"m",77*/
    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x3B,0x46,0x42,0x42,0x42,0x42,0xE7,0x00,0x00},/*"n",78*/
    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x3C,0x42,0x42,0x42,0x42,0x42,0x3C,0x00,0x00},/*"o",79*/
    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x1B,0x26,0x42,0x42,0x42,0x22,0x1E,0x02,0x07},/*"p",80*/
    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x78,0x44,0x42,0x42,0x42,0x44,0x78,0x40,0xE0},/*"q",81*/
    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x77,0x4C,0x04,0x04,0x04,0x04,0x1F,0x00,0x00},/*"r",82*/
    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x7C,0x42,0x02,0x3C,0x40,0x42,0x3E,0x00,0x00},/*"s",83*/
    {0x00,0x00,0x00,0x00,0x00,0x08,0x08,0x3E,0x08,0x08,0x08,0x08,0x08,0x30,0x00,0x00},/*"t",84*/
    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x63,0x42,0x42,0x42,0x42,0x62,0xDC,0x00,0x00},/*"u",85*/
    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xE7,0x42,0x24,0x24,0x14,0x08,0x08,0x00,0x00},/*"v",86*/
    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xEB,0x49,0x49,0x55,0x55,0x22,0x22,0x00,0x00},/*"w",87*/
    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x76,0x24,0x18,0x18,0x18,0x24,0x6E,0x00,0x00},/*"x",88*/
    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xE7,0x42,0x24,0x24,0x14,0x18,0x08,0x08,0x07},/*"y",89*/
    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x7E,0x22,0x10,0x08,0x08,0x44,0x7E,0x00,0x00},/*"z",90*/
    {0x00,0xC0,0x20,0x20,0x20,0x20,0x20,0x10,0x20,0x20,0x20,0x20,0x20,0x20,0xC0,0x00},/*"{",91*/
    {0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10},/*"|",92*/
    {0x00,0x06,0x08,0x08,0x08,0x08,0x08,0x10,0x08,0x08,0x08,0x08,0x08,0x08,0x06,0x00},/*"}",93*/
    {0x0C,0x32,0xC2,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},/*"~",94*/
};

//gImage_lsd
//-------------------//

const unsigned char gImage_lsd[2970] = { /* 0X00,0X01,0XF0,0X00,0X63,0X00, */
    0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
    0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0XF8,0X00,0X00,
    0X00,0X00,0X07,0XE0,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
    0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X03,0X06,0X00,0X00,0X00,0X00,
    0X1F,0XF0,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
    0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X02,0XFA,0X00,0X00,0X00,0X00,0X3F,0XF8,
    0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
    0X00,0X00,0X00,0X00,0X00,0X00,0X04,0X89,0X00,0X00,0X00,0X00,0X3F,0XFC,0X00,0X00,
    0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
    0X00,0X00,0X00,0X00,0X04,0X89,0X00,0X00,0X00,0X00,0X7F,0XFE,0X00,0X00,0X00,0X00,
    0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
    0X00,0X00,0X04,0XF1,0X00,0X00,0X00,0X00,0X7F,0XFE,0X00,0X00,0X00,0X00,0X00,0X00,
    0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
    0X04,0X99,0X00,0X00,0X00,0X00,0X7F,0XFE,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
    0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X04,0X89,
    0X00,0X07,0XFF,0X80,0X7F,0XFE,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
    0X00,0X00,0X00,0X00,0X7F,0XF8,0X00,0X00,0X00,0X00,0X00,0X00,0X02,0X8E,0X00,0X07,
    0XFF,0X80,0X7F,0XFE,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
    0X00,0X00,0X7F,0XF8,0X00,0X00,0X00,0X00,0X00,0X00,0X01,0X04,0X00,0X07,0XFF,0X80,
    0X7F,0XFE,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
    0X7F,0XF8,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0XF8,0X00,0X07,0XFF,0X80,0X7F,0XFC,
    0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X7F,0XF8,
    0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X07,0XFF,0X80,0X3F,0XFC,0X00,0X00,
    0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X7F,0XF8,0X00,0X00,
    0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X07,0XFF,0X80,0X3F,0XF8,0X00,0X00,0X00,0X00,
    0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X7F,0XF8,0X00,0X00,0X00,0X00,
    0X00,0X00,0X00,0X00,0X00,0X07,0XFF,0X80,0X1F,0XF8,0X00,0X00,0X00,0X00,0X00,0X00,
    0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X7F,0XF8,0X00,0X00,0X00,0X00,0X00,0X00,
    0X00,0X00,0X00,0X07,0XFF,0X80,0X1F,0XF0,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
    0X00,0X00,0X00,0X00,0X00,0X00,0X7F,0XF8,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
    0X00,0X07,0XFF,0X80,0X0F,0XF0,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
    0X00,0X00,0X00,0X00,0X7F,0XF8,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X07,
    0XFF,0X80,0X07,0XE0,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
    0X00,0X00,0X7F,0XF8,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X07,0XFF,0X80,
    0X07,0XC0,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
    0X7F,0XF8,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X07,0XFF,0X80,0X03,0XC0,
    0X00,0X00,0X01,0XF8,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X7C,0X00,0X7F,0XF8,
    0X00,0X00,0X07,0XC0,0X00,0X00,0X00,0X00,0X00,0X07,0XFF,0X80,0X01,0X80,0X00,0X00,
    0X3F,0XFF,0XC0,0X00,0X00,0X00,0XFF,0XFC,0X00,0X07,0XFF,0XC0,0X7F,0XF8,0X00,0X00,
    0X7F,0XFC,0X07,0XFF,0X00,0X00,0X00,0X07,0XFF,0X80,0X01,0X80,0X00,0X00,0XFF,0XFF,
    0XF0,0X00,0X00,0X07,0XFF,0XFC,0X00,0X1F,0XFF,0XF0,0X7F,0XF8,0X00,0X03,0XFF,0XFF,
    0X07,0XFF,0X80,0X00,0X00,0X07,0XFF,0X80,0X00,0X00,0X00,0X03,0XFF,0XFF,0XFC,0X00,
    0X00,0X1F,0XFF,0XFC,0X00,0X7F,0XFF,0XFC,0X7F,0XF8,0X00,0X07,0XFF,0XFF,0XC7,0XFF,
    0X80,0X00,0X00,0X07,0XFF,0X80,0X00,0X00,0X00,0X07,0XFF,0XFF,0XFF,0X00,0X00,0X3F,
    0XFF,0XFC,0X00,0XFF,0XFF,0XFF,0X7F,0XF8,0X00,0X0F,0XFF,0XFF,0XE7,0XFF,0X80,0X00,
    0X00,0X07,0XFF,0X80,0X00,0X00,0X00,0X1F,0XFF,0XFF,0XFF,0X80,0X00,0X7F,0XFF,0XFC,
    0X01,0XFF,0XFF,0XFF,0X7F,0XF8,0X00,0X1F,0XFF,0XFF,0XFF,0XFF,0X80,0X00,0X00,0X07,
    0XFF,0X80,0X3F,0XFC,0X00,0X3F,0XFF,0XFF,0XFF,0XC0,0X00,0XFF,0XFF,0XFC,0X03,0XFF,
    0XFF,0XFF,0XFF,0XF8,0X00,0X7F,0XFF,0XFF,0XFF,0XFF,0X80,0X00,0X00,0X07,0XFF,0X80,
    0X3F,0XFC,0X00,0X7F,0XFF,0XFF,0XFF,0XE0,0X00,0XFF,0XFF,0XFC,0X07,0XFF,0XFF,0XFF,
    0XFF,0XF8,0X00,0X7F,0XFF,0XFF,0XFF,0XFF,0X80,0X00,0X00,0X07,0XFF,0X80,0X3F,0XFC,
    0X00,0X7F,0XFF,0XFF,0XFF,0XF0,0X01,0XFF,0XFF,0XFC,0X0F,0XFF,0XFF,0XFF,0XFF,0XF8,
    0X00,0XFF,0XFF,0XFF,0XFF,0XFF,0X80,0X00,0X00,0X07,0XFF,0X80,0X3F,0XFC,0X00,0XFF,
    0XFF,0XFF,0XFF,0XF8,0X01,0XFF,0XFF,0XFC,0X1F,0XFF,0XFF,0XFF,0XFF,0XF8,0X01,0XFF,
    0XFF,0XFF,0XFF,0XFF,0X80,0X00,0X00,0X07,0XFF,0X80,0X3F,0XFC,0X01,0XFF,0XFC,0X01,
    0XFF,0XF8,0X03,0XFF,0XF0,0X00,0X1F,0XFF,0XF0,0XFF,0XFF,0XF8,0X01,0XFF,0XFF,0X0F,
    0XFF,0XFF,0X80,0X00,0X00,0X07,0XFF,0X80,0X3F,0XFC,0X01,0XFF,0XE0,0X00,0X7F,0XFC,
    0X03,0XFF,0XE0,0X00,0X3F,0XFF,0X80,0X1F,0XFF,0XF8,0X03,0XFF,0XF8,0X01,0XFF,0XFF,
    0X80,0X00,0X00,0X07,0XFF,0X80,0X3F,0XFC,0X03,0XFF,0XC0,0X00,0X3F,0XFC,0X03,0XFF,
    0XC0,0X00,0X3F,0XFE,0X00,0X07,0XFF,0XF8,0X03,0XFF,0XE0,0X00,0X7F,0XFF,0X80,0X00,
    0X00,0X07,0XFF,0X80,0X3F,0XFC,0X03,0XFF,0X80,0X00,0X1F,0XFE,0X03,0XFF,0XC0,0X00,
    0X7F,0XFC,0X00,0X03,0XFF,0XF8,0X07,0XFF,0XC0,0X00,0X3F,0XFF,0X80,0X00,0X00,0X07,
    0XFF,0X80,0X3F,0XFC,0X07,0XFF,0X80,0X00,0X0F,0XFE,0X03,0XFF,0XC0,0X00,0X7F,0XF8,
    0X00,0X01,0XFF,0XF8,0X07,0XFF,0X80,0X00,0X1F,0XFF,0X80,0X00,0X00,0X07,0XFF,0X80,
    0X3F,0XFC,0X07,0XFF,0X00,0X00,0X07,0XFE,0X03,0XFF,0XC0,0X00,0X7F,0XF8,0X00,0X00,
    0XFF,0XF8,0X07,0XFF,0X00,0X00,0X1F,0XFF,0X80,0X00,0X00,0X07,0XFF,0X80,0X3F,0XFC,
    0X07,0XFE,0X00,0X00,0X07,0XFF,0X03,0XFF,0XC0,0X00,0X7F,0XF0,0X00,0X00,0XFF,0XF8,
    0X0F,0XFF,0X00,0X00,0X0F,0XFF,0X80,0X00,0X00,0X07,0XFF,0X80,0X3F,0XFC,0X0F,0XFF,
    0XFF,0XFF,0XFF,0XFF,0X03,0XFF,0XC0,0X00,0XFF,0XF0,0X00,0X00,0X7F,0XF8,0X0F,0XFE,
    0X00,0X00,0X0F,0XFF,0X80,0X00,0X00,0X07,0XFF,0X80,0X3F,0XFC,0X0F,0XFF,0XFF,0XFF,
    0XFF,0XFF,0X03,0XFF,0XC0,0X00,0XFF,0XE0,0X00,0X00,0X7F,0XF8,0X0F,0XFE,0X00,0X00,
    0X07,0XFF,0X80,0X00,0X00,0X07,0XFF,0X80,0X3F,0XFC,0X0F,0XFF,0XFF,0XFF,0XFF,0XFF,
    0X03,0XFF,0XC0,0X00,0XFF,0XE0,0X00,0X00,0X7F,0XF8,0X0F,0XFE,0X00,0X00,0X07,0XFF,
    0X80,0X00,0X00,0X07,0XFF,0X80,0X3F,0XFC,0X0F,0XFF,0XFF,0XFF,0XFF,0XFF,0X03,0XFF,
    0XC0,0X00,0XFF,0XE0,0X00,0X00,0X7F,0XF8,0X0F,0XFE,0X00,0X00,0X07,0XFF,0X80,0X00,
    0X00,0X07,0XFF,0X80,0X3F,0XFC,0X0F,0XFF,0XFF,0XFF,0XFF,0XFF,0X83,0XFF,0XC0,0X00,
    0XFF,0XE0,0X00,0X00,0X3F,0XF8,0X0F,0XFC,0X00,0X00,0X07,0XFF,0X80,0X00,0X00,0X07,
    0XFF,0X80,0X3F,0XFC,0X0F,0XFF,0XFF,0XFF,0XFF,0XFF,0X83,0XFF,0XC0,0X00,0XFF,0XE0,
    0X00,0X00,0X3F,0XF8,0X0F,0XFC,0X00,0X00,0X07,0XFF,0X80,0X00,0X00,0X07,0XFF,0X80,
    0X3F,0XFC,0X0F,0XFF,0XFF,0XFF,0XFF,0XFF,0X83,0XFF,0XC0,0X00,0XFF,0XE0,0X00,0X00,
    0X3F,0XF8,0X0F,0XFC,0X00,0X00,0X07,0XFF,0X80,0X00,0X00,0X07,0XFF,0X80,0X3F,0XFC,
    0X0F,0XFF,0XFF,0XFF,0XFF,0XFF,0X83,0XFF,0XC0,0X00,0XFF,0XE0,0X00,0X00,0X7F,0XF8,
    0X0F,0XFE,0X00,0X00,0X07,0XFF,0X80,0X00,0X00,0X07,0XFF,0X80,0X3F,0XFC,0X0F,0XFF,
    0XFF,0XFF,0XFF,0XFF,0X03,0XFF,0XC0,0X00,0XFF,0XE0,0X00,0X00,0X7F,0XF8,0X0F,0XFE,
    0X00,0X00,0X07,0XFF,0X80,0X00,0X00,0X07,0XFF,0X80,0X3F,0XFC,0X0F,0XFE,0X00,0X00,
    0X00,0X00,0X03,0XFF,0XC0,0X00,0XFF,0XE0,0X00,0X00,0X7F,0XF8,0X0F,0XFE,0X00,0X00,
    0X07,0XFF,0X80,0X00,0X00,0X07,0XFF,0X80,0X3F,0XFC,0X0F,0XFE,0X00,0X00,0X00,0X00,
    0X03,0XFF,0XC0,0X00,0XFF,0XF0,0X00,0X00,0X7F,0XF8,0X0F,0XFE,0X00,0X00,0X0F,0XFF,
    0X80,0X00,0X00,0X07,0XFF,0X80,0X3F,0XFC,0X07,0XFE,0X00,0X00,0X07,0XFF,0X03,0XFF,
    0XC0,0X00,0X7F,0XF0,0X00,0X00,0XFF,0XF8,0X0F,0XFF,0X00,0X00,0X0F,0XFF,0X80,0X00,
    0X00,0X07,0XFF,0X80,0X3F,0XFC,0X07,0XFF,0X00,0X00,0X07,0XFF,0X03,0XFF,0XC0,0X00,
    0X7F,0XF8,0X00,0X00,0XFF,0XF8,0X07,0XFF,0X00,0X00,0X1F,0XFF,0X80,0X00,0X00,0X07,
    0XFF,0X80,0X3F,0XFC,0X07,0XFF,0X00,0X00,0X0F,0XFE,0X03,0XFF,0XC0,0X00,0X7F,0XF8,
    0X00,0X01,0XFF,0XF8,0X07,0XFF,0X80,0X00,0X1F,0XFF,0X80,0X00,0X00,0X07,0XFF,0X80,
    0X3F,0XFC,0X07,0XFF,0X80,0X00,0X1F,0XFE,0X03,0XFF,0XC0,0X00,0X7F,0XFC,0X00,0X03,
    0XFF,0XF8,0X07,0XFF,0XC0,0X00,0X3F,0XFF,0X80,0X00,0X00,0X07,0XFF,0X80,0X3F,0XFC,
    0X03,0XFF,0XC0,0X00,0X3F,0XFC,0X03,0XFF,0XC0,0X00,0X3F,0XFE,0X00,0X07,0XFF,0XF8,
    0X03,0XFF,0XE0,0X00,0X7F,0XFF,0X80,0X00,0X00,0X07,0XFF,0X80,0X3F,0XFC,0X03,0XFF,
    0XF0,0X00,0XFF,0XFC,0X03,0XFF,0XC0,0X00,0X3F,0XFF,0X80,0X1F,0XFF,0XF8,0X03,0XFF,
    0XF8,0X01,0XFF,0XFF,0X80,0X00,0X00,0X07,0XFF,0X80,0X3F,0XFC,0X01,0XFF,0XFC,0X03,
    0XFF,0XF8,0X03,0XFF,0XC0,0X00,0X1F,0XFF,0XF0,0XFF,0XFF,0XF8,0X01,0XFF,0XFF,0X0F,
    0XFF,0XFF,0X80,0X00,0X00,0X07,0XFF,0X80,0X3F,0XFC,0X00,0XFF,0XFF,0XFF,0XFF,0XF8,
    0X03,0XFF,0XC0,0X00,0X0F,0XFF,0XFF,0XFF,0XFF,0XF8,0X01,0XFF,0XFF,0XFF,0XFF,0XFF,
    0X80,0X00,0X00,0X07,0XFF,0X80,0X3F,0XFC,0X00,0XFF,0XFF,0XFF,0XFF,0XF0,0X03,0XFF,
    0XC0,0X00,0X0F,0XFF,0XFF,0XFF,0XFF,0XF8,0X00,0XFF,0XFF,0XFF,0XFF,0XFF,0X80,0X00,
    0X00,0X07,0XFF,0X80,0X3F,0XFC,0X00,0X7F,0XFF,0XFF,0XFF,0XE0,0X03,0XFF,0XC0,0X00,
    0X07,0XFF,0XFF,0XFF,0XFF,0XF8,0X00,0X7F,0XFF,0XFF,0XFF,0XFF,0X80,0X00,0X00,0X07,
    0XFF,0X80,0X3F,0XFC,0X00,0X3F,0XFF,0XFF,0XFF,0XC0,0X03,0XFF,0XC0,0X00,0X03,0XFF,
    0XFF,0XFF,0XFF,0XF8,0X00,0X3F,0XFF,0XFF,0XFF,0XFF,0X80,0X00,0X00,0X07,0XFF,0X80,
    0X3F,0XFC,0X00,0X1F,0XFF,0XFF,0XFF,0X80,0X03,0XFF,0XC0,0X00,0X01,0XFF,0XFF,0XFF,
    0XFF,0XF8,0X00,0X1F,0XFF,0XFF,0XFF,0XFF,0X80,0X00,0X00,0X07,0XFF,0X80,0X3F,0XFC,
    0X00,0X07,0XFF,0XFF,0XFF,0X00,0X03,0XFF,0XC0,0X00,0X00,0XFF,0XFF,0XFF,0X7F,0XF8,
    0X00,0X0F,0XFF,0XFF,0XE7,0XFF,0X80,0X00,0X00,0X07,0XFF,0X80,0X3F,0XFC,0X00,0X03,
    0XFF,0XFF,0XFC,0X00,0X03,0XFF,0XC0,0X00,0X00,0X7F,0XFF,0XFC,0X7F,0XF8,0X00,0X07,
    0XFF,0XFF,0XC7,0XFF,0X80,0X00,0X00,0X07,0XFF,0X80,0X3F,0XFC,0X00,0X00,0XFF,0XFF,
    0XF0,0X00,0X03,0XFF,0XC0,0X00,0X00,0X1F,0XFF,0XF8,0X7F,0XF8,0X00,0X01,0XFF,0XFF,
    0X07,0XFF,0X80,0X00,0X00,0X07,0XFF,0X80,0X3F,0XFC,0X00,0X00,0X1F,0XFF,0XC0,0X00,
    0X03,0XFF,0XC0,0X00,0X00,0X07,0XFF,0XE0,0X7F,0XF8,0X00,0X00,0X7F,0XFC,0X07,0XFF,
    0X80,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0XF0,0X00,0X00,0X00,0X00,
    0X00,0X00,0X00,0X00,0X3C,0X00,0X00,0X00,0X00,0X00,0X03,0X80,0X00,0X00,0X00,0X00,
    0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
    0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
    0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
    0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
    0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
    0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
    0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
    0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
    0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
    0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
    0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
    0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
    0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
    0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
    0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
    0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
    0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
    0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
    0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X07,0XFF,0XFC,
    0X07,0X81,0XF0,0X00,0X00,0X07,0X80,0X00,0X00,0X00,0X00,0X01,0XE0,0X07,0X80,0X7C,
    0X00,0X1E,0X03,0XC0,0X01,0XFF,0XFF,0XFF,0X80,0X00,0X00,0X07,0XFF,0XFC,0X07,0X81,
    0XF0,0X00,0X00,0X07,0X80,0X0F,0X00,0X7F,0XFF,0X71,0XE0,0X07,0X80,0X3C,0X00,0X3F,
    0XFF,0XFF,0XE3,0XFF,0XFF,0XFF,0X80,0X00,0X00,0X07,0XFF,0XFD,0XE7,0X81,0XFF,0XFF,
    0XFF,0X07,0X80,0X1E,0X00,0X7F,0XFF,0X71,0XE0,0X07,0X80,0X7C,0X00,0X7F,0XFF,0XFF,
    0XE3,0XFF,0XFF,0XFF,0X80,0X00,0X00,0X03,0X5F,0X7D,0XE7,0X83,0XFF,0XFF,0XFF,0X07,
    0X80,0X1E,0X00,0X3B,0XE6,0X71,0XE0,0XFF,0XFF,0XFF,0XF0,0X7F,0XFF,0XFF,0XE3,0XE0,
    0X00,0X07,0X80,0X00,0X00,0X00,0X0F,0X01,0XE7,0X83,0XC0,0X00,0X0F,0X87,0X9F,0XFF,
    0XF8,0X03,0XE0,0X71,0XE0,0X7F,0XFF,0XFF,0XF0,0XF8,0X03,0XE0,0X03,0XE0,0X00,0X07,
    0X80,0X00,0X00,0X00,0X0F,0X01,0XE7,0X87,0XC0,0X00,0X0F,0X80,0X1F,0XFF,0XF8,0X03,
    0XE0,0X71,0XE0,0X7F,0XF7,0XFF,0XE1,0XFF,0XFF,0XFF,0XE3,0XE0,0X03,0XC7,0X80,0X00,
    0X00,0X07,0XFF,0XFD,0XE7,0X87,0X80,0XF0,0X0F,0X80,0X1F,0XFF,0XF8,0X7F,0XFF,0X79,
    0XE0,0X07,0X80,0X3C,0X00,0X7F,0XFF,0XFF,0XE3,0XEF,0XFF,0XF7,0X80,0X00,0X00,0X07,
    0XFF,0XFD,0XE7,0X80,0X00,0XF0,0X07,0X1F,0X00,0X78,0X00,0X7F,0XFF,0X39,0XE0,0X07,
    0X80,0X3C,0X00,0X78,0X03,0XC0,0X03,0XEF,0XFF,0XF7,0X80,0X00,0X00,0X07,0XFF,0XFD,
    0XE7,0X80,0X00,0XF0,0X00,0X1F,0X80,0XF8,0X00,0X7F,0XFF,0X01,0XE0,0X07,0X8F,0XFF,
    0XF0,0X7B,0XFF,0XFF,0XE3,0XEF,0XFF,0XF7,0X80,0X00,0X00,0X00,0X0F,0X01,0XE7,0X80,
    0X78,0XF1,0XE0,0X07,0X80,0XFE,0X00,0X03,0XE0,0X71,0XE0,0X07,0XAF,0XFF,0XF0,0X7F,
    0XFF,0XFF,0XE3,0XE0,0X03,0XC7,0X80,0X00,0X00,0X01,0XCF,0X71,0XE7,0X80,0X78,0XF1,
    0XE0,0X07,0X80,0XFE,0X00,0X03,0XE0,0X71,0XE0,0X07,0XEF,0XFF,0XF0,0X7F,0XFF,0XFF,
    0XE3,0XE0,0X7B,0XC7,0X80,0X00,0X00,0X01,0XCF,0X71,0XE7,0X80,0XF0,0XF1,0XF0,0X07,
    0X81,0XFF,0X00,0X3F,0XFC,0X71,0XE0,0X07,0XE0,0X03,0XE0,0X78,0X03,0XC0,0X03,0XE0,
    0XF3,0XC7,0X80,0X00,0X00,0X01,0XCF,0X71,0XE7,0X80,0XF0,0XF0,0XF0,0X07,0X81,0XEF,
    0X00,0X3B,0XFC,0X71,0XE0,0X07,0XE7,0X87,0XE0,0X7F,0XFF,0XFF,0XE3,0XE0,0XF3,0XC7,
    0X80,0X00,0X00,0X01,0XCF,0X71,0XE7,0X80,0XF0,0XF0,0XF0,0X07,0X83,0XEF,0X00,0X3B,
    0XFC,0X79,0XE0,0X07,0XC7,0X87,0XC0,0X7F,0XFF,0XFF,0XE3,0XE1,0XF3,0XC7,0X80,0X00,
    0X00,0X03,0XCF,0X79,0XE7,0X81,0XF0,0XF0,0XF0,0X07,0X83,0XCF,0X80,0X3B,0XFC,0X39,
    0XE0,0X1F,0X83,0XCF,0X80,0X7B,0X40,0XBF,0X43,0XE1,0XE3,0XC7,0X80,0X00,0X00,0X03,
    0XCF,0X79,0XE7,0X81,0XE0,0XF0,0XF0,0X07,0X87,0XC7,0X80,0X3B,0XDC,0X19,0XE0,0X3F,
    0X81,0XFF,0X00,0X00,0X1E,0X00,0X03,0XE3,0XE3,0XC7,0X80,0X00,0X00,0X03,0XCF,0X79,
    0XE7,0X81,0XE0,0XF0,0XF0,0X07,0X87,0X87,0X80,0X3B,0XDC,0X01,0XE0,0X7F,0X81,0XFE,
    0X01,0XFF,0XFF,0XFF,0XE3,0XE3,0XE3,0XC7,0X80,0X00,0X00,0X03,0XCF,0X79,0XE7,0X81,
    0XE0,0XF0,0XF0,0X07,0X8F,0X87,0XC0,0X3B,0XDC,0X01,0XE0,0XFF,0X80,0XFC,0X01,0XFF,
    0XFF,0XFF,0XE3,0XEF,0XC3,0XC7,0X80,0X00,0X00,0X03,0XCF,0X79,0XE7,0X83,0XE0,0XF0,
    0XF0,0X07,0X8F,0X03,0XC0,0X3B,0XDC,0XFF,0XF0,0XF7,0X80,0XFC,0X00,0X00,0X1E,0X00,
    0X03,0XEF,0X8F,0XC7,0X80,0X00,0X00,0X03,0XCF,0X78,0X07,0X83,0XC0,0XF0,0X78,0X07,
    0X8F,0X03,0XFC,0X3B,0XFC,0XFF,0XF8,0XC7,0X80,0XFC,0X00,0X00,0X1E,0X1C,0X03,0XE0,
    0X0F,0XC7,0X80,0X00,0X00,0X03,0X8F,0X38,0X07,0X83,0XC0,0XF0,0X78,0X07,0X9E,0X01,
    0XFC,0X3B,0XFE,0X01,0XE0,0X87,0X81,0XFE,0X00,0X1F,0X1E,0X1E,0X03,0XE0,0X1F,0X87,
    0X80,0X00,0X00,0X03,0XCF,0X38,0X07,0X83,0XC0,0XF0,0X7F,0X87,0X80,0X00,0X00,0X3B,
    0XFE,0X01,0XE0,0X07,0X83,0XFF,0X00,0X3E,0X1E,0X1F,0X03,0XE0,0X00,0X07,0X80,0X00,
    0X00,0X03,0XCF,0X39,0XFF,0X87,0XC7,0XF0,0X7F,0X0F,0XFF,0XFF,0XFE,0X3B,0XFE,0X01,
    0XE0,0X7F,0X87,0XDF,0XE0,0X7C,0X1E,0X0F,0XE3,0XFF,0XFF,0XFF,0X80,0X00,0X00,0X03,
    0XCF,0X39,0XFF,0X00,0X87,0XF0,0X3F,0X1F,0XFF,0XFF,0XFE,0X3B,0XEE,0X01,0XE0,0X7F,
    0X0F,0XCF,0XF0,0X7C,0X1E,0X0F,0XE3,0XFF,0XFF,0XFF,0X00,0X00,0X00,0X00,0X0F,0X01,
    0XFE,0X00,0X07,0XC0,0X00,0X0F,0XFF,0XFF,0XFC,0X03,0XE0,0X01,0XE0,0X7F,0X0F,0X87,
    0XE0,0XF8,0X1E,0X07,0XC1,0XFF,0XFF,0XFC,0X00,0X00,
};


void  LCD_WriteCommand(uint8_t comd)
{
    uint8_t temp = 0,temp1 = 0 ;
    uint16_t reg = 0;

    DC_LOW();
    CS_LOW();
    //Delay_loop(1);
    WR_LOW();

    GPIOE->ODR &= 0x00FF;
    reg = comd;
    reg = comd<<8;
    GPIOE->ODR |= reg;

    WR_HIGH();
    //Delay_loop(2);
    CS_HIGH();
    DC_HIGH();
}

//========================================================

void LCD_WriteData_8bit(uint8_t data)
{
    uint8_t temp =0,temp1 = 0;
    uint16_t reg = 0;
    temp = data;

    DC_HIGH();
    CS_LOW();
    //Delay_loop(1);
    WR_LOW();

    GPIOE->ODR &= 0x00FF;
    reg = data;
    reg = data<<8;
    GPIOE->ODR |= reg;


    WR_HIGH();
    //Delay_loop(2);
    CS_HIGH();
    DC_HIGH();
}

void  ST7789V_INIT(void)
{
    //----------------------------------------------------------
    //---------------IM0 IM1 IM2 ---------------------------
    IM2_LOW();
    IM1_LOW();
    IM0_HIGH();

    HAL_Delay(20);
    CS_HIGH();
    WR_HIGH();
    RD_HIGH();                      //modify
    DC_LOW();
    HAL_Delay(20);

    //-------------------------------------------------------
    IC_ON();
    HAL_Delay(20);
    REST_HIGH();
    HAL_Delay(10);
    REST_LOW();
    HAL_Delay(150);
    REST_HIGH();
    HAL_Delay(120);
    LCD_WriteCommand(0x11); //Exit Sleep
    HAL_Delay(10);

    //***************************************************************//LCD SETING
    LCD_WriteCommand(0x3A);
    LCD_WriteData_8bit(0x05);

    LCD_WriteCommand(0x36);
    LCD_WriteData_8bit(0x00);
    //--------------------------------ST7789S Frame rate setting----------------------------------//
    LCD_WriteCommand(0xb2);
    LCD_WriteData_8bit(0x0c);
    LCD_WriteData_8bit(0x0c);
    LCD_WriteData_8bit(0x00);
    LCD_WriteData_8bit(0x33);
    LCD_WriteData_8bit(0x33);

    LCD_WriteCommand(0xb7);
    LCD_WriteData_8bit(0x35);
    //---------------------------------ST7789S Power setting--------------------------------------//
    LCD_WriteCommand(0xbb);
    LCD_WriteData_8bit(0x35);//vcom

    LCD_WriteCommand(0xc0);
    LCD_WriteData_8bit(0x2C);//

    LCD_WriteCommand(0xc2);
    LCD_WriteData_8bit(0x01);//

    LCD_WriteCommand(0xc3);
    LCD_WriteData_8bit(0x0F);//

    LCD_WriteCommand(0xc4);
    LCD_WriteData_8bit(0x20);

    LCD_WriteCommand(0xc6);
    LCD_WriteData_8bit(0x11);

    LCD_WriteCommand(0xd0);
    LCD_WriteData_8bit(0xa4);
    LCD_WriteData_8bit(0xa1);

    LCD_WriteCommand(0xe8);
    LCD_WriteData_8bit(0x03);

    LCD_WriteCommand(0xe9);
    LCD_WriteData_8bit(0x09);
    LCD_WriteData_8bit(0x09);
    LCD_WriteData_8bit(0x08);
    //--------------------------------ST7789S gamma setting---------------------------------------//
//	LCD_WriteCommand(0xe0);
//	LCD_WriteData_8bit(0xd0);
//	LCD_WriteData_8bit(0x05);
//	LCD_WriteData_8bit(0x09);
//	LCD_WriteData_8bit(0x09);
//	LCD_WriteData_8bit(0x08);
//	LCD_WriteData_8bit(0x14);
//	LCD_WriteData_8bit(0x28);
//	LCD_WriteData_8bit(0x33);
//	LCD_WriteData_8bit(0x3F);
//	LCD_WriteData_8bit(0x07);
//	LCD_WriteData_8bit(0x13);
//	LCD_WriteData_8bit(0x14);
//	LCD_WriteData_8bit(0x28);
//	LCD_WriteData_8bit(0x30);

//	LCD_WriteCommand(0xe1);
//	LCD_WriteData_8bit(0xd0);
//	LCD_WriteData_8bit(0x05);
//	LCD_WriteData_8bit(0x09);
//	LCD_WriteData_8bit(0x09);
//	LCD_WriteData_8bit(0x08);
//	LCD_WriteData_8bit(0x03);
//	LCD_WriteData_8bit(0x24);
//	LCD_WriteData_8bit(0x32);
//	LCD_WriteData_8bit(0x32);
//	LCD_WriteData_8bit(0x3B);
//	LCD_WriteData_8bit(0x38);
//	LCD_WriteData_8bit(0x14);
//	LCD_WriteData_8bit(0x13);
//	LCD_WriteData_8bit(0x28);
//	LCD_WriteData_8bit(0x2F);


    LCD_WriteCommand(0xe0);
    LCD_WriteData_8bit(0xd0);
    LCD_WriteData_8bit(0x00);
    LCD_WriteData_8bit(0x05);
    LCD_WriteData_8bit(0x0e);
    LCD_WriteData_8bit(0x15);
    LCD_WriteData_8bit(0x0d);
    LCD_WriteData_8bit(0x37);
    LCD_WriteData_8bit(0x43);
    LCD_WriteData_8bit(0x47);
    LCD_WriteData_8bit(0x09);
    LCD_WriteData_8bit(0x15);
    LCD_WriteData_8bit(0x12);
    LCD_WriteData_8bit(0x16);
    LCD_WriteData_8bit(0x19);

    LCD_WriteCommand(0xe1);
    LCD_WriteData_8bit(0xd0);
    LCD_WriteData_8bit(0x00);
    LCD_WriteData_8bit(0x05);
    LCD_WriteData_8bit(0x0d);
    LCD_WriteData_8bit(0x0c);
    LCD_WriteData_8bit(0x06);
    LCD_WriteData_8bit(0x2d);
    LCD_WriteData_8bit(0x44);
    LCD_WriteData_8bit(0x40);
    LCD_WriteData_8bit(0x0e);
    LCD_WriteData_8bit(0x1c);
    LCD_WriteData_8bit(0x18);
    LCD_WriteData_8bit(0x16);
    LCD_WriteData_8bit(0x19);

//	LCD_WriteCommand(0x21); //反显

//	LCD_WriteCommand(0x2A); //Frame rate control
//	LCD_WriteData_8bit(0x00);
//	LCD_WriteData_8bit(0x00);
//	LCD_WriteData_8bit(0x00);
//	LCD_WriteData_8bit(0xEF);

//	LCD_WriteCommand(0x2B); //Display function control
//	LCD_WriteData_8bit(0x00);
//	LCD_WriteData_8bit(0x00);
//	LCD_WriteData_8bit(0x01);
//	LCD_WriteData_8bit(0x3F);

    LCD_WriteCommand(0x11);
    HAL_Delay(120);;      //Delay 120ms

    LCD_WriteCommand(0x29); //display on
    LCD_WriteCommand(0x2c);

}


//-------------------------------------------------------
void LCDOpenWindows(uint16_t x, uint16_t y, uint16_t len, uint16_t wid)
{
    LCD_WriteCommand(0x2A);
    LCD_WriteData_8bit(x/256);
    LCD_WriteData_8bit(x%256);
    LCD_WriteData_8bit((x+len-1)/256);
    LCD_WriteData_8bit((x+len-1)%256);

    LCD_WriteCommand(0x2B);
    LCD_WriteData_8bit(y/256);
    LCD_WriteData_8bit(y%256);
    LCD_WriteData_8bit((y+wid-1)/256);
    LCD_WriteData_8bit((y+wid-1)%256);
    LCD_WriteCommand(0x2C);
}


//========================================================

void LCD_WriteData_16bit(uint16_t data)
{
    uint16_t temp =0,temp1 = 0 ;
    uint16_t reg = 0;
    temp = data;

    static uint16_t test = 0;
    test = data;

    DC_HIGH();
    CS_LOW();
    //Delay_loop(1);
    WR_LOW();

    GPIOE->ODR &= 0x00FF;
    reg = data&0x00ff;
    reg = reg<<8;
    GPIOE->ODR |= reg;

    GPIOD->ODR &= 0x00FF;
    reg = data&0xFF00;
    GPIOD->ODR |= reg;


    WR_HIGH();
    //Delay_loop(2);
    CS_HIGH();
    DC_LOW();
}


////////////////////////////手动描点、数字、字符 ////////////////////

void LCD_DrawPoint(uint16_t x,uint16_t y,uint8_t size,uint16_t color)
{
    static uint16_t i=0,j=0;

    LCDOpenWindows(x,y,size,size);

    for(i=0; i<size; i++)
    {
        for(j=0; j<size; j++)
        {
            LCD_WriteData_16bit(color);
        }
    }
}

//画线
//x1,y1:起点坐标
//x2,y2:终点坐标
void LCD_DrawLine(u16 x1, u16 y1, u16 x2, u16 y2, u16 color)
{
    u16 t;
    int xerr=0,yerr=0,delta_x,delta_y,distance;
    int incx,incy,uRow,uCol;

    delta_x=x2-x1; //计算坐标增量
    delta_y=y2-y1;
    uRow=x1;
    uCol=y1;
    if(delta_x>0)incx=1; //设置单步方向
    else if(delta_x==0)incx=0;//垂直线
    else {
        incx=-1;
        delta_x=-delta_x;
    }
    if(delta_y>0)incy=1;
    else if(delta_y==0)incy=0;//水平线
    else {
        incy=-1;
        delta_y=-delta_y;
    }
    if( delta_x>delta_y)distance=delta_x; //选取基本增量坐标轴
    else distance=delta_y;
    for(t=0; t<=distance+1; t++ ) //画线输出
    {
        LCD_DrawPoint(uRow,uCol,1,color);//画点
        xerr+=delta_x ;
        yerr+=delta_y ;
        if(xerr>distance)
        {
            xerr-=distance;
            uRow+=incx;
        }
        if(yerr>distance)
        {
            yerr-=distance;
            uCol+=incy;
        }
    }
}

//在指定位置画一个指定大小的圆
//(x,y):中心点
//r    :半径
void Draw_Circle(u16 x0,u16 y0,u8 r, u16 color)
{
    int a,b;
    int di;
    a=0;
    b=r;
    di=3-(r<<1);             //判断下个点位置的标志
    while(a<=b)
    {
        LCD_DrawPoint(x0-b,y0-a,1,color);             //3
        LCD_DrawPoint(x0+b,y0-a,1,color);             //0
        LCD_DrawPoint(x0-a,y0+b,1,color);             //1
        LCD_DrawPoint(x0-b,y0-a,1,color);             //7
        LCD_DrawPoint(x0-a,y0-b,1,color);             //2
        LCD_DrawPoint(x0+b,y0+a,1,color);             //4
        LCD_DrawPoint(x0+a,y0-b,1,color);             //5
        LCD_DrawPoint(x0+a,y0+b,1,color);             //6
        LCD_DrawPoint(x0-b,y0+a,1,color);
        a++;
        //使用Bresenham算法画圆
        if(di<0)di +=4*a+6;
        else
        {
            di+=10+4*(a-b);
            b--;
        }
        LCD_DrawPoint(x0+a,y0+b,1,color);
    }
}

//画矩形
void LCD_DrawRectangle(u16 x1, u16 y1, u16 x2, u16 y2 ,u16 color)
{
    LCD_DrawLine(x1,y1,x2,y1,color);
    LCD_DrawLine(x1,y1,x1,y2,color);
    LCD_DrawLine(x1,y2,x2,y2,color);
    LCD_DrawLine(x2,y1,x2,y2,color);
}


void LCD_ShowChar(uint16_t x,uint16_t y,uint8_t num,uint16_t color)
{
    uint8_t pos,temp,t;

    num=num-' ';//得到偏移后的值

    for(pos=0; pos<16; pos++)
    {
        temp=asc2_1608[num][pos];//调用1206字体

        for(t=0; t<8; t++)
        {
            if(temp&0x01)LCD_DrawPoint(x+t,y+pos,1,color);//画一个点
            temp>>=1;
        }
    }

}

u32 mypow(u8 m,u8 n)
{
    u32 result=1;
    while(n--)result*=m;
    return result;
}

//显示2个数字
//x,y :起点坐标
//len :数字的位数
//size:字体大小
//color:颜色
//num:数值(0~4294967295);
void LCD_ShowNum(u16 x,u16 y,u32 num,u8 len,u16 color)
{
    u8 t,temp;
    u8 enshow=0;
    for(t=0; t<len; t++)
    {
        temp=(num/mypow(10,len-t-1))%10;
        if(enshow==0&&t<(len-1))
        {
            if(temp==0)
            {
                LCD_ShowChar(x+(8)*t,y,' ',color);
                continue;
            } else enshow=1;

        }
        LCD_ShowChar(x+(8)*t,y,temp+'0',color);
    }
}


void LCD_Clear(uint16_t color)
{
    static uint16_t i=0,j=0;

    LCDOpenWindows(0,0,240,320);

    for(i=0; i<240; i++)
    {
        for(j=0; j<320; j++)
        {
            LCD_WriteData_16bit(color);
        }
    }
}

//显示字符串
//x,y:起点坐标
//*p:字符串起始地址
//用16字体
void LCD_ShowString(uint16_t x,uint16_t y,const uint8_t *p,uint16_t color)
{
    while(*p!='\0')
    {
        if(x>240) {
            x=0;
            y+=16;
        }
        if(y>320) {
            y=x=0;
            LCD_Clear(WHITE);
        }
        LCD_ShowChar(x,y,*p,color);
        x+=8;
        p++;
    }
}


//在指定区域内填充指定颜色
//区域大小:
//  (xend-xsta)*(yend-ysta)

void LCD_Fill(uint16_t xsta,uint16_t ysta,uint16_t xend,uint16_t yend,uint16_t color)
{
    static uint16_t i=0,j=0;

    LCDOpenWindows(xsta,ysta,xend-xsta,yend-ysta);

    for(i=xsta; i<xend; i++)
    {
        for(j=ysta; j<yend; j++)
        {
            LCD_WriteData_16bit(color);
        }
    }
}

void delay_10ms(int count)   /* 10ms */
{
    uint32_t now =0 , GoTo = 0;
    now = HAL_GetTick();

    GoTo = now + 10*count;

    while(GoTo != HAL_GetTick());
}




//-----------------------------------------------------------------------------------------------
void LCD_Test(void)
{
    LCD_Clear(0xbefe);
    LCD_Fill(0,0,240,105,0xd6db);
    LCD_ST7789_DrawPicture(0,105,240,204,gImage_lsd,4);
//    LCD_Fill(0,204,240,320,0xd6db);
//    LCD_Fill(170,0,240,20,0xed68);
//    LCD_Fill(0,20,240,290,0x0348);
//    LCD_Fill(0,0,240,290,BLACK);
//    LCD_ShowString(120,120,"LoRaWAN",YELLOW);

//    LCD_Fill(140,200,200,260,WHITE);
//    LCD_DrawLine(50,220,120,260,RED);
//    LCD_DrawRectangle(20,100,60,140,YELLOW);
//    Draw_Circle(150,80,30,BLUE);
}

void LCD_Init(void)
{
    if(HAL_GPIO_ReadPin(GPIOD,GPIO_PIN_2) == 1 && HAL_GPIO_ReadPin(GPIOD,GPIO_PIN_0) == 1)
    {
        LCD_EN = 1;

    } else
    {
        LCD_EN = 0;
    }

    if(LCD_EN)   //如果有液晶，需要进行初始化
    {
        ST7789V_INIT();
        XPT2046_init();
        MX_TIM15_Init(1500);
        LCD_Test();
        Touch_Adjust();
			  LCD_Clear(0xbefe);
			  LCD_ShowString(30,120,"Start Programming Now",BLUE);
    }
}

/****************************************************************************
* 名    称：void LCD_ST7789_DrawPicture(u16 StartX,u16 StartY,u16 EndX,u16 EndY,const unsigned char *pic)
* 功    能：在指定座标范围显示一副图片
* 入口参数：StartX     行起始座标
*           StartY     列起始座标
*           EndX      行结束座标
*           EndY       列结束座标
            pic             图片头指针
* 出口参数：无
* 说    明：图片取模格式为水平扫描，16位颜色模式
* 调用方法：LCD_ST7789_DrawPicture(0,0,100,100,(u16*)demo);
****************************************************************************/
void LCD_ST7789_DrawPicture(uint16_t StartX,uint16_t StartY,uint16_t Xend,uint16_t Yend,const unsigned char *pic,uint8_t mode)
{
    uint32_t i=0,j=0;
    uint8_t temp = 0;
    uint32_t piont = 0,zheng = 0,yu= 0;


    uint16_t *bitmap = (uint16_t *)pic;
    uint8_t *bitmap1 = (uint8_t *)pic;

    if(mode == 1)
    {

        LCDOpenWindows(StartX,StartY,Xend-StartX,Yend-StartY);

        for(i=StartY; i<Yend; i++)
        {
            for(j=StartX; j<Xend; j++)
            {
                LCD_WriteData_16bit(*bitmap++);
            }
        }
    } else if(mode ==4)
    {
        LCDOpenWindows(StartX,StartY,Xend-StartX,Yend-StartY);
        piont = (Xend-StartX) * (Yend-StartY);
        zheng = piont/8;
        yu = piont%8;

        for(i= 0 ; i<zheng; i++)
        {
            temp = *bitmap1;
            for(uint8_t u=0; u<8; u++)
            {
                if((temp & 0x80) == 0x00)
                {
                    LCD_WriteData_16bit(0xd6db);
                } else
                {
                    LCD_WriteData_16bit(0x0a93);
                }
                temp = temp<<1;
            }
            bitmap1++;
        }

        for(j=0; j<yu; j++)
        {
            temp = *bitmap1;
            for(uint8_t u=0; u<8; u++)
            {
                if((temp & 0x80) == 0x00)
                {
                    LCD_WriteData_16bit(0xd6db);
                } else
                {
                    LCD_WriteData_16bit(0x0a93);
                }
                temp = temp<<1;
            }
        }

    }

}

