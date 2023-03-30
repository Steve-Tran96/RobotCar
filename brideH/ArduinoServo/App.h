#ifndef _APP_H_
#define _APP_H_

#define DIR0   12
#define EN0    13
#define M0     11 //pwm

#define DIR1   2
#define EN1    4
#define M1     3 //pwm

#define DIR2   8
#define EN2    7
#define M2     9 //pwm

#define DIR3   6
#define EN3    5
#define M3     10 //pwm

void InitPS2();
void Uart1SendData(BYTE dat);
void TaskRun(void);
#endif

