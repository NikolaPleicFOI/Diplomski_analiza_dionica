#ifndef NPLEIC_CALCULATE
#define NPLEIC_CALCULATE

#include <stdlib.h>
#include "CSVreading.h"
#include "OpenCLInteraction/CLinit.h"

#define WRITE_FILE "A.txt"

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
static DaysData *dates;

void initCalcValues(DaysData *dates, size_t daysCount);

int prepareADIndex(TradingDay *data);
int executeADIndex();

int prepareMomentum(TradingDay *data);
int executeMomentum();

int prepareMovingAverage(TradingDay* data);
int executeMovingAverage();

static int writeResults(float* res, int offset, char* prefix);

#endif