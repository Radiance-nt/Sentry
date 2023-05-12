#ifndef HANDLERUTILS_HPP
#define HANDLERUTILS_HPP

#include <sstream>
#include <iostream>
template<class type>
char *serialize(type &data) {
    size_t size = sizeof(data);
    char *data_char = new char[size];
    memcpy(data_char, &data, size);
    return data_char;
}

template<class type>
type deserialize(const char *data_str) {
    type readData;
    memcpy(&readData, data_str, sizeof(readData));
    return readData;
}
#endif
