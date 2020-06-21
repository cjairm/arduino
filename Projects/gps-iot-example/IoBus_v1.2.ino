#include <SoftwareSerial.h>
#include <ESP8266WiFi.h>
SoftwareSerial myGPS(4, 5);

int updates;
int failed_updates;
int position_helper;
int string_place = 0;
int delay_to_send = 0;
String nmea[15];
String gps_data[4];
const int time_to_wait = 10000;

unsigned long delay_time_previous = 0;
unsigned long heart_beat_previous = 0;
const int heart_beat = 13;
int heart_beat_state = LOW; 

const char* ssid     = "**DEVICE_NAME**";
const char* password = "**DEVICE_PASS**";
const char* host     = "api.hadaelectronics.com";

void setup() {
//  Serial.begin(57600);
  
  myGPS.begin(9600);

  pinMode(heart_beat, OUTPUT);
  digitalWrite(heart_beat, LOW);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }

  WiFi.localIP();

  digitalWrite(heart_beat, LOW);
  delay(1000);
  digitalWrite(heart_beat, HIGH);
  delay(2000);
  digitalWrite(heart_beat, LOW);
  delay(1000);
  digitalWrite(heart_beat, HIGH);
  delay(2000);
}

void loop() {
  WiFiClient client;
  const int http_port = 80;

  if(delay_to_send >= time_to_wait){  
    if(runGPS() == HIGH){
      if(gps_data[0] == "A"){
        if (!client.connect(host, http_port)) {
          return;
        }
    
        String url = "/api/places/";
        url += String(gps_data[1]);//lat
        url += "/";
        url += String(gps_data[2]);//lng
        url += "/";
        url += String(gps_data[3]);//speed
        url += "/";
        url += String("eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJfaWQiOiI1ZDBmYTcyM2U4MjVmMDFkZDQ2MjhkZTgiLCJpYXQiOjE1NjEzMDY5MTV9.aqsH5p_rn5kSXWauw7ntYxQ1u4aK1NvcACOeaP678nQ");
  
        client.print(String("GET ") + url + " HTTP/1.1\r\n" +
                     "Host: " + host + "\r\n" + 
                     "Connection: close\r\n\r\n");
                     
        unsigned long timeout = millis();
        while (client.available() == 0) {
          if (millis() - timeout > 1000) {
            delay_to_send = (time_to_wait/2);
            client.stop();
            return;
          }
        }
  
        if(client.available()){
//          String line = client.readStringUntil('\r');
//          Serial.print(line);
          digitalWrite(heart_beat, LOW);
          delay(100);
          digitalWrite(heart_beat, HIGH);
          delay(100);
          digitalWrite(heart_beat, LOW);
          delay(100);
          digitalWrite(heart_beat, HIGH);
          delay(100);
          digitalWrite(heart_beat, LOW);
          delay(100);
          digitalWrite(heart_beat, HIGH);
          delay(100);
          digitalWrite(heart_beat, LOW);
          delay(100);
          digitalWrite(heart_beat, HIGH);
          delay(100);
          delay_to_send = 0;
        }
      }
    }
  }

  hearBeat(500);
  delay_to_send++;
  delay(1);
}

int runGPS() {
  int state_sent = LOW;
  myGPS.flush();
  
  while (myGPS.available() > 0) {
    myGPS.read();
  }
  
  if (myGPS.find("$GPRMC,")) {
    String tempMsg = myGPS.readStringUntil('\n');
    for (int i = 0; i < tempMsg.length(); i++) {
      if (tempMsg.substring(i, i + 1) == ",") {
        nmea[position_helper] = tempMsg.substring(string_place, i);
        string_place = i + 1;
        position_helper++;
      }
      
      if (i == tempMsg.length() - 1) {
        nmea[position_helper] = tempMsg.substring(string_place, i);
      }
    }
    updates++;
    nmea[2] = ConvertLat();
    nmea[4] = ConvertLng();

    gps_data[0] = nmea[1]; //Status
    gps_data[1] = nmea[2]; //Lat
    gps_data[2] = nmea[4]; //Long
    gps_data[3] = nmea[6]; //Speed

    state_sent = HIGH;    
  } else {
    failed_updates++;
  }
  string_place = 0;
  position_helper = 0;

  return state_sent;
}

String ConvertLat() {
  String posneg = "";
  if (nmea[3] == "S") {
    posneg = "-";
  }
  String latfirst;
  float latsecond;
  for (int i = 0; i < nmea[2].length(); i++) {
    if (nmea[2].substring(i, i + 1) == ".") {
      latfirst = nmea[2].substring(0, i - 2);
      latsecond = nmea[2].substring(i - 2).toFloat();
    }
  }
  latsecond = latsecond / 60;
  String CalcLat = "";

  char charVal[9];
  dtostrf(latsecond, 4, 6, charVal);
  for (int i = 0; i < sizeof(charVal); i++)
  {
    CalcLat += charVal[i];
  }
  latfirst += CalcLat.substring(1);
  latfirst = posneg += latfirst;
  return latfirst;
}

String ConvertLng() {
  String posneg = "";
  if (nmea[5] == "W") {
    posneg = "-";
  }

  String lngfirst;
  float lngsecond;
  for (int i = 0; i < nmea[4].length(); i++) {
    if (nmea[4].substring(i, i + 1) == ".") {
      lngfirst = nmea[4].substring(0, i - 2);
      //Serial.println(lngfirst);
      lngsecond = nmea[4].substring(i - 2).toFloat();
    }
  }
  lngsecond = lngsecond / 60;
  String CalcLng = "";
  char charVal[9];
  dtostrf(lngsecond, 4, 6, charVal);
  for (int i = 0; i < sizeof(charVal); i++)
  {
    CalcLng += charVal[i];
  }
  lngfirst += CalcLng.substring(1);
  lngfirst = posneg += lngfirst;
  return lngfirst;
}

void hearBeat(const long heart_beat_delay) {
  unsigned long heart_beat_current = millis();
  if (heart_beat_current - heart_beat_previous >= heart_beat_delay) {
    heart_beat_previous = heart_beat_current;
    if (heart_beat_state == LOW) {
      heart_beat_state = HIGH;
    } else {
      heart_beat_state = LOW;
    }
    digitalWrite(heart_beat, heart_beat_state);
  }
}
