#ifndef SENSORDATASTORE_H
#define SENSORDATASTORE_H

#include "AccelerationData.h"

class SensorDataStore {

public:
    SensorDataStore() : isSampledAccelerationDataValid(false) {}

    AccelerationData rawAccelerationData;
    AccelerationData sampledAccelerationData;

    bool isSampledAccelerationDataValid; // TODO make private, add getter.
};

#endif
