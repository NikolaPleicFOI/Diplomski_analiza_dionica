cmake_minimum_required (VERSION 3.8)

set(OpenCL_DIR "C:\\OpenCL-SDK\\share\\cmake\\OpenCL")
set(projFolder "nikola_pleic_diplomski")
set(opencl_inter_folder "nikola_pleic_diplomski/OpenCLInteraction")

project(nikola_pleic_diplomski LANGUAGES C)

find_package(OpenCL CONFIG REQUIRED)
if(OpenCL_FOUND)
    message ("OpenCL ucitani")
else()
    message (FATAL_ERROR "Nisam pronasao OpenCL")
endif()

add_library(CSVReading STATIC ${projFolder}/CSVreading.c)

add_library(CLinit STATIC ${opencl_inter_folder}/CLinit.c)
target_link_libraries(CLinit PUBLIC OpenCL::OpenCL)
target_link_libraries(CLinit PRIVATE CSVReading)

add_executable(${PROJECT_NAME} ${projFolder}/nikola_pleic_diplomski.c)

target_link_libraries(${PROJECT_NAME} PRIVATE CSVReading)
target_link_libraries(${PROJECT_NAME} PRIVATE CLinit)

set_target_properties(${PROJECT_NAME} PROPERTIES C_STANDARD 17
                                                 C_STANDARD_REQUIRED ON
                                                 C_EXTENSIONS OFF)

target_compile_definitions(${PROJECT_NAME} PRIVATE CL_TARGET_OPENCL_VERSION=100)

