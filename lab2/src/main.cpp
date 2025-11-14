#include "gui.h"
#include <iostream>

int main() {
    try {
        ImageProcessingGUI app;
        app.run();
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return -1;
    }
}
