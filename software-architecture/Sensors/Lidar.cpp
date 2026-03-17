#include "Lidar.h"

// Constructor
Lidar::Lidar(uint32_t id, SensorRole role) : Sensor(id, role) {
    // Lidar-specific initialization if needed
}

// Destructor
Lidar::~Lidar() {
    // Lidar-specific cleanup if needed
}

// Implementation of generateData
SensorData Lidar::generateData() {
    // Create base sensor data with LIDAR type
    SensorData data;

    data.timestamp = getCurrentTimestamp();
    data.id = this->id;  // Use sensor's ID
    data.sensor_type = SENSOR_LIDAR;  // Correct type for lidar    
    generateLidarScan(data);
    return data;
}

// Override topic name generation for lidar
void Lidar::generateTopicName(char* topic_buffer, int buffer_size) {
    if (topic_buffer == nullptr || buffer_size < 20) {
        return;
    }
    
    const char* role_str = (this->role == SENSOR_MAIN) ? "main" : "backup";
    
    // Create topic: "lidar_main" or "lidar_backup"
    int idx = 0;
    const char* prefix = "lidar_";
    while (*prefix && idx < buffer_size - 10) {
        topic_buffer[idx++] = *prefix++;
    }
    
    const char* role_ptr = role_str;
    while (*role_ptr && idx < buffer_size - 1) {
        topic_buffer[idx++] = *role_ptr++;
    }
    
    topic_buffer[idx] = '\0';
}

// Generate lidar scan data (distance at multiple angles)
void Lidar::generateLidarScan(SensorData& data) {
    // Simulate simplified 360-degree scan
    // Using fewer points to fit in our array (every 10 degrees = 36 points)
    int scan_points = MAX_LIDAR_POINTS; // Limited by our current array size
    data.data_count = scan_points;
    
    // Generate distance measurement for each angle
    for (int i = 0; i < scan_points; i++) {

        data.raw_data[i] = simpleRandFloat(1.0f, 100.0f);
        
        // Simulate some obstacles closer than others
        // Occasionally generate closer objects
        if ((simpleRand() % 10) < 2) { // 20% chance of close object
            data.raw_data[i] = simpleRandFloat(1.0f, 10.0f);
        }
    }
}

