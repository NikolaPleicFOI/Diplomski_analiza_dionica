typedef struct TradingDay {
	ushort year;
	uchar month;
	uchar day;
	float open;
	float high;
	float low;
	float close;
	uint volume;
}TradingDay;

kernel void CLV(global TradingDay *days, global float *res){
	uint i = get_global_id(0);
	res[i] = (((days[i].close - days[i].low) - (days[i].high - days[i].close)) / (days[i].high - days[i].low));
}