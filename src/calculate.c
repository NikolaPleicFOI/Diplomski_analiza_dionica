#include "calculate.h"
#include <stdio.h>

int distIndex(TradingDay *days, size_t daysCount) {
	clProgramData clv;
    char* file = CLV_FILE;

    int err = prepareKernel(days, daysCount, &clv, file);
    if (err != 0) {
        printf("Dogodila se graska %u\n", err);
        return err;
    }

    float* res = execute(daysCount, &clv);

    if (res == NULL) {
        return -1;
    }

    return writeResults(res, daysCount);
}

static int writeResults(float *res, size_t size) {
    FILE *f = NULL;
    f = fopen(WRITE_FILE, "w");
    if (f == NULL) {
        printf("Failed to open file for writing results\n");
        return -1;
    }

    for (int i = 0; i < size; i++) {
        fprintf(f, "%u: %f\n", i, res[i]);
    }
    fclose(f);
    printf("Results were written to " WRITE_FILE);
    return 0;
}