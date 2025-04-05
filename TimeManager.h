#ifndef TIMEMANAGER_H
#define TIMEMANAGER_H

#include <WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <time.h>

class TimeManager {
private:
  WiFiUDP ntpUDP;
  NTPClient timeClient;

public:
  TimeManager() : timeClient(ntpUDP, "pool.ntp.org", 0, 60000) {}

  void begin() {
    timeClient.begin();
  }

  void syncTime() {
    if (WiFi.status() == WL_CONNECTED) {
      timeClient.update();
      time_t rawTime = timeClient.getEpochTime();
      struct tm* ti = localtime(&rawTime);
      Serial.printf("Synced Time: %s", asctime(ti));
    }
  }

  uint32_t getEpochTime() {
    timeClient.update();
    return timeClient.getEpochTime();
  }

  String getDateFileName() {
    time_t now = getEpochTime();
    struct tm* t = localtime(&now);
    char fileName[20];
    sprintf(fileName, "/data/%04d%02d%02d.dat", t->tm_year + 1900, t->tm_mon + 1, t->tm_mday);
    return String(fileName);
  }
};

#endif
