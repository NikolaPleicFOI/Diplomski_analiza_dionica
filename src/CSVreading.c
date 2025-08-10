#include "CSVreading.h"
#include <memory.h>
#include <string.h>
#include <stdlib.h>

int readCSVFile(const char *folderName, ProgData *data, size_t *totalDays) {
	data->numStocks = getStockCount(folderName);
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
			printf("Greska pri otvaranju datoteke %s", files[i]);
			return -1;
		}
		loadCSVData(f, data, totalDays);
		fclose(f);
	}

	for (int i = 0; i < data->numStocks; i++) {
		free(files[i]);
	}
	free(files);
	
	//for (int i = 0; i < data->numDays; i++) {
	//	printf("Vrijednosti: %u, %u, %u, %f, %f, %f, %f, %u\n", data->days[i].year, data->days[i].month, data->days[i].day, data->trades[i].open,
	//		data->trades[i].high, data->trades[i].low, data->trades[i].close, data->trades[i].volume);
	//}
	return 0;
}

static inline size_t getStockCount(const char *folder) {
	return 1;
}

static inline char **getFiles(const char *folder, uint16_t numFiles) {
	char **files = malloc(numFiles * sizeof(char*));
	for (int i = 0; i < numFiles; i++) {
		files[i] = malloc(FILENAME_MAX);
		if (files[i] == NULL) return NULL;
	}
	strcpy(files[0], folder);
	return files;
}

static inline size_t getLineCount(FILE* file) {
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
			printf("Greska pri otvaranju datoteke %s", files[i]);
			return -1;
		}
		data->numDays[i] = getLineCount(f);
		totalDays += data->numDays[i];
		fclose(f);
	}
	return totalDays;
}

static inline void loadCSVData(FILE *file, ProgData *data, size_t *day) {
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
}
