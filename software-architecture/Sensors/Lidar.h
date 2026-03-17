#ifndef LIDAR_H
#define LIDAR_H

#include "Sensor.h"

// Lidar sensor class
class Lidar : public Sensor {
public:
    // Constructor
    Lidar(uint32_t id, SensorRole role);
    
    // Destructor
    ~Lidar();
    
    // Implementation of pure virtual method from base class
    SensorData generateData() override;

    // Override topic name generation for lidar-specific topics
    void generateTopicName(char* topic_buffer, int buffer_size) override;
private:
    // Generate lidar-specific data (distance measurements at different angles)
    void generateLidarScan(SensorData& data);
};

#endif // LIDAR_H