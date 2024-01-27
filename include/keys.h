#include "8052.h"
#include "delay.h"
#include "display.h"

#define KEY_0 P2_0
#define KEY_1 P2_1
#define KEY_2 P2_2
#define KEY_3 P2_3

/*
1 3
0 2
*/

bool Key0Up()
{
    if (KEY_0 == 1)
        return 0;
    else
    {
        LED_1 = 0;
        while(KEY_0 == 0);
        Delay10us();
        if (KEY_0 == 1)
        {
            LED_1 = 1;
            KEY_0 = 1;
            return 1;
        }
    }
    return 0;
}

bool Key1Up()
{
    if (KEY_1 == 1)
        return 0;
    else
    {
        LED_1 = 0;
        while(KEY_1 == 0);
        Delay10us();
        if (KEY_1 == 1)
        {
            LED_1 = !LED_1;
            KEY_1 = 1;
            return 1;
        }
    }
    return 0;
}

bool Key2Up()
{
    if (KEY_2 == 1)
        return 0;
    else
    {
        LED_1 = 0;
        while(KEY_2 == 0);
        Delay10us();
        if (KEY_2 == 1)
        {
            LED_1 = 1;
            KEY_2 = 1;
            return 1;
        }
    }
    return 0;
}

bool Key3Up()
{
    if (KEY_3 == 1)
        return 0;
    else
    {
        LED_1 = 0;
        while(KEY_3 == 0);
        Delay10us();
        if (KEY_3 == 1)
        {
            LED_1 = 1;
            KEY_3 = 1;
            return 1;
        }
    }
    return 0;
}