#ifndef OPENCL_INIT_NPLEIC
#define OPENCL_INIT_NPLEIC

#include <stdio.h>
#include <CL/cl.h>

#define NUM_PLATFORMS 5
#define NUM_DEVICES 10

cl_platform_id platform;
cl_device_id device;
cl_context context;

int initOCL();

void destryoOCL();

static int chooseDevice();

#endif