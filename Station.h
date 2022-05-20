#ifndef _STATION_H_
#define _STATION_H_

#include"BusLine.h"

//has the code,the name of the station and an array of buses passing through it
class Station {
public:
	Station();
	Station(string name, string code):name_(name), code_(code){}
	~Station() {
		if (!bus_lines_.empty()) {
			vector<BusLine*>::iterator line_it = bus_lines_.begin();
			while (line_it != bus_lines_.end()) {
				delete (*line_it++);
			}
		}
	}
	void writeStationInfo();
	void addBusLine(BusLine* bus_line) { bus_lines_.push_back(bus_line);}

	string getStationName() { return name_;}
	string getStationCode() { return code_; }
	vector<BusLine*> getBusLines() { return bus_lines_; }
private:
	string name_, code_;
	vector<BusLine*> bus_lines_;
};
#endif // !_STATION_H_
