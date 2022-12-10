#ifndef INPUT_PARAMETERS
#define INPUT_PARAMETERS

#include "socketAddress.hpp"
#include <string>
class InputParameters {
public:
    bool isInputting;
    bool isHotConnection;
    std::string filePath;
    std::string text;
    SocketAddress socket;
};

#endif
