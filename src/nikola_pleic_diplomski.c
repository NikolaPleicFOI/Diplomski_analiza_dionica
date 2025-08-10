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
        printf("Nisam uspio ucitati podatke\n");
        return -1;
    }

    int res = 1;
    /*printf("Odaberi aktivnost:\n1. distribucijski indeks\n");
    if (scanf("%d", &res) != 1) {
        printf("Greska pri citanju unosa\n");
        return -1;
    }*/
    switch (res) {
        case 1:
            if (distIndex(data.trades, totalDays) != 0) {
                printf("Failed to calculate distribution index\n");
                return -1;
            }
            break;
        default:
            printf("Unos nije u ponudenim opcijama\n");
            return -1;
    }

    destryoOCL();
	return 0;
}
