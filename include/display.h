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
// DP   11011111
// -    11110111

#ifndef DISPLAY
#define DISPLAY

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
#define MINUS 0xf7

#define P_DIG P0
#define P_DIG1 P1_0
#define P_DIG2 P1_1
#define P_DIG3 P1_2
#define P_DIG4 P1_3

#define LED_0 P2_7
#define LED_1 P2_6
#define LED_2 P2_5

void DisplayNumber(unsigned char num,unsigned char dot)
{
    switch (num)
    {
    case 0:
        P_DIG = ZERO-dot*DP;
        break;
    case 1:
        P_DIG = ONE-dot*DP;
        break;
    case 2:
        P_DIG = TWO-dot*DP;
        break;
    case 3:
        P_DIG = THREE-dot*DP;
        break;
    case 4:
        P_DIG = FOUR-dot*DP;
        break;
    case 5:
        P_DIG = FIVE-dot*DP;
        break;
    case 6:
        P_DIG = SIX-dot*DP;
        break;
    case 7:
        P_DIG = SEVEN-dot*DP;
        break;
    case 8:
        P_DIG = EIGHT-dot*DP;
        break;
    case 9:
        P_DIG = NINE-dot*DP;
        break;
    case 10:
        P_DIG = C_DISPLAY;
        break;
    case 11:
        P_DIG = MINUS;
        break;
    case 0xf:
        P_DIG = BLANK;
        break; 
    default:
        P_DIG = C_DISPLAY;
        break;
    }
}

void DisplayScan(unsigned char high_hex, unsigned char low_hex, unsigned char dot)
{
    for (int i = 10; i > 0; --i)
    {
        P_DIG1 = 0;
        P_DIG2 = 0;
        P_DIG3 = 0;
        P_DIG4 = 0;

        P_DIG1 = 1;
        DisplayNumber(high_hex / 16,0);
        Delay1ms();
        P_DIG1 = 0;
        DisplayNumber(0xf,0);
        
        P_DIG2 = 1;
        DisplayNumber(high_hex % 16,dot);
        Delay1ms();
        P_DIG2 = 0;
        DisplayNumber(0xf,0);
        
        P_DIG3 = 1;
        DisplayNumber(low_hex / 16,0);
        Delay1ms();
        P_DIG3 = 0;
        DisplayNumber(0xf,0);
        
        P_DIG4 = 1;
        DisplayNumber(low_hex % 16,0);
        Delay1ms();
        P_DIG4 = 0;
        DisplayNumber(0xf,0);
        
    }
}

// void TestDisplay(){
//     for(int i=0;i<10000;i++){
//         DisplayScan(0x11, 0x11, 0);
//     }
// }

#endif