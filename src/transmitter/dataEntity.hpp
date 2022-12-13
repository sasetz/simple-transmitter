#ifndef SIMPLE_TRANSMITTER_DATAENTITY_HPP
#define SIMPLE_TRANSMITTER_DATAENTITY_HPP

#include <string>
#include <utility>

class DataEntity {
public:
    explicit DataEntity(std::string value): value(std::move(value)) {};
    std::string value;
};


#endif //SIMPLE_TRANSMITTER_DATAENTITY_HPP
