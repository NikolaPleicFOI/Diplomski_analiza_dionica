#include "calculate.h"
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <windows.h>

int initCalcValues(ProgData *progData, size_t daysCount) {
    totalDays = daysCount;
    data = progData;

    CreateDirectory(OUT_FOLDER, NULL);
    adi.offset = 0;
    adi.kernelName = ADI_KERNEL_NAME;
    momentum.offset = MOMEN_DAYS_OFFSET;
    momentum.kernelName = MOMEN_KERNEL_NAME;
    ma.offset = MA_DAYS_OFFSET;
    ma.kernelName = MA_KERNEL_NAME;
    return 0;
}

clProgramData *prepareADIndex() {
    char *kernel = CLV_FILE;
    
    int err = prepareKernel(data->trades, totalDays, &adi, kernel);
    if (err != 0) {
        printf("Dogodila se graska %u\n", err);
        return NULL;
    }
    return &adi;
}

float *enqueue(clProgramData *prog) {
    float *res = execute(totalDays, prog);
    if (res == NULL) {
        return NULL;
    }
    return res;
}

int resultADIndex(float *res) {
    res[0] = 0;
    for (int i = 1; i < totalDays; i++) {
        if (isnan(res[i])) {
            res[i] = res[i - 1];
        }
        else {
            res[i] = res[i - 1] + res[i];
        }
    }

    int err = writeResults(res, 1, ADI_PREFIX);
    return err;
}

clProgramData *prepareMomentum() {
    char *kernel = MOMEN_FILE;
    int err = prepareKernel(data->trades, totalDays, &momentum, kernel);
    if (err != 0) {
        printf("Dogodila se graska %u\n", err);
        return NULL;
    }
    return &momentum;
}

int resultMomentum(float *res) {
    int err = writeResults(res, MOMEN_DAYS_OFFSET, MOMEN_PREFIX);
    return err;
}

clProgramData *prepareMovingAverage() {
    char *kernel = MA_FILE;
    int err = prepareKernel(data->trades, totalDays, &ma, kernel);
    if (err != 0) {
        printf("Dogodila se graska %u\n", err);
        return NULL;
    }
    return &ma;
}

int resultMovingAverage(float *res) {
    int err = writeResults(res, MA_DAYS_OFFSET, MA_PREFIX);
    return err;
}

static int writeResults(float *res, int offset, char *prefix) {
    char* filename[FILENAME_MAX];
    uint32_t curr = 0;
    for (int i = 0; i < data->numStocks; i++) {
        strcpy(filename, OUT_FOLDER "\\");
        strcat(filename, prefix);
        strcat(filename, data->stocks[i]);
        strcat(filename, OUT_FILE_EXTENTION);
        FILE *f = NULL;
        f = fopen(filename, "w");
        if (f == NULL) {
            printf("Nisam uspio otvoriti datoteku za pisanje:%s\n", filename);
            return -1;
        }
        int end = curr + data->numDays[i];
        curr += offset;
        int j = curr;

        while (j < end) {
            fprintf(f, "%u-%02u-%02u:%20f\n", data->days[curr].year, data->days[curr].month, data->days[curr].day, res[curr]);
            j++;
            curr++;
        }
        fflush(f);
        fclose(f);
    }
    free(res);
    return 0;
}