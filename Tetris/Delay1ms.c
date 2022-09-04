// 延迟一毫秒
void Delay1ms(unsigned int xms)		//@11.0592MHz
{
	unsigned char i, j;
	
	while (xms--) {
		i = 2;
		j = 199;
		do
		{
			while (--j);
		} while (--i);
	}
}