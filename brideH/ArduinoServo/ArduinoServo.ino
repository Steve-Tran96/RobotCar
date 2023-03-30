#include "include.h"
#include <avr/wdt.h>      //  ���Ź�ͷ�ļ�
void setup()              //  ִֻ��һ��
{ 
   pinMode(DIR0,OUTPUT);  //  ���õ�����ƿ�Ϊ���ģʽ
   pinMode(EN0,OUTPUT);  
   pinMode(DIR1,OUTPUT); 
   pinMode(EN1,OUTPUT); 

   #if 1
   pinMode(DIR2,OUTPUT);  //���õ�����ƿ�Ϊ���ģʽ
   pinMode(EN2,OUTPUT);  
   pinMode(DIR3,OUTPUT); 
   pinMode(EN3,OUTPUT);  
   #endif
   TCCR1B=((1<<CS10));   // ���ö�ʱ��0��2��ʱ�ӷ�Ƶϵ��
   TCCR2B=((1<<CS20));
   Serial.begin(9600);   //  ʹ�ܴ���0 ���貨����Ϊ9600
   wdt_enable(WDTO_1S);  //
   InitPS2();            //  ��ʼ��PS2������
   analogWrite(M0, 0);   //   �����ĸ�����ĳ�ʼ״̬Ϊֹͣ
   analogWrite(M1, 0);
   #if 1
   analogWrite(M2, 0);
   analogWrite(M3, 0);
   #endif
   digitalWrite(EN0, 0);
   digitalWrite(EN1, 0);
   digitalWrite(EN2, 0);
   digitalWrite(EN3, 0);
}
void loop()             // ѭ��ִ��
{
   TaskRun();          //  ���ܺ���
   wdt_reset();        //  ι������
}

