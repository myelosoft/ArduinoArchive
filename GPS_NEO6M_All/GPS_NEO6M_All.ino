/*
 * Project: GPS NEO6M Demo v1.0
 * Hardware: Arduino Nano and NEO6M GPS Module
 * Not categorized with any class, Electrical Engineering, Chulalongkorn University
 * Rewrite code by Dr.Natthakorn Kasamsumran (NKR) February 23, 2026
 * Serial monitor output example : 
    [ $GPGLL ] - Geographic Latitude and Longitude
    -----------------------------------------
    Latitude: 13.736230
    Longitude: 100.532184
    Time: 07:PR:MC
    Status: 095237.00

    [ $GPVTG ] - Velocity Made Good
    -----------------------------------------
    Track True (Deg): 
    Track Magnetic (Deg): 
    Speed (Knots): 0.070
    Speed (Km/h): 0.130

    [ $GPGSV ] - Detailed GPS Satellite Information
    -----------------------------------------
    Total Messages: 4
    Message Number: 4
    Satellites in View: 13
    Sat 1 -> PRN: 24, Elev: 08, Azimuth: 249, SNR: *4A
 */

#include "SoftwareSerial.h"

SoftwareSerial GPSModule(3, 2); // TX, RX

String nmea[20]; 
bool addOneDay = false; 

void setup() {
  Serial.begin(115200); 
  GPSModule.begin(9600);
  Serial.println("Starting GPS... Waiting for NMEA Sentences...");
  Serial.println("=============================================");
}

void loop() {
  if (GPSModule.available() > 0) {
    String line = GPSModule.readStringUntil('\n');
    
    if (line.length() == 0) return;

    for (int i = 0; i < 20; i++) {
      nmea[i] = "";
    }

    int pos = 0;
    int stringplace = 0;
    for (int i = 0; i < line.length(); i++) {
      if (line.substring(i, i + 1) == ",") {
        if (pos < 20) {
          nmea[pos] = line.substring(stringplace, i);
          pos++;
        }
        stringplace = i + 1;
      }
      if (i == line.length() - 1) {
        if (pos < 20) {
          nmea[pos] = line.substring(stringplace, i + 1);
        }
      }
    }

    if (nmea[0] == "$GPRMC") {
      Serial.println("\n[ $GPRMC ] - Essential GPS PVT Data");
      Serial.println("-----------------------------------------");
      Serial.println("Time: " + ConvertTime(nmea[1], true));
      Serial.println("Status (A=Active, V=Void): " + nmea[2]);
      Serial.println("Latitude: " + ConvertLat(nmea[3], nmea[4]));
      Serial.println("Longitude: " + ConvertLng(nmea[5], nmea[6]));
      Serial.println("Speed (Knots): " + nmea[7]);
      Serial.println("Track Angle: " + nmea[8]);
      Serial.println("Date: " + ConvertDate(nmea[9]));
    }
    else if (nmea[0] == "$GPGLL") {
      Serial.println("\n[ $GPGLL ] - Geographic Latitude and Longitude");
      Serial.println("-----------------------------------------");
      Serial.println("Latitude: " + ConvertLat(nmea[1], nmea[2]));
      Serial.println("Longitude: " + ConvertLng(nmea[3], nmea[4]));
      Serial.println("Time: " + ConvertTime(nmea[5], false));
      Serial.println("Status: " + nmea[6]);
    }
    else if (nmea[0] == "$GPGSA") {
      Serial.println("\n[ $GPGSA ] - GPS DOP and Active Satellites");
      Serial.println("-----------------------------------------");
      Serial.println("Mode 1 (M=Manual, A=Auto): " + nmea[1]);
      Serial.println("Mode 2 (1=No fix, 2=2D, 3=3D): " + nmea[2]);
      
      String sats = "";
      for(int i = 3; i <= 14; i++) {
        if(nmea[i] != "") sats += nmea[i] + " ";
      }
      Serial.println("Active Satellites PRN: " + sats);
      Serial.println("PDOP: " + nmea[15]);
      Serial.println("HDOP: " + nmea[16]);
      Serial.println("VDOP: " + nmea[17].substring(0, nmea[17].indexOf('*')));
    }
    else if (nmea[0] == "$GPGSV") {
      Serial.println("\n[ $GPGSV ] - Detailed GPS Satellite Information");
      Serial.println("-----------------------------------------");
      Serial.println("Total Messages: " + nmea[1]);
      Serial.println("Message Number: " + nmea[2]);
      Serial.println("Satellites in View: " + nmea[3]);
      
      // PRN, Elevation, Azimuth, SNR
      int satCount = 1;
      for (int i = 4; i < pos - 1; i += 4) {
         if (nmea[i] != "") {
           Serial.println(" Sat " + String(satCount) + " -> PRN: " + nmea[i] + ", Elev: " + nmea[i+1] + ", Azimuth: " + nmea[i+2] + ", SNR: " + nmea[i+3]);
           satCount++;
         }
      }
    }
    
    else if (nmea[0] == "$GPVTG") {
      Serial.println("\n[ $GPVTG ] - Velocity Made Good");
      Serial.println("-----------------------------------------");
      Serial.println("Track True (Deg): " + nmea[1]);
      Serial.println("Track Magnetic (Deg): " + nmea[3]);
      Serial.println("Speed (Knots): " + nmea[5]);
      Serial.println("Speed (Km/h): " + nmea[7]);
    }
  }
}


String ConvertTime(String rawTime, bool updateDateTrigger) {
  if (rawTime.length() < 6) return rawTime; 
  if (updateDateTrigger) addOneDay = false; 
  
  int h = rawTime.substring(0, 2).toInt();
  String m = rawTime.substring(2, 4);
  String s = rawTime.substring(4, 6);
  
  h = h + 7; 
  if (h >= 24) {
    h = h - 24;
    if (updateDateTrigger) addOneDay = true; 
  }
  
  String hStr = String(h);
  if (h < 10) hStr = "0" + hStr;
  
  return hStr + ":" + m + ":" + s;
}

String ConvertDate(String rawDate) {
  if (rawDate.length() < 6) return rawDate; 
  
  int dd = rawDate.substring(0, 2).toInt();
  int mm = rawDate.substring(2, 4).toInt();
  int yy = rawDate.substring(4, 6).toInt();
  int yearFull = 2000 + yy;

  if (addOneDay) {
    dd++;
    int daysInMonth = 31;
    if (mm == 4 || mm == 6 || mm == 9 || mm == 11) daysInMonth = 30;
    else if (mm == 2) {
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

String ConvertLat(String rawLat, String dir) {
  if (rawLat == "") return ""; 
  String posneg = (dir == "S") ? "-" : "";
  
  int dotIndex = rawLat.indexOf('.');
  if (dotIndex == -1) return rawLat;
  
  String latfirst = rawLat.substring(0, dotIndex - 2);
  float latsecond = rawLat.substring(dotIndex - 2).toFloat() / 60.0;
  
  char charVal[9];
  dtostrf(latsecond, 4, 6, charVal);
  String CalcLat = String(charVal);
  CalcLat.trim(); 
  
  return posneg + latfirst + CalcLat.substring(1); 
}


String ConvertLng(String rawLng, String dir) {
  if (rawLng == "") return ""; 

  String posneg = (dir == "W") ? "-" : "";
  
  int dotIndex = rawLng.indexOf('.');
  if (dotIndex == -1) return rawLng;

  String lngfirst = rawLng.substring(0, dotIndex - 2);
  float lngsecond = rawLng.substring(dotIndex - 2).toFloat() / 60.0;
  
  char charVal[9];
  dtostrf(lngsecond, 4, 6, charVal);
  String CalcLng = String(charVal);
  CalcLng.trim();
  
  return posneg + lngfirst + CalcLng.substring(1); 
}