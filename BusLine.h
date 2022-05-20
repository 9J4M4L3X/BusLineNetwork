#ifndef _BUSLINE_H_
#define _BUSLINE_H_
#include"MyTime.h"

class Station;

//has a name of the bus and all the stations it covers
class BusLine {
public:
	BusLine() { bus_name_ = ""; delay_ = 0; line_stations_ = {}; first_departure_ = nullptr; last_departure_ = nullptr; }
	BusLine(BusLine* line) {
		bus_name_ = line->getBusName();
		delay_ = line->getDelayTime();
		line_stations_ = line->getLineStations();
		first_departure_ = line->getFirstDepartureTime();
		last_departure_ = line->getLastDepartureTime();
	}
	~BusLine() { 
		delete first_departure_; 
		delete last_departure_;
		if (!line_stations_.empty()) {
			vector<Station*>::iterator st_it = line_stations_.begin();
			while (st_it != line_stations_.end()) {
				delete (*st_it++);
			}
		}
	}
	void setLineName(string line_name) { bus_name_ = line_name; }
	void addStation(Station* st) { line_stations_.push_back(st); }
	void setDepartureMyTimes(MyTime* first, MyTime* last) { first_departure_ = first; last_departure_ = last; }
	void setDelay(unsigned delay) { delay_ = delay; }
	void writeBusLineInfo();

	string getBusName() { return bus_name_; }
	vector<Station*> getLineStations() { return line_stations_; }
	MyTime* getFirstDepartureTime() { return first_departure_; }
	MyTime* getLastDepartureTime() { return last_departure_; }
	unsigned getDelayTime() { return delay_; }
private:
	string bus_name_;
	vector<Station*> line_stations_;
	MyTime* first_departure_, * last_departure_;
	unsigned delay_;//MyTime delay between 2 departures in minutes
};

#endif // 
