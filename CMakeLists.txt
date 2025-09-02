cmake_minimum_required(VERSION 3.21)
project(recons VERSION 1.0.0 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

function(cache_force VAR VAL)
  set(${VAR} ${VAL} CACHE INTERNAL "" FORCE)
endfunction()

# ==== Clipper2 ====
cache_force(CLIPPER2_TESTS OFF)
cache_force(CLIPPER2_EXAMPLES OFF)

# ==== nlohmann-json ====
cache_force(JSON_BuildTests OFF)

# ==== Eigen3 ====
cache_force(EIGEN_BUILD_DOC OFF)
cache_force(EIGEN_BUILD_TESTING OFF)

# ==== Catch2 ====
cache_force(CATCH_BUILD_TESTING OFF)
cache_force(CATCH_BUILD_EXAMPLES OFF)
cache_force(CATCH_INSTALL_DOCS OFF)

# ==== raylib ====
cache_force(RAYLIB_BUILD_EXAMPLES OFF)
cache_force(RAYLIB_BUILD_TESTS OFF)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(RAYLIB_BACKEND "X11" CACHE STRING "Linux backend for raylib: X11 or Wayland")
if (WIN32)
  set(PLATFORM Desktop CACHE STRING "" FORCE)
elseif(UNIX AND NOT APPLE)
  set(PLATFORM Desktop CACHE STRING "" FORCE)
  if (RAYLIB_BACKEND STREQUAL "Wayland")
    set(GLFW_BUILD_WAYLAND ON  CACHE BOOL "" FORCE)
    set(GLFW_BUILD_X11     OFF CACHE BOOL "" FORCE)
  else()
    set(GLFW_BUILD_WAYLAND OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_X11     ON  CACHE BOOL "" FORCE)
  endif()
endif()

# ==== OpenCV (core,imgproc,imgcodecs) ====
cache_force(BUILD_LIST "core,imgproc,imgcodecs")
cache_force(BUILD_EXAMPLES OFF)
cache_force(BUILD_TESTS OFF)
cache_force(BUILD_PERF_TESTS OFF)
cache_force(BUILD_DOCS OFF)
cache_force(BUILD_opencv_apps OFF)
cache_force(BUILD_opencv_js OFF)
cache_force(BUILD_PACKAGE OFF)
cache_force(BUILD_ANDROID_PROJECTS OFF)
cache_force(BUILD_ANDROID_EXAMPLES OFF)
cache_force(BUILD_JAVA OFF)
cache_force(BUILD_OBJC OFF)
cache_force(BUILD_KOTLIN_EXTENSIONS OFF)
cache_force(WITH_GTK OFF)
cache_force(WITH_QT OFF)
cache_force(WITH_VTK OFF)
cache_force(WITH_FFMPEG OFF)
cache_force(WITH_GSTREAMER OFF)
cache_force(WITH_V4L OFF)
cache_force(WITH_AVFOUNDATION OFF)
cache_force(WITH_MSMF OFF)
cache_force(WITH_DSHOW OFF)
cache_force(WITH_CUDA OFF)
cache_force(WITH_OPENCL OFF)
cache_force(WITH_TBB OFF)
cache_force(WITH_IPP OFF)
cache_force(WITH_OPENGL OFF)
cache_force(WITH_VULKAN OFF)
cache_force(WITH_ITT OFF)
cache_force(WITH_EIGEN OFF)
cache_force(WITH_PNG OFF)
cache_force(WITH_JPEG OFF)
cache_force(WITH_TIFF OFF)
cache_force(WITH_WEBP OFF)
cache_force(WITH_AVIF OFF)
cache_force(WITH_JASPER OFF)
cache_force(WITH_OPENJPEG OFF)
cache_force(WITH_JPEGXL OFF)
cache_force(WITH_IMGCODEC_GIF OFF)
cache_force(WITH_IMGCODEC_HDR OFF)
cache_force(WITH_IMGCODEC_SUNRASTER OFF)
cache_force(WITH_IMGCODEC_PXM OFF)
cache_force(WITH_IMGCODEC_PFM OFF)
cache_force(OPENCV_FORCE_3RDPARTY_BUILD OFF)
cache_force(BUILD_ZLIB OFF)
cache_force(BUILD_PNG OFF)
cache_force(BUILD_JPEG OFF)
cache_force(BUILD_TIFF OFF)
cache_force(BUILD_OPENEXR OFF)
cache_force(BUILD_WEBP OFF)

# ==== dependencies ====
add_subdirectory(third_party/clipper2)
add_subdirectory(third_party/nlohmann_json)
add_subdirectory(third_party/eigen)
add_subdirectory(third_party/catch2)
add_subdirectory(third_party/raylib)
add_subdirectory(third_party/opencv opencv_build)

# ==== project files ====
file(GLOB_RECURSE SOURCES "src/*.cpp")
file(GLOB_RECURSE HEADERS "src/*.hpp")
list(REMOVE_ITEM SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/Main.cpp")

add_library(recons_lib ${SOURCES} ${HEADERS})
target_include_directories(recons_lib PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src")
target_link_libraries(recons_lib PUBLIC
  Clipper2::Clipper2
  raylib
  opencv_core
  opencv_imgproc
  opencv_imgcodecs
  Eigen3::Eigen
  nlohmann_json::nlohmann_json
)

add_executable(recons "src/Main.cpp")
target_link_libraries(recons PRIVATE recons_lib)
#add_subdirectory(tests)
