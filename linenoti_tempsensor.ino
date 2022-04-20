#include<Wire.h>
#include <TridentTD_LineNotify.h>
#include<WiFi.h>
const char * ssid = ""; // wifi name
const char * pass = "";    // wifi password

#define I2C_SDA 4
#define I2C_SCL 5
#define I2C_addr 0x4D
#define LINE_TOKEN  "" // go gen new

float Temp_value ; 
float value ;

void setup() 
{
  // temp sensor
  Wire.begin(I2C_SDA , I2C_SCL); // setting i2c channel 1
  Serial.begin (115200);
  
  // wifi
  WiFi.begin(ssid,pass); // connect to wifi
  Serial.println("Connecting");
  while(WiFi.status() != WL_CONNECTED) {
    delay(5000);
    Serial.print(".");
  } 
  Serial.println("Connected success"); //connect success

  //setting line
  LINE.setToken(LINE_TOKEN);
}

void loop() {
  // value temp sensor
  value = read_tempval() ; // call function read_tempval
  Serial.println("อุณหภูมิ"+String(value)+" องศา");
  
  if ( value >= 33 )
  {
    LINE.notify("อุณหภูมิขณะนี้ "+String(value)+" องศา !!! ");
  }
  delay(3000);
}

float read_tempval() // function for read temp LM73
{
  byte Temp_data[2]; 
  Wire.beginTransmission(I2C_addr);  // LM73 address
  Wire.write(0);
  Wire.endTransmission();
  Wire.requestFrom(0x4D,2);
  Temp_data[1] = Wire.read();  //MSB
  Temp_data[0] = Wire.read();  //LSB
  Temp_value = 0 ;

  // convert bit to celsius
  if (Temp_data[0] & 0b10000000) Temp_value += 1;       
  if (Temp_data[0] & 0b01000000) Temp_value += 0.5;
  if (Temp_data[0] & 0b00100000) Temp_value += 0.25;
  if (Temp_data[0] & 0b00010000) Temp_value += 0.125;
  if (Temp_data[0] & 0b00001000) Temp_value += 0.0625;
  if (Temp_data[0] & 0b00000100) Temp_value += 0.03125;

  if (Temp_data[1] & 0b10000000) Temp_value *= -1;   // sign bit
  if (Temp_data[1] & 0b01000000) Temp_value += 128;
  if (Temp_data[1] & 0b00100000) Temp_value += 64;
  if (Temp_data[1] & 0b00010000) Temp_value += 32;
  if (Temp_data[1] & 0b00001000) Temp_value += 16;
  if (Temp_data[1] & 0b00000100) Temp_value += 8;
  if (Temp_data[1] & 0b00000010) Temp_value += 4;
  if (Temp_data[1] & 0b00000001) Temp_value += 2;
  return Temp_value;
}
