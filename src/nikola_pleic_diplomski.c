#include "nikola_pleic_diplomski.h"
#include "CSVreading.h"
#include "OpenCLInteraction/CLinit.h"
#include "calculate.h"

int main(int argc, char** argv)
{
    initOCL();
    size_t daysCount;

    struct TradingDay* days = readCSVFile(FILE_TO_READ, &daysCount);
    if (days == NULL) {
        printf("Nisam uspio procitati datoteku");
        return 0;
    }

    if (distIndex(days, daysCount) != 0) {
        printf("Failed to calculate distribution index");
    }

    destryoOCL();
    free(days);
	return 0;
}
