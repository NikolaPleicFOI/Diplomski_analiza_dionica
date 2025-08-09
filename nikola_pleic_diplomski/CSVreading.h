#ifndef CSVREADING_NPLEIC
#define CSVREADING_NPLEIC

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#define LINE_LENGTH 100

struct TradingDay {
	uint16_t year;
	uint8_t month;
	uint8_t day;
	float open;
	float high;
	float low;
	float close;
	uint32_t volume;
};

static inline void loadCSVData(FILE *file, struct TradingDay *tradingDays);

static inline uint32_t getLineCount(FILE *file);

struct TradingDay* readCSVFile(const char *fileName);

#endif
