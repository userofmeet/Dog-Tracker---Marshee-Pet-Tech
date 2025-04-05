#ifndef SIMULATED_QMI8658_H
#define SIMULATED_QMI8658_H

struct IMUdata {
  float x, y, z;
};

class SimulatedQMI8658 {
private:
  bool motionDetected;
  void (*wakeupCallback)();

public:
  SimulatedQMI8658() : motionDetected(false), wakeupCallback(nullptr) {}

  bool begin() {
    Serial.println("SimulatedQMI8658 initialized.");
    return true;
  }

  bool readFromFifo(IMUdata* acc, int accCount, IMUdata* gyr, int gyrCount) {
    for (int i = 0; i < accCount; i++) {
      acc[i].x = random(-8000, 8000) / 1000.0;
      acc[i].y = random(-8000, 8000) / 1000.0;
      acc[i].z = random(9000, 12000) / 1000.0;
    }

    for (int i = 0; i < gyrCount; i++) {
      gyr[i].x = random(-2000, 2000) / 100.0;
      gyr[i].y = random(-2000, 2000) / 100.0;
      gyr[i].z = random(-2000, 2000) / 100.0;
    }

    return true;
  }

  void configWakeOnMotion() {
    Serial.println("Wake-on-motion configured.");
  }

  void setWakeupMotionEventCallBack(void (*callback)()) {
    wakeupCallback = callback;
  }

  void simulateMotion() {
    if (random(100) < 5 && !motionDetected) {
      motionDetected = true;
      if (wakeupCallback) wakeupCallback();
    } else {
      motionDetected = false;
    }
  }
};

#endif
