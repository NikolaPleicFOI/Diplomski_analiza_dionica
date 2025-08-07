#include "nikola_pleic_diplomski.h"
#include "CSVreading.h"

int main(int argc, char** argv)
{
    cl_int CL_err = CL_SUCCESS;
    cl_uint numPlatforms = 0;

    CL_err = clGetPlatformIDs(0, NULL, &numPlatforms);

    if (CL_err == CL_SUCCESS)
        printf_s("%u platform(s) found\n", numPlatforms);
    else
        printf_s("clGetPlatformIDs(%i)\n", CL_err);
    printf("size of TradingDay: %d", sizeof (struct TradingDay));
	return 0;
}
