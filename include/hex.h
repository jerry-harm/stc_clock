
unsigned char HexClock(unsigned char num,unsigned char max){
    unsigned char low = num%0x10;
    unsigned char high = num/0x10;
    if(low > 9){
        high++;
        low = 0;
    }
    if(high*10+low > max){
        high = 0;
        low = 0;
    }
    return high*0x10 + low;
}
