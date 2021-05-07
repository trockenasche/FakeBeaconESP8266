/*
   Fake beacon frames for ESP8266 using the Arduino IDE
   Compiled with Arduino 1.6.9 and esp8266 2.1.0, but other versions should work too

   Based on the WiFiBeaconJam by kripthor (https://github.com/kripthor/WiFiBeaconJam)

   More info: http://nomartini-noparty.blogspot.com/2016/07/esp8266-and-beacon-frames.html
*/

#include <ESP8266WiFi.h> //more about beacon frames https://mrncciew.com/2014/10/08/802-11-mgmt-beacon-frame/

extern "C" {
#include "user_interface.h"
}

void setup() {
  delay(500);
  wifi_set_opmode(STATION_MODE);
  wifi_promiscuous_enable(1);
}



void loop() {
  //sendBeacon("test"); //sends beacon frames with the SSID 'test'
  //sendRandomBeacon(10); //sends beacon frames with 10 character long random SSID
  //sendFuzzedBeacon("test",10); //sends beacon frames with 10 different SSID all starting with 'test' and ending with whitespaces (spaces and/or tabs)
  //RickRoll();
  sendVaccineChip(); // Send fake vaccine beacons out.
}

void sendFuzzedBeacon(char* baseSsid, int nr) {
  int baseLen = strlen(baseSsid);
  int i = 0;
  for (int j = 0; j < 32 - baseLen; j++) { //32 is the maximum length of the SSID
    for (int k = 0; k < pow(2, j); k++) {
      int kk = k;
      String ssid = baseSsid;
      for (int l = 0; l < j; l++) {
        if (kk % 2 == 1) ssid += " "; //add a space
        else ssid += "\t"; //add a tab
        kk /= 2;
      }
      char charBufSsid[33];
      ssid.toCharArray(charBufSsid, 33);
      sendBeacon(charBufSsid);
      if (++i >= nr) return;
    }
  }
}

void sendVaccineChip() {
  char* baseSsid;
  int nr;
  delay(25); // So as not to be too noisy.
  long  randomChipName = random(10); // This randomisation should look as if different numbers of "people" with "vaccination chips" come along.
  if (randomChipName < 4) { // To have a higher chance of appearing (4 out of 10).
    baseSsid = "BioNTech/Pfizer - Chip ID#";
    nr = random(10); // Randomly generate the number of chips to be displayed simultaneously.
  }
  else if (randomChipName <= 6) { // To appear 3 times out of 10.
    baseSsid = "AstraZeneca - Chip ID#";
    nr = random(5); // Randomly generate the number of chips to be displayed simultaneously.
  }
  else if (randomChipName <= 8) { // To appear 2 times out of 10.
    baseSsid = "Moderna - Chip ID#";
    nr = random(4); // Randomly generate the number of chips to be displayed simultaneously.
  }
  else if (randomChipName = 9) { // To appear only one time out of 10.
    baseSsid = "Johnson & Johnson - Chip ID#";
    nr = random(3); // Randomly generate the number of chips to be displayed simultaneously.
  }
  int baseLen = strlen(baseSsid);
  int i = 0;
  String ssid = baseSsid;
  long randomNumber = random(10000, 10000000); // Chips ID
  ssid += randomNumber;
  int ssidLen = ssid.length();
  char charBufSsid[ssidLen];
  ssid.toCharArray(charBufSsid, ssidLen);
  sendBeacon(charBufSsid);
  if (++i >= nr) return;
}


void sendRandomBeacon(int len) {
  char ssid[len + 1];
  randomString(len, ssid);
  sendBeacon(ssid);
}

void randomString(int len, char* ssid) {
  String alfa = "1234567890qwertyuiopasdfghjkklzxcvbnm QWERTYUIOPASDFGHJKLZXCVBNM_";
  for (int i = 0; i < len; i++) {
    ssid[i] = alfa[random(65)];
  }
}

void sendBeacon(char* ssid) {
  // Randomize channel //
  byte channel = random(1, 12);
  wifi_set_channel(channel);

  uint8_t packet[128] = { 0x80, 0x00, //Frame Control
                          0x00, 0x00, //Duration
                          /*4*/   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, //Destination address
                          /*10*/  0x01, 0x02, 0x03, 0x04, 0x05, 0x06, //Source address - overwritten later
                          /*16*/  0x01, 0x02, 0x03, 0x04, 0x05, 0x06, //BSSID - overwritten to the same as the source address
                          /*22*/  0xc0, 0x6c, //Seq-ctl
                          //Frame body starts here
                          /*24*/  0x83, 0x51, 0xf7, 0x8f, 0x0f, 0x00, 0x00, 0x00, //timestamp - the number of microseconds the AP has been active
                          /*32*/  0xFF, 0x00, //Beacon interval
                          /*34*/  0x01, 0x04, //Capability info
                          /* SSID */
                          /*36*/  0x00
                        };

  int ssidLen = strlen(ssid);
  packet[37] = ssidLen;

  for (int i = 0; i < ssidLen; i++) {
    packet[38 + i] = ssid[i];
  }

  uint8_t postSSID[13] = {0x01, 0x08, 0x82, 0x84, 0x8b, 0x96, 0x24, 0x30, 0x48, 0x6c, //supported rate
                          0x03, 0x01, 0x04 /*DSSS (Current Channel)*/
                         };

  for (int i = 0; i < 12; i++) {
    packet[38 + ssidLen + i] = postSSID[i];
  }

  packet[50 + ssidLen] = channel;

  // Randomize SRC MAC
  packet[10] = packet[16] = random(256);
  packet[11] = packet[17] = random(256);
  packet[12] = packet[18] = random(256);
  packet[13] = packet[19] = random(256);
  packet[14] = packet[20] = random(256);
  packet[15] = packet[21] = random(256);

  int packetSize = 51 + ssidLen;

  wifi_send_pkt_freedom(packet, packetSize, 0);
  wifi_send_pkt_freedom(packet, packetSize, 0);
  wifi_send_pkt_freedom(packet, packetSize, 0);
  delay(1);
}

void RickRoll() {
  sendBeacon("01 Never gonna give you up,");
  sendBeacon("02 never gonna let you down");
  sendBeacon("03 Never gonna run around");
  sendBeacon("04  and desert you");
  sendBeacon("05 Never gonna make you cry,");
  sendBeacon("06 never gonna say goodbye");
  sendBeacon("07 Never gonna tell a lie");
  sendBeacon("08  and hurt you");
}
