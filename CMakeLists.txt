cmake_minimum_required(VERSION 3.21)
project(recons VERSION 1.0.0 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Disable unnecessary libraries features
function(configure_dependency VAR VAL)
    set(${VAR} ${VAL} CACHE INTERNAL "" FORCE)
endfunction()

configure_dependency(CLIPPER2_TESTS OFF)
configure_dependency(CLIPPER2_EXAMPLES OFF)
configure_dependency(JSON_BuildTests OFF)
configure_dependency(EIGEN_BUILD_DOC OFF)
configure_dependency(EIGEN_BUILD_TESTING OFF)
configure_dependency(CATCH_BUILD_TESTING OFF)
configure_dependency(CATCH_BUILD_EXAMPLES OFF)
configure_dependency(CATCH_INSTALL_DOCS OFF)

set(RAYLIB_BACKEND "X11" CACHE STRING "Linux backend for raylib: X11 or Wayland")
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

if(WIN32)
    set(PLATFORM Desktop CACHE STRING "" FORCE)
elseif(UNIX AND NOT APPLE)
    set(PLATFORM Desktop CACHE STRING "" FORCE)
    if(RAYLIB_BACKEND STREQUAL "Wayland")
        set(GLFW_BUILD_WAYLAND ON CACHE BOOL "" FORCE)
        set(GLFW_BUILD_X11 OFF CACHE BOOL "" FORCE)
    else()
        set(GLFW_BUILD_WAYLAND OFF CACHE BOOL "" FORCE)
        set(GLFW_BUILD_X11 ON CACHE BOOL "" FORCE)
    endif()
endif()

configure_dependency(BUILD_LIST "core,imgproc,imgcodecs")
set(OPENCV_DISABLE_LIST BUILD_EXAMPLES BUILD_TESTS BUILD_PERF_TESTS
    BUILD_DOCS BUILD_opencv_apps BUILD_opencv_js BUILD_ANDROID_PROJECTS 
    BUILD_ANDROID_EXAMPLES BUILD_JAVA BUILD_OBJC BUILD_KOTLIN_EXTENSIONS)

foreach(ITEM ${OPENCV_DISABLE_LIST})
    configure_dependency(${ITEM} OFF)
endforeach()

set(OPENCV_UI_BACKENDS WITH_GTK WITH_QT WITH_VTK WITH_FFMPEG
    WITH_GSTREAMER WITH_V4L WITH_AVFOUNDATION WITH_MSMF WITH_DSHOW)

foreach(BACKEND ${OPENCV_UI_BACKENDS})
    configure_dependency(${BACKEND} OFF)
endforeach()

set(OPENCV_HW_ACCELERATION WITH_CUDA WITH_OPENCL WITH_TBB WITH_IPP
    WITH_OPENGL WITH_VULKAN WITH_ITT WITH_VA WITH_VA_INTEL)

foreach(HW ${OPENCV_HW_ACCELERATION})
    configure_dependency(${HW} OFF)
endforeach()

set(OPENCV_IMAGE_FORMATS WITH_PNG WITH_JPEG WITH_TIFF WITH_WEBP
    WITH_AVIF WITH_JASPER WITH_OPENJPEG WITH_JPEGXL WITH_IMGCODEC_GIF
    WITH_IMGCODEC_HDR WITH_IMGCODEC_SUNRASTER WITH_IMGCODEC_PXM 
    WITH_IMGCODEC_PFM WITH_OPENEXR)

foreach(FORMAT ${OPENCV_IMAGE_FORMATS})
    configure_dependency(${FORMAT} OFF)
endforeach()

set(OPENCV_3RDPARTY_BUILDS OPENCV_FORCE_3RDPARTY_BUILD BUILD_ZLIB
    BUILD_PNG BUILD_JPEG BUILD_TIFF BUILD_OPENEXR BUILD_WEBP)

foreach(BUILD ${OPENCV_3RDPARTY_BUILDS})
    configure_dependency(${BUILD} OFF)
endforeach()

configure_dependency(WITH_EIGEN OFF)
configure_dependency(WITH_PROTOBUF OFF)
configure_dependency(WITH_FLATBUFFERS OFF)
set(OPENCV_CONFIG_FILE_INCLUDE_DIR "${CMAKE_CURRENT_BINARY_DIR}/opencv_build" CACHE PATH "" FORCE)

# Add each dependency of the project
add_subdirectory(external/Clipper2/CPP)
add_subdirectory(external/json)
add_subdirectory(external/eigen)
add_subdirectory(external/Catch2)
add_subdirectory(external/raylib)
add_subdirectory(external/opencv opencv_build)

# Obtain all the files from the project
file(GLOB_RECURSE RECONS_SOURCES "src/*.cpp")
file(GLOB_RECURSE RECONS_HEADERS "src/*.hpp")
list(REMOVE_ITEM RECONS_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/Main.cpp")

add_library(recons_lib ${RECONS_SOURCES} ${RECONS_HEADERS})
target_include_directories(recons_lib PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
    "${CMAKE_CURRENT_SOURCE_DIR}/external/opencv/modules/core/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/external/opencv/modules/imgproc/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/external/opencv/modules/imgcodecs/include"
    "${CMAKE_CURRENT_BINARY_DIR}/opencv_build"
)

# Link all dependencies
target_link_libraries(recons_lib PUBLIC
    Clipper2
    raylib
    opencv_core
    opencv_imgproc
    opencv_imgcodecs
    Eigen3::Eigen
    nlohmann_json::nlohmann_json
)

add_executable(recons "src/Main.cpp")
target_link_libraries(recons PRIVATE recons_lib)
