#include "8052.h"
#include "delay.h"

#define KEY_0 P2_0
#define KEY_1 P2_1
#define KEY_2 P2_2
#define KEY_3 P2_3

/*
1 3
0 2
*/

bool KeyDown(unsigned char key)
{
    if (key == 1)
        return 0;
    else
    {
        Delay10ms();
        if (key == 1)
        {
            key = 1;
            return 0;
        }
        else
        {
            key = 1;
            return 1;
        }
    }
}
