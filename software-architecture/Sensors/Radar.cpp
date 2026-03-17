#include "Radar.h"

// Constructor
Radar::Radar(uint32_t id, SensorRole role) : Sensor(id, role) {
}

// Destructor
Radar::~Radar() {

}

// Implementation of generateData
SensorData Radar::generateData() {
    // Create base sensor data with RADAR type
    SensorData data;
    
    data.timestamp = getCurrentTimestamp();
    data.id = this->id;  // Use sensor's ID
    data.sensor_type = SENSOR_RADAR;  // Correct type for Radar
    generateRadarObjects(data);
    return data;
}

// Override topic name generation for radar
void Radar::generateTopicName(char* topic_buffer, int buffer_size) {
    if (topic_buffer == nullptr || buffer_size < 20) {
        return;
    }
    
    const char* role_str = (this->role == SENSOR_MAIN) ? "main" : "backup";
    
    // Create topic: "radar_main" or "radar_backup"
    int idx = 0;
    const char* prefix = "radar_";
    while (*prefix && idx < buffer_size - 10) {
        topic_buffer[idx++] = *prefix++;
    }
    
    const char* role_ptr = role_str;
    while (*role_ptr && idx < buffer_size - 1) {
        topic_buffer[idx++] = *role_ptr++;
    }
    
    topic_buffer[idx] = '\0';
}

// Generate radar objects (distance, velocity, angle)
void Radar::generateRadarObjects(SensorData& data) {
    // Simulate 2-5 detected objects
    int num_objects = 2 + (simpleRand() % 4); // 2-5 objects
    
    // Each object needs 3 data points: distance, velocity, angle
    data.data_count = num_objects * 3;
    
    // Ensure we don't exceed array bounds
    if (data.data_count > MAX_RADAR_POINTS) {
        data.data_count = MAX_RADAR_POINTS;
        num_objects = data.data_count / 3;
    }
    
    // Generate data for each object
    for (int obj = 0; obj < num_objects; obj++) {
        int base_idx = obj * 3;
        
        if (base_idx + 2 < MAX_RADAR_POINTS) {
            // Distance: 5-200 meters
            data.raw_data[base_idx] = simpleRandFloat(5.0f, 200.0f);
            
            // Velocity: -25 to +25 m/s (negative = approaching)
            data.raw_data[base_idx + 1] = simpleRandFloat(-25.0f, 25.0f);
            
            // Angle: -90 to +90 degrees (relative to vehicle front)
            data.raw_data[base_idx + 2] = simpleRandFloat(-90.0f, 90.0f);
        }
    }
}
