#ifndef OPENCL_INIT_NPLEIC
#define OPENCL_INIT_NPLEIC

#include <CL/cl.h>
#include "../CSVreading.h"

#define NUM_PLATFORMS 5
#define NUM_DEVICES 10

#define KERNELS_FOLDER "..\\..\\..\\src\\kernels\\"

typedef struct clProgramData {
	cl_kernel kernel;
	cl_mem inBuff;
	cl_mem resBuff;
}clProgramData;

cl_platform_id platform;
cl_device_id device;
size_t maxItems;
size_t *maxPerDim;
cl_context context;
cl_command_queue queue;

int initOCL();

int prepareKernel(TradingDay* days, size_t daysCount, clProgramData* data, char* file);

void destryoOCL();

static int chooseDevice();

static int kernelSetup(clProgramData *data, char *file);

float* execute(size_t size, clProgramData *data);

#endif