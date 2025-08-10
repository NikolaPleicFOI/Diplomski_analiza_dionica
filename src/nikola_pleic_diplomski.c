#include "nikola_pleic_diplomski.h"
#include "CSVreading.h"
#include "OpenCLInteraction/CLinit.h"
#include "calculate.h"

int main(int argc, char** argv)
{
    initOCL();
    size_t totalDays;
    ProgData data;
    int r = readCSVFile(FILE_TO_READ, &data, &totalDays);
    if (r != 0) {
        printf("Nisam uspio ucitati podatke");
        return 0;
    }

    /*if (distIndex(data.trades, totalDays) != 0) {
        printf("Failed to calculate distribution index");
    }*/

    destryoOCL();
	return 0;
}
