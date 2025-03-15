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
0x90, //9
0xc1, //U-10
0X8C, //P-11
0xc8, //n-12
0xff //Ãð-13
};
void seg_disp(unsigned char wela,unsigned char dula,unsigned char point){
			P0=0XFF;
			P2=(P2&0X1F)|0XE0;
			P2&=0X1F;
			
			P0=0x01<<wela;
			P2=(P2&0X1F)|0Xc0;
			P2&=0X1F;		

			P0=Seg_Table[dula];
			if(point) P0&=0X7F;
			P2=(P2&0X1F)|0XE0;
			P2&=0X1F;	
}