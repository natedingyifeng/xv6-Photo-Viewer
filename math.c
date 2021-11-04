#include "math.h"

static unsigned int g_seed = 0;

int fastrand() {
    ++g_seed;
    g_seed = (214013*g_seed+2531011);
    return (g_seed>>16)&0x7FFF;
}

#define PI 3.1415926536

float abs(float x) 
{
	if (x > 0) return x;
	return -x;
}

float pow(float a, int b)
{
	float r = a;
	if (b > 0) {
		while (--b)
			r *= a;
	}
	else if (b < 0) {
		while (++b)
			r *= a;
		r = 1.0 / r;
	}
	else r = 0;
	return r;
}

float sqrt(float number) {
	float new_guess;
	float last_guess;

	if (number < 0) {
		return -1;
	}

	new_guess = 1;
	do {
		last_guess = new_guess;
		new_guess = (last_guess + number / last_guess) / 2;
	} while (new_guess != last_guess);

	return new_guess;
}

float cos(float x)
{
    float fl = 1;
    if (x > 2 * PI || x < -2 * PI) x -= (int)(x / (2 * PI)) * 2 * PI;
    if (x > PI) x -= 2 * PI;
    if (x < -PI) x += 2 * PI;
    if (x > PI / 2)
    {
        x -= PI;
        fl *= -1;
    }
    if (x < -PI / 2)
    {
        x += PI;
        fl *= -1;
    }
    if (x > PI / 4) return fl * sin(PI / 2 - x);
    else return fl * (1 - pow(x, 2) / 2 + pow(x, 4) / 24 - pow(x, 6) / 720 + pow(x, 8) / 40320);//̩�չ�ʽ
}

float sin(float x)
{
    float fl = 1;
    if (x > 2 * PI || x < -2 * PI) x -= (int)(x / (2 * PI)) * 2 * PI;
    if (x > PI) x -= 2 * PI;
    if (x < -PI) x += 2 * PI;
    if (x > PI / 2)
    {
        x -= PI;
        fl *= -1;
    }
    if (x < -PI / 2)
    {
        x += PI;
        fl *= -1;
    }
    if (x < 0)
    {
        x *= -1;
        fl *= -1;
    }
    if (x > PI / 4) return fl * cos(PI / 2 - x);
    else return fl * (x - pow(x, 3) / 6 + pow(x, 5) / 120 - pow(x, 7) / 5040 + pow(x, 9) / 362880);//̩�չ�ʽ
}
