#ifndef OPENCL_INIT_NPLEIC
#define OPENCL_INIT_NPLEIC

#include <stdbool.h>
#include <CL/cl.h>
#include "../CSVreading.h"

#define NUM_PLATFORMS 5
#define NUM_DEVICES 10
#define INFO_STRING_SIZE 256

#define KERNELS_FOLDER "..\\..\\..\\src\\kernels\\"
#define COMPILED_PROGRAMS_FOLDER "compiled_GPU_programs\\"
#define KERNEL_FILE_EXTENTION ".cl"

typedef struct clProgramData {
	cl_program program;
	cl_kernel kernel;
	cl_mem inBuff;
	cl_mem resBuff;
	uint16_t offset;
	char *kernelName;
	cl_event readEvent;
	cl_event execEvent;
	bool binaryPostoji;
}clProgramData;

static cl_platform_id platform;
static cl_device_id device;
static size_t maxItems;
static size_t *maxPerDim;
static cl_context context;
static cl_command_queue queue;

int initOCL();

int prepareKernel(TradingDay* days, size_t daysCount, clProgramData* data, char* file);

void destryoOCL();

static int chooseDevice();

static int kernelSetup(clProgramData *data, char *file);

static int getGPUProgram(char* kernelFile, clProgramData* program);

static int getProgramFromSource(char* kernelFile, cl_program* prog);
static void printCLErrors(cl_program* program, size_t size);
int storeBinaryProgram(cl_program *data, char* path);
static int getProgramFromBinary(char* binPath, cl_program* prog);


float* execute(size_t size, clProgramData *data);

#endif