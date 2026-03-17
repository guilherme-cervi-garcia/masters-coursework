#ifndef SENSOR_DATA_H
#define SENSOR_DATA_H

#include "Types.h"

// Simple sensor data structure - just holds data
struct SensorData {
    uint64_t timestamp;
    uint32_t id;
    SensorType sensor_type;
    float raw_data[MAX_LIDAR_POINTS];
    int data_count;
    
    // Simple default constructor to initialize fields
    SensorData() : timestamp(0), id(0), sensor_type(SENSOR_RADAR), 
                   data_count(0) {
        for (int i = 0; i < MAX_LIDAR_POINTS; i++) {
            raw_data[i] = 0.0f;
        }
    }
};

// Simple processed data structure - just holds data
struct ProcessedData {
    uint64_t timestamp;
    uint32_t id;
    SensorType sensor_type;
    float processed_data[MAX_PROCESSED_DATA];
    int data_count;
    
    // Simple default constructor to initialize fields
    ProcessedData() : timestamp(0), id(0), sensor_type(SENSOR_RADAR), 
                      data_count(0) {
        for (int i = 0; i < MAX_PROCESSED_DATA; i++) {
            processed_data[i] = 0.0f;
        }
    }
};

#endif // SENSOR_DATA_H