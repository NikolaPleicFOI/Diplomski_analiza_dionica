#ifndef CSVREADING_NPLEIC
#define CSVREADING_NPLEIC

#include <stdbool.h>
#include <stdint.h>

#define LINE_LENGTH 100

struct TradingDay {
	uint16_t year;
	uint8_t month;
	uint8_t day;
	float open;
	float close;
	float high;
	float low;
	uint32_t volume;
};

bool readCSVFile(char*);

inline uint32_t getLineCount(const *FILE);

struct TradingDay lineToDay(const char* line);

#endif
