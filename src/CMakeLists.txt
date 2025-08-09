# CMakeList.txt : CMake project for nikola_pleic_diplomski, include source and define
# project specific logic here.
#

# Add source to this project's executable.
add_executable (nikola_pleic_diplomski "nikola_pleic_diplomski.c" "nikola_pleic_diplomski.h")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET nikola_pleic_diplomski PROPERTY C_STANDARD 17
                                                 C_STANDARD_REQUIRED ON
                                                 C_EXTENSIONS OFF)
endif()

# TODO: Add tests and install targets if needed.
