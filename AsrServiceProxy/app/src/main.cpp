#include <iostream>
#include "application.h"

int main(int argc, char** argv) {
    int ret = 0;

    Application app;
    ret = app.run(argc, argv);

    return ret;
}
