#include <stdbool.h>
#include "lint.h"
#include "8052.h"
#include "compiler.h"
#include "buzz.h"
#include "ds1302.h"
#include "keys.h"
#include "display.h"
#include "dht11.h"

unsigned int global_t = 0;
unsigned char global_0_5Hz = 0;
unsigned int global_mode = 0;

void Interrupt_Init(void) // 1毫秒@11.0592MHz
{
    TMOD &= 0xF0; // 设置定时器模式
    TL0 = 0xCD;   // 设置定时初始值
    TH0 = 0xD4;   // 设置定时初始值
    TR0 = 1;      // 启动定时器
    TF0 = 0;      // 清除TF0标志
    ET0 = 1;      // T0中断

    IE0 = 0; // 清除外部IE0标志
    IT0 = 0; // 外部中断0设置为低电平触发
    EX0 = 1; // 使能外部中断
    INT0 = 1;

    EA = 1; // 总中断
}

void Timer_interrupt() __interrupt(1)
{
    TL0 = 0xCD; // 设置定时初始值
    TH0 = 0xD4; // 设置定时初始值
    if (global_t < 1000)
        global_t++;
    else
    {
        global_t = 0;
        global_0_5Hz = !global_0_5Hz;
    }
    TL0 = 0xCD; // 设置定时初始值
    TH0 = 0xD4; // 设置定时初始值
    TF0 = 0;    // 清除TF0标志
    LED_0 = global_0_5Hz;
}

void Ext_interrupt() __interrupt(0)
{
    if (global_mode < 2)
        global_mode++;
    else
        global_mode = 0;
}

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
        if (KeyDown(KEY_1))
        {
            if (mode > 0)
                mode--;
            else
                mode = 7;
        }
        else if (KeyDown(KEY_3))
        {
            if (mode < 7)
                mode++;
            else
                mode = 0;
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
        if (KeyDown(KEY_1) && flag > 0)
        {
            flag--;
        }
        else if (KeyDown(KEY_3) && flag < 7)
        {
            flag++;
        }
        else if (KeyDown(KEY_0))
        {
            time_all[flag]++;
        }
        else if (KeyDown(KEY_2))
        {
            time_all[flag]--;
        }

        // 边缘条件检测,处理进位

        // year

        // month

        // day

        // hour

        // minute

        // second

        // week


        if (flag < 12)
        {
            DisplayScan(time_all[flag / 2], time_all[flag / 2 + 1], global_0_5Hz);
        }
        else
        {
            DisplayScan(time_all[flag / 2 - 1], time_all[flag / 2], global_0_5Hz);
        }
    }
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

// 测温模块
void Temperature()
{
    unsigned char tem_h = 0, tem_l = 0;
    unsigned char hum_h = 0, hum_l = 0;
    unsigned char mode = 0;
    while (global_mode == 2)
    {
        if (KeyDown(KEY_2))
            mode = !mode;
        if (global_0_5Hz)
        {
            unsigned char check;
            Dht11Start(); // 这样就能实现两秒运行一次
            tem_h = Dht11ReadByte();
            tem_l = Dht11ReadByte();
            hum_h = Dht11ReadByte();
            hum_l = Dht11ReadByte();
            check = Dht11ReadByte();

            if (check != tem_h + tem_l + hum_h + hum_l)
            {
                // 读错了
                tem_h = 0xff;
                tem_l = 0xff;
                hum_h = 0xff;
                hum_l = 0xff;
            }
        }
        if (mode)
        {
            unsigned char h, l;
            h = (tem_h / 0x0f);
            l = ((tem_h & 0x0f) << 1) + (tem_l & 0x0f);
            if (tem_l / 0x0f)
                h += 0xC0;
            DisplayScan(h, l, 0x2);
        }
        else
        {
            // hum_l will be 0
            DisplayScan(0x00, hum_h, 0);
        }
    }
}

void InitPin()
{
    KEY_0 = 1;
    KEY_1 = 1;
    KEY_2 = 1;
    KEY_3 = 1;
}

// 主循环
void main()
{
    InitPin();

    Interrupt_Init(); // 初始化中断
    Ds1302Init();
    // TestDisplay();
    // main cycle

    while (1)
    {
        Ds1302Protect(1); // 开启写保护
                          //     // modes 0:clock 1:adjust_clock 2:temperature
        switch (global_mode)
        {

        case 0:
            Clock();
            break;
        case 1:
            AdjustClock();
            break;
        // case 2:
        //     Temperature();
        //     break;
        default:
            global_mode = 0;
            break;
        }
    }
}