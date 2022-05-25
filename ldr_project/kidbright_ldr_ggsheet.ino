#define BLYNK_TEMPLATE_ID "xxxx"
#define BLYNK_DEVICE_NAME "xxxx"
#define BLYNK_AUTH_TOKEN "xxxxxxxxxxxxxxx"

#define BLYNK_FIRMWARE_VERSION        "0.1.0"
#define BLYNK_PRINT Serial
#define APP_DEBUG

#include <WiFi.h>
#include <WiFiClient.h>
#include <HTTPClient.h>
#include <BlynkSimpleEsp32.h>
#include <TridentTD_LineNotify.h>

#define LINE_TOKEN "xxxxxxxxxxxxx"

char auth[] = BLYNK_AUTH_TOKEN; // Token App Blynk
char ssid[] = "xxxxx"; // ชื่อ Wi-Fi
char pass[] = "xxxxx"; // รหัส Wi-Fi

int ldrpin = 36; // ldr pin in kidbright
int R = 18; // led red pin
int G = 19; // led green pin
int B = 23; // led blue pin
int ldr_value = 0;
int swopen = 16 ; // button 1 pin
int swmode = 14 ; // button 2 pin

boolean flag = true ; // for check status


// google app script
/* https://script.google.com/macros/s/AKfycbwka7goyIT7QuhJy3iRg2PnWhl9A4aPDK93aCmGcsfOFZR6-d8/exec */

String GOOGLE_SCRIPT_ID = "xxxxxxxxxxxxxxxxxx"; //--> spreadsheet script ID

// function send data to google sheet
void sendData(String value , String value2) {
   HTTPClient http;
   String url="https://script.google.com/macros/s/"+GOOGLE_SCRIPT_ID+"/exec?md="+value+"&stat="+value2 ;
   Serial.print(url);
   Serial.print("Making a request");
   http.begin(url); //Specify the URL and certificate
   int httpCode = http.GET();  
   http.end();
   Serial.println(": done "+httpCode);
}
 





// blynk button on-off
int status_butt = 0 ;
BLYNK_WRITE(V3){
  if (param.asInt() == 1) {
    status_butt = 1;
  } else {
    status_butt = 0;
  }
}

// blynk button mode
int mode_check = 0;
BLYNK_WRITE(V1){
  if (param.asInt() == 1) {
    mode_check = 1;
  } else {
    mode_check = 0;
  }
}

// blynk slide switch state 
int color = 0;
BLYNK_WRITE(V2){
  if (param.asInt() == 1) {
    color = 1; //red
  } else if (param.asInt() == 2){
    color = 2; //green
  }else if (param.asInt() == 3){
    color = 3; // blue
  }
  else {
    color = 0;  //white
  }
}

void setup()
{
  pinMode(R, OUTPUT);
  pinMode(G, OUTPUT);
  pinMode(B, OUTPUT);
  pinMode(swopen, INPUT_PULLUP);
  pinMode(swmode, INPUT_PULLUP);
  Serial.begin(115200);
  //setting blynk
  Blynk.begin(auth, ssid, pass); 
  //setting line
  LINE.setToken(LINE_TOKEN);
 
  // wifi
  WiFi.begin(ssid,pass); // connect to wifi
  Serial.println("Connecting");
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Ready to go");
}

void loop()
{
  Blynk.run();
  ldr_chart();
  check_all();
  
  // button switch 1 on-off status
  int sw1_state = digitalRead(swopen);
  if (sw1_state == 0 ){
    if (status_butt == 0 ){
      status_butt = 1;
      Blynk.virtualWrite(V3, status_butt);  // sync with blynk app
    }
    else {
      status_butt = 0;
      Blynk.virtualWrite(V3, status_butt);
    }
  }

  // button switch 2 mode status
  int sw2_state = digitalRead(swmode);
  if (sw2_state == 0 ){
    if (mode_check == 0 ){
      mode_check = 1;
      Blynk.virtualWrite(V1, mode_check);
    }
    else {
      mode_check = 0;
      Blynk.virtualWrite(V1, mode_check);
    }
  }
}

void check_all(){
  if (status_butt == 0){
    digitalWrite(R, LOW);
    digitalWrite(G, LOW);
    digitalWrite(B, LOW);
    while(flag==true){
      LINE.notify(" มีการปิดไฟ LED ");
      flag = false ;
      sendData("switch","off"); //********************************
      //sendData("switch","off"); //********************************
    }
  }
  else{
    if(mode_check == 1 and status_butt == 1){
      write_led_auto(); // ldr use mode
    }
    else if (mode_check == 0 and status_butt == 1){
      write_led_manual(); // on - off only
    }
  }
}

void write_led_manual(){   // write led in manual mode
  
  if (color == 0) {
    digitalWrite(R, HIGH);
    digitalWrite(G, HIGH);
    digitalWrite(B, HIGH);
  } else if (color == 1) {
    digitalWrite(R, HIGH);
    digitalWrite(G, LOW);
    digitalWrite(B, LOW);
  } else if (color == 2 ) {
    digitalWrite(R, LOW);
    digitalWrite(G, HIGH);
    digitalWrite(B, LOW);
  }else {
    digitalWrite(R, LOW);
    digitalWrite(G, LOW);
    digitalWrite(B, HIGH);
  }
  while (flag==false){
      LINE.notify(" มีการเปิดไฟ LED ");
      flag = true ;
      sendData("manual","on");    //********************************
      //sendData("manual","on");    //********************************
  }
  delay(500);
}

void write_led_auto(){   // write led in auto mode
  ldr_value = analogRead(ldrpin);
  
  if (ldr_value >= 800 ){
    // กำหนด LED สีขาว
    digitalWrite(R, HIGH);
    digitalWrite(G, HIGH);
    digitalWrite(B, HIGH);
    delay(1500);
    while (flag==false){
      LINE.notify(" มีการเปิดไฟ LED "); 
      flag = true ;
      sendData("auto","on");  //********************************
      //sendData("auto","on");  //********************************
    }
    
  }
   else{
    // กำหนด LED ไม่มีสี
    digitalWrite(R, LOW);
    digitalWrite(G, LOW);
    digitalWrite(B, LOW);
    while(flag==true){
      LINE.notify(" มีการปิดไฟ LED ");
      flag = false ;
      sendData("auto","off"); //*******************
      //sendData("auto","off"); //*******************
    }
    delay(1500);
  }
}

// chart in blynk app
int sensorValue = 0;      
void ldr_chart() {
  // read the analog in value:
  sensorValue = analogRead(ldrpin); 
 
  Serial.print("sensor = ");     
  Serial.println(sensorValue);
  delay(500); 

  Blynk.virtualWrite(V0, sensorValue);
}
