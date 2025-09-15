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

    for (int i = 0; i < argc; ++i) {
        const std::string arg = argv[i];

        if ((arg == "--path" || "-p") && i + 1 < argc) {
            path = argv[++i];
        }
        
        else if ((arg == "--info" || arg == "-i")) {
            info = true;
        }

        else if ((arg == "-help" || "--h")) {
            help = true;
        }

        else if ((arg == "--resolution" || arg == "-r") && i + 1 < argc) {
            try {
                resolution = std::stoi(argv[++i]);
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


/*    
    // Initialize Raylib context
    SetConfigFlags(FLAG_FULLSCREEN_MODE);
    SetTraceLogLevel(LOG_ERROR);
    InitWindow(w, h, "Object reconstruction");
    SetTargetFPS(60);
    
    // Scale all UI variables
    const int monitor = GetCurrentMonitor();
    const int w = GetMonitorWidth(monitor);
    const int h = GetMonitorHeight(monitor);
    SetWindowSize(w, h);

    const int width_scale = w / base_width;    
    const int height_scale = h / base_height;
    box[0] *= width_scale;
    box[1] *= height_scale;    
    box[2] *= width_scale;
    box[3] *= height_scale;
    text_fontsize *= width_scale;
    */
    return 0;
}
