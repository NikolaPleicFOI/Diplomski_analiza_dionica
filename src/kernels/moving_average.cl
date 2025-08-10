typedef struct TradingDay {
	float open;
	float high;
	float low;
	float close;
	uint volume;
}TradingDay;

kernel void MA(global TradingDay *trade, global float *res, ushort offset){
	uint i = get_global_id(0);
	float total = 0;
	for(int j = 0; j < offset ; j++){
		total = total + trade[i - j].close;
	}
	res[i] = total/(offset + 1);
}