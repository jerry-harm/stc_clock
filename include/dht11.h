
#include "delay.h"

#include "8052.h"

#include "display.h"
#define DHT11_DATA P1_4

// 两次读取时间不要短于2s
void Dht11Start()
{

    DHT11_DATA = 0;
    Delay10ms();
    Delay10ms();
    DHT11_DATA = 1;
    Delay10us();
    Delay10us();
    Delay10us();
    Delay10us();
    Delay10us();
    Delay10us();
}

// 依次读取湿度整数，小数，温度整数，小数，以及前四项的和
unsigned char Dht11ReadByte()
{
    unsigned char res = 0;
    for (int i = 0; i < 8; i++)
    {
        // 等待拉高电平

        while (DHT11_DATA == 0)
            ;

        // 等待30us后读取
        Delay10us();
        Delay10us();
        Delay10us();
        res <<= 1;
        res |= DHT11_DATA;
        unsigned int t = 0;
        while (DHT11_DATA == 1)
        {
            // 等拉低
            if (t > 60000)
                break; // 超时退出
            t++;
        }
    }
    return res;
}