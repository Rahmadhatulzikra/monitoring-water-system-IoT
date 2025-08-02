#include <SoftwareSerial.h>
#include <FirebaseESP8266.h>
#include <ESP8266WiFi.h>

// Harus diisi
#define FIREBASE_HOST "dteksi-limbah-cair-default-rtdb.firebaseio.com//"
#define FIREBASE_AUTH "Yx3vgnRVeXnxjEQirf9NG57eOGo2Y28UJkN6Naq4"
#define WIFI_SSID "ZIKRA"
#define WIFI_PASSWORD "1sampai8"  

SoftwareSerial mcu(12,13); // RX TX // D6 D7
// mendeklarasikan objek data dari FirebaseESP8266
FirebaseData firebaseData;

unsigned long prevMillis = 0;
const long interval = 1000;

String arrData[2];

void setup() {
  mcu.begin(9600);
  Serial.begin(115200);
   
  // Koneksi ke Wifi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("connecting");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
}

void loop(){
  unsigned long currentMillis = millis();
  if (currentMillis - prevMillis >= interval)
  {
    prevMillis = currentMillis;

    String data = "";

    while(mcu.available()>0)
    {
      data += char(mcu.read());
    }
    data.trim();

    if (data != "")
    {
      int index = 0;
      for(int i=0; i<= data.length(); i++)
      {
        char delimiter = '#' ;
        if (data[i] != delimiter)
          arrData[index] += data[i];
        else
          index++;
      }
      
      if (index == 1)
      { 
        mcu.println("Ph  : " + arrData[0]);
        mcu.println("TBD : " + arrData[1]);
      }

      int pH = arrData[0].toInt();
      int tBD = arrData[1].toInt();

      if (Firebase.setString(firebaseData, "/Hasil_Pembacaan/ph", pH)){
        Serial.println("Ph terkirim");
      } 
      else {
        Serial.println("Ph tidak terkirim");
        Serial.println("Karena: " + firebaseData.errorReason());
      } 
    
      if (Firebase.setString(firebaseData, "/Hasil_Pembacaan/tbd", tBD)){
        Serial.println("TBD terkirim");
        Serial.println();
      } 
      else {
        Serial.println("TBD tidak terkirim");
        Serial.println("Karena: " + firebaseData.errorReason());
      }
      delay(50);
      
      arrData[0] = " ";
      arrData[1] = " ";
    }
   mcu.println("Ya");
   delay(1000); 
  }
}
