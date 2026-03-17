#ifndef ARBITER_H
#define ARBITER_H

#include "Types.h"
#include "Data.h"
#include "Broker.h"
#include "Utils.h"


// Base Arbiter class
class Arbiter : public SubscriberCallback {
protected:
    Broker* broker;
    bool is_connected;
    uint32_t id;
    
    // Data storage for main and backup sensors
    ProcessedData main_sensor_data;
    ProcessedData backup_sensor_data;
    bool has_main_data;
    bool has_backup_data;
    uint64_t last_arbitration_time;
    
    // Statistics
    uint32_t main_selected_count;
    uint32_t backup_selected_count;
    uint32_t arbitration_count;
    
public:
    Arbiter(uint32_t id);
    virtual ~Arbiter();
    
    // Broker connectivity
    bool connectToBroker(Broker* broker_instance);
    void disconnectFromBroker();
    bool isConnectedToBroker() const;
    
    // Subscription management - pure virtual, implemented by derived classes
    virtual bool subscribeToSensorTopics() = 0;
    virtual void unsubscribeFromSensorTopics() = 0;
    
    // SubscriberCallback interface implementation
    void onMessage(const SimpleString& topic, const ProcessedData& data) override;
    
    // Arbitration logic - pure virtual, implemented by derived classes
    virtual ProcessedData performArbitration() = 0;
    virtual const char* getOutputTopicName() = 0;
    
    // Public interface
    bool processArbitration(); // Perform arbitration and publish result
    
    // Statistics
    uint32_t getArbitrationCount() const;
    uint32_t getMainSelectedCount() const;
    uint32_t getBackupSelectedCount() const;
    float getMainSelectionRatio() const;
    
    // Data access
    bool hasRecentData() const;
    uint64_t getLastArbitrationTime() const;
    
protected:
    // Helper methods for derived classes
    virtual void handleMainSensorData(const ProcessedData& data);
    virtual void handleBackupSensorData(const ProcessedData& data);
    virtual bool isMainSensorTopic(const SimpleString& topic) = 0;
    virtual bool isBackupSensorTopic(const SimpleString& topic) = 0;
    
    // Arbitration criteria helpers
    bool isDataFresh(const ProcessedData& data, uint64_t max_age_ms = 1000) const;
    bool isDataValid(const ProcessedData& data) const;
    float calculateDataQuality(const ProcessedData& data) const;
    
    // Publishing helper
    bool publishArbitratedData(const ProcessedData& selected_data);
};


#endif // ARBITER_H