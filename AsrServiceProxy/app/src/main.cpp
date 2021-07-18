#include <iostream>
#include "pipeline.h"

int main(int argc, char** argv) {
    int ret = 0;

    Pipeline pipeline;
    ret = pipeline.run(argc, argv);

    return ret;
}
