#include <ESP8266WiFi.h>
#include <TridentTD_LineNotify.h>

// WiFi Connected
#define SSID        "kan220618_2.4G"      // WIFI name
#define PASSWORD    "kim95399"        // WIFI password

// Line Notify TOKEN 
#define LINE_TOKEN  "jUgw41wBOWEWF70TEURhXKnYMOiRycX4z5tcsA3WuYl"   // LINE TOKEN (Bright's) (ESP8266wMQ2 (NEW))

// Set IFTTT Webhooks event name and key
#define IFTTT_Key "l4h1eLmi34ErJ5rHby-oRHDv0iPsIGWvhXGuUAEa9gr" // IFTTT KEY(Del's)
#define IFTTT_Event "MQ-2" 
WiFiClient client;

int IFTTT_Value1 = 0;

// Define LED LIGHT
#define LEDpin1 5   // Green  // D1
#define LEDpin2 4   // Yellow // D2
#define LEDpin3 14  // Red    // D5

// Define BUZZER
#define buzzer 13   // buzzer // D7

// Define ANALOG A0
int smokeA0 = A0;

void setup() {
  // I/O PIN
  pinMode(smokeA0, INPUT);
  pinMode(LEDpin1, OUTPUT);
  pinMode(LEDpin2, OUTPUT);
  pinMode(LEDpin3, OUTPUT);
  pinMode(buzzer, OUTPUT);

  Serial.begin(115200);

  // Line With WiFi Connect
  Serial.println(LINE.getVersion());
  WiFi.begin(SSID, PASSWORD);
  Serial.printf("WiFi connecting to %s\n",  SSID);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(400);
  }
  Serial.printf("\nWiFi connected\nIP : ");
  Serial.println(WiFi.localIP());

  // กำหนด Line Token
  LINE.setToken(LINE_TOKEN);
}

// function IFTTT
void send_webhook(){// function Send Webook to IFTTT
  // construct the JSON payload
  String jsonString = "";
  jsonString += "{\"value1\":\"";
  jsonString += IFTTT_Value1;
  jsonString += "\"}";
  int jsonLength = jsonString.length();  
  String lenString = String(jsonLength);
  // connect to the Maker event server
  client.connect("maker.ifttt.com", 80);
  // construct the POST request
  String postString = "";
  postString += "POST /trigger/";
  postString += IFTTT_Event;
  postString += "/with/key/";
  postString += IFTTT_Key;
  postString += " HTTP/1.1\r\n";
  postString += "Host: maker.ifttt.com\r\n";
  postString += "Content-Type: application/json\r\n";
  postString += "Content-Length: ";
  postString += lenString + "\r\n";
  postString += "\r\n";
  postString += jsonString; // combine post request and JSON
  client.print(postString);
  delay(500);
  client.stop();
}

void loop() {
  int analogSensor = analogRead(A0); // เก็บค่าควันและแก๊สไว้ในตัวแปร
  Serial.print("Pin A0 ="); // แสดงข้อความ Pin A0 = ...
  Serial.println(analogSensor); // แสดงค่าของควันและแก๊ส

  // IFTTT
  IFTTT_Value1 = analogSensor;
  // Send Webook to IFTTT
  send_webhook();
  delay(1000);

  if (analogSensor > 280) {        // Red Condition3
    digitalWrite(LEDpin1, LOW);       // Green off
    digitalWrite(LEDpin2, LOW);       // Yellow off
    digitalWrite(LEDpin3, HIGH);      // RED ON

    tone(buzzer, 1000); // BUZZER ON

    // ส่งข้อความไปยังผู้ใช้งาน
    LINE.notify("ควันอยู่ในระดับอันตรายมาก!!"); // ส่งข้อความผ่านไลน์
  }
  else if (analogSensor >= 250) { // Yellow Condition2
    digitalWrite(LEDpin1, LOW);     // Green off
    digitalWrite(LEDpin2, HIGH);    // YELLOW ON
    digitalWrite(LEDpin3, LOW);     // Red off

    tone(buzzer, 1000); // BUZZER ON

    // ส่งข้อความไปยังผู้ใช้งาน
    LINE.notify("ควันอยู่ในระดับเริ่มอันตราย"); // ส่งข้อความผ่านไลน์
  }
  else if (analogSensor >= 200) { // Green Condition1
    digitalWrite(LEDpin1, HIGH);    // GREEN ON
    digitalWrite(LEDpin2, LOW);     // Yellow off
    digitalWrite(LEDpin3, LOW);     // Red off

    noTone(buzzer); // Buzzer off

    // ส่งข้อความไปยังผู้ใช้งาน
    LINE.notify("ควันอยู่ในระดับไม่อันตรายมาก"); // ส่งข้อความผ่านไลน์
  }
  else {
    digitalWrite(LEDpin1, LOW); /** ALL LED OFF**/
    digitalWrite(LEDpin2, LOW);
    digitalWrite(LEDpin3, LOW);

    noTone(buzzer); // Buzzer off
  }
}
