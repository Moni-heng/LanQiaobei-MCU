#include<led.h>
void led_disp(unsigned char addr,unsigned char enable){
	static unsigned char temp=0x00;
	static unsigned char temp_old=0x00;
	if(enable)
		temp|=0x01<<addr;
	else
		temp&=~(0x01<<addr);
	if(temp!=temp_old){
	P0=~temp;
	P2=(P2&0X1F)|0X80;
	P2&=0X1F;
	temp_old=temp;
	}
}
