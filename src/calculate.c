#include "calculate.h"
#include <stdio.h>
#include <math.h>
#include <string.h>

void initCalcValues(DaysData* datesData, size_t daysCount) {
    totalDays = daysCount;
    dates = datesData;

    adi.offset = 0;
    adi.kernelName = ADI_KERNEL_NAME;
    momentum.offset = MOMEN_DAYS_OFFSET;
    momentum.kernelName = MOMEN_KERNEL_NAME;
    ma.offset = MA_DAYS_OFFSET;
    ma.kernelName = MA_KERNEL_NAME;
}

int prepareADIndex(TradingDay *data) {
    char *file = CLV_FILE;
    
    int err = prepareKernel(data, totalDays, &adi, file);
    if (err != 0) {
        printf("Dogodila se graska %u\n", err);
        return err;
    }
    return 0;
}

int executeADIndex() {
    float *res = execute(totalDays, &adi);
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

    char *prefix = malloc(32);
    if (prefix == NULL) {
        printf("malloc nije uspio\n");
        return - 1;
    }
    strcpy(prefix, ADI_PREFIX);
    int err = writeResults(res, 1, prefix);
    free(prefix);
    return err;
}

int prepareMomentum(TradingDay *data) {
    char *file = MOMEN_FILE;
    int err = prepareKernel(data, totalDays, &momentum, file);
    if (err != 0) {
        printf("Dogodila se graska %u\n", err);
        return err;
    }
    return 0;
}

int executeMomentum() {
    float *res = execute(totalDays, &momentum);
    if (res == NULL) {
        return -1;
    }
    char *prefix = malloc(32);
    if (prefix == NULL) {
        printf("malloc nije uspio\n");
        return -1;
    }
    strcpy(prefix, MOMEN_PREFIX);
    int err = writeResults(res, MOMEN_DAYS_OFFSET, prefix);
    free(prefix);
    return err;
}

int prepareMovingAverage(TradingDay* data) {
    char* file = MA_FILE;
    int err = prepareKernel(data, totalDays, &ma, file);
    if (err != 0) {
        printf("Dogodila se graska %u\n", err);
        return err;
    }
    return 0;
}

int executeMovingAverage() {
    float* res = execute(totalDays, &ma);
    if (res == NULL) {
        return -1;
    }
    char* prefix = malloc(32);
    if (prefix == NULL) {
        printf("malloc nije uspio\n");
        return -1;
    }
    strcpy(prefix, MA_PREFIX);
    int err = writeResults(res, MA_DAYS_OFFSET, prefix);
    free(prefix);
    return err;
}

static int writeResults(float *res, int offset, char *prefix) {
    char *filename = WRITE_FILE;
    FILE *f = NULL;
    strcat(prefix, filename);
    f = fopen(prefix, "w");
    if (f == NULL) {
        printf("Failed to open file for writing results\n");
        return -1;
    }

    int i = 0, end = totalDays;
    if (offset > 0) {
        i += offset;
    }
    else {
        end -= offset;
    }
    while (i < end) {
        fprintf(f, "%u-%02u-%02u:%20f\n", dates[i].year, dates[i].month, dates[i].day, res[i]);
        i++;
    }
    fclose(f);
    free(res);
    return 0;
}