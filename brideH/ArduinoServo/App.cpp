#include "include.h"
#include <string.h>
bool R_Running = FALSE;
bool L_Running = FALSE;
PS2X ps2X;                                //
void InitPS2()
{
  ps2X.config_gamepad(A0, A2, A1, A3);   // (clk, cmd, att, dat)  ����ң�ؽ�����clk, cmd, att, dat��Ӧ��IO�� 
}

void ps2Handle() {
  static uint32_t Timer;
  unsigned char PSS_RX_VALUE;
  unsigned char PSS_RY_VALUE;
  unsigned char PSS_LX_VALUE;
  unsigned char PSS_LY_VALUE;
  if (Timer > millis())
    return;

  ps2X.read_gamepad();                  //��ȡң�������µļ�ֵ        
  
  PSS_RX_VALUE = ps2X.Analog(PSS_RX);  //  ҡ��x�������ֵ
  PSS_RY_VALUE = ps2X.Analog(PSS_RY);  // ҡ��Y�������ֵ
  
  if (PSS_RX_VALUE == 255 && PSS_RY_VALUE == 255)    //��Чֵ
    return;
  #if 1
  if (PSS_RX_VALUE == 128 && PSS_RY_VALUE == 255){  // debug
  	
         ps2X.read_gamepad();   
		 PSS_RX_VALUE = ps2X.Analog(PSS_RX);
         PSS_RY_VALUE = ps2X.Analog(PSS_RY);
		 if(PSS_RX_VALUE == 128 && PSS_RY_VALUE == 255){
		 }
		 else return;   
	 Serial.println("Error!");
  }
	  
  #endif


// ҡ����ԭ�����ֵΪPSS_RX_VALUE = 128 ,PSS_RY_VALUE = 128

// PSS_RX_VALUE �ķ�ΧΪ 0 - 255    PSS_RY_VALUE �ķ�ΧΪ 0 - 255

  if (PSS_RX_VALUE < 120) {  // x�Ḻ����
    Serial.print("x= "); 
    Serial.println(PSS_RX_VALUE);
    digitalWrite(EN0, 1);  // x������0�Ŀ��ƽӿ�   00 ֹͣ  01 ��ת 10 ��ת
    digitalWrite(DIR0, 0);   //
    analogWrite(M0,  PSS_RX_VALUE*2+10);   // ������������pwm�ź�
    
	digitalWrite(EN2, 1);  // x������0�Ŀ��ƽӿ�	 00 ֹͣ  01 ��ת 10 ��ת
	digitalWrite(DIR2, 0);	 //
	analogWrite(M2,  PSS_RX_VALUE*2+10);	// ������������pwm�ź�

    digitalWrite(EN1, 1);   
    digitalWrite(DIR1, 0);
	analogWrite(M1,  PSS_RX_VALUE*2+10);

	digitalWrite(EN3, 1);   
    digitalWrite(DIR3, 0);
	analogWrite(M3,  PSS_RX_VALUE*2+10);
    
  }
  else if (PSS_RX_VALUE > 144)  //  X��������
  {
    Serial.print("x= ");
    Serial.println(PSS_RX_VALUE);
    digitalWrite(EN0, 1);
    digitalWrite(DIR0, 1);
    analogWrite(M0, 510 - PSS_RX_VALUE*2+10);

    digitalWrite(EN2, 1);
    digitalWrite(DIR2, 1);
    analogWrite(M2, 510 - PSS_RX_VALUE*2+10);

    digitalWrite(EN1, 1);
    digitalWrite(DIR1, 1);
    analogWrite(M1, 510 - PSS_RX_VALUE*2+10);

	digitalWrite(EN3, 1);
    digitalWrite(DIR3, 1);
    analogWrite(M3, 510 - PSS_RX_VALUE*2+10);
  }
  else {                               //  x����ҡ���޶���
        if (PSS_RY_VALUE < 120)        //  Y ������x����ͬ��
      {
		Serial.print("y= ");
		Serial.println(PSS_RY_VALUE);
		digitalWrite(EN0, 1);
		digitalWrite(DIR0, 1);
		analogWrite(M0, PSS_RY_VALUE*2+10);  //

        digitalWrite(EN2, 1);
		digitalWrite(DIR2, 1);
		analogWrite(M2, PSS_RY_VALUE*2+10);  //

		digitalWrite(EN1, 1);
		digitalWrite(DIR1, 0);
		analogWrite(M1, PSS_RY_VALUE*2+10);

        digitalWrite(EN3, 1);
		digitalWrite(DIR3, 0);
		analogWrite(M3, PSS_RY_VALUE*2+10);

		
    }
  else if (PSS_RY_VALUE > 138) {
		Serial.print("y= ");
		Serial.println(PSS_RY_VALUE);
		digitalWrite(EN0, 1);
		digitalWrite(DIR0, 0);
		analogWrite( M0, 510 - PSS_RY_VALUE*2+10);

        digitalWrite(EN2, 1);
		digitalWrite(DIR2, 0);
		analogWrite( M2, 510 - PSS_RY_VALUE*2+10);
	
		digitalWrite(EN1, 1);
		digitalWrite(DIR1, 1);
		analogWrite(M1, 510 - PSS_RY_VALUE*2+10); //510 

		digitalWrite(EN3, 1);
		digitalWrite(DIR3, 1);
		analogWrite(M3, 510 - PSS_RY_VALUE*2+10); //510 
  }
  else {
	    analogWrite(M0, 0);
	    analogWrite(M1, 0);
		analogWrite(M2, 0);
	    analogWrite(M3, 0);
		
	    digitalWrite(EN0, 0);
		digitalWrite(EN1, 0);
		digitalWrite(EN2, 0);
		digitalWrite(EN3, 0);
       }
  	}
  Timer = millis() + 50;       //ÿ50msִ��һ��

 }
void TaskRun(void)
{
  ps2Handle();
}


