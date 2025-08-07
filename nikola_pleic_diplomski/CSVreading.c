#include "CSVreading.h"
#include <stdio.h>
#include <memory.h>

bool readCSVFile(const char* fileName) {
	FILE* file = fopen(fileName, "r");
	if (!file) {
		printf("Greska pri otvaranju datoteke %s", fileName);
		return false;
	}
	
	struct TradingDay* lines = malloc(getLineCount(file) * sizeof (struct TradingDay));
	char line[LINE_LENGTH];

	while (fgets(line, LINE_LENGTH, file) != NULL) {
		struct TradingDay day = lineToDay(line);
	}

	free(lines);
	fclose(file);
	return true;
}


inline uint32_t getLineCount(const FILE* file) {
	uint32_t lines = 0;
	char c;
	while (!feof(file)) {
		c = fgetc(file);
		if (c == '\n') lines++;
	}
	rewind(file);
	return lines;
}

struct TradingDay lineToDay(const char* line) {
	struct TradingDay d;
	return d;
}
