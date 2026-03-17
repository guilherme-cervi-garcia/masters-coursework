#ifndef TYPES_H
#define TYPES_H

// Basic type definitions
typedef unsigned int uint32_t;
typedef unsigned long long uint64_t;
typedef float float32_t;

// System constants
const int MAX_RADAR_POINTS = 21;
const int MAX_LIDAR_POINTS = 36; // Simplified from 360
const int MAX_PROCESSED_DATA = 50;

// Sensor type enumeration
enum SensorType {
    SENSOR_RADAR = 1,
    SENSOR_LIDAR = 2
};

#endif // TYPES_H