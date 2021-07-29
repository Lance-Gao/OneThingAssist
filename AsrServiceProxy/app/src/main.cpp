#include <iostream>
#include "pipeline.h"

int main(int argc, char** argv) {
    int ret = 0;

    Pipeline pipeline;
    ret = pipeline.run(argc, argv);
    if (ret == 0) {
      std::cout << "main ret is "
                << ret << "."
                << std::endl;
    }

    return ret;
}
