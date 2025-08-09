#ifndef CSVREADING_NPLEIC
#define CSVREADING_NPLEIC

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#define LINE_LENGTH 100

typedef struct TradingDay {
	uint16_t year;
	uint8_t month;
	uint8_t day;
	float open;
	float high;
	float low;
	float close;
	uint32_t volume;
}TradingDay;

TradingDay* readCSVFile(const char* fileName, size_t* daysCount);

static inline size_t getLineCount(FILE* file);

static inline void loadCSVData(FILE *file, TradingDay *tradingDays);

#endif
