
file(GLOB_RECURSE SOURCES "*.cpp")
add_executable(recons_tests ${SOURCES})

target_include_directories(recons_tests PRIVATE ../src)
target_link_libraries(recons_tests PRIVATE
   recons_lib
   Catch2::Catch2
   Catch2::Catch2WithMain
)
