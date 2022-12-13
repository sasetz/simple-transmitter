#ifndef SIMPLE_TRANSMITTER_FILE_HPP
#define SIMPLE_TRANSMITTER_FILE_HPP


#include "dataEntity.hpp"

class File : public DataEntity {
public:
    std::string homePath;

    File(std::string foreignPath, std::string homePath) : DataEntity(std::move(foreignPath)), homePath(homePath) {};
};


#endif //SIMPLE_TRANSMITTER_FILE_HPP
