#include "nikola_pleic_diplomski.h"
#include "calculate.h"
#include <Shlobj_core.h>

int WINAPI WinMain(
    _In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPSTR     lpCmdLine,
    _In_ int       nCmdShow
){
    initOCL();
    if (initWindow(hInstance, nCmdShow) != 0) return -1;

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    
    if (adi.program != NULL) {
        cleanUpClProgramData(adi);
    }
    if (momentum.program != NULL) {
        cleanUpClProgramData(momentum);
    }
    if (ma.program != NULL) {
        cleanUpClProgramData(ma);
    }
    destryoOCL();
    return (int) msg.wParam;
}

static int ucitajPodatke(){
    int r = readCSVFiles(data_source_folder, &pData, &totalDays);
    if (r != 0) {
        MessageBox(NULL, TEXT("Nisam uspio ucitati podatke"), TEXT("Error"), NULL);
        return -1;
    }
    sprintf(inDataInfo, "Ucitano je %u datoteka sa %llu redaka", pData.numStocks, totalDays);
    if (initCalcValues(&pData, totalDays) != 0) {
        MessageBox(NULL, TEXT("Greska pri inicijalizaciji vrijednosti"), TEXT("Error"), NULL);
        return -1;
    }
    return 0;
}

static int initWindow(HINSTANCE hInstance, int nCmdShow) {
    if (OleInitialize(NULL) != S_OK) {
        return -1;
    }

    WNDCLASSEX opts = {
        .cbSize = sizeof(WNDCLASSEX),
        .style = CS_HREDRAW | CS_VREDRAW,
        .lpfnWndProc = WindowProc,
        .cbClsExtra = 0,
        .cbWndExtra = 0,
        .hInstance = hInstance,
        .hIcon = NULL,
        .hCursor = LoadCursor(NULL, IDC_ARROW),
        .hbrBackground = (HBRUSH)(COLOR_WINDOW + 1),
        .lpszMenuName = NULL,
        .lpszClassName = windowCName,
        .hIconSm = NULL
    };
    if (RegisterClassEx(&opts) == 0) {
        showWinError("RegisterClassEx je bacio gresku");
        return -1;
    }

    mainWindow = CreateWindowEx(
        WS_EX_OVERLAPPEDWINDOW,
        windowCName,
        windowTitle,
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        810, 350,
        NULL,
        NULL,
        hInstance,
        NULL
    );
    if (mainWindow == NULL)
    {
        showWinError("CreateWindowEx je bacio gresku");
        return -1;
    }
    ShowWindow(mainWindow, nCmdShow);
}

static LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg){
        case WM_PAINT:
            PAINTSTRUCT ps;
            HDC hdc;
            char* opcijeText = "Odaberi aktivnost";

            hdc = BeginPaint(hwnd, &ps);
            TextOut(hdc, 345, 5, opcijeText, strlen(opcijeText));
            TextOut(hdc, 350, 110, data_source_folder, strlen(data_source_folder));
            TextOut(hdc, 350, 170, getOutDataFolder(), strlen(getOutDataFolder()));
            TextOut(hdc, 350, 230, getKernelFolder(), strlen(getKernelFolder()));
            TextOut(hdc, 480, 275, inDataInfo, strlen(inDataInfo));
            EndPaint(hwnd, &ps);
            break;

        case WM_CREATE:
            //racunanje metrika
            CreateWindow(TEXT("button"), TEXT(AD_BUTTON_TEXT),
                WS_VISIBLE | WS_CHILD,
                20, 40, 150, 25,
                hwnd, (HMENU)AD_BUTTON, NULL, NULL);
            CreateWindow(TEXT("button"), TEXT(MM_BUTTON_TEXT),
                WS_VISIBLE | WS_CHILD,
                220, 40, 150, 25,
                hwnd, (HMENU)MM_BUTTON, NULL, NULL);
            CreateWindow(TEXT("button"), TEXT(MOV_BUTTON_TEXT),
                WS_VISIBLE | WS_CHILD,
                420, 40, 150, 25,
                hwnd, (HMENU)MOV_BUTTON, NULL, NULL);
            CreateWindow(TEXT("button"), TEXT(ALL_BUTTON_TEXT),
                WS_VISIBLE | WS_CHILD,
                620, 40, 150, 25,
                hwnd, (HMENU)ALL_BUTTON, NULL, NULL);

            //dohvacanje foldera
            CreateWindow(TEXT("button"), TEXT(FOLDER_IN_BUTTON_TEXT),
                WS_VISIBLE | WS_CHILD,
                350, 80, 120, 25,
                hwnd, (HMENU)FOLDER_IN_BUTTON, NULL, NULL);
            CreateWindow(TEXT("button"), TEXT(FOLDER_OUT_BUTTON_TEXT),
                WS_VISIBLE | WS_CHILD,
                350, 140, 120, 25,
                hwnd, (HMENU)FOLDER_OUT_BUTTON, NULL, NULL);
            CreateWindow(TEXT("button"), TEXT(FOLDER_KERNELS_BUTTON_TEXT),
                WS_VISIBLE | WS_CHILD,
                350, 200, 120, 25,
                hwnd, (HMENU)FOLDER_KERNELS_BUTTON, NULL, NULL);
            CreateWindow(TEXT("button"), TEXT(READ_DATA_BUTTON_TEXT),
                WS_VISIBLE | WS_CHILD,
                350, 270, 120, 25,
                hwnd, (HMENU)READ_DATA_BUTTON, NULL, NULL);

            //ispis rezultata
            CreateWindow(TEXT("edit"), TEXT(OUTPUT_FIELD_DEFAULT_TEXT),
                WS_VISIBLE | WS_CHILD | WS_VSCROLL | ES_MULTILINE,
                20, 80, 300, 200,
                hwnd, (HMENU)OUTPUT_FIELD, NULL, NULL);
            SendDlgItemMessage(hwnd, (HMENU)OUTPUT_FIELD, EM_SETREADONLY, 1, 0);
            break;

        case WM_COMMAND:
            switch (LOWORD(wParam)) {
                case AD_BUTTON:
                    handleButtonPress(hwnd, obradiAD, 0);
                    break;

                case MM_BUTTON:
                    handleButtonPress(hwnd, obradiMM, MOMEN_DAYS_OFFSET);
                    break;

                case MOV_BUTTON:
                    handleButtonPress(hwnd, obradiMov, MA_DAYS_OFFSET);
                    break;

                case ALL_BUTTON:
                    handleButtonPress(hwnd, obradiSve, 0);
                    break;

                case FOLDER_IN_BUTTON:
                    char* newInFolder = getFolderFromPicker("Odaberite direktorij sa ulaznim podacima dionica");
                    if (newInFolder != NULL) {
                        data_source_folder = newInFolder;
                        InvalidateRect(mainWindow, NULL, true);
                    }
                    break;

                case FOLDER_OUT_BUTTON:
                    char* newOutFolder = getFolderFromPicker("Odaberite direktorij u kojemu ce se spremiti podaci");
                    if (newOutFolder != NULL) {
                        setOutDataFolder(newOutFolder);
                        InvalidateRect(mainWindow, NULL, true);
                    }
                    break;

                case FOLDER_KERNELS_BUTTON:
                    char* newKernFolder = getFolderFromPicker("Odaberite direktorij u kojemu se nalaze kernel datoteke (.cl)");
                    if (newKernFolder != NULL) {
                        setKernelFolder(newKernFolder);
                        InvalidateRect(mainWindow, NULL, true);
                    }
                    break;

                case READ_DATA_BUTTON:
                    sprintf(inDataInfo, "Ucitavam podatke...");
                    InvalidateRect(mainWindow, NULL, true);
                    UpdateWindow(mainWindow);
                    if (ucitajPodatke() != 0) {
                        MessageBox(NULL, TEXT("Ucitavanje podataka nije uspjelo"), TEXT("Error"), NULL);
                    }
                    else {
                        InvalidateRect(mainWindow, NULL, true);
                    }
                    break;
                }
            break;

        case WM_DESTROY:
            PostQuitMessage(0);
            break;

        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
            break;
    }
}

static void showWinError(char *title) {
    DWORD error = GetLastError();
    LPTSTR errortext;

    if (FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER |
        FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        error,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR)&errortext,
        0, NULL
        ) == 0) {
        MessageBox(NULL, TEXT("FormatMessage failed"), TEXT("Error"), NULL);
        ExitProcess(error);
    }
    MessageBox(NULL, errortext, title, NULL);
}

static int handleButtonPress(HWND hwnd, float *(*obrada)(), int offset) {
    char* out = "Kalkuliranje metrika u tijeku...";
    SendDlgItemMessage(hwnd, (HMENU)OUTPUT_FIELD, WM_SETTEXT, NULL, out);
    UpdateWindow(hwnd);
    valRes = obrada();
    if (valRes == NULL) {
        return;
    }
    out = formatOutputText(valRes, offset);
    if (out == NULL) return 0;
    SendDlgItemMessage(hwnd, (HMENU)OUTPUT_FIELD, WM_SETTEXT, NULL, out);
    free(out);
    free(valRes);
}

static float* obradiAD() {
    clProgramData *ad = prepareADIndex();
    if (ad == NULL) {
        MessageBox(NULL, TEXT("Pogreska u pripremi kalkulacije A/D indeksa"), TEXT("Error"), NULL);
        return NULL;
    }
    float *res = enqueue(ad);
    if (resultADIndex(res) != 0) {
        MessageBox(NULL, TEXT("Pogreska u kalkulaciji A/D indeksa"), TEXT("Error"), NULL);
        return NULL;
    }
    return res;
}

static float* obradiMM() {
    clProgramData *mm = prepareMomentum();
    if (mm == NULL) {
        MessageBox(NULL, TEXT("Pogreska u pripremi kalkulacije momentuma"), TEXT("Error"), NULL);
        return NULL;
    }
    float *res = enqueue(mm);
    if (resultMomentum(res) != 0) {
        MessageBox(NULL, TEXT("Pogreska u kalkulaciji momentuma"), TEXT("Error"), NULL);
        return NULL;
    }
    return res;
}

static float* obradiMov() {
    clProgramData *mov = prepareMovingAverage();
    if (mov == NULL) {
        MessageBox(NULL, TEXT("Pogreska u pripremi kalkulacije pomicnog prosjeka"), TEXT("Error"), NULL);
        return NULL;
    }
    float * res = enqueue(mov);
    if (resultMovingAverage(res) != 0) {
        MessageBox(NULL, TEXT("Pogreska u kalkulaciji pomicnog prosjeka"), TEXT("Error"), NULL);
        return NULL;
    }
    return res;
}

static float* obradiSve() {
    clProgramData *ad = prepareADIndex();
    if (ad == NULL) {
        MessageBox(NULL, TEXT("Pogreska u pripremi kalkulacije A/D indeksa"), TEXT("Error"), NULL);
        return NULL;
    }
    clProgramData *mm = prepareMomentum();
    if (mm == NULL) {
        MessageBox(NULL, TEXT("Pogreska u pripremi kalkulacije momentuma"), TEXT("Error"), NULL);
        return NULL;
    }
    clProgramData *mov = prepareMovingAverage();
    if (mov == NULL) {
        MessageBox(NULL, TEXT("Pogreska u pripremi kalkulacije pomicnog prosjeka"), TEXT("Error"), NULL);
        return NULL;
    }
    clUnloadCompiler();
    float *resa = enqueue(ad);
    float *resmm = enqueue(mm);
    float *resmov = enqueue(mov);
    if (resultADIndex(resa) != 0) {
        MessageBox(NULL, TEXT("Pogreska u kalkulaciji A/D indeksa"), TEXT("Error"), NULL);
        return NULL;
    }
    if (resultMomentum(resmm) != 0) {
        MessageBox(NULL, TEXT("Pogreska u kalkulaciji momentuma"), TEXT("Error"), NULL);
        return NULL;
    }
    if (resultMovingAverage(resmov) != 0) {
        MessageBox(NULL, TEXT("Pogreska u kalkulaciji pomicnog prosjeka"), TEXT("Error"), NULL);
        return NULL;
    }
    return resa;
}

static char* formatOutputText(float *in, int offset) {
    char formattedOut[4096] = "";
    char temp[50] = "";
    int end, curr = (offset + 1);
    if (pData.numDays[0] < curr + 51) {
        end = pData.numDays[0];
    }
    else {
        end = curr + 50;
    }

    while (curr < end) {
        sprintf(temp, "%u-%02u-%02u:%20f\r\n", pData.days[curr].year, pData.days[curr].month, pData.days[curr].day, in[curr]);
        strncat(formattedOut, temp, 49);
        curr++;
    }
    char* actualOut = malloc(4096);
    if (actualOut == NULL) {
        MessageBox(NULL, TEXT("Malloc je bacio gresku"), TEXT("Error"), NULL);
        return NULL;
    }
    strcpy(actualOut, formattedOut);
    return actualOut;
}

static char *getFolderFromPicker(char *message) {
    BROWSEINFO info = {
        .hwndOwner = mainWindow,
        .pidlRoot = NULL,
        .pszDisplayName = NULL,
        .lpszTitle = message,
        .ulFlags = BIF_RETURNONLYFSDIRS | BIF_DONTGOBELOWDOMAIN | BIF_USENEWUI,
        .lpfn = NULL,
        .lParam = NULL,
    };

    PIDLIST_ABSOLUTE result = SHBrowseForFolder(&info);
    if (result == NULL) {
        return NULL;
    }
    char* folderPath = malloc(260);
    if (folderPath == NULL) {
        return NULL;
    }
    SHGetPathFromIDList(result, folderPath);
    return folderPath;
}