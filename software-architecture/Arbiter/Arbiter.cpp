#include "Arbiter.h"

Arbiter::Arbiter(uint32_t id) {
    this->id = id;
    this->broker = nullptr;
    this->is_connected = false;
    this->has_main_data = false;
    this->has_backup_data = false;
    this->last_arbitration_time = 0;
    this->main_selected_count = 0;
    this->backup_selected_count = 0;
    this->arbitration_count = 0;
}

Arbiter::~Arbiter() {
    disconnectFromBroker();
}

bool Arbiter::connectToBroker(Broker* broker_instance) {
    bool output = false;
    if (broker_instance != nullptr) {
        this->broker = (broker_instance);
        this->is_connected = true;
        output = true;
    }
    return output;
}

void Arbiter::disconnectFromBroker() {
    this->broker = nullptr;
    this->is_connected = false;
}

bool Arbiter::isConnectedToBroker() const {
    return this->is_connected && (this->broker != nullptr);
}

void Arbiter::onMessage(const SimpleString& topic, const ProcessedData& data) {
    // Route message to appropriate handler based on topic
    if (isMainSensorTopic(topic)) {
        handleMainSensorData(data);
    } else if (isBackupSensorTopic(topic)) {
        handleBackupSensorData(data);
    }
    
    // Attempt arbitration if we have data from both sensors
    if (has_main_data && has_backup_data) {
        processArbitration();
    }
}

bool Arbiter::processArbitration() {
    bool success = false;
    if (isConnectedToBroker()) {
        // Perform arbitration logic
        ProcessedData selected_data = performArbitration();
        success = publishArbitratedData(selected_data);
        if (success) {
            arbitration_count++;
            last_arbitration_time = getCurrentTimestamp();
            
            // Update selection statistics
            if (selected_data.id == main_sensor_data.id) {
                main_selected_count++;
            } else {
                backup_selected_count++;
            }
        }
    }
    return success;
}

void Arbiter::handleMainSensorData(const ProcessedData& data) {
    this->main_sensor_data = data;
    this->has_main_data = true;
}

void Arbiter::handleBackupSensorData(const ProcessedData& data) {
    this->backup_sensor_data = data;
    this->has_backup_data = true;
}

bool Arbiter::isDataFresh(const ProcessedData& data, uint64_t max_age_ms) const {
    uint64_t current_time = getCurrentTimestamp();
    return (current_time - data.timestamp) <= max_age_ms;
}

float Arbiter::calculateDataQuality(const ProcessedData& data) const {
    float quality = 0.0f;
    return quality;
}

bool Arbiter::publishArbitratedData(const ProcessedData& selected_data) {
    bool output = false;
    if (isConnectedToBroker()) {
        output = this->broker->publish(getOutputTopicName(), selected_data);
    }
    return output;
}

// getters
uint32_t Arbiter::getArbitrationCount() const {
    return this->arbitration_count;
}

uint32_t Arbiter::getMainSelectedCount() const {
    return this->main_selected_count;
}

uint32_t Arbiter::getBackupSelectedCount() const {
    return this->backup_selected_count;
}

float Arbiter::getMainSelectionRatio() const {
    float output = 0.0f;
    if (this->arbitration_count != 0) {
        output = (float)(this->main_selected_count) / (float)(this->arbitration_count); 
    }
    return output;
}

bool Arbiter::hasRecentData() const {
    return this->has_main_data || this->has_backup_data;
}

uint64_t Arbiter::getLastArbitrationTime() const {
    return this->last_arbitration_time;
}

