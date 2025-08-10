#include "CLinit.h"
#include <stdlib.h>
#include <stdio.h>

int initOCL() {
    cl_int clerr = CL_SUCCESS;

    int err = chooseDevice(&device, &platform);
    if (err != 0) return err;

    cl_context_properties prop[3] = {CL_CONTEXT_PLATFORM, platform, 0};

    context = clCreateContext(prop, 1, &device, NULL, NULL, &clerr);
    if (clerr != CL_SUCCESS) {
        printf("Nisam uspio napraviti kontekst\n");
        return 4;
    }

    queue = clCreateCommandQueue(context, device, CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE, &clerr);
    if (clerr != CL_SUCCESS) {
        printf("Nisam uspio napraviti command queue\n");
        return 5;
    }

    return 0;
}

int prepareKernel(TradingDay* days, size_t daysCount, clProgramData* data, char* file) {
    cl_int clerr = CL_SUCCESS;

    data->inBuff = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR, sizeof(TradingDay) * daysCount, days, &clerr);
    data->resBuff = clCreateBuffer(context, CL_MEM_WRITE_ONLY, daysCount * sizeof(float), NULL, &clerr);
    if (clerr != CL_SUCCESS) {
        printf("Nisam uspio napraviti buffer\n");
        return 6;
    }

    int err = kernelSetup(data, file);
    if (err != 0) return err;
}

void destryoOCL() {
    clReleaseContext(context);
}

static int chooseDevice() {
    cl_platform_id platforms[NUM_PLATFORMS];
    cl_uint numPlatforms;
    cl_int clerr = CL_SUCCESS;

    clerr = clGetPlatformIDs(NUM_PLATFORMS, platforms, &numPlatforms);

    if (clerr != CL_SUCCESS) {
        printf("Nisam uspio dobiti ID platforme\n");
        return 1;
    }

    cl_device_id clDevices[NUM_DEVICES];
    uint32_t devSize = 0;
    for (int i = 0; i < numPlatforms; i++) {
        int numDevices;
        clerr = clGetDeviceIDs(platforms[i], CL_DEVICE_TYPE_GPU, NUM_DEVICES, clDevices, &numDevices);
        if (clerr != CL_SUCCESS) {
            printf("Nisam uspio dobiti ID device-a\n");
            return 2;
        }

        cl_bool available;
        cl_ulong memsize;
        for (int j = 0; j < numDevices; j++){
            clerr = clGetDeviceInfo(clDevices[j], CL_DEVICE_AVAILABLE, sizeof available, &available, NULL);
            if (clerr != CL_SUCCESS || available == CL_FALSE) return 3;

            //Dohvati device sa najvise memorije
            clerr = clGetDeviceInfo(clDevices[j], CL_DEVICE_GLOBAL_MEM_SIZE, sizeof memsize, &memsize, NULL);
            if (clerr != CL_SUCCESS) {
                printf("Nisam uspio dobiti informacije device-a\n");
                return 3;
            }

            int currSize = memsize / (1024 * 1024);
            if (currSize > devSize) {
                device = clDevices[j];
                platform = platforms[i];
            }
        }
    }
    
    clerr = clGetDeviceInfo(device, CL_DEVICE_MAX_WORK_GROUP_SIZE, sizeof(maxItems), &maxItems, NULL);
    if (clerr != CL_SUCCESS) {
        printf("Nisam uspio dobiti informacije device-a za maks velicinu work group-a\n");
        return 3;
    }

    cl_uint dim;
    clerr = clGetDeviceInfo(device, CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS, sizeof(dim), &dim, NULL);
    if (clerr != CL_SUCCESS) {
        printf("Nisam uspio dobiti informacije device-a za broj dimenzija\n");
        return 3;
    }
    
    maxPerDim = malloc(sizeof(size_t) * dim);

    clerr = clGetDeviceInfo(device, CL_DEVICE_MAX_WORK_ITEM_SIZES, sizeof(size_t) * dim, maxPerDim, NULL);
    if (clerr != CL_SUCCESS) {
        printf("Nisam uspio dobiti informacije device-a za velicine work grupa\n");
        return 3;
    }

    return 0;
}

static int kernelSetup(clProgramData *data, char *file) {
    char filePath[128] = KERNELS_FOLDER;
    strcat(filePath, file);

    cl_program program;
    FILE* f = NULL;
    char* program_source;
    size_t size;
    cl_int err = CL_SUCCESS;

    f = fopen(filePath, "r");
    if (f == NULL) {
        printf("Nisam uspio otvoriti datoteku: %s\n", filePath);
        return 7;
    }

    fseek(f, 0, SEEK_END);
    size = ftell(f);
    rewind(f);
    program_source = malloc(size + 1);
    if (program_source == NULL) {
        printf("malloc je bacio gresku\n");
        return -1;
    }
    fread(program_source, sizeof(char), size, f);
    program_source[size] = '\0';
    fclose(f);

    program = clCreateProgramWithSource(context, 1, &program_source, &size, &err);
    if (err != CL_SUCCESS) {
        printf("Nisam uspio napraviti program\n");
        return 8;
    }

    free(program_source);

    err = clBuildProgram(program, 1, &device, NULL, NULL, NULL);
    if (err != CL_SUCCESS) {
        clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG,
            0, NULL, &size);
        char *program_log = malloc(size + 1);
        if (program_log == NULL) {
            printf("malloc je bacio gresku\n");
            return -1;
        }
        
        clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG,
            size + 1, program_log, NULL);
        program_log[size] = '\0';
        printf("%s\n", program_log);
        free(program_log);

        return 9;
    }

    data->kernel = clCreateKernel(program, "CLV", &err);
    if (err != CL_SUCCESS) {
        printf("Nisam uspio napraviti kernel\n");
        return 10;
    }

    clUnloadCompiler();

    err = clSetKernelArg(data->kernel, 0, sizeof(data->inBuff), &data->inBuff);
    if (err != CL_SUCCESS) {
        printf("Greska pri dodavanju argumenata kernelu: %d\n", err);
        return 11;
    }
    err = clSetKernelArg(data->kernel, 1, sizeof(data->resBuff), &data->resBuff);
    if (err != CL_SUCCESS) {
        printf("Greska pri dodavanju argumenata kernelu: %d\n", err);
        return 12;
    }

    return 0;
}

float* execute(size_t size, clProgramData *data) {
    cl_int err = CL_SUCCESS;

    size_t sizes[] = {4, 8, 8};

    err = clEnqueueNDRangeKernel(queue, data->kernel, 1, NULL, &size, NULL, 0, NULL, NULL);
    if (err != CL_SUCCESS) {
        printf("Greska pri stavljanju kernela u queue %d\n", err);
        return NULL;
    }

    float* res = malloc(sizeof(float) * size);
    if (res == NULL) {
        printf("malloc je bacio gresku\n");
        return NULL;
    }

    err = clEnqueueReadBuffer(queue, data->resBuff, CL_TRUE, 0, size * sizeof(float), res, 0, NULL, NULL);
    if (err != CL_SUCCESS) {
        printf("Greska pri citanju buffera\n");
        return NULL;
    }

    err = clFinish(queue);
    if (err != CL_SUCCESS) {
        printf("Greska pri izvrsavanju kernela (nema memorije?)\n");
        return NULL;
    }

    return res;
}