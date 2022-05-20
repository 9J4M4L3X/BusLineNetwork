#include "Station.h"

Station::Station()
{
	code_ = name_ = "";
	bus_lines_.clear();
}

void Station::writeStationInfo()
{
	string file_name = "stajaliste_" + code_ + ".txt";

	ofstream output_file;
	output_file.open(file_name, ios::out);
	output_file << code_ << ' ' << name_ << " [";
	
	string station_lines = "";
	vector<BusLine*>::iterator bus_iterator = bus_lines_.begin();
	while (bus_iterator != bus_lines_.end()) {
		station_lines.append((*bus_iterator++)->getBusName());
		if(bus_iterator != bus_lines_.end()) station_lines.push_back(' ');
	}
	output_file << station_lines;
	output_file << ']';
	output_file.close();
}
