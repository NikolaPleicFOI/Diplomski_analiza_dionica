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
    initCalcValues(data.days, totalDays);

    int res;
    printf("Odaberi aktivnost:\n1. distribucijski indeks\n2. momentum\n");
    if (scanf("%d", &res) != 1) {
        printf("Greska pri citanju unosa\n");
        return -1;
    }
    switch (res) {
        case 1:
            if (prepareADIndex(data.trades) != 0) {
                printf("Pogreska u pripremi kalkulacije A/D indeksa\n");
                return -1;
            }
            if (executeADIndex() != 0) {
                printf("Pogreska u kalkulaciji A/D indeksa\n");
                return -1;
            }
            break;
        case 2:
            if (prepareMomentum(data.trades) != 0){ 
                printf("Pogreska u pripremi kalkulacije momentuma\n");
                return -1;
            }
            if (executeMomentum() != 0) {
                printf("Pogreska u kalkulaciji momentuma\n");
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
