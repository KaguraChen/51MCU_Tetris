#include <REGX52.H>

void Timer2_Init(void)		
{
	T2MOD = 0;	
	T2CON = 0;	
	TL2 = 0x66;	
	TH2 = 0xFC;	
	ET2=1;
	EA=1;
	PT2=0;
	RCAP2L  = 0x66;	   
	RCAP2H = 0xFC;	
   TR2 = 1;		
}

//void Timer2_Routine() interrupt 5
//{
//	static unsigned int Count = 0;
//	TF2 = 0;
//	++Count;
//	if (Count >= 500) {
//		Count = 0;
//		
//	}
//}
