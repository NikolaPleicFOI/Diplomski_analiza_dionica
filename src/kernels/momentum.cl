typedef struct TradingDay {
	float open;
	float high;
	float low;
	float close;
	uint volume;
}TradingDay;

kernel void momentum(global TradingDay *trade, global float *res, ushort offset){
	uint i = get_global_id(0);
	res[i] = trade[i].close - trade[i - offset].close;
}