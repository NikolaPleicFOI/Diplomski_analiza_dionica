cmake_minimum_required (VERSION 3.8)

set(OpenCL_DIR "E:\\stuff\\prog\\tools\\OpenCL-SDK-v2025.07.23-Win-x64\\share\\cmake\\OpenCL")
set(projFolder "nikola_pleic_diplomski")

project(nikola_pleic_diplomski LANGUAGES C)

find_package(OpenCL CONFIG REQUIRED)

add_library(CSVReading STATIC ${projFolder}/CSVreading.c ${projFolder}/CSVreading.h)

target_compile_features(CSVReading PUBLIC c_std_17)

add_executable(${PROJECT_NAME} ${projFolder}/nikola_pleic_diplomski.c "${projFolder}/CSVreading.c" "${projFolder}/CSVreading.h")

target_link_libraries(${PROJECT_NAME} PRIVATE OpenCL::OpenCL)
target_link_libraries(${PROJECT_NAME} PUBLIC CSVReading)

set_target_properties(${PROJECT_NAME} PROPERTIES C_STANDARD 17
                                                 C_STANDARD_REQUIRED ON
                                                 C_EXTENSIONS OFF)

target_compile_definitions(${PROJECT_NAME} PRIVATE CL_TARGET_OPENCL_VERSION=100)

