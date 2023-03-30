/*
    DIY Arduino based RC Transmitter Project
              == Receiver Code ==
              //**************** PIN  *******************
              // NRF24L01 : 6, 10, 11, 12, 13
              // Motor 1: 2 - RPWM1, 4 - LPWM1
              // Moror 2: 3 - RPWM2, 5 - LPWM2
              // encoder: 38, 45        
*/
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

#define RPWM1  2
#define LPWM1  4
#define RPWM2  3
#define LPWM2  5

#define Step1  13
#define En1    15
#define Dir1   17

#define Step2  11
#define En2    21
#define Dir2   23

#define RL1    9
#define RL2    7

/****NPN*** -> 0V*/
#define ss1    A0
#define ss2    A2
#define ss3    A4
/*****PNP****-> 5V*/
#define ss4    A6
#define ss5    A8
#define ss6    A10

#define kd 26 // cm/100 (pulse)

struct Data_Send {
  uint32_t dimension;
  uint8_t  warning;
  uint16_t speedcar;
};
Data_Send datasend;

uint8_t  sensor;
uint32_t pulse;
static uint32_t Timer;
float speedDC=0;

int  xAxis, yAxis;
int valuepwm =0;

/*Pin NRF24L01:  6, 10, 11, 12, 13*/
RF24 radio(6, 10);   // nRF24L01 (CE, CSN)
//const byte address[6] = "00001";
const byte address [][6] = {"00001", "00002"};  //Setting the two addresses. One for 
unsigned long lastReceiveTime = 0;
unsigned long currentTime = 0;
// Max size of this struct is 32 bytes - NRF24L01 buffer limit
struct Data_Package {
  byte j1PotX;
  byte j1PotY;
  byte j1Button;
  byte j2PotX;
  byte j2PotY;
  byte j2Button;
  byte pot1;
  byte pot2;
  byte tSwitch1;
  byte tSwitch2;
  byte button1;
  byte button2;
  byte button3;
  byte button4;
};
Data_Package data; //Create a variable with the above structure

void setup() {
  Serial.begin(9600);
  radio.begin();

  pinMode(RPWM1, OUTPUT);
  pinMode(LPWM1, OUTPUT);
  pinMode(RPWM1, OUTPUT);
  pinMode(LPWM1, OUTPUT);

  pinMode(En1, OUTPUT);
  pinMode(Step1, OUTPUT);
  pinMode(Dir1, OUTPUT);

  pinMode(En2, OUTPUT);
  pinMode(Step2, OUTPUT);
  pinMode(Dir2, OUTPUT);

  pinMode(RL1, OUTPUT);
  pinMode(RL2, OUTPUT);

  pinMode(ss1, INPUT);
  pinMode(ss2, INPUT);
  pinMode(ss3, INPUT);
  pinMode(ss4, INPUT);
  pinMode(ss5, INPUT);
  pinMode(ss6, INPUT);


  digitalWrite(En1,HIGH);
  digitalWrite(En2,HIGH);

  /*****SPEED = 0******/
  analogWrite(RPWM1, 0);   
  analogWrite(LPWM1, 0);
  digitalWrite(RPWM2, 0);
  digitalWrite(LPWM2, 0);

  /*********Khoi tao Encoder**********/
  
  cli();
  TCCR5A = 0;
  TCCR5B = 0;
  TIMSK5 = 0;

  /**********Khoi tao NRF24L01*********/
  radio.openWritingPipe(address[1]);     //Setting the address at which we will send the data
  radio.openReadingPipe(1, address[0]);
  radio.setAutoAck(false);
  radio.setDataRate(RF24_250KBPS);
  radio.setPALevel(RF24_PA_LOW);
  radio.startListening(); //  Set the module as receiver
  resetData();
  
  /* CTC mode 4, External Clock source from pin T1, clock on falling edge.*/
  TCCR5B |= (1 << WGM52) | (1 << CS52) | (1 << CS51); 
  //OCR5A = 100;
  //TIMSK5 |= (1 << OCIE5A);
  sei();
  datasend.dimension=0;
}


void loop() {
  // Counter for encoder
  if (Timer < millis())
  {
    pulse = TCNT5;
    TCNT5=0;
    Timer = millis() + 200;
  }

  Serial.print("Pulse: "); Serial.println(pulse);
  
 if(radio.available()) {
    radio.read(&data, sizeof(Data_Package)); // Read the whole data and store it into the 'data' structure
    lastReceiveTime = millis(); // At this moment we have received the data
  }
  xAxis = data.j1PotX;
  yAxis = data.j1PotY;
  speedDC = (float)data.pot2/255;

  /******Control Relay*******/
  digitalWrite(RL1, data.tSwitch1);
  digitalWrite(RL1, data.tSwitch2);
  
  // Check whether we keep receving data, or we have a connection between the two modules
  currentTime = millis();
  if ( currentTime - lastReceiveTime > 1000 ) { // If current time is more then 1 second since we have recived the last data, that means we have lost connection
    resetData(); // If connection is lost, reset the data. It prevents unwanted behavior, for example if a drone has a throttle up and we lose connection, it can keep flying unless we reset the values  
  }


  /*************Send data to NRF*******/
  radio.stopListening(); 
  
  datasend.dimension = datasend.dimension + ((pulse/100)*kd); // each duty cycle( 100 pulse) = 260mm(kd).
  Serial.print("s: "); Serial.println(datasend.dimension);
  /**  
   *   ns: so xung xuat ra trong 1s
   *   Ne: So vong encoder
   *   Ts: thoi gian lay mau (1s)
  */ 
  datasend.speedcar = (TCNT5*1000)/(100*200); // v = (ns*1000)/(Ne*Ts) 
  
  if(digitalRead(ss1 == 0) && digitalRead(ss2 == 0) && digitalRead(ss3 == 0) && digitalRead(ss4 == 0)&& digitalRead(ss5 == 0)&& digitalRead(ss6 == 0)){
    datasend.warning = 1;
  }
  radio.write(&datasend, sizeof(datasend));

  // user's code send x to NRF radio.write(&x, sizeof(x));

  radio.startListening();  
  /*--------------------------------*/

/*** Print the data in the Serial Monitor ***/
//  Serial.print("j1PotX: ");
//  Serial.println(xAxis);
//  Serial.print("j1PotY: ");
//  Serial.println(yAxis);

  if (yAxis < 123) { 
    // Set Motor A backward
    valuepwm= map(yAxis, 123, 0, 0, 255);
    //Serial.println(valuepwm*speedDC);
    pwmMotor('b', valuepwm*speedDC);
  
  }
  else if (yAxis > 144) { //140 - 144
    valuepwm= map(yAxis, 144, 255, 0, 255);
    pwmMotor('f', valuepwm*speedDC);
    //Serial.println(valuepwm*speedDC);
  }
  else if (xAxis < 125) { //110- 120
    valuepwm= map(xAxis, 123, 0, 0, 255);
    pwmMotor('r', valuepwm*speedDC);
  }
  else if (xAxis > 140) { //140- 144
    valuepwm = map(xAxis, 138, 255, 0, 255);
    pwmMotor('l', valuepwm*speedDC);
  }
  else {
    analogWrite(RPWM1, 0);
    analogWrite(LPWM1, 0);
    digitalWrite(RPWM2, 0);
    digitalWrite(LPWM2, 0);
  }
}

/*********** PWM MT1 (%) ********************/
void pwmMotor(char direct, uint8_t value){
  uint8_t analogValue;
  analogValue = value;

  /*** TIEN ***/
  if(direct == 'f'){
    analogWrite(RPWM1, analogValue);
    analogWrite(LPWM1, 0);

    analogWrite(RPWM2, 0);
    analogWrite(LPWM2, analogValue);
  }
  /*** LUI ***/
  else if(direct == 'b'){
    analogWrite(RPWM1, 0);
    analogWrite(LPWM1, analogValue);

    analogWrite(RPWM2, analogValue);
    analogWrite(LPWM2, 0);
  }
  /*** TRAI ***/
  else if(direct == 'l'){
    analogWrite(RPWM1, (analogValue));
    analogWrite(LPWM1, 0);

    analogWrite(RPWM2, (analogValue/4));
    analogWrite(LPWM2, 0);
  }
  /*** PHAI ***/
  else if(direct == 'r'){
    analogWrite(RPWM1, (analogValue/4));
    analogWrite(LPWM1, 0);

    analogWrite(RPWM2, 0);
    analogWrite(LPWM2, analogValue);
  }
}

/*************Dieu Khien Dong Co Buoc***************/ 
void controlStep1(char dir, uint16_t circle){
  digitalWrite(En1, LOW);
  if(dir == 'r'){
    digitalWrite(Dir1, HIGH);
  }else{
    digitalWrite(Dir1, LOW);
  }
  for(uint16_t i=0; i< circle*200; i++){
    digitalWrite(Step1, HIGH);
    delayMicroseconds(800); 
    digitalWrite(Step1, LOW);
    delayMicroseconds(800); 
  }
}

void controlStep2(char dir, uint16_t circle){
  digitalWrite(En2, LOW);
  if(dir == 'r'){
    digitalWrite(Dir2, HIGH);
  }else{
    digitalWrite(Dir2, LOW);
  }
  for(uint16_t i=0; i< circle*200; i++){
    digitalWrite(Step2, HIGH);
    delayMicroseconds(800); 
    digitalWrite(Step2, LOW);
    delayMicroseconds(800); 
  }
}

void resetData() {
  data.j1PotX = 127;
  data.j1PotY = 127;
  data.j2PotX = 127;
  data.j2PotY = 127;
  data.j1Button = 1;
  data.j2Button = 1;
  data.pot1 = 0;
  data.pot2 = 0;
  data.tSwitch1 = 0;
  data.tSwitch2 = 0;
  data.button1 = 0;
  data.button2 = 0;
  data.button3 = 0;
  data.button4 = 0;
}
