#ifndef RADARARBITER_H
#define RADARARBITER_H

#include "Arbiter.h"

// Radar Arbiter - handles radar_main and radar_backup topics
class RadarArbiter : public Arbiter {
public:
    RadarArbiter(uint32_t id);
    ~RadarArbiter();
    
    // Subscription management
    bool subscribeToSensorTopics() override;
    void unsubscribeFromSensorTopics() override;
    
    // Arbitration logic specific to radar
    ProcessedData performArbitration() override;
    const char* getOutputTopicName() override;
    
protected:
    // Topic identification
    bool isMainSensorTopic(const SimpleString& topic) override;
    bool isBackupSensorTopic(const SimpleString& topic) override;
    
private:
    // Radar-specific arbitration criteria
    bool isRadarDataConsistent(const ProcessedData& data) const;
    float calculateRadarQuality(const ProcessedData& data) const;
};

#endif