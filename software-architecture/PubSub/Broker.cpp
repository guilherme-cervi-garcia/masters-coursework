#include "Broker.h"

// ============== BrokerMessage Implementation ==============

BrokerMessage::BrokerMessage() {
    this->timestamp = 0;
    this->is_valid = false;
}

BrokerMessage::BrokerMessage(const ProcessedData& msg_data) {
    this->data = msg_data;
    this->timestamp = getCurrentTimestamp();
    this->is_valid = true;
}

// ============== Topic Implementation ==============

Topic::Topic() {
    this->message_count = 0;
    this->next_message_index = 0;
    this->subscriber_count = 0;
    this->is_active = false;
    
    // Initialize subscriber array
    for (int i = 0; i < MAX_SUBSCRIBERS_PER_TOPIC; i++) {
        this->subscribers[i] = nullptr;
    }
}

Topic::Topic(const SimpleString& topic_name) : Topic() {
    this->name = topic_name;
    this->is_active = true;
}

void Topic::addMessage(const ProcessedData& data) {
    // Add message to circular buffer
    this->messages[this->next_message_index] = BrokerMessage(data);
    
    // Update indices
    this->next_message_index = (this->next_message_index + 1) % MAX_MESSAGES_PER_TOPIC;
    
    if (this->message_count < MAX_MESSAGES_PER_TOPIC) {
        this->message_count++;
    }
    
    // Notify subscribers
    notifySubscribers(data);
}

bool Topic::getLatestMessage(ProcessedData& data) const {
    bool output = false;
    if (this->message_count > 0) {
        int latest_index = (this->next_message_index - 1 + MAX_MESSAGES_PER_TOPIC) % MAX_MESSAGES_PER_TOPIC;
        
        if (this->messages[latest_index].is_valid) {
            data = this->messages[latest_index].data;
            output = true;
        }
    }        
    return output;
}

bool Topic::addSubscriber(SubscriberCallback* callback) {
    bool output = false;
    if (callback != nullptr && this->subscriber_count < MAX_SUBSCRIBERS_PER_TOPIC) {
        bool alreadySub = false;
        // Check if already subscribed
        for (int i = 0; i < subscriber_count; i++) {
            if (subscribers[i] == callback) {
                alreadySub = true; // Already subscribed
            }
        }
        if(!alreadySub){
            subscribers[subscriber_count] = callback;
            subscriber_count++;
            output = true;
        }
    }
    return output;
}

bool Topic::removeSubscriber(SubscriberCallback* callback) {
    bool output = false;
    if (callback != nullptr) {
        for (int i = 0; i < subscriber_count; i++) {
            if (subscribers[i] == callback) {
                // Shift remaining subscribers down
                for (int j = i; j < subscriber_count - 1; j++) {
                    subscribers[j] = subscribers[j + 1];
                }
                subscribers[subscriber_count - 1] = nullptr;
                subscriber_count--;
                output = true;
            }
        }
    }
    return output;
}

void Topic::notifySubscribers(const ProcessedData& data) {
    for (int i = 0; i < subscriber_count; i++) {
        if (subscribers[i] != nullptr) {
            subscribers[i]->onMessage(name, data);
        }
    }
}

// ============== Broker Implementation ==============

Broker::Broker() {
    topic_count = 0;
}

Broker::~Broker() {
    // Cleanup if needed - for now, nothing to do
}

bool Broker::createTopic(const SimpleString& topic_name) {
    bool output = false;
    // Check if topic already exists
    if (findTopicIndex(topic_name) >= 0) {
        output = true; // Topic already exists
    } else {
        output = addNewTopic(topic_name) >= 0;
    }
    return output;
}

Topic* Broker::getTopic(const SimpleString& topic_name) {
    int index = findTopicIndex(topic_name);
    Topic *output = nullptr;
    if (index >= 0) {
        output = &topics[index];
    }
    return output;
}

bool Broker::publish(const SimpleString& topic, const ProcessedData& data) {
    bool output = false;
    // Create topic if it doesn't exist
    if (createTopic(topic)) {
        Topic* topic_ptr = getTopic(topic);
        if (topic_ptr != nullptr) {
            topic_ptr->addMessage(data);
            output = true;
        }
    }    
    return output;
}

bool Broker::publish(const char* topic, const ProcessedData& data) {
    SimpleString topic_str(topic);
    return publish(topic_str, data);
}

bool Broker::subscribe(const SimpleString& topic, SubscriberCallback* callback) {
    bool output = false;
    // Create topic if it doesn't exist
    if (createTopic(topic)) {
        Topic* topic_ptr = getTopic(topic);
        if (topic_ptr != nullptr) {
            output = topic_ptr->addSubscriber(callback);
        }
    }
    return output;
}

bool Broker::subscribe(const char* topic, SubscriberCallback* callback) {
    SimpleString topic_str(topic);
    return subscribe(topic_str, callback);
}

bool Broker::unsubscribe(const SimpleString& topic, SubscriberCallback* callback) {    
    bool output = false;
    Topic* topic_ptr = getTopic(topic);
    if (topic_ptr != nullptr) {
        output = topic_ptr->removeSubscriber(callback);
    }
    return output;
}

bool Broker::unsubscribe(const char* topic, SubscriberCallback* callback) {
    SimpleString topic_str(topic);
    return unsubscribe(topic_str, callback);
}

bool Broker::getLatestMessage(const SimpleString& topic, ProcessedData& data) {
    bool output = false;
    Topic* topic_ptr = getTopic(topic);
    if (topic_ptr != nullptr) {
        output = topic_ptr->getLatestMessage(data);
    }
    return output;
}

bool Broker::getLatestMessage(const char* topic, ProcessedData& data) {
    SimpleString topic_str(topic);
    return getLatestMessage(topic_str, data);
}

int Broker::getTopicCount() const {
    return topic_count;
}

bool Broker::getTopicName(int index, SimpleString& name) const {
    bool output = false;
    if (index >= 0 && index < topic_count) {
        name = topics[index].name;
        output = true;
    }
    return output;
}

int Broker::getSubscriberCount(const SimpleString& topic) const {
    int output = 0;
    int index = findTopicIndex(topic);
    if (index >= 0) {
        output = topics[index].subscriber_count;
    }
    return output;
}

int Broker::getMessageCount(const SimpleString& topic) const {
    int output = 0;
    int index = findTopicIndex(topic);
    if (index >= 0) {
        output = topics[index].message_count;
    }
    return output;
}

int Broker::findTopicIndex(const SimpleString& topic_name) const {
    int output = -1;
    for (int i = 0; i < topic_count; i++) {
        if (topics[i].name.equals(topic_name)) {
            output = i;
        }
    }
    return output;
}

int Broker::addNewTopic(const SimpleString& topic_name) {
    int output = -1;
    if (topic_count < MAX_TOPICS) {   
        this->topics[topic_count] = Topic(topic_name);
        output = topic_count;
        topic_count++;
    }    
    return output;
}