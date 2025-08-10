#include "calculate.h"
#include <stdio.h>
#include <math.h>

int prepareADIndex(TradingDay *days, size_t daysCount) {
    totalDays = daysCount;
    char* file = CLV_FILE;

    int err = prepareKernel(days, daysCount, &adi, file);
    if (err != 0) {
        printf("Dogodila se graska %u\n", err);
        return err;
    }
}

int executeADIndex() {
    float* res = execute(totalDays, &adi);
    if (res == NULL) {
        return -1;
    }

    res[0] = 0;
    for (int i = 1; i < totalDays; i++) {
        if (isnan(res[i])) {
            res[i] = res[i - 1];
        }
        else {
            res[i] = res[i - 1] + res[i];
        }
    }
    return writeResults(res, totalDays);
}

static int writeResults(float *res, size_t size) {
    FILE *f = NULL;
    f = fopen(ADI_PREFIX WRITE_FILE, "w");
    if (f == NULL) {
        printf("Failed to open file for writing results\n");
        return -1;
    }

    for (int i = 1; i < size; i++) {
        fprintf(f, "%u: %f\n", i, res[i]);
    }
    fclose(f);
    free(res);
    return 0;
}