#include "main.h"
/*
�������ڣ� T = 0.001
ת�������� J = 1
Ħ��ϵ���� f = 1
���ٶ�/���أ� Gf(a) = 1/(s+1)
ǰ�����ڣ�Gf(s)=s+1
�����out = in'+in = (in-last_in)/T + in;
*/
float last_in =0;
float T = 0.001;
float forwardfeed(float in)
{
	float out;
	out = (in -  last_in)/T +in;
	last_in =in;
	return out;
}