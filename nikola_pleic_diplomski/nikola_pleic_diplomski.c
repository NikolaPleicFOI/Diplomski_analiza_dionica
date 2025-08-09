#include "nikola_pleic_diplomski.h"
#include "CSVreading.h"
#include "OpenCLInteraction/CLinit.h"

int main(int argc, char** argv)
{
    int res = initOCL();
    if (res != 0) {
        printf("Dogodila se graska %u\n", res);
    }

    struct TradingDay* days = readCSVFile(FILE_TO_READ);
    if (days == NULL) {
        printf("Nisam uspio procitati datoteku");
        return 0;
    }

    destryoOCL();
    free(days);
	return 0;
}
