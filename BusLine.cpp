#include "BusLine.h"
#include "Station.h"

void BusLine::writeBusLineInfo()
{
	ofstream output_file;
	string output_file_name = "linija_" + bus_name_ + ".txt";
	output_file.open(output_file_name, ios::out);
	
	vector<Station*>::iterator st_iter = line_stations_.begin();
	vector<Station*>::iterator st_iter_end = line_stations_.end();
	//writes bus name, starting and ending station in the first row
	output_file << bus_name_ + " ";
	output_file << (*st_iter)->getStationName() << "->" << (*(st_iter_end - 1))->getStationName() << endl;
	//then it writes all the stations that the bus is passing
	while (st_iter != line_stations_.end()) {
		output_file << (*st_iter)->getStationCode() << ' ' << (*st_iter)->getStationName() << endl;
		st_iter++;
	}
	output_file.close();
}
