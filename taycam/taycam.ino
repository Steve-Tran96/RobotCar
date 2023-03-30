/*
        DIY Arduino based RC Transmitter
  by Dejan Nedelkovski, www.HowToMechatronics.com
  Library: TMRh20/RF24, https://github.com/tmrh20/RF24/
*/
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
// Set the LCD address to 0x3f for a 16 chars and 2 line display
LiquidCrystal_I2C lcd(0x3F, 20, 4);

// Define the digital inputs
#define jB1 1  // Joystick button 1
#define jB2 0  // Joystick button 2
#define t1 7   // Toggle switch 1
#define t2 4   // Toggle switch 1
#define b1 8   // Button 1
#define b2 9   // Button 2
#define b3 2   // Button 3
#define b4 3   // Button 4


RF24 radio(5, 6);   // nRF24L01 (CE, CSN)
//const byte address[6] = "00001"; // Address
const byte address [][6] = {"00001", "00002"};  //Setting the two addresses. One for 
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


unsigned long lastReceiveTime = 0;
unsigned long currentTime = 0;

struct Data_Receive {
  uint32_t dimension;
  uint8_t  warning;
  uint16_t speedcar;
};

Data_Receive datarece;
byte pot2temp=0;
int16_t speedtemp=0;


void setup() {
  Serial.begin(9600);
  lcd.begin();
  delay(100);
  lcd.backlight();
  lcd.setCursor(4,0);
  lcd.print("ROBOT CAR");
  lcd.setCursor(0,1);
  lcd.print("S: ");
  lcd.setCursor(0,2);
  lcd.print("P: ");
  lcd.setCursor(0,3);
  lcd.print("V: ");
  
  // Define the radio communication
  radio.begin();
  radio.openWritingPipe(address[0]);
  radio.openReadingPipe(1, address[1]);  
  radio.setAutoAck(false);
  radio.setDataRate(RF24_250KBPS);
  radio.setPALevel(RF24_PA_LOW);
  
  // Activate the Arduino internal pull-up resistors
  pinMode(jB1, INPUT_PULLUP);
  pinMode(jB2, INPUT_PULLUP);
  pinMode(t1, INPUT_PULLUP);
  pinMode(t2, INPUT_PULLUP);
  pinMode(b1, INPUT_PULLUP);
  pinMode(b2, INPUT_PULLUP);
  pinMode(b3, INPUT_PULLUP);
  pinMode(b4, INPUT_PULLUP);
  
  // Set initial default values
  data.j1PotX = 127; // Values from 0 to 255. When Joystick is in resting position, the value is in the middle, or 127. We actually map the pot value from 0 to 1023 to 0 to 255 because that's one BYTE value
  data.j1PotY = 127;
  data.j2PotX = 127;
  data.j2PotY = 127;
  data.j1Button = 1;
  data.j2Button = 1;
  data.pot1 = 1;
  data.pot2 = 1;
  data.tSwitch1 = 1;
  data.tSwitch2 = 1;
  data.button1 = 1;
  data.button2 = 1;
  data.button3 = 1;
  data.button4 = 1;
  datarece.dimension = 0;
  Serial.println("Start...");
  
}


void loop() {
  // Read all analog inputs and map them to one Byte value
  data.j1PotX = map(analogRead(A1), 0, 1023, 0, 255); // Convert the analog read value from 0 to 1023 into a BYTE value from 0 to 255
  data.j1PotY = map(analogRead(A0), 0, 1023, 0, 255);
  data.j2PotX = map(analogRead(A2), 0, 1023, 0, 255);
  data.j2PotY = map(analogRead(A3), 0, 1023, 0, 255);
  data.pot1 = map(analogRead(A7), 0, 1023, 0, 255);
  data.pot2 = map(analogRead(A6), 0, 1023, 0, 255);
  // Read all digital inputs
  data.j1Button = digitalRead(jB1);
  data.j2Button = digitalRead(jB2);
  data.tSwitch2 = digitalRead(t2);
  data.button1 = digitalRead(b1);
  data.button2 = digitalRead(b2);
  data.button3 = digitalRead(b3);
  data.button4 = digitalRead(b4);
  // Send the whole data from the structure to the receiver
  
  radio.write(&data, sizeof(Data_Package)); 

  radio.startListening();   
  currentTime = millis();
  if ( currentTime - lastReceiveTime > 30 ) { // If current time is more then 1 second since we have recived the last data, that means we have lost connection
    if((pot2temp==100 && ((data.pot2*100)/255)<100)||(pot2temp==10 && ((data.pot2*100)/255)<10)
      || (speedtemp==100 && (datarece.speedcar)<100)||(speedtemp==10 && (datarece.speedcar)<10)){
      lcd.clear();
      lcd.setCursor(4,0);
      lcd.print("ROBOT CAR");
      lcd.setCursor(0,1);
      lcd.print("S: ");
      lcd.setCursor(0,2);
      lcd.print("P: ");
      lcd.setCursor(0,3);
      lcd.print("V: ");
    }
    
    lcd.setCursor(3,1);
    lcd.print(datarece.dimension); lcd.print(" cm"); 
      
    lcd.setCursor(3,2);
    lcd.print((data.pot2*100)/255);lcd.print(" %"); 
    pot2temp = (data.pot2*100)/255;  
    
    lcd.setCursor(3,3);
    lcd.print(datarece.speedcar);  lcd.print(" m/s");  
    speedtemp = datarece.speedcar;
    lastReceiveTime = millis(); // At this moment we have received the data 
  }
  
  if(radio.available()) {
    radio.read(&datarece, sizeof(datarece)); // Read the whole data and store it into the 'data' structure
  }
  radio.stopListening();  
}
