#include "CSVreading.h"
#include <memory.h>
#include <string.h>
#include <stdlib.h>
#include <windows.h>

int readCSVFiles(const char *folderName, ProgData *data, size_t *totalDays) {
	data->numStocks = getStockCount(folderName);
	if (data->numStocks == -1) {
		return -1;
	}
	
	data->numDays = malloc(data->numStocks * sizeof(uint16_t*));
	data->stocks = malloc(data->numStocks * sizeof(char*));
	for (int i = 0; i < data->numStocks; i++) {
		data->stocks[i] = malloc(data->numStocks * STOCK_NAME_LENGTH);
	}

	char **files = getFiles(folderName, data->numStocks);
	if (files == NULL) {
		printf("Nisam uspio alocirati imena datoteka\n");
		return -1;
	}
	
	*totalDays = prepareData(files, data);
	if (*totalDays < 0) {
		return -1;
	}

	data->trades = malloc(*totalDays * sizeof(TradingDay));
	data->days = malloc(*totalDays * sizeof(DaysData));

	*totalDays = 0;
	for (int i = 0; i < data->numStocks; i++) {
		FILE* f = fopen(files[i], "r");
		if (!f) {
			printf("Greska pri otvaranju datoteke %s\n", files[i]);
			return -1;
		}
		if (loadCSVData(f, data, totalDays, i) != 0) {
			printf("greska pri ucitavanju datoteke %s\n", files[i]);
			return -1;
		}
		fclose(f);
	}

	for (int i = 0; i < data->numStocks; i++) {
		free(files[i]);
	}
	/*for (int i = 0; i < *totalDays; i++) {
		printf("Vrijednosti: %u, %u, %u, %f, %f, %f, %f, %u\n", data->days[i].year, data->days[i].month, data->days[i].day, data->trades[i].open,
			data->trades[i].high, data->trades[i].low, data->trades[i].close, data->trades[i].volume);
	}*/
	free(files);
	return 0;
}

static inline size_t getStockCount(const char *folder) {
	size_t count = 0;
	WIN32_FIND_DATA fdata;
	wchar_t path[FILENAME_MAX];
	
	sprintf(path, "%s\\*.csv", folder);
	HANDLE hFind = FindFirstFile(path, &fdata);

	if (hFind  == INVALID_HANDLE_VALUE){
		printf("Nisam pronasao direktorij: %s\n", folder);
		return -1;
	}
	do {
		count++;
	} while (FindNextFile(hFind, &fdata));
	
	FindClose(hFind);
	if (count > MAX_STOCKS) {
		count = MAX_STOCKS;
	}
	return count;
}

static inline char **getFiles(const char *folder, uint16_t numFiles) {
	char** files = malloc(numFiles * sizeof(char*));
	if (files == NULL) return NULL;

	WIN32_FIND_DATA fdata;
	char path[FILENAME_MAX];

	sprintf(path, "%s\\*.csv", folder);
	HANDLE hFind = FindFirstFile(path, &fdata);

	int i = 0;
	size_t pom;
	do {
		if (i == numFiles) {
			printf("greska u kalkuliranju broja datoteka");
			return NULL;
		}
		files[i] = malloc(FILENAME_MAX);
		if (files[i] == NULL) return NULL;
		strcpy(files[i], folder);
		strcat(files[i], "\\");
		strcat(files[i], fdata.cFileName);
		i++;
	} while (FindNextFile(hFind, &fdata) && (i < MAX_STOCKS));

	FindClose(hFind);
	return files;
}

static inline size_t getDaysCount(FILE* file) {
	size_t lines = 0;
	char c;
	while (!feof(file)) {
		c = fgetc(file);
		if (c == '\n') lines++;
	}
	rewind(file);
	//Prva linija je samo format
	return lines - 1;
}

static inline size_t prepareData(const char **files, ProgData *data) {
	size_t totalDays = 0;
	for (int i = 0; i < data->numStocks; i++) {
		char* file = strrchr(files[i], '\\');
		file++;
		int end = 0;
		while (file[end] != '.') {
			data->stocks[i][end] = file[end];
			end++;
		}
		data->stocks[i][end] = '\0';

		FILE* f = fopen(files[i], "r");
		if (!f) {
			printf("Greska pri otvaranju datoteke %s\n", files[i]);
			return -1;
		}
		data->numDays[i] = getDaysCount(f);
		totalDays += data->numDays[i];
		fclose(f);
	}
	return totalDays;
}

static inline int loadCSVData(FILE *file, ProgData *data, size_t *day, int num) {
	char line[LINE_LENGTH];
	//Prva linija je samo format
	fgets(line, LINE_LENGTH, file);
	size_t i = 0;
	TradingDay *t = &(data->trades[*day]);
	DaysData *d = &(data->days[*day]);

	while (fscanf(file, "%hu-%hhu-%hhu,%f,%f,%f,%f,%*f,%u\n", &d->year, &d->month, &d->day,
		&t->open, &t->high, &t->low, &t->close, &t->volume) == 8) {
		i++;
		(*day)++;
		t = &(data->trades[*day]);
		d = &(data->days[*day]);
	}
	if (i != data->numDays[num]) {
		printf("GRESKA! datoteka %u bi trebao imati %u dana, ali ima %u\n", num, data->numDays[num], i);
		return -1;
	}
	return 0;
}
