#include <getopt.h> // getting the arguments from cli
#include "input/inputParameters.hpp"

enum class InputMode {
    Interactive,
    Listening,
    SendFile,
    SendText,
    SendInput
};

void input(int argc, char* argv[]) {
    int opt;
    InputMode mode;
    bool listen = false, file = false, text = false, input = false, socket = false, port = false, address = false;

    while((opt = getopt(argc, argv, "l:f:t:is:p:a:")) != -1) {
        //case 'l':
            //listen = true;
    }
}
