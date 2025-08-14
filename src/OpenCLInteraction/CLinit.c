#include "CLinit.h"
#include <stdlib.h>
#include <stdio.h>
#include <windows.h>

int initOCL() {
    cl_int clerr = CL_SUCCESS;
    int err = chooseDevice();
    if (err != 0) return err;

    char *info = malloc(INFO_STRING_SIZE);
    clerr = clGetPlatformInfo(platform, CL_PLATFORM_NAME, INFO_STRING_SIZE, info, NULL);
    if (clerr == CL_SUCCESS) {
        printf("Odabrana platforma %s\n", info);
    }
    clerr = clGetPlatformInfo(platform, CL_PLATFORM_VERSION, INFO_STRING_SIZE, info, NULL);
    if (clerr == CL_SUCCESS) {
        printf("Podrzava OpenCl verziju %s\n", info);
    }
    clerr = clGetDeviceInfo(device, CL_DEVICE_NAME, INFO_STRING_SIZE, info, NULL);
    if (clerr == CL_SUCCESS) {
        printf("Odabran device %s\n", info);
    }
    free(info);

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

int prepareKernel(TradingDay* trade, size_t daysCount, clProgramData* data, char* kernelFile) {
    cl_int clerr = CL_SUCCESS;

    data->inBuff = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR, sizeof(TradingDay) * daysCount, trade, &clerr);
    data->resBuff = clCreateBuffer(context, CL_MEM_WRITE_ONLY, daysCount * sizeof(float), NULL, &clerr);
    if (clerr != CL_SUCCESS) {
        printf("Nisam uspio napraviti buffer\n");
        return 6;
    }

    int err = kernelSetup(data, kernelFile);
    if (err != 0) return err;
}

void destryoOCL() {
    free(maxPerDim);
    clReleaseCommandQueue(queue);
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
            clerr = clGetDeviceInfo(clDevices[j], CL_DEVICE_AVAILABLE, sizeof (available), &available, NULL);
            if (clerr != CL_SUCCESS || available == CL_FALSE) continue;

            //Dohvati device sa najvise memorije
            clerr = clGetDeviceInfo(clDevices[j], CL_DEVICE_GLOBAL_MEM_SIZE, sizeof (memsize), &memsize, NULL);
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

static int kernelSetup(clProgramData *data, char *kernelFile) {
    cl_int err = CL_SUCCESS;

    if (getGPUProgram(kernelFile, data) != 0) {
        printf("Nisam uspio dobiti GPU program za datoteku %s\n", kernelFile);
        return -1;
    }

    data->kernel = clCreateKernel(data->program, data->kernelName, &err);
    if (err != CL_SUCCESS) {
        printf("Nisam uspio napraviti kernel, greska %d\n", err);
        return 10;
    }
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
    err = clSetKernelArg(data->kernel, 2, sizeof(data->offset), &data->offset);
    if (err != CL_SUCCESS) {
        printf("Greska pri dodavanju argumenata kernelu: %d\n", err);
        return 13;
    }
    return 0;
}

static int getGPUProgram(char *kernelFile, clProgramData *progData) {
    size_t count = 0;
    WIN32_FIND_DATA fdata;
    char *path = malloc(512);
    if (path == NULL) {
        printf("malloc je bacio gresku\n");
        return -1;
    }
    CreateDirectory(COMPILED_PROGRAMS_FOLDER, NULL);

    sprintf(path, "%s%s", COMPILED_PROGRAMS_FOLDER, kernelFile);
    HANDLE hFind = FindFirstFile(path, &fdata);
    if (hFind == INVALID_HANDLE_VALUE) {
        progData->binaryPostoji = false;
        printf("Nema vec spremljenih binary-a na %s, idem buildati\n", path);
        if (getProgramFromSource(kernelFile, &progData->program) != 0) {
            FindClose(hFind);
            free(path);
            return -1;
        }
    }
    else {
        progData->binaryPostoji = true;
        printf("Pronasao binary, ucitavam iz %s\n", path);
        if (getProgramFromBinary(path, &progData->program) != 0) {
            printf("Idem buildati iz izvornog koda\n");
            if (getProgramFromSource(kernelFile, &progData->program) != 0) {
                FindClose(hFind);
                free(path);
                return -1;
            }
        }
    }
    FindClose(hFind);
    free(path);
    return 0;
}

static int getProgramFromSource(char *kernelFile, cl_program *prog){
    cl_int err = CL_SUCCESS;
    FILE *f = NULL;
    char *program_source;
    size_t size;
    char filePath[256] = KERNELS_FOLDER;
    strcat(filePath, kernelFile);
    
    f = fopen(filePath, "r");
    if (f == NULL) {
        printf("Nisam uspio otvoriti datoteku izvornog koda: %s\n", filePath);
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

    size++;

    *prog = clCreateProgramWithSource(context, 1, &program_source, &size, &err);
    if (err != CL_SUCCESS) {
        printf("Nisam uspio napraviti program\n");
        return 8;
    }

    free(program_source);

    err = clBuildProgram(*prog, 1, &device, NULL, NULL, NULL);
    if (err != CL_SUCCESS) {
        printCLErrors(prog, size);
        return 9;
    }
    return 0;
}

int storeBinaryProgram(cl_program *prog, char *path) {
    printf("Idem spremiti binary na %s\n", path);
    cl_int err = CL_SUCCESS;
    size_t binSize;

    err = clGetProgramInfo(*prog, CL_PROGRAM_BINARY_SIZES, sizeof(size_t), &binSize, NULL);
    if (err != CL_SUCCESS) return -1;

    unsigned char** binary = malloc(sizeof(unsigned char*));
    if (binary == NULL) {
        printf("malloc je bacio gresku\n");
        return -1;
    }
    binary[0] = malloc(binSize);
    if (binary[0] == NULL) {
        printf("malloc je bacio gresku\n");
        return -1;
    }

    err = clGetProgramInfo(*prog, CL_PROGRAM_BINARIES, binSize, binary, NULL);

    FILE* f = NULL;
    f = fopen(path, "wb");
    if (f == NULL) {
        printf("Nisam uspio otvoriti datoteku za pisanje:%s\n", path);
        return -1;
    }
    fwrite(binary[0], sizeof(char), binSize, f);
    fflush(f);
    fclose(f);
    free(binary[0]);
    return 0;
}

static int getProgramFromBinary(char* binPath, cl_program *prog) {
    size_t binSize;
    char *binary;

    FILE* f = NULL;
    f = fopen(binPath, "rb");
    if (f == NULL) {
        printf("Nisam uspio otvoriti datoteku sa GPU binary-em %s\n", binPath);
        return -1;
    }
    fseek(f, 0, SEEK_END);
    binSize = ftell(f);
    rewind(f);

    binary = malloc(binSize);
    if (binary == NULL) {
        printf("malloc je bacio gresku\n");
        return -1;
    }
    fread(binary, sizeof(char), binSize, f);
    fclose(f);
    
    cl_int err;
    *prog = clCreateProgramWithBinary(context, 1, &device, &binSize, &binary, NULL, &err);
    free(binary);
    if (err != CL_SUCCESS) {
        printf("Stvaranje programa nije uspjelo, error %d\n", err);
        return -1;
    }

    err = clBuildProgram(*prog, 1, &device, NULL, NULL, NULL);
    if (err != CL_SUCCESS) {
        printf("Buildanje programa nije uspjelo, error %d\n", err);
        return -1;
    }
    return 0;
}

static void printCLErrors(cl_program *program, size_t size) {
    clGetProgramBuildInfo(*program, device, CL_PROGRAM_BUILD_LOG, 0, NULL, &size);
    char *program_log = malloc(size + 1);
    if (program_log == NULL) {
        printf("malloc je bacio gresku\n");
        return;
    }
    clGetProgramBuildInfo(*program, device, CL_PROGRAM_BUILD_LOG, size + 1, program_log, NULL);
    program_log[size] = '\0';
    printf("%s\n", program_log);
    free(program_log);
}

float* execute(size_t size, clProgramData *data) {
    cl_int err = CL_SUCCESS;

    err = clEnqueueNDRangeKernel(queue, data->kernel, 1, NULL, &size, NULL,
        0, NULL, &data->execEvent);
    if (err != CL_SUCCESS) {
        printf("Greska pri stavljanju kernela u queue %d\n", err);
        return NULL;
    }

    float* res = malloc(sizeof(float) * size);
    if (res == NULL) {
        printf("malloc je bacio gresku\n");
        return NULL;
    }
    err = clEnqueueReadBuffer(queue, data->resBuff, CL_FALSE, 0, size * sizeof(float), res,
        1, &data->execEvent, &data->readEvent);
    if (err != CL_SUCCESS) {
        printf("Greska pri citanju buffera\n");
        return NULL;
    }

    return res;
}