typedef struct TradingDay {
	float open;
	float high;
	float low;
	float close;
	uint volume;
}TradingDay;

kernel void CLV(constant TradingDay *days, global float *res, ushort offset){
	uint i = get_global_id(0);
	res[i] = (((days[i].close - days[i].low) - (days[i].high - days[i].close)) / (days[i].high - days[i].low)) *days[i].volume;
}