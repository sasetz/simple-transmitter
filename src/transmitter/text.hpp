#ifndef SIMPLE_TRANSMITTER_TEXT_HPP
#define SIMPLE_TRANSMITTER_TEXT_HPP

#include <utility>
#include "dataEntity.hpp"

class Text : public DataEntity {
    Text(std::string text): DataEntity(std::move(text)) {};
};


#endif //SIMPLE_TRANSMITTER_TEXT_HPP
