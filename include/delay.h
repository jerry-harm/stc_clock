#ifndef DELAY_H
#define DELAY_H

void Delay10us()		//@11.0592MHz
{
	unsigned char i;

	i = 2;
	while (--i);
}

void Delay10ms()		//@11.0592MHz
{
	unsigned char i, j;

	i = 18;
	j = 235;
	do
	{
		while (--j);
	} while (--i);
}

#endif