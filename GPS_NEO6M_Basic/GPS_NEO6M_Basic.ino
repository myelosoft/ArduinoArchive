/*
 * Project: GPS NEO6M Demo v1.0
 * Hardware: Arduino Nano and NEO6M GPS Module
 * Not categorized with any class, Electrical Engineering, Chulalongkorn University
 * Rewrite code by Dr.Natthakorn Kasamsumran (NKR) February 23, 2026
 * Serial monitor output example : 
    -------------------------
    Time: 16:50:43
    Status: A
    Latitude: 13.736237
    Hemisphere: N
    Longitude: 100.532153
    Hemisphere: E
    Speed: 0.043
    Track Angle: 
    Date: 23/02/2026
    ------------------------- 
 */

#include "SoftwareSerial.h"

SoftwareSerial GPSModule(3, 2); // TX, RX

int updates;
int failedUpdates;
int pos;
int stringplace = 0;

String timeUp;
String nmea[15];
String labels[12] {"Time: ", "Status: ", "Latitude: ", "Hemisphere: ", "Longitude: ", "Hemisphere: ", "Speed: ", "Track Angle: ", "Date: "};

bool addOneDay = false; 

void setup() {
  Serial.begin(9600);
  GPSModule.begin(9600);
}

void loop() {
  Serial.flush();
  GPSModule.flush();
  while (GPSModule.available() > 0)
  {
    GPSModule.read();
  }
  
  if (GPSModule.find("$GPRMC,")) {
    String tempMsg = GPSModule.readStringUntil('\n');
    for (int i = 0; i < tempMsg.length(); i++) {
      if (tempMsg.substring(i, i + 1) == ",") {
        nmea[pos] = tempMsg.substring(stringplace, i);
        stringplace = i + 1;
        pos++;
      }
      if (i == tempMsg.length() - 1) {
        nmea[pos] = tempMsg.substring(stringplace, i);
      }
    }
    updates++;
    
    nmea[0] = ConvertTime(); 
    nmea[2] = ConvertLat();
    nmea[4] = ConvertLng();
    nmea[8] = ConvertDate(); 
    
    for (int i = 0; i < 9; i++) {
      Serial.print(labels[i]);
      Serial.print(nmea[i]);
      Serial.println("");
    }
    Serial.println("-------------------------");
  }
  else {
    failedUpdates++;
  }
  stringplace = 0;
  pos = 0;
}

String ConvertTime() {
  addOneDay = false; // รีเซ็ตค่าทุกครั้งที่เริ่มคำนวณใหม่
  if (nmea[0].length() < 6) return nmea[0]; 
  
  int h = nmea[0].substring(0, 2).toInt();
  String m = nmea[0].substring(2, 4);
  String s = nmea[0].substring(4, 6);
  
  h = h + 7; // Thailand Time Zone
  if (h >= 24) {
    h = h - 24;
    addOneDay = true;
  }
  
  String hStr = String(h);
  if (h < 10) hStr = "0" + hStr;
  
  return hStr + ":" + m + ":" + s;
}


String ConvertDate() {
  if (nmea[8].length() < 6) return nmea[8]; 
  
  int dd = nmea[8].substring(0, 2).toInt();
  int mm = nmea[8].substring(2, 4).toInt();
  int yy = nmea[8].substring(4, 6).toInt();
  int yearFull = 2000 + yy;

  if (addOneDay) {
    dd++;
    int daysInMonth = 31;
    
    if (mm == 4 || mm == 6 || mm == 9 || mm == 11) {
      daysInMonth = 30;
    } else if (mm == 2) {
      bool isLeap = ((yearFull % 4 == 0 && yearFull % 100 != 0) || (yearFull % 400 == 0));
      daysInMonth = isLeap ? 29 : 28;
    }

    if (dd > daysInMonth) {
      dd = 1;
      mm++;
      if (mm > 12) {
        mm = 1;
        yearFull++;
      }
    }
  }


  String ddStr = String(dd);
  if (dd < 10) ddStr = "0" + ddStr;
  
  String mmStr = String(mm);
  if (mm < 10) mmStr = "0" + mmStr;
  
  return ddStr + "/" + mmStr + "/" + String(yearFull);
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