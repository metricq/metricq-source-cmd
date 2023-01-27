cmake_minimum_required(VERSION 3.8)

project(metricq-source-cmd VERSION 1.0)

include(cmake/DefaultBuildType.cmake)
include(cmake/GitSubmoduleUpdate.cmake)
git_submodule_update()

add_subdirectory(lib/metricq)

add_executable(metricq-source-cmd src/main.cpp src/cmd_source.cpp src/metric_executor.cpp)
target_compile_features(metricq-source-cmd PRIVATE cxx_std_17)
target_compile_options(metricq-source-cmd PRIVATE -Wall -Wextra -pedantic)

target_link_libraries(metricq-source-cmd
    PRIVATE
        metricq::source
        metricq::logger-nitro
        Nitro::options
)

install(TARGETS metricq-source-cmd RUNTIME DESTINATION bin)
