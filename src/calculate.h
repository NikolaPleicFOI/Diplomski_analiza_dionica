#include <stdlib.h>
#include "CSVreading.h"
#include "OpenCLInteraction/CLinit.h"

#define WRITE_FILE "A.txt"
#define CLV_FILE "CLV.cl"

int distIndex(TradingDay* days, size_t daysCount);

static int writeResults(float* res, size_t size);