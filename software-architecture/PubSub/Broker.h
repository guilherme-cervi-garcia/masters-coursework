#ifndef BROKER_H
#define BROKER_H

#include "Types.h"
#include "Data.h"
#include "Utils.h"

// Maximum number of topics the broker can handle
const int MAX_TOPICS = 8;

// Maximum number of subscribers per topic
const int MAX_SUBSCRIBERS_PER_TOPIC = 4;

// Maximum messages to buffer per topic
const int MAX_MESSAGES_PER_TOPIC = 3;


// Message structure for buffering
struct BrokerMessage {
    ProcessedData data;
    uint64_t timestamp;
    bool is_valid;
    
    BrokerMessage();
    BrokerMessage(const ProcessedData& msg_data);
};

// Subscriber callback interface
class SubscriberCallback {
public:
    virtual ~SubscriberCallback() {}
    virtual void onMessage(const SimpleString& topic, const ProcessedData& data) = 0;
};

// Topic management structure
struct Topic {
    SimpleString name;
    BrokerMessage messages[MAX_MESSAGES_PER_TOPIC];
    int message_count;
    int next_message_index; // For circular buffer
    
    SubscriberCallback* subscribers[MAX_SUBSCRIBERS_PER_TOPIC];
    int subscriber_count;
    
    bool is_active;
    
    Topic();
    Topic(const SimpleString& topic_name);
    
    // Add message to topic buffer
    void addMessage(const ProcessedData& data);
    
    // Get latest message
    bool getLatestMessage(ProcessedData& data) const;
    
    // Add subscriber
    bool addSubscriber(SubscriberCallback* callback);
    
    // Remove subscriber
    bool removeSubscriber(SubscriberCallback* callback);
    
    // Notify all subscribers
    void notifySubscribers(const ProcessedData& data);
};

// Main Broker class
class Broker {
private:
    Topic topics[MAX_TOPICS];
    int topic_count;
    
public:
    Broker();
    ~Broker();
    
    // Create or get topic
    bool createTopic(const SimpleString& topic_name);
    Topic* getTopic(const SimpleString& topic_name);
    
    // Publishing interface
    bool publish(const SimpleString& topic, const ProcessedData& data);
    bool publish(const char* topic, const ProcessedData& data);
    
    // Subscription interface  
    bool subscribe(const SimpleString& topic, SubscriberCallback* callback);
    bool subscribe(const char* topic, SubscriberCallback* callback);

    bool unsubscribe(const SimpleString& topic, SubscriberCallback* callback);
    bool unsubscribe(const char* topic, SubscriberCallback* callback);
    
    // Message retrieval (alternative to callbacks)
    bool getLatestMessage(const SimpleString& topic, ProcessedData& data);
    bool getLatestMessage(const char* topic, ProcessedData& data);
    
    // Broker information
    int getTopicCount() const;
    bool getTopicName(int index, SimpleString& name) const;
    int getSubscriberCount(const SimpleString& topic) const;
    int getMessageCount(const SimpleString& topic) const;
    
    
private:
    // Find topic by name
    int findTopicIndex(const SimpleString& topic_name) const;
    
    // Add new topic
    int addNewTopic(const SimpleString& topic_name);
};

#endif // BROKER_H