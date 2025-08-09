#include "nikola_pleic_diplomski.h"
#include "CSVreading.h"
#include "OpenCLInteraction/CLinit.h"

int main(int argc, char** argv)
{
    size_t daysCount;

    struct TradingDay* days = readCSVFile(FILE_TO_READ, &daysCount);
    if (days == NULL) {
        printf("Nisam uspio procitati datoteku");
        return 0;
    }

    clProgramData clv;
    char* file = "CLV.cl";
    int err = initOCL(days, daysCount, &clv, file);
    if (err != 0) {
        printf("Dogodila se graska %u\n", err);
    }
    float *res = execute(daysCount, &clv);
    if (res == NULL) {
        return -1;
    }

    for (int i = 0; i < daysCount; i++) {
        printf("%u: %f\n", i, res[i]);
    }

    destryoOCL();
    free(days);

	return 0;
}
