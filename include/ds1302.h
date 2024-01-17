#ifndef DS1302_H
#define DS1302_H

// https://zhuanlan.zhihu.com/p/674678422
#include "8052.h"

#define DS1302_RST P1_7
#define DS1302_DATA P1_6
#define DS1302_CLK P1_5

// 寄存器操作

#define DS1302_SECOND_WRITE 0x80
#define DS1302_SECOND_READ 0x81
#define DS1302_MINUTE_WRITE 0x82
#define DS1302_MINUTE_READ 0x83
#define DS1302_HOUR_WRITE 0x84
#define DS1302_HOUR_READ 0x85
#define DS1302_DATE_WRITE 0x86
#define DS1302_DATE_READ 0x87
#define DS1302_MONTH_WRITE 0x88
#define DS1302_MONTH_READ 0x89
#define DS1302_WEEK_WRITE 0x8A
#define DS1302_WEEK_READ 0x8B
#define DS1302_YEAR_WRITE 0x8C
#define DS1302_YEAR_READ 0x8D
#define DS1302_CONTROL_WRITE 0x8E
#define DS1302_CONTROL_READ 0x8F

unsigned char Ds1302ReadByte()
{
    unsigned char res = 0x00;
    for (int i = 0; i < 8; i++)
    {
        res = DS1302_DATA << i | res;
        DS1302_CLK = 1;
        DS1302_CLK = 0;
    }
    return res;
}

void Ds1302WriteByte(int d)
{
    for (int i = 0; i < 8; i++)
    {
        if (d % 2)
        {
            DS1302_DATA = 1;
        }
        else
        {
            DS1302_DATA = 0;
        }
        DS1302_CLK = 1;
        DS1302_CLK = 0;
        d = d >> 1;
    }
}

unsigned char Ds1302Read(unsigned char addr)
{
    DS1302_RST = 0;
    DS1302_CLK = 1;
    DS1302_RST = 1;
    Ds1302WriteByte(addr);
    DS1302_RST = 0;
    return Ds1302ReadByte();
}

void Ds1302Write(unsigned char addr, unsigned char data_hex)
{
    DS1302_RST = 0;
    DS1302_CLK = 1;
    DS1302_RST = 1;
    Ds1302WriteByte(addr);
    Ds1302WriteByte(data_hex);
    DS1302_RST = 0;
}

// 1 is open and 0 is close
void Ds1302Protect(unsigned char protect)
{
    if (protect)
    {
        Ds1302Write(DS1302_CONTROL_WRITE, 0x80);
    }
    else
    {
        Ds1302Write(DS1302_CONTROL_WRITE, 0x00);
    }
}

void Ds1302Init()
{
    Ds1302Protect(0);

    Ds1302Write(DS1302_YEAR_WRITE, 0x24);
    Ds1302Write(DS1302_MONTH_WRITE, 0x01);
    Ds1302Write(DS1302_DATE_WRITE, 0x01);
    Ds1302Write(DS1302_WEEK_WRITE, 0x01);
    Ds1302Write(DS1302_HOUR_WRITE, 0x00);
    Ds1302Write(DS1302_MINUTE_WRITE, 0x00);
    Ds1302Write(DS1302_SECOND_WRITE, 0x00);

    Ds1302Protect(1);
}

#endif