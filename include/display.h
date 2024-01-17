// 高到低是E,D,DP,C,G,B,F,A,用的共阴数码管
// 0    00101000
// 1    11101011
// 2    00110010
// 3    10100010
// 4    11100001
// 5    10100100
// 6    00100100
// 7    11101010
// 8    00100000
// 9    10100000
// C    00111100
// DP    11011111


#include "8052.h"
#include "ds1302.h"

#define ZERO 0x28
#define ONE 0xEB
#define TWO 0x32
#define THREE 0xA2
#define FOUR 0xE1
#define FIVE 0xA4
#define SIX 0x24
#define SEVEN 0xEA
#define EIGHT 0x20
#define NINE 0xA0
#define C_DISPLAY 0x3C
#define DP 0x20
#define BLANK 0xff

#define P_DIG P0
#define P_DIG1 P1_0
#define P_DIG2 P1_1
#define P_DIG3 P1_2
#define P_DIG4 P1_3

#define LED_0 P2_7
#define LED_1 P2_6
#define LED_2 P2_5

void DisplayNumber(unsigned char num)
{
    switch (num)
    {
    case 0:
        P_DIG = ZERO;
        break;
    case 1:
        P_DIG = ONE;
        break;
    case 2:
        P_DIG = TWO;
        break;
    case 3:
        P_DIG = THREE;
        break;
    case 4:
        P_DIG = FOUR;
        break;
    case 5:
        P_DIG = FIVE;
        break;
    case 6:
        P_DIG = SIX;
        break;
    case 7:
        P_DIG = SEVEN;
        break;
    case 8:
        P_DIG = EIGHT;
        break;
    case 9:
        P_DIG = NINE;
        break;
    case 10:
        P_DIG = C_DISPLAY;
        break;
    case 11:
        P_DIG = BLANK;
        break;
    }
}

void DisplayScan(unsigned char high_hex, unsigned char low_hex, unsigned char dot, unsigned char blink)
{
    for (int i = 500; i > 0; --i)
    {
        P_DIG1 = 1;
        DisplayNumber((blink >> 3) % 2 ? 11 : (high_hex >> 4) - DP * ((dot >> 3) % 2));
        P_DIG1 = 0;
        P_DIG2 = 1;
        DisplayNumber((blink >> 2) % 2 ? 11 : (high_hex & 0x0F) - DP * ((dot >> 2) % 2));
        P_DIG2 = 0;
        P_DIG3 = 1;
        DisplayNumber((blink >> 1) % 2 ? 11 : ((low_hex >> 4) - DP * ((dot >> 1) % 2)));
        P_DIG3 = 0;
        P_DIG4 = 1;
        DisplayNumber((blink) % 2 ? 11 : ((low_hex & 0x0F) - DP * (dot % 2)));
        P_DIG4 = 0;
    }
}