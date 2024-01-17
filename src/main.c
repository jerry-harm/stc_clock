#include <stdbool.h>
#include "8052.h"
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

    EA = 1; // 总中断
}

// 初始化端口
void InitPin()
{
    P0 = 0x00;        // 设置P0口为低电平输出
    P1 = 0x00;        // 设置P1口为低电平输出
    P2 = 0x00;        // 设置P2口为低电平输出
    Ds1302Protect(1); // 开启写保护
}

void Timer_interrupt() __interrupt(1)
{
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
}

void Ext_interrupt() __interrupt(0)
{
    if (global_mode < 3)
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
    unsigned char mode = 4;
    while (global_mode == 0)
    {
        if (KeyDown(KEY_1) && mode > 0)
        {
            mode--;
        }
        else if (KeyDown(KEY_3) && mode < 5)
        {
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

        DisplayScan(Ds1302Read(read_map[mode]), Ds1302Read(read_map[mode + 1]), global_0_5Hz << 2, 0);
    }
}

// 校时模块
void AdjustClock()
{
    // 把所有数据全部读出来，然后一个一个改再全部存入芯片
    unsigned char time_all[7]={
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
        else if (KeyDown(KEY_3) && flag < 14)
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

        // 边缘条件检测(出于储存空间考虑，这里就只把溢出会影响计时的秒D7和小时D7限制)
        if (time_all[5] > 59)
        {
            time_all[5] = 0;
        }
        if (time_all[3] > 23)
        {
            time_all[3] = 0;
        }
        if (flag < 12)
            DisplayScan(time_all[flag / 2], time_all[flag / 2 + 1], global_0_5Hz << 2, global_0_5Hz << (4 - flag % 2));
        else
            DisplayScan(time_all[flag / 2 - 1], time_all[flag / 2], global_0_5Hz << 2, global_0_5Hz << (2 - flag % 2));
    }
    Ds1302Protect(0); // close protect
    
    // start write
    Ds1302Write(DS1302_YEAR_WRITE,time_all[0]);
    Ds1302Write(DS1302_MONTH_WRITE,time_all[1]);
    Ds1302Write(DS1302_DATE_WRITE,time_all[2]);
    Ds1302Write(DS1302_HOUR_WRITE,time_all[3]);
    Ds1302Write(DS1302_MINUTE_WRITE,time_all[4]);
    Ds1302Write(DS1302_SECOND_WRITE,time_all[5]);
    Ds1302Write(DS1302_WEEK_WRITE,time_all[6]);

    Ds1302Protect(1); // open protect
}

// 测温模块
void Temperature(){
    unsigned char tem_h=0,tem_l=0;
    unsigned char hum_h=0,hum_l=0;
    unsigned char mode=0;
    while (global_mode==2)
    {
        if(KeyDown(KEY_2)) mode=!mode;
        if(global_0_5Hz){
            unsigned char check;
            Dht11Start();//这样就能实现两秒运行一次
            tem_h=Dht11ReadByte();
            tem_l=Dht11ReadByte();
            hum_h=Dht11ReadByte();
            hum_l=Dht11ReadByte();
            check=Dht11ReadByte();

            if(check!=tem_h+tem_l+hum_h+hum_l){
                //读错了
                tem_h=0xff;
                tem_l=0xff;
                hum_h=0xff;
                hum_l=0xff;
            }
        }
        if(mode){
            DisplayScan(tem_h,tem_l,0x4,0);
        }else{
            DisplayScan(hum_h,hum_l,0x4,0);
        }
        
    }
    
}

// 主循环
int main()
{

    InitPin();        // 初始化端口
    Interrupt_Init(); // 初始化中断

    // main cycle
    while (1)
    {
        Ds1302Protect(1); // 开启写保护
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