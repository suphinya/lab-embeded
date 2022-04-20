#include<WiFi.h>
#include<PubSubClient.h>
#include<Wire.h>
const char * ssid = ""; // wifi name
const char * pass = "";    // wifi password

#define MQTT_SERVER   "mqtt3.thingspeak.com"
#define MQTT_PORT     1883
#define MQTT_USERNAME ""
#define MQTT_PASSWORD ""
#define MQTT_NAME ""
//const char * topic = "" ;

int analogpin = 36; 
int analog_value = 0;

#define I2C_SDA 4
#define I2C_SCL 5
#define I2C_addr 0x4D
float Temp_value ; 
float value ;

WiFiClient client;
PubSubClient mqtt(client);

void setup()
{
   // temp sensor
  Wire.begin(I2C_SDA , I2C_SCL); // setting i2c channel 1
  Serial.begin(115200);
  // wifi
  WiFi.begin(ssid,pass); // connect to wifi
  Serial.println("Connecting");
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  //connect success
  Serial.println("");
  Serial.println("Connected success");

  mqtt.setServer(MQTT_SERVER, MQTT_PORT);
}

void loop()
{
  value = read_tempval() ;
  analog_value = analogRead(analogpin);
  //Serial.println(analog_value);
  
  if (mqtt.connect(MQTT_NAME, MQTT_USERNAME, MQTT_PASSWORD)) {
    Serial.println("connected");
    String topic = "channels/1679709/publish" ;
    String da = "field1="+String(value)+"&field2="+String(analog_value) ;
    mqtt.publish(topic.c_str(),da.c_str());
  } else {
    Serial.println("failed");
  }
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
