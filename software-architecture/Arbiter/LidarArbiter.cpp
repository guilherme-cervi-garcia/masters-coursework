#include "LidarArbiter.h"


// ============== LidarArbiter Implementation ==============

LidarArbiter::LidarArbiter(uint32_t id) : Arbiter(id) {
}

LidarArbiter::~LidarArbiter() {
}

bool LidarArbiter::subscribeToSensorTopics() {
    bool output = false;
    if (isConnectedToBroker()) {
        bool main_subscribed = this->broker->subscribe("lidar_main", this);
        bool backup_subscribed = this->broker->subscribe("lidar_backup", this);
        output = main_subscribed && backup_subscribed;
    }
    return output;
}

void LidarArbiter::unsubscribeFromSensorTopics() {
    if (isConnectedToBroker()) {
        this->broker->unsubscribe("lidar_main", this);
        this->broker->unsubscribe("lidar_backup", this);
    }
}

ProcessedData LidarArbiter::performArbitration() {
    ProcessedData result;    
    // Calculate quality scores for available data
    float main_quality = 0.0f;
    float backup_quality = 0.0f;
    
    if (this->has_main_data) {
        main_quality = calculateLidarQuality(this->main_sensor_data);
    }
    
    if (this->has_backup_data) {
        backup_quality = calculateLidarQuality(this->backup_sensor_data);
    }
    
    // Arbitration logic: similar to radar but with lidar-specific thresholds
    if (main_quality > 0.75f) {
        // Main sensor quality is good, use it
        result = this->main_sensor_data;
    } else if (backup_quality > 0.6f) {
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

const char* LidarArbiter::getOutputTopicName() {
    return "arbitrated_lidar";
}

bool LidarArbiter::isMainSensorTopic(const SimpleString& topic) {
    return topic.equals("lidar_main");
}

bool LidarArbiter::isBackupSensorTopic(const SimpleString& topic) {
    return topic.equals("lidar_backup");
}

bool LidarArbiter::isLidarDataConsistent(const ProcessedData& data) const {
    // Check if lidar data makes sense (basic validation)
    bool output = true;
    if (data.data_count > 0 && data.data_count < MAX_PROCESSED_DATA) {
        // Check for reasonable distance values (lidar specific)
        for (int i = 0; i < data.data_count; i ++) {
            if (i < MAX_PROCESSED_DATA) {
                float distance = data.processed_data[i];
                if (distance < 0.0f || distance > 150.0f) { // Unreasonable distance
                    output = false;
                }
            }
        }
    }
    return output;
}

float LidarArbiter::calculateLidarQuality(const ProcessedData& data) const {
    float quality = calculateDataQuality(data);
    
    // Apply lidar-specific quality adjustments
    if (!isLidarDataConsistent(data)) {
        quality *= 0.5f; // Penalize inconsistent data
    }
    
    // Lidar-specific: prefer more data points (richer scan)
    if (data.data_count > 20) {
        quality *= 1.1f; // Bonus for rich scan data
    } else if (data.data_count < 5) {
        quality *= 0.8f; // Penalize sparse data
    }
    
    return quality;
}