#ifndef SENSOR_H
#define SENSOR_H

#include "Types.h"
#include "Data.h"
#include "Utils.h"
#include "Broker.h"

// Sensor role enumeration
enum SensorRole {
    SENSOR_MAIN = 1,
    SENSOR_BACKUP = 2
};

// Base sensor class
class Sensor {
protected:
    uint32_t id;
    SensorRole role;
    Broker* broker;
    bool is_connected;

    // Generate topic name for publishing
    virtual void generateTopicName(char* topic_buffer, int buffer_size);
    
public:
    // Constructor with explicit role
    Sensor(uint32_t id, SensorRole role);
    
    // Virtual destructor for proper cleanup
    virtual ~Sensor();

    // Broker connectivity
    bool connectToBroker(Broker* broker_instance);
    void disconnectFromBroker();
    bool isConnectedToBroker() const;

    // Publishing interface
    bool publishData();

    // Pure virtual method - each sensor type implements its own data generation
    virtual SensorData generateData() = 0;
    
    // Getters
    // Get topic name for this sensor
    void getTopicName(char* topic_buffer, int buffer_size);

    uint32_t getSensorId() const;
    SensorRole getSensorRole() const;
};

#endif // SENSOR_H