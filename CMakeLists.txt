cmake_minimum_required (VERSION 3.21)
project(recons VERSION 1.0.0 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if (DEFINED ENV{VCPKG_ROOT})
  if (WIN32)
    # Special path for Windows -- $VCPKG_ROOT global variable must be set.
    include("$ENV{VCPKG_ROOT}\\scripts\\buildsystems\\vcpkg.cmake")
  else()
    # Special path for other OS -- $VCPKG_ROOT global variable must be set.
    include("$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake")
  endif()
endif()

find_package(Clipper2 CONFIG REQUIRED)
find_package(raylib CONFIG REQUIRED)
find_package(OpenCV REQUIRED)
find_package(Eigen3 CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(Catch2 CONFIG REQUIRED)

file(GLOB_RECURSE SOURCES "src/*.cpp")
file(GLOB_RECURSE HEADERS "src/*.hpp")
list(REMOVE_ITEM SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/Main.cpp")
add_library(recons_lib ${SOURCES} ${HEADERS})

target_link_libraries(recons_lib PUBLIC
  Clipper2::Clipper2
  raylib
  ${OpenCV_LIBS}
  Eigen3::Eigen
  nlohmann_json::nlohmann_json
)

add_executable(recons "src/Main.cpp")
target_link_libraries(recons PRIVATE recons_lib)
add_subdirectory(tests)
