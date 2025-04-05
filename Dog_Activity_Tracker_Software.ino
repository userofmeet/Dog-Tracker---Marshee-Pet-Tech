#include "SimulatedQMI8658.h"
#include "TimeManager.h"
#include "ActivityClassifier.h"
#include <LittleFS.h>
#include <WiFi.h>
#include <esp_sleep.h>
#define WAKEUP_INTERVAL_SEC (120) // 2 minutes
RTC_DATA_ATTR int bootCount = 0;
RTC_DATA_ATTR uint16_t activityCounters[4] = {0};

SimulatedQMI8658 imu;
IMUdata acc[1], gyr[1];

TimeManager timeMgr;
ActivityClassifier classifier;

//
const char* ssid = "Meet Jain";
const char* password = "userofmeet";

void onMotionDetected() {
  Serial.println("Triggered Wakeup Callback ");
}

void connectWiFi() {
  WiFi.begin(ssid, password);
  Serial.print("Connecting to Wi-Fi");

  int retries = 0;
  while (WiFi.status() != WL_CONNECTED && retries < 20) {
    Serial.print(".");
    delay(500);
    retries++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("Wifi connected");
    timeMgr.syncTime();
  } else {
    Serial.println("WiFi connection failed");
  }

  WiFi.disconnect(true);
}

void logActivityData() {
  String fileName = timeMgr.getDateFileName();
  File file = LittleFS.open(fileName, FILE_APPEND);
  if (!file) {
    Serial.println("Failed to open log file");
    return;
  }

  uint32_t ts = timeMgr.getEpochTime();
  file.write((uint8_t*)&ts, sizeof(ts));
  for (int i = 0; i < 4; i++) {
    file.write((uint8_t*)&activityCounters[i], sizeof(uint16_t));
  }

  file.close();
  Serial.printf("Logged to %s\n", fileName.c_str());
}

void setup() {
  Serial.begin(115200);
  delay(300);

  bootCount++;
  Serial.printf("Boot #%d\n", bootCount);

  // Start LittleFS
  if (!LittleFS.begin(true)) {
    Serial.println("LittleFS initilisation failed");
    return;
  }

  // Init sensor
  imu.begin();
  imu.configWakeOnMotion();
  imu.setWakeupMotionEventCallBack(onMotionDetected);

  // Wi-Fi + Time Sync
  connectWiFi();

  //Read IMU and classify it
  if (imu.readFromFifo(acc, 1, gyr, 1)) {
    ActivityType act = classifier.classify(acc[0].x, acc[0].y, acc[0].z);
    activityCounters[act] += 2; // 2 seconds for this boot cycle

    const char* actLabels[] = {"Resting", "Walking", "Running", "Playing"};
    Serial.printf("Activity: %s (%.2f, %.2f, %.2f)\n", actLabels[act], acc[0].x, acc[0].y, acc[0].z);
  }

  //10 minutes
  if ((bootCount * 2) % 600 == 0) {
    logActivityData();
    memset(activityCounters, 0, sizeof(activityCounters));
  }

  Serial.println("Sleeping");
  esp_sleep_enable_timer_wakeup(WAKEUP_INTERVAL_SEC * 1000000ULL);
  esp_deep_sleep_start();
}

void loop() {}
