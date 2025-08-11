#include "nikola_pleic_diplomski.h"
#include "CSVreading.h"
#include "OpenCLInteraction/CLinit.h"
#include "calculate.h"

int main(int argc, char** argv)
{
    initOCL();
    size_t totalDays;
    ProgData data;
    clProgramData *ad, *mm, *mov;
    float* resa, * resmm, * resmov;
    int r = readCSVFile(FILE_TO_READ, &data, &totalDays);
    if (r != 0) {
        printf("Nisam uspio ucitati podatke\n");
        return -1;
    }
    initCalcValues(data.days, totalDays);

    int opcija;
    printf("Odaberi aktivnost:\n1. distribucijski indeks\n2. momentum\n3. pomicni prosjek\n4. sve\n");
    if (scanf("%d", &opcija) != 1) {
        printf("Greska pri citanju unosa\n");
        return -1;
    }
    switch (opcija) {
        case 1:
            ad = prepareADIndex(data.trades);
            if (ad == NULL) {
                printf("Pogreska u pripremi kalkulacije A/D indeksa\n");
                return -1;
            }
            resa = enqueue(ad);
            if (resultADIndex(resa) != 0) {
                printf("Pogreska u kalkulaciji A/D indeksa\n");
                return -1;
            }
            break;
        case 2:
            mm = prepareMomentum(data.trades);
            if (mm == NULL) {
                printf("Pogreska u pripremi kalkulacije momentuma\n");
                return -1;
            }
            resmm = enqueue(mm);
            if (resultMomentum(resmm) != 0) {
                printf("Pogreska u kalkulaciji momentuma\n");
                return -1;
            }
            break;
        case 3:
            mov = prepareMovingAverage(data.trades);
            if (mov == NULL) {
                printf("Pogreska u pripremi kalkulacije pomicnog prosjeka\n");
                return -1;
            }
            resmov = enqueue(mov);
            if (resultMovingAverage(resmov) != 0) {
                printf("Pogreska u kalkulaciji pomicnog prosjeka\n");
                return -1;
            }
            break;
        case 4:
            ad = prepareADIndex(data.trades);
            if (ad == NULL) {
                printf("Pogreska u pripremi kalkulacije A/D indeksa\n");
                return -1;
            }
            mm = prepareMomentum(data.trades);
            if (mm == NULL) {
                printf("Pogreska u pripremi kalkulacije momentuma\n");
                return -1;
            }
            mov = prepareMovingAverage(data.trades);
            if (mov == NULL) {
                printf("Pogreska u pripremi kalkulacije pomicnog prosjeka\n");
                return -1;
            }
            clUnloadCompiler();
            float *resad = enqueue(ad);
            float *resmm = enqueue(mm);
            float *resmov = enqueue(mov);
            if (resultADIndex(resad) != 0) {
                printf("Pogreska u kalkulaciji A/D indeksa\n");
                return -1;
            }
            if (resultMomentum(resmm) != 0) {
                printf("Pogreska u kalkulaciji momentuma\n");
                return -1;
            }
            if (resultMovingAverage(resmov) != 0) {
                printf("Pogreska u kalkulaciji pomicnog prosjeka\n");
                return -1;
            }
            break;
        default:
            printf("Ne postoji opcija %d\n", opcija);
            return -1;
    }

    destryoOCL();
	return 0;
}
