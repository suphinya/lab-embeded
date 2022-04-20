#include<Wire.h>
#define I2C_SDA 4
#define I2C_SCL 5
#define I2C_addr 0x4D
float Temp_value ; 
float value ;

void setup() 
{
  Wire.begin(I2C_SDA , I2C_SCL); // setting i2c channel 1
  Serial.begin (115200);
}

void loop() // main
{
  value = read_tempval() ; // call function read_tempval

  // print value temp degree celsius
  Serial.print("Temperature:");
  Serial.print(value); 
  Serial.println(" °C");
  delay(5000);
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
 
  Serial.print("Temp_data[1] MSB :");
  Serial.println(Temp_data[1],HEX);
  Serial.print("Temp_data[0] LSB :");
  Serial.println(Temp_data[0],HEX);

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
