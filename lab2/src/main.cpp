#include "gui.h"
#include <iostream>

int main() {
    try {
        ImageProcessorGUI app;
        app.run();
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return -1;
    }
}