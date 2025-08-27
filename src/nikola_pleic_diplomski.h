#ifndef NPLEIC
#define NPLEIC

#include <windows.h>
#include "CSVreading.h"
#include "OpenCLInteraction/CLinit.h"

#define DATA_FOLDER "..\\..\\..\\..\\podaci\\nasdaq"
#define AD_BUTTON 10
#define MM_BUTTON 11
#define MOV_BUTTON 12
#define ALL_BUTTON 13
#define FOLDER_IN_BUTTON 14
#define FOLDER_OUT_BUTTON 15
#define FOLDER_KERNELS_BUTTON 16
#define READ_DATA_BUTTON 17
#define OUTPUT_FIELD 18
#define AD_BUTTON_TEXT "A/D indeks"
#define MM_BUTTON_TEXT "Momentum"
#define MOV_BUTTON_TEXT "Pomicni prosjek"
#define ALL_BUTTON_TEXT "Sve metrike"
#define FOLDER_IN_BUTTON_TEXT "Ulaz direktorij"
#define FOLDER_OUT_BUTTON_TEXT "Izlaz direktorij"
#define FOLDER_KERNELS_BUTTON_TEXT "Kernels dir"
#define READ_DATA_BUTTON_TEXT "Ucitaj podatke"
#define OUTPUT_FIELD_DEFAULT_TEXT "Ovdje se ispisuju prvih nekoliko redova"

static char* windowCName = "npleic_main_window";
static char* windowTitle = "Nikola Pleic - diplomski";
HWND mainWindow;
static char* data_source_folder = DATA_FOLDER;
static char inDataInfo[64] = "Podaci nisu ucitani";

static size_t totalDays;
static ProgData pData;
static clProgramData *ad = NULL, *mm = NULL, *mov = NULL;
float *valRes;

int WINAPI WinMain(
    _In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPSTR     lpCmdLine,
    _In_ int       nCmdShow
);

static int ucitajPodatke();

static int initWindow(HINSTANCE hInstance, int nCmdShow);
static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

static void showWinError(char* title);
static float* obradiAD();
static float* obradiMM();
static float* obradiMov();
static float* obradiSve();
static char* formatOutputText(float* in, int offset);
static char* getFolderFromPicker(char *message);

#endif