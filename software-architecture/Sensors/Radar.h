#ifndef RADAR_H
#define RADAR_H

#include "Sensor.h"

// Radar sensor class
class Radar : public Sensor {
public:
    // Constructor
    Radar(uint32_t id, SensorRole role);
    
    // Destructor
    ~Radar();
    
    // Implementation of pure virtual method from base class
    SensorData generateData() override;

    // Override topic name generation for lidar-specific topics
    void generateTopicName(char* topic_buffer, int buffer_size) override;
private:
    // Generate radar-specific data (distance, velocity, angle for objects)
    void generateRadarObjects(SensorData& data);
};

#endif // RADAR_H