#include "Sensor.h"

// Constructor
Sensor::Sensor(uint32_t sensor_id, SensorRole sensor_role) {
    this->id = sensor_id;
    this->role = sensor_role;
    this->broker = nullptr;
    this->is_connected = false;
}

// Virtual destructor
Sensor::~Sensor() {
    disconnectFromBroker();
}

// Connect to broker
bool Sensor::connectToBroker(Broker* broker_instance) {
    bool output = false;
    if (broker_instance != nullptr) {
        this->broker = (broker_instance);
        this->is_connected = true;
        output = true;
    }
    return output;
}

// Disconnect from broker
void Sensor::disconnectFromBroker() {
    this->broker = nullptr;
    this->is_connected = false;
}

// Check broker connection
bool Sensor::isConnectedToBroker() const {
    return this->is_connected && (this->broker != nullptr);
}

// Publish sensor data to broker
bool Sensor::publishData() {
    bool success = false;
    if (isConnectedToBroker()) {
    
    // Generate fresh sensor data
    SensorData raw_data = generateData();
    
    // Create ProcessedData from SensorData (basic conversion for now)
    ProcessedData processed_data;
    processed_data.timestamp = raw_data.timestamp;
    processed_data.id = raw_data.id;
    processed_data.sensor_type = raw_data.sensor_type;
    
    // Copy data (limited by ProcessedData array size)
    int copy_count = raw_data.data_count;
    if (copy_count > MAX_PROCESSED_DATA) {
        copy_count = MAX_PROCESSED_DATA;
    }
    processed_data.data_count = copy_count;
    
    for (int i = 0; i < copy_count; i++) {
        processed_data.processed_data[i] = raw_data.raw_data[i];
    }
    
    // Generate topic name
    char topic_name[50];
    generateTopicName(topic_name, sizeof(topic_name));
    
    // Publish to broker
    success = broker->publish(topic_name, processed_data);
    
    }
    return success;
}

// Generate topic name based on sensor type and role
void Sensor::generateTopicName(char* topic_buffer, int buffer_size) {
    if (topic_buffer == nullptr || buffer_size < 20) {
        return;
    }
    // Base implementation creates generic topic
    const char* role_str = (this->role == SENSOR_MAIN) ? "main" : "backup";
    
    // Simple string construction
    int idx = 0;
    const char* prefix = "sensor_";
    while (*prefix && idx < buffer_size - 10) {
        topic_buffer[idx++] = *prefix++;
    }
    
    // Add role
    const char* role_ptr = role_str;
    while (*role_ptr && idx < buffer_size - 5) {
        topic_buffer[idx++] = *role_ptr++;
    }
    
    // Add sensor ID
    topic_buffer[idx++] = '_';
    if (id < 10) {
        topic_buffer[idx++] = '0' + (char)id;
    } else {
        topic_buffer[idx++] = 'X';
    }
    
    topic_buffer[idx] = '\0';
}

// Getters
// Get topic name (public interface)
void Sensor::getTopicName(char* topic_buffer, int buffer_size) {
    generateTopicName(topic_buffer, buffer_size);
}

uint32_t Sensor::getSensorId() const {
    return this->id;
}

SensorRole Sensor::getSensorRole() const {
    return this->role;
}