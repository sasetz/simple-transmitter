#ifndef SIMPLE_TRANSMITTER_DATAENTITY_HPP
#define SIMPLE_TRANSMITTER_DATAENTITY_HPP


#include <string>
#include <utility>

class DataEntity {
public:
    enum class InputType {
        File,
        Text
    };
    DataEntity(InputType type, std::string payload): type(type), payload(std::move(payload)) {};

    InputType type;
    std::string payload;
};


#endif //SIMPLE_TRANSMITTER_DATAENTITY_HPP
