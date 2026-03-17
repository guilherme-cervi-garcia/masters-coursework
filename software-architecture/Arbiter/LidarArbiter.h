#ifndef LIDARARBITER_H
#define LIDARARBITER_H

#include "Arbiter.h"


// Lidar Arbiter - handles lidar_main and lidar_backup topics  
class LidarArbiter : public Arbiter {
public:
    LidarArbiter(uint32_t id);
    ~LidarArbiter();
    
    // Subscription management
    bool subscribeToSensorTopics() override;
    void unsubscribeFromSensorTopics() override;
    
    // Arbitration logic specific to lidar
    ProcessedData performArbitration() override;
    const char* getOutputTopicName() override;
    
protected:
    // Topic identification
    bool isMainSensorTopic(const SimpleString& topic) override;
    bool isBackupSensorTopic(const SimpleString& topic) override;
    
private:
    // Lidar-specific arbitration criteria
    bool isLidarDataConsistent(const ProcessedData& data) const;
    float calculateLidarQuality(const ProcessedData& data) const;
};

#endif // ARBITER_H