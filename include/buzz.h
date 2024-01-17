//https://www.gotozhuan.com/cn/tools/tool_pitch_map.php
//以十秒为单位的半周期
#define C3 382
#define Cb3 360
#define D3 341
#define Db3 321
#define E3 303
#define F3 286
#define Fb3 270
#define G3 255
#define Gb3 240
#define A3 227
#define Ab3 214
#define B3 202
#define C4 191
#define Cb4 180
#define D4 170
#define Db4 161
#define E4 152
#define F4 143
#define Fb4 135
#define G4 128
#define Gb4 120
#define A4 113
#define Ab4 107
#define B4 101
#define C5 96
#define Cb5 90
#define D5 85
#define Db5 80
#define E5 76
#define F5 72
#define Fb5 68
#define G5 64
#define Gb5 60
#define A5 56
#define Ab5 53
#define B5 50


#include "8052.h"
#include "lint.h"
#include "delay.h"

#define P_TONE P2_4

void PlayTone(int tone_halfcycle_10us, int duration_1ms){
    // 这里实现播放音符的代码
    
    int t= duration_1ms*100/(tone_halfcycle_10us*2);
    while(--t){
    P_TONE = 1;
    for(int i=0;i<tone_halfcycle_10us;i++)Delay10us();
    P_TONE = 0;
    for(int i=0;i<tone_halfcycle_10us;i++)Delay10us();
    };
    
}

void PlayAlarm(){
    unsigned char table[]={C4,E4,D4,C4,C4,D4,E4,C4};
    for(int i=0;i<8;i++){
        PlayTone(table[i],1000);
    }
}