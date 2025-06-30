
cmake_minimum_required (VERSION 3.21)

if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("Objects")
find_package(Clipper2 CONFIG REQUIRED)
find_package(raylib CONFIG REQUIRED)
find_package(OpenCV CONFIG REQUIRED)
find_package(tomlplusplus CONFIG REQUIRED)

add_executable (Objects "src/Objects.cpp")

target_link_libraries(Objects PRIVATE
    Clipper2::Clipper2
    raylib
    ${OpenCV_LIBS}
    tomlplusplus::tomlplusplus
)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET Objects PROPERTY CXX_STANDARD 20)
endif()