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
find_package(tomlplusplus CONFIG REQUIRED)
find_package(Eigen3 CONFIG REQUIRED)

file(GLOB_RECURSE SOURCES "src/*.cpp")
file(GLOB_RECURSE HEADERS "src/*.hpp")
add_executable(recons ${SOURCES} ${HEADERS})

target_link_libraries(recons PRIVATE
  Clipper2::Clipper2
  raylib
  ${OpenCV_LIBS}
  tomlplusplus::tomlplusplus
  Eigen3::Eigen
)
