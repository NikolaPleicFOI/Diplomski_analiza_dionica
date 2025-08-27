cmake_minimum_required (VERSION 3.8)

set(OpenCL_DIR "C:\\OpenCL-SDK\\share\\cmake\\OpenCL")
set(projFolder "src")
set(opencl_inter_folder "${projFolder}/OpenCLInteraction")

project(nikola_pleic_diplomski LANGUAGES C)

find_package(OpenCL CONFIG REQUIRED)
if(OpenCL_FOUND)
    message ("OpenCL ucitani")
else()
    message (FATAL_ERROR "Nisam pronasao OpenCL")
endif()

add_library(Utils STATIC ${projFolder}/CSVreading.c ${projFolder}/calculate.c ${opencl_inter_folder}/CLinit.c)

target_link_libraries(Utils PUBLIC OpenCL::OpenCL)

add_executable(${PROJECT_NAME} WIN32 ${projFolder}/nikola_pleic_diplomski.c)

target_link_libraries(${PROJECT_NAME} PRIVATE Utils)

set_target_properties(${PROJECT_NAME} PROPERTIES C_STANDARD 17
                                                 C_STANDARD_REQUIRED ON
                                                 C_EXTENSIONS OFF)

target_compile_definitions(${PROJECT_NAME} PRIVATE CL_TARGET_OPENCL_VERSION=100)

