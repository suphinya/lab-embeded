#include<Wire.h>
#define I2C_SDA 4   // I2C channel 1
#define I2C_SCL 5
#define I2C_addr 0x6F  // RTC Address

const char* day_week[] = {"Monday","Tuesday","Wednesday","Thursday","Friday","Saturday","Sunday"};

void setup() 
{
  Wire.begin(I2C_SDA , I2C_SCL); // setting i2c channel 1
  Serial.begin (115200);
}

void loop() // main
{
  set_date_time(4,24,2,2022,16,24,31) ; // call function set_date_time
  show_date();
  
  delay(5000);
}

// ================================ read value ===============================================

// read_date_time : function for read date and time from RTC
byte * read_date_time() 
{
  static byte valdata[7];
  
  Wire.beginTransmission(I2C_addr);  // RTC-MCP79411 address
  Wire.write(0); 
  Wire.endTransmission();
  Wire.requestFrom(I2C_addr,7); // request data 7 byte from device
  
  for( int i=0 ; i <= 6 ; i++){
    valdata[i] = Wire.read();
  }
  return valdata;
}

void show_date(){
  byte * all_data;
  all_data = read_date_time() ;

  Serial.print("The day is : ");
  Serial.print(day_week[all_data[3]-1]); 
  Serial.print(" " + String(all_data[4],HEX));
  Serial.print("/" + String(all_data[5],HEX));
  Serial.print("/20" + String(all_data[6],HEX));
  Serial.println(" Time : "+String(all_data[2],HEX)+":"+String(all_data[1],HEX)+":"+String(all_data[0],HEX));
}

// ================================ set value =================================================

// convert DEC to HEX or BIN
byte con_to_bi(int dec){
  // second digit
  byte second_b = (dec/10);
  // first digit
  byte first_b = dec%10;
  
  byte bi_val = second_b*10000 + first_b; // second digit skip 4 bit for first digit
  return bi_val ; 
}

void writeTo(byte address, byte value) {
  Wire.beginTransmission(I2C_addr);
  Wire.write(address);               //send register address
  Wire.write(con_to_bi(value));      //send value to write
  Wire.endTransmission();
}

// set data : function for setting date and time
void set_date_time(int day,int date,int month,int year,int hour,int minute,int second ) 
{
  //Wire.beginTransmission(I2C_addr);  // RTC-MCP79411 address
  
  writeTo(0x00,second);
  writeTo(0x01,minute);
  writeTo(0x02,hour);
  writeTo(0x03,day);
  writeTo(0x04,date);
  writeTo(0x05,month);
  writeTo(0x06,year%100);
  
  //Wire.endTransmission();
}
