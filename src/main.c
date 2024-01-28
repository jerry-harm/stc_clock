#include <stdbool.h>
#include "8052.h"
#include "8051.h"
#include "compiler.h"
#include "buzz.h"
#include "ds1302.h"
#include "keys.h"
#include "display.h"
#include "dht11.h"
#include "hex.h"

unsigned int global_t = 0;
unsigned char global_0_5Hz = 0;
unsigned char global_mode = 0;
unsigned int global_time = 0;

// key0:up key2:down, key1:left, key3:right

// 时钟显示模块
void Clock()
{
    // clock,default show h:m
    // ymdhmsw
    // 012345
    // and blink
    // alarm when
    PlayAlarm();
    unsigned char mode = 4;
    while (global_mode == 0)
    {
        if (Key1Up())
        {
            if (mode > 0)
                mode--;
        }
        else if (Key3Up())
        {
            if (mode < 6)
                mode++;
        }

        unsigned char read_map[7] = {
            DS1302_YEAR_READ,
            DS1302_MONTH_READ,
            DS1302_DATE_READ,
            DS1302_HOUR_READ,
            DS1302_MINUTE_READ,
            DS1302_SECOND_READ,
            DS1302_WEEK_READ};

        unsigned char h = Ds1302Read(read_map[mode]);
        // Delay10ms();
        unsigned char l = Ds1302Read(read_map[mode + 1]);
        // unsigned char l = 0xff;
        // unsigned char h = 0xff;
        DisplayScan(h, l, global_0_5Hz);
    }
}

// 校时模块
void AdjustClock()
{
    // 把所有数据全部读出来，然后一个一个改再全部存入芯片
    unsigned char time_all[7] = {
        Ds1302Read(DS1302_YEAR_READ),
        Ds1302Read(DS1302_MONTH_READ),
        Ds1302Read(DS1302_DATE_READ),
        Ds1302Read(DS1302_HOUR_READ),
        Ds1302Read(DS1302_MINUTE_READ),
        Ds1302Read(DS1302_SECOND_READ),
        Ds1302Read(DS1302_WEEK_READ)};
    // flag用于标记屏幕上的位数以供更改
    unsigned char flag = 0;

    while (global_mode == 1)
    {
        if (Key1Up() && flag > 0)
        {
            flag--;
        }
        else if (Key3Up() && flag < 6)
        {
            flag++;
        }
        else if (Key0Up())
        {
            time_all[flag]++;
        }
        else if (Key2Up())
        {
            time_all[flag]--;
        }

        // 边缘条件检测,处理进位

        // year

        time_all[0] = HexClock(time_all[0], 99);

        // month
        time_all[1] = HexClock(time_all[1], 12);
        // day
        time_all[2] = HexClock(time_all[2], 31);
        // hour
        time_all[3] = HexClock(time_all[3], 23);
        // minute
        time_all[4] = HexClock(time_all[4], 59);
        // second
        time_all[5] = HexClock(time_all[5], 59);
        // week
        time_all[6] = HexClock(time_all[6], 6);

        if (flag < 6)
        {
            DisplayScan(global_t < 300 ? 0xff : time_all[flag], time_all[flag + 1], global_0_5Hz);
        }
        else
        {
            DisplayScan(time_all[flag - 1], global_t < 300 ? 0xff : time_all[flag], global_0_5Hz);
        }
    }
    if (flag = 6)
    {
        Ds1302Protect(0); // close protect

        // start write
        Ds1302Write(DS1302_YEAR_WRITE, time_all[0]);
        Ds1302Write(DS1302_MONTH_WRITE, time_all[1]);
        Ds1302Write(DS1302_DATE_WRITE, time_all[2]);
        Ds1302Write(DS1302_HOUR_WRITE, time_all[3]);
        Ds1302Write(DS1302_MINUTE_WRITE, time_all[4]);
        Ds1302Write(DS1302_SECOND_WRITE, time_all[5]);
        Ds1302Write(DS1302_WEEK_WRITE, time_all[6]);

        Ds1302Protect(1); // open protect
    }
}

// 测温模块
void Temperature()
{
    unsigned char tem_h = 0, tem_l = 0;
    unsigned char hum_h = 0, hum_l = 0;
    unsigned char check;
    unsigned char mode = 0;
    while (global_mode == 2)
    {
        if (Key2Up())
            mode = !mode;
        if ( Key3Up())
        {
            EA = 0;

            Dht11Start();
            if (DHT11_DATA == 0)
            {
                while (DHT11_DATA == 0)
                    ;
                while (DHT11_DATA == 1)
                    ;

                hum_h = Dht11ReadByte();

                hum_l = Dht11ReadByte();

                tem_h = Dht11ReadByte();

                tem_l = Dht11ReadByte();

                check = Dht11ReadByte();

                if (check != tem_h + tem_l + hum_h + hum_l)
                {
                    // 读错了
                    LED_2 = 0;
                }
                else
                {
                    LED_2 = 1;
                }
            }
            DHT11_DATA=1;
            EA = 1;
        }
        // DisplayScan(hum_h, hum_l, 1);
        if (mode)
        {
            // unsigned char h, l;
            // h = (tem_h / 0x10);
            // l = ((tem_h % 0x10) << 1) + (tem_l % 0x10);
            // if (tem_l / 0x10)
            //     h += 0xB0;
            DisplayScan(tem_h, tem_l, 1);
        }
        else
        {
            // hum_l will be 0
            DisplayScan(hum_h, hum_l, 0);
        }
    }
}

void InitPin()
{
    DHT11_DATA = 1;
    KEY_0 = 1;
    KEY_1 = 1;
    KEY_2 = 1;
    KEY_3 = 1;
}

void Interrupt_Init(void) // 1毫秒@11.0592MHz
{
    TMOD &= 0xF0; // 设置定时器模式
    TL0 = 0xCD;   // 设置定时初始值
    TH0 = 0xD4;   // 设置定时初始值
    TR0 = 1;      // 启动定时器
    TF0 = 0;      // 清除TF0标志
    ET0 = 1;      // T0中断

    IE0 = 0; // 清除外部IE0标志
    IT0 = 0; // 外部中断0设置为上下沿都触发
    EX0 = 1; // 使能外部中断
    INT0 = 1;

    EA = 1; // 总中断
}

void Timer_interrupt() __interrupt(1)
{
    if (global_t < 1000)
        global_t++;
    else
    {
        global_t = 0;
        global_time++;
        global_0_5Hz = !global_0_5Hz;
    }
    TL0 = 0xCD; // 设置定时初始值
    TH0 = 0xD4; // 设置定时初始值
    TF0 = 0;    // 清除TF0标志
    LED_0 = global_0_5Hz;
}

void Ext_interrupt() __interrupt(0)
{
    LED_1 = 0;
    for (int i = 5; i > 0; i--)
        Delay10ms();
    if (INT0 == 1)
    {
        LED_1 = 1;
        if (global_mode < 3)
            global_mode++;

        else
            global_mode = 0;
    }
}

// 主循环
void main()
{
    InitPin();

    Interrupt_Init(); // 初始化中断
    // Ds1302Init();
    // TestDisplay();
    // main cycle

    while (1)
    {
        LED_0 = 1;
        LED_1 = 1;
        LED_2 = 1;
        // modes 0:clock 1:adjust_clock 2:temperature
        switch (global_mode)
        {
        case 0:
            Clock();
            break;
        case 1:
            AdjustClock();
            break;
        case 2:
            Temperature();
            break;
        default:
            global_mode = 0;
            break;
        }
    }
}