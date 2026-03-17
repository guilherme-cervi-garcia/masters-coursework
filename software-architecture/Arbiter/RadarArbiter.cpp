
#include "RadarArbiter.h"

RadarArbiter::RadarArbiter(uint32_t id) : Arbiter(id) {
}

RadarArbiter::~RadarArbiter() {
}

bool RadarArbiter::subscribeToSensorTopics() {
    bool output = false;
    if (isConnectedToBroker()) {
        bool main_subscribed = this->broker->subscribe("radar_main", this);
        bool backup_subscribed = this->broker->subscribe("radar_backup", this);
        output = main_subscribed && backup_subscribed;
    }
    return output;
}

void RadarArbiter::unsubscribeFromSensorTopics() {
    if (isConnectedToBroker()) {
        this->broker->unsubscribe("radar_main", this);
        this->broker->unsubscribe("radar_backup", this);
    }
}

ProcessedData RadarArbiter::performArbitration() {
    ProcessedData result;    
    // Calculate quality scores for available data
    float main_quality = 0.0f;
    float backup_quality = 0.0f;
    
    if (this->has_main_data) {
        main_quality = calculateRadarQuality(this->main_sensor_data);
    }
    
    if (this->has_backup_data) {
        backup_quality = calculateRadarQuality(this->backup_sensor_data);
    }
    
    // Arbitration logic: prefer main if quality is acceptable, otherwise use backup
    if (main_quality > 0.7f) {
        // Main sensor quality is good, use it
        result = this->main_sensor_data;
    } else if (backup_quality > 0.5f) {
        // Main quality poor, but backup is acceptable
        result = this->backup_sensor_data;
    } else if (main_quality > 0.0f) {
        // Both poor quality, but main is available
        result = this->main_sensor_data;
    } else if (backup_quality > 0.0f) {
        // Only backup available
        result = this->backup_sensor_data;
    }
    
    return result;
}

const char* RadarArbiter::getOutputTopicName() {
    return "arbitrated_radar";
}

bool RadarArbiter::isMainSensorTopic(const SimpleString& topic) {
    return topic.equals("radar_main");
}

bool RadarArbiter::isBackupSensorTopic(const SimpleString& topic) {
    return topic.equals("radar_backup");
}

bool RadarArbiter::isRadarDataConsistent(const ProcessedData& data) const {
    bool output = true;
    if (data.data_count > 0 && data.data_count < MAX_PROCESSED_DATA) {
    // Check for reasonable distance values (radar specific)
        for (int i = 0; i < data.data_count; i += 3) {
            if (i < MAX_PROCESSED_DATA) {
                float distance = data.processed_data[i];
                if (distance < 0.0f || distance > 300.0f) { // Unreasonable distance
                    output = false;
                }
            }
        }
    }
    return output;
}

float RadarArbiter::calculateRadarQuality(const ProcessedData& data) const {
    float quality = calculateDataQuality(data);
    
    // Apply radar-specific quality adjustments
    if (!isRadarDataConsistent(data)) {
        quality *= 0.5f; // Penalize inconsistent data
    }
    
    // Radar-specific: penalize if too few or too many objects
    int estimated_objects = data.data_count / 3;
    if (estimated_objects < 1 || estimated_objects > 10) {
        quality *= 0.9f;
    }
    
    return quality;
}