/* Copyright 2019 SiFive, Inc */
// SPDX-License-Identifier: Apache-2.0 

#include <stdint.h>
#include <metal/machine/platform.h>


#define REG32(p, i)	((p)[(i) >> 2])


void num2bcd(int num, char * array)
{
	char bcd2seg[]={0xc0, 0xf9, 0xa4, 0xb0, 0x99, 0x92, 0x82, 0xf8, 0x80, 0x90, 0xff};
	int i;
	for(i=0;i<8;i++)
		array[i]=bcd2seg[10];
	if(num==0)
		array[0]=bcd2seg[0];
	else
	{
		i=num;
		while(i)
		{
			int r = i % 10;
			i = i / 10;
			*array++ = bcd2seg[r];
		}
	}
}

volatile uint32_t * const gpio0 = (void *)(METAL_SIFIVE_GPIO0_0_BASE_ADDRESS);
volatile uint32_t * const gpio1 = (void *)(METAL_SIFIVE_GPIO0_1_BASE_ADDRESS);
volatile uint32_t * const gpio2 = (void *)(METAL_SIFIVE_GPIO0_2_BASE_ADDRESS);
volatile uint32_t * const gpio3 = (void *)(METAL_SIFIVE_GPIO0_3_BASE_ADDRESS);

#define GPIO_INPUT_VAL METAL_SIFIVE_GPIO0_VALUE
#define GPIO_OUTPUT_VAL METAL_SIFIVE_GPIO0_PORT

unsigned int x=3,count=10, display=10000000;
int main(void)
{
	int state = 0;
	char bcdarray[8];
	
	int turn=0, iter=0;
	num2bcd(display,bcdarray);
	
	REG32(gpio0, GPIO_OUTPUT_VAL) = x;

	int i,j;
	
	while(1)
	{
		for(j=0;j<1000;j++)
		{
			for(i=0,iter=0;i<1000;i++)
			{
			  iter++;
			  REG32(gpio1, GPIO_OUTPUT_VAL)=iter;
			  if(iter == 1000)
			  {
			    REG32(gpio3, GPIO_OUTPUT_VAL)=(~(1 << turn) & 0xFF ) | (bcdarray[turn] << 8);
			    turn++;
			    if(turn==8)	turn=0;
			  }
			}
		}
		
		display++;
		if(display==100000000)
			display=0;
		num2bcd(display,bcdarray);
			
		if(count==21)
		{
			count=0;
			x=1;
		}
		else
		{
			x=x << 1;
			count++;
		}
		
		REG32(gpio0, GPIO_OUTPUT_VAL) = x & REG32(gpio1, GPIO_INPUT_VAL);	
		REG32(gpio2, GPIO_OUTPUT_VAL) = state;
		state = ~state;
	}

	return 0;
}
