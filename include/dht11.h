
#include "delay.h"

#include "8052.h"

#define DHT11_DATA P1_4

// 两次读取时间不要短于2s
void Dht11Start()
{
    DHT11_DATA = 0;
    Delay10ms();
    Delay10ms();
    DHT11_DATA = 1;
    while (DHT11_DATA)
        ;
    while (!DHT11_DATA)
        ;
    while (DHT11_DATA)
        ;
}

// 依次读取温度整数，小数，湿度整数，小数，以及前四项的和
unsigned char Dht11ReadByte()
{
    unsigned char res = 0;
    for (int i = 0; i < 8; i++)
    {
        // 等待拉高电平
        while (!DHT11_DATA)
            ;
        // 等待50us后读取
        for (int i = 0; i < 5; i++)
            Delay10us();
        res <<= 1;
        res |= DHT11_DATA;
    }
    return res;
}