#ifndef CALCULATE_NPLEIC
#define CALCULATE_NPLEIC

#include <stdlib.h>
#include "CSVreading.h"
#include "OpenCLInteraction/CLinit.h"

#define OUT_FOLDER "results"
#define OUT_FILE_EXTENTION ".txt"

#define CLV_FILE "CLV.cl"
#define ADI_PREFIX "ADI_"
#define ADI_KERNEL_NAME "CLV"

#define MOMEN_FILE "momentum.cl"
#define MOMEN_PREFIX "MM_"
#define MOMEN_DAYS_OFFSET 5
#define MOMEN_KERNEL_NAME "momentum"

#define MA_FILE "moving_average.cl"
#define MA_PREFIX "MA_"
#define MA_DAYS_OFFSET 5
#define MA_KERNEL_NAME "MA"

static clProgramData adi;
static clProgramData momentum;
static clProgramData ma;

static size_t totalDays;
static ProgData *data;

int initCalcValues(ProgData *data, size_t daysCount);
float* enqueue(clProgramData *prog);

clProgramData* prepareADIndex();
int resultADIndex(float* res);

clProgramData* prepareMomentum();
int resultMomentum(float* res);

clProgramData* prepareMovingAverage();
int resultMovingAverage(float* res);

static int writeResults(float* res, int offset, char* prefix);

#endif