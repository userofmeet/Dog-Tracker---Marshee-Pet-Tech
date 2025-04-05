#ifndef ACTIVITYCLASSIFIER_H
#define ACTIVITYCLASSIFIER_H

enum ActivityType { RESTING, WALKING, RUNNING, PLAYING };

class ActivityClassifier {
public:
  ActivityType classify(float ax, float ay, float az) {
    float mag = sqrt(ax * ax + ay * ay + az * az);

    if (mag < 1.2) return RESTING;
    else if (mag < 3.0) return WALKING;
    else if (mag < 6.0) return RUNNING;
    else return PLAYING;
  }
};

#endif
