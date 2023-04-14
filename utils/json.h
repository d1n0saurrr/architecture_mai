#ifndef JSON_UTILS_H
#define JSON_UTILS_H

#include <string>
#include <iostream>
#include <Poco/JSON/Parser.h>

template<typename T>
T getOrDefault(Poco::JSON::Object::Ptr object, std::string field, T defaultValue) {
    if (object->has(field)) {
        return object->getValue<T>(field);
    }
    return defaultValue;
};

#endif