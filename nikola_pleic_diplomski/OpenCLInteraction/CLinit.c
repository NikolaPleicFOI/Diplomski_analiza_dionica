#include "CLinit.h"

int initOCL() {
    cl_int clerr = CL_SUCCESS;

    int res = chooseDevice(&device, &platform);
    if (res != 0) return res;

    cl_context_properties prop[3] = {CL_CONTEXT_PLATFORM, platform, 0};

    context = clCreateContext(prop, 1, &device, NULL, NULL, clerr);
    if (clerr != CL_SUCCESS) return 4;
    
    return 0;
}

void destryoOCL() {
    clReleaseDevice(device);
    clReleaseContext(context);
}

static int chooseDevice() {
    cl_platform_id platforms[NUM_PLATFORMS];
    cl_uint numPlatforms;
    cl_int clerr = CL_SUCCESS;

    clerr = clGetPlatformIDs(NUM_PLATFORMS, platforms, &numPlatforms);

    if (clerr != CL_SUCCESS) return 1;

    cl_device_id clDevices[NUM_DEVICES];
    uint32_t devSize = 0;
    for (int i = 0; i < numPlatforms; i++) {
        int numDevices;
        clerr = clGetDeviceIDs(platforms[i], CL_DEVICE_TYPE_ALL, NUM_DEVICES, clDevices, &numDevices);
        if (clerr != CL_SUCCESS) return 2;

        cl_bool available;
        cl_ulong memsize;
        for (int j = 0; j < numDevices; j++){
            clerr = clGetDeviceInfo(clDevices[j], CL_DEVICE_AVAILABLE, sizeof available, &available, NULL);
            if (clerr != CL_SUCCESS || available == CL_FALSE) return 3;

            //Dohvati device sa najvise memorije
            clerr = clGetDeviceInfo(clDevices[j], CL_DEVICE_GLOBAL_MEM_SIZE, sizeof memsize, &memsize, NULL);
            if (clerr != CL_SUCCESS) return 3;

            int currSize = memsize / (1024 * 1024);
            if (currSize > devSize) {
                device = clDevices[j];
                platform = platforms[i];
            }
        }
    }
    return 0;
}
