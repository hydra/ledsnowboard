#ifndef SENSORDATASTORE_H
#define SENSORDATASTORE_H

#include "AccelerationData.h"

class SensorDataStore {

public:
  AccelerationData rawAccelerationData;
  AccelerationData sampledAccelerationData;
};

#endif
