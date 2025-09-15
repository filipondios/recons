#include <algorithm>
#include <cmath>
#include <exception>
#include <iostream>
#include <iterator>
#include <stdexcept>
#include <string>
#include <raylib.h>
#include "ModelRender.hpp"
#include "VoxelModel.hpp"


void print_help(void) {
    std::cout << "Usage: recons -p <path> [options]" << std::endl
        << "Options:" << std::endl
        << "    -p, --path <string>    Model path (required)"  << std::endl
        << "    -r, --resolution <int> Voxel space resolution" << std::endl
        << "    -i, --info             Print addditional info" << std::endl
        << "    -h, --help             Show this help message" << std::endl;
}

int main(int argc, char* argv[]) {
    // Model variables
    std::string path;
    int resolution {16};
    bool info {false};
    bool help {false};

    for (int i = 1; i < argc; ++i) {
        const std::string arg = argv[i];

        if ((arg == "--path" || arg == "-p") && (i + 1 < argc)) {
            path = argv[i + 1];
        }
        
        else if ((arg == "--info" || arg == "-i")) {
            info = true;
        }

        else if ((arg == "--help" || arg == "-h")) {
            help = true;
        }

        else if ((arg == "--resolution" || arg == "-r") && (i + 1 < argc)) {
            try {
                resolution = std::stoi(argv[i + 1]);
                if (resolution <= 0) {
                    throw std::invalid_argument("resolution must be positive");
                }
            } catch (const std::exception& e) { // catch -> throw lol
                throw std::invalid_argument("invalid resolution value");
            }
        }
    }

    if (help || path.empty()) {
        print_help();
        return help ? 0 : 1;
    }

    std::cout << "[+] Creating voxel model from " << path << std::endl;
    VoxelModel model(path, resolution, info);
    ModelRender render(&model);
    render.initialize_render_context();
    render.start_render_loop();
    return 0;
}
