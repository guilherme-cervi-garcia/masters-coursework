#ifndef UTILS_H
#define UTILS_H

#include "Types.h"
#include <iostream>
#include <iomanip>

// Only the utilities we actually need right now

// Simple timestamp function
uint64_t getCurrentTimestamp();

// Simple string class to avoid external dependencies
class SimpleString {
private:
    char data[50];
    int length;
    
public:
    SimpleString();
    SimpleString(const char* str);
    SimpleString(const SimpleString& other);
    SimpleString& operator=(const SimpleString& other);
    SimpleString& operator=(const char* str);
    
    bool equals(const SimpleString& other) const;
    bool equals(const char* str) const;
    const char* c_str() const;
    int size() const;
    
private:
    void copyFrom(const char* str);
    int stringLength(const char* str) const;
    void stringCopy(char* dest, const char* src, int max_len) const;
};


// Simple random number generation
int simpleRand();
float simpleRandFloat(float min, float max);

#endif // UTILS_H