#include <stdlib.h>
#include "CSVreading.h"
#include "OpenCLInteraction/CLinit.h"

#define WRITE_FILE "A.txt"
#define CLV_FILE "CLV.cl"
#define ADI_PREFIX "ADI_"

clProgramData adi;
size_t totalDays;

int prepareADIndex(TradingDay* days, size_t daysCount);

int executeADIndex();

static int writeResults(float* res, size_t size);