#include "main.h"
/*
采样周期： T = 0.001
转动惯量： J = 1
摩擦系数： f = 1
角速度/力矩： Gf(a) = 1/(s+1)
前馈环节：Gf(s)=s+1
输出：out = in'+in = (in-last_in)/T + in;
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