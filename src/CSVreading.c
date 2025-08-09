#include "CSVreading.h"
#include <memory.h>
#include <string.h>
#include <stdlib.h>

//uint8_t daysInMonths[12] = {31,29,31,30,31,30,31,31,30,31,30,31};

TradingDay* readCSVFile(const char *fileName, size_t *daysCount) {
	FILE* file = fopen(fileName, "r");
	if (!file) {
		printf("Greska pri otvaranju datoteke %s", fileName);
		return NULL;
	}

	*daysCount = getLineCount(file);
	
	TradingDay* days = malloc(*daysCount * sizeof (TradingDay));
	loadCSVData(file, days);
	fclose(file);
	return days;
}


static inline size_t getLineCount(FILE *file) {
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

static inline void loadCSVData(FILE *file, TradingDay *tradingDays) {
	char line[LINE_LENGTH];

	//Prva linija je samo format
	fgets(line, LINE_LENGTH, file);
	size_t index = 0;

	while (fscanf(file, "%u-%u-%u,%f,%f,%f,%f,%*f,%u\n", &tradingDays[index].year, &tradingDays[index].month, &tradingDays[index].day,
		&tradingDays[index].open, &tradingDays[index].high, &tradingDays[index].low, &tradingDays[index].close, &tradingDays[index].volume) == 8) {

		//printf("Vrijednosti: %u, %u, %u, %f, %f, %f, %f, %u\n", tradingDays[index].year, tradingDays[index].month, tradingDays[index].day, tradingDays[index].open,
		//	tradingDays[index].high, tradingDays[index].low, tradingDays[index].close, tradingDays[index].volume);
		index++;
	}
	return;
}
