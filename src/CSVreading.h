#ifndef CSVREADING_NPLEIC
#define CSVREADING_NPLEIC

#include <stdint.h>
#include <stdio.h>

#define MAX_STOCKS 10

#define LINE_LENGTH 128
#define STOCK_NAME_LENGTH 8
#define FILENAME_MAX 256

typedef struct TradingDay {
	float open;
	float high;
	float low;
	float close;
	uint32_t volume;
}TradingDay;

typedef struct DaysData {
	uint16_t year;
	uint8_t month;
	uint8_t day;
}DaysData;

typedef struct ProgData {
	char **stocks;
	TradingDay *trades;
	DaysData *days;
	uint16_t *numDays;
	uint16_t numStocks;
}ProgData;

int readCSVFiles(const char* folderName, ProgData* data, size_t* totalDays);

static inline size_t getStockCount(const char* folder);

static inline char** getFiles(const char* folder, uint16_t numFiles);

static inline size_t getDaysCount(FILE* file);

static inline size_t prepareData(const char** files, ProgData* data);

static inline int loadCSVData(FILE* file, ProgData* data, size_t* day, int num);

#endif