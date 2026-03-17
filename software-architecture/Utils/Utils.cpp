#include "Utils.h"

// Simple timestamp function
uint64_t getCurrentTimestamp() {
    static uint64_t counter = 1000;
    return counter++;
}

// Simple random number generator
int simpleRand() {
    static int seed = 12345;
    seed = (seed * 1103515245 + 12345) & 0x7fffffff;
    return seed;
}

float simpleRandFloat(float min, float max) {
    return min + (max - min) * (simpleRand() % 1000) / 1000.0f;
}


// ============== SimpleString Implementation ==============

SimpleString::SimpleString() {
    data[0] = '\0';
    length = 0;
}

SimpleString::SimpleString(const char* str) {
    copyFrom(str);
}

SimpleString::SimpleString(const SimpleString& other) {
    copyFrom(other.data);
}

SimpleString& SimpleString::operator=(const SimpleString& other) {
    if (this != &other) {
        copyFrom(other.data);
    }
    return *this;
}

SimpleString& SimpleString::operator=(const char* str) {
    copyFrom(str);
    return *this;
}

bool SimpleString::equals(const SimpleString& other) const {
    if (length != other.length) return false;
    
    for (int i = 0; i < length; i++) {
        if (data[i] != other.data[i]) return false;
    }
    return true;
}

bool SimpleString::equals(const char* str) const {
    int str_len = stringLength(str);
    if (length != str_len) return false;
    
    for (int i = 0; i < length; i++) {
        if (data[i] != str[i]) return false;
    }
    return true;
}

const char* SimpleString::c_str() const {
    return data;
}

int SimpleString::size() const {
    return length;
}

void SimpleString::copyFrom(const char* str) {
    if (str == nullptr) {
        data[0] = '\0';
        length = 0;
        return;
    }
    
    stringCopy(data, str, sizeof(data) - 1);
    length = stringLength(data);
}

int SimpleString::stringLength(const char* str) const {
    if (str == nullptr) return 0;
    
    int len = 0;
    while (str[len] != '\0' && len < 49) { // Max 49 chars + null terminator
        len++;
    }
    return len;
}

void SimpleString::stringCopy(char* dest, const char* src, int max_len) const {
    int i = 0;
    while (src[i] != '\0' && i < max_len) {
        dest[i] = src[i];
        i++;
    }
    dest[i] = '\0';
}
