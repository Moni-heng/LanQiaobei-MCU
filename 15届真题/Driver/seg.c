#include<seg.h>
code unsigned char Seg_Table[] = 
{
0xc0, //0
0xf9, //1
0xa4, //2
0xb0, //3
0x99, //4
0x92, //5
0x82, //6
0xf8, //7
0x80, //8
0x90, //9,
0xff, //mie_10
0x8c, //P-11
0x89, //H-12
0xc7, //L-13
0x8e, //F-14,
0xbf,//--15
0x88//a--16
};
void seg_disp(unsigned char wela,unsigned char dula,bit point){
	P0=0XFF;
	P2=(P2&0X1F)|0XE0;
	P2&=0X1F;
	
	P0=0x01<<wela;
	P2=(P2&0X1F)|0Xc0;
	P2&=0X1F;	
	
	P0=Seg_Table[dula];
	if(point) P3&=0X7F;
	P2=(P2&0X1F)|0XE0;
	P2&=0X1F;	
}