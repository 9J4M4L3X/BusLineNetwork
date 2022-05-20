#include "Simulator.h"

void Simulator::readStations(const string& station_file)
{
	//station_file is a string that represents a relative path to the file that contains all the stations
	//we just read that input and add all the stations to the graph
	//file is formated so it has a station code and its name in one row
	
	ifstream input_file(station_file);
	stringstream ss;
	ss << input_file.rdbuf();
	string stations = ss.str();
	
	unsigned counter = 0;
	//reads the whole file
	while (counter < stations.size()) {
		//reads one line
		while (stations[counter] != '\n') {
			//reads the code and the name of the station
			string code, name;
			while (stations[counter] != ' ') {
				if ((stations[counter] < '0') || (stations[counter] > '9')) {
					//throw invalid station code? osim ako moze kod da bude bilo kakav
				}
				code.push_back(stations[counter++]);
			}
			while(stations[counter] == ' ') counter++;//skips ' '
			if (stations[counter] == '\n') {
				//throw error missing station name
			}
			bool eof = false;
			while (stations[counter] != '\n') {
				name.push_back(stations[counter++]);
				if (counter == stations.size()) {
					eof = true;
					break;
				}
			}
			Station* new_station = new Station(name, code);
			stations_.push_back(new_station);
			if (eof) break;
		}
		counter++;
	}
	input_file.close();
}

void Simulator::readBusLines(const string& bus_line_file)
{
	ifstream input_file(bus_line_file);
	stringstream ss;
	ss << input_file.rdbuf();
	input_file.close();
	string bus_lines = ss.str();

	unsigned st_size = stations_.size();
	connections_matrix_ = new int* [st_size * st_size];
	for (unsigned i = 0; i < st_size; i++)
	{
		connections_matrix_[i] = new int[st_size];
		for (unsigned j = 0;  j < st_size;  j++)
		{
			connections_matrix_[i][j] = 0;
		}
	}

	bool end_of_file = false;
	unsigned counter = 0;
	while (counter < bus_lines.size()) {
		
		BusLine* new_bus_line = new BusLine();
		
		string bus_line_name;
		while (bus_lines[counter] != ' ') {
			bus_line_name.push_back(bus_lines[counter++]);
		}
		new_bus_line->setLineName(bus_line_name);

		while (bus_lines[counter] == ' ') counter++;

		//reads MyTimes of first and last departure and the delay between departure given in minutes
		if (bus_lines[counter] == '[') {
			//first departure
			while ((bus_lines[counter] < '0') || (bus_lines[counter] > '9')) counter++;
			string hours0, minutes0;
			while ((bus_lines[counter] >= '0') && (bus_lines[counter] <= '9')) {
				hours0.push_back(bus_lines[counter++]);
			}
			while ((bus_lines[counter] < '0') || (bus_lines[counter] > '9')) counter++;
			while ((bus_lines[counter] >= '0') && (bus_lines[counter] <= '9')) {
				minutes0.push_back(bus_lines[counter++]);
			}
			unsigned h0, m0;
			h0 = stoi(hours0);
			m0 = stoi(minutes0);
			MyTime* first_departure = new MyTime(h0, m0);

			//last departure
			while ((bus_lines[counter] < '0') || (bus_lines[counter] > '9')) counter++;
			string hours1, minutes1;
			while ((bus_lines[counter] >= '0') && (bus_lines[counter] <= '9')) {
				hours1.push_back(bus_lines[counter++]);
			}
			while ((bus_lines[counter] < '0') || (bus_lines[counter] > '9')) counter++;
			while ((bus_lines[counter] >= '0') && (bus_lines[counter] <= '9')) {
				minutes1.push_back(bus_lines[counter++]);
			}
			unsigned h1, m1;
			h1 = stoi(hours1);
			m1 = stoi(minutes1);
			MyTime* last_departure = new MyTime(h1, m1);

			new_bus_line->setDepartureMyTimes(first_departure, last_departure);

			//reads delay between departures
			string delay;
			while ((bus_lines[counter] < '0') || (bus_lines[counter] > '9')) counter++;
			while ((bus_lines[counter] >= '0') && (bus_lines[counter] <= '9')) {
				delay.push_back(bus_lines[counter++]);
			}
			new_bus_line->setDelay(stoi(delay));

		}
		else {
			//throw invalid input
		}

		//reads line stations
		while (bus_lines[counter] != '\n') {
			while ((bus_lines[counter] < '0') || (bus_lines[counter] > '9')) counter++;
			
			string station_code;
			while ((bus_lines[counter] >= '0') && (bus_lines[counter] <= '9')) {
				station_code.push_back(bus_lines[counter++]);

				if (counter == bus_lines.size()) {
					end_of_file = true;
					break;
				}
			}
			if ((bus_lines[counter] != ' ') && (bus_lines[counter] != '\n')) {
				//throw invalid station code character
			}

			bool valid_code = false;
			vector<Station*>::iterator st_iter = stations_.begin();
			while (st_iter != stations_.end()) {
				if ((*st_iter)->getStationCode() == station_code) {
					(*st_iter)->addBusLine(new_bus_line);
					new_bus_line->addStation(*st_iter);
					valid_code = true;
					break;
				}
				st_iter++;
			}
			if (!valid_code) {
				//throw error: line has a station that does not exist
			}
			if (end_of_file == true) break;
		}
		counter++;//skips newline character
		bus_lines_.push_back(new_bus_line);//adds a new line
		
		vector<Station*> line_stations = bus_lines_.back()->getLineStations();
		//connects all stations inside the scope of the current read bus line
		vector<Station*>::iterator line_st_it = line_stations.begin();
		unsigned cnt = 0;
		while (cnt + 1 < line_stations.size()) {
			unsigned index0 = findStationIndex(*line_st_it);
			unsigned index1 = findStationIndex(*(line_st_it + 1));
			connections_matrix_[index0][index1] += 1; 
			connections_matrix_[index1][index0] += 1;
			line_st_it++; cnt++;
		}
	}
}

void Simulator::writeBusLineStats(string bus_line_name)
{
	ofstream output_file;
	output_file.open("statistika_" + bus_line_name + ".txt", ios::out);
	output_file << bus_line_name << endl;//first row is the name of the bus

	BusLine* line = nullptr;
	vector<BusLine*>::iterator line_iterator = bus_lines_.begin();
	//first it finds the bus line
	while (line_iterator != bus_lines_.end()) {
		if ((*line_iterator)->getBusName() == bus_line_name) {
			line = *line_iterator;//saves the bus_line that is going to be processed
			break;
		}
		line_iterator++;
	}

	if(line != nullptr){
		vector<BusLine*> similar_lines = this->findSimilarBusLines(line);
		vector<BusLine*>::iterator similar_lines_iter = similar_lines.begin();
		while (similar_lines_iter != similar_lines.end()) {
			if(similar_lines_iter + 1 != similar_lines.end()) 
				output_file << (*similar_lines_iter++)->getBusName() << ' ';
			else 
				output_file << (*similar_lines_iter++)->getBusName() << endl;
		}
		MyTime* first_departure = line->getFirstDepartureTime();
		MyTime* last_departure = line->getLastDepartureTime();
		unsigned delay = line->getDelayTime();
		unsigned first_hours, last_hours, first_mins, last_mins;
		first_hours = first_departure->getHours();
		last_hours = last_departure->getHours();
		first_mins = first_departure->getMinutes();
		last_mins = last_departure->getMinutes();
		unsigned MyTime_in_mins = (last_hours - first_hours) * 60 + last_mins - first_mins;
		unsigned number_of_departures = MyTime_in_mins/delay + 1;//+1 for the last departure
		output_file << number_of_departures;
	}
	else {
		//throw invalid line name 
	}

	output_file.close();
}

void Simulator::writeConnections()
{
	for (unsigned i = 0; i < stations_.size(); i++)
	{
		for (unsigned j = 0; j < stations_.size(); j++)
		{
			cout << connections_matrix_[i][j] << ' ';
		}
		cout << endl;
	}
}

MyTime* Simulator::convertMinsToMyTime(unsigned mins)
{
	if (mins <= 59){
		return new MyTime(0, mins);
	}
	else {
		unsigned hours = mins / 60;
		mins = mins % 60;
		return new MyTime(hours, mins);
	}
}

void Simulator::writeStationInfo(string station_code) {
	vector<Station*>::iterator station_iterator = stations_.begin();
	while (station_iterator != stations_.end()) {
		if ((*station_iterator)->getStationCode() == station_code) {
			(*station_iterator)->writeStationInfo();
			break;
		}
		station_iterator++;
	}
}

vector<BusLine*> Simulator::findSimilarBusLines(BusLine* bus_line)
{
	vector<Station*> line_stations = bus_line->getLineStations();
	string ln = bus_line->getBusName();
	vector<Station*>::iterator st_iter = line_stations.begin();
	vector<BusLine*> similar_lines;
	while (st_iter != line_stations.end()) {
		vector<BusLine*> station_lines = (*st_iter)->getBusLines();
		vector<BusLine*>::iterator st_lines_iter = station_lines.begin();
		//adds all lines that have not been added and have the same station as the bus_line
		while (st_lines_iter != station_lines.end()) {
			bool already_added = false;
			vector<BusLine*>::iterator similar_lines_iter = similar_lines.begin();
			while (similar_lines_iter != similar_lines.end()) {
				if ((*similar_lines_iter)->getBusName() == (*st_lines_iter)->getBusName()) already_added = true;
				similar_lines_iter++;
			}
			if (((*st_lines_iter)->getBusName() != ln) && !already_added) similar_lines.push_back((*st_lines_iter));
			st_lines_iter++;
		}
		st_iter++;
	}

	return similar_lines;
}

int** Simulator::matrixMultiplication(int** m1, int** m2, unsigned m_size)
{	
	int** m_result = new int* [m_size]; 
	for (unsigned i = 0; i < m_size; i++)
	{
		m_result[i] = new int[m_size];
		for (unsigned j = 0; j < m_size; j++)
		{
			m_result[i][j] = 0;
			unsigned cnt = 0;
			while (cnt < m_size) {
				m_result[i][j] += m1[i][cnt] * m2[cnt][j];
				cnt++;
			}
		}
	}
	return m_result;
}

void Simulator::writeBusLineInfo(string bus_line_name) {
	vector<BusLine*>::iterator bus_line_iterator = bus_lines_.begin();
	while (bus_line_iterator != bus_lines_.end()) {
		if ((*bus_line_iterator)->getBusName() == bus_line_name) {
			(*bus_line_iterator)->writeBusLineInfo();
			break;
		}
		bus_line_iterator++;
	}
}

int** Simulator::matrixPower(int** conn_mat, unsigned n)
{
	//finds the nth power of the input matrix
	
	int** mat_nth = new int*[n*n];
	for (unsigned i = 0; i < n; i++)
	{
		mat_nth[i] = new int[n];
		for (unsigned j = 0; j < n; j++)
		{
			mat_nth[i][j] = conn_mat[i][j];
		}
	}

	unsigned cnt = n - 1;//number of lines
	while (cnt > 1) {
		mat_nth = this->matrixMultiplication(mat_nth, conn_mat, n);
		cnt--;
	}
	return mat_nth;
}

bool Simulator::StationsConnected(unsigned s1, unsigned s2)
{
	/*
		find if there are any paths between two stations
		maximum path length between two nodes can be n - 1, where n is the number of nodes
		finding the number of paths length m between two stations in a graph with n nodes 
	
		find the (n - 1) power of the matrix of connections representing a graph
		indexes in the resulting matrix are still graph nodes, and the number inside the matrix
		is a number of paths between those two nodes
	*/

	int** path_matrix = this->matrixPower(connections_matrix_, stations_.size());
	if (path_matrix[s1][s2] != 0) 
		return true;
	else
		return false;
}

vector<unsigned> Simulator::linesBetweenStations(Station* st1, Station* st2)
{
	vector<unsigned> lines_between;
	vector<BusLine*>::iterator line_iter = bus_lines_.begin();
	unsigned counter = 0;
	while (line_iter != bus_lines_.end()) {
		bool stations_connected_by_line = false;
		vector<Station*> line_stations = (*line_iter)->getLineStations();
		vector<Station*>::iterator line_stations_iter = line_stations.begin();
		while (line_stations_iter != line_stations.end()) {
			if ((*line_stations_iter)->getStationCode() == st1->getStationCode()) {
				line_stations_iter++;
				if (line_stations_iter != line_stations.end()) {
					if ((*line_stations_iter)->getStationCode() == st2->getStationCode()) {
						stations_connected_by_line = true;
					}
				}
				break;
			}
			if ((*line_stations_iter)->getStationCode() == st2->getStationCode()) {
				line_stations_iter++;
				if (line_stations_iter != line_stations.end()) {
					if ((*line_stations_iter)->getStationCode() == st1->getStationCode()) {
						stations_connected_by_line = true;
					}
				}
				break;
			}
			line_stations_iter++;
		}
		if (stations_connected_by_line == true) lines_between.push_back(counter);
		line_iter++; counter++;
	}
	return lines_between;
}

unsigned Simulator::findStationIndex(Station* station)
{
	unsigned index = 0;
	vector<Station*>::iterator st_iter = stations_.begin();
	while (st_iter != stations_.end()) {
		if ((*st_iter)->getStationCode() == station->getStationCode()) return index;
		index++; st_iter++;
	}
	return stations_.size() + 1;
}

unsigned Simulator::findLineIndex(BusLine* line)
{
	unsigned index = 0;
	vector<BusLine*>::iterator line_iter = bus_lines_.begin();
	while (line_iter != bus_lines_.end()) {
		if ((*line_iter)->getBusName() == line->getBusName()) {
			return index;
		}
		line_iter++; index++;
	}
	return bus_lines_.size() + 1;
}

void Simulator::simulate(string start_station_code, string end_station_code, string curr_time, int strategy_number)
{
	Station* start_station = nullptr, * end_station = nullptr;
	unsigned start_index, end_index, counter = 0;
	vector<Station*>::iterator st_iter = stations_.begin();
	bool found = false;
	while (st_iter != stations_.end()) {
		if ((*st_iter)->getStationCode() == start_station_code) {
			start_station = *st_iter;
			start_index = counter;
			if (found) break;
			found = true;
		}
		if ((*st_iter)->getStationCode() == end_station_code) {
			end_station = *st_iter;
			end_index = counter;
			if (found) break;
			found = true;
		}
		st_iter++; counter++;
	}
	Path* found_path = nullptr;
	switch (strategy_number) {
	case 1:
		found_path = firstStrategy(start_station_code, end_station_code, curr_time);
		this->writePathToFile(found_path, start_index, end_index);
		break;
	case 2:
		found_path = secondStrategy(start_station_code, end_station_code, curr_time);
		this->writePathToFile(found_path, start_index, end_index);
		break;
	case 3:
		found_path = thirdStrategy(start_station_code, end_station_code, curr_time);
		this->writePathToFile(found_path, start_index, end_index);
		break;
	}
}

vector<Path*> Simulator::findAllPaths(string start_station_code, string end_station_code, string curr_time)
{
	ofstream output_file;
	output_file.open("putanja_" + start_station_code + "_" + end_station_code + ".txt", ios::out);

	Station* start_station = nullptr, * end_station = nullptr;
	unsigned start_index, end_index, counter = 0;
	vector<Station*>::iterator st_iter = stations_.begin(); bool found = false;
	while (st_iter != stations_.end()) {
		if ((*st_iter)->getStationCode() == start_station_code) {
			start_station = *st_iter;
			start_index = counter;
			if (found) break;
			found = true;
		}
		if ((*st_iter)->getStationCode() == end_station_code) {
			end_station = *st_iter;
			end_index = counter; if (found) break;
			found = true;
		}
		st_iter++; counter++;
	}

	//invalid station code/s
	if (!start_station || !end_station) {
		if (!start_station && end_station) {
			cout << "Uneli ste nepostojeci kod pocetne stanice, molimo Vas unesite sve podatke ponovo!\n";
			string sc;
			cout << "Kod pocetne stanice:\n";
			cin >> sc;
			this->firstStrategy(sc, end_station_code, curr_time);
		}
		else if(!end_station && start_station) {
			cout << "Uneli ste nepostojeci kod krajnje stanice, molimo Vas pokusajte ponovo!\n";
			string ec;
			cout << "Kod krajnje stanice:\n";
			cin >> ec;
			this->firstStrategy(start_station_code, ec, curr_time);
		}
		else{
			cout << "Uneli ste nepostojece kodove stanica, molimo Vas pokusajte ponovo!\n";
			string sc, ec, ct;
			cout << "Kod pocetne stanice:\n";
			cin >> sc;
			cout << "Kod krajnje stanice:\n";
			cin >> ec;
			cout << "Trenutno vreme:\n";
			cin >> ct;
			this->firstStrategy(sc, ec, ct);
		}
	}

	string hours, minutes;
	unsigned cnt = 0;
	while ((curr_time[cnt] >= '0') && (curr_time[cnt] <= '9')) hours.push_back(curr_time[cnt++]);

	while ((curr_time[cnt] < '0') || (curr_time[cnt] > '9')) cnt++;//skip non number characters

	while ((curr_time[cnt] >= '0') && (curr_time[cnt] <= '9')) minutes.push_back(curr_time[cnt++]);

	unsigned h = stoi(hours);
	unsigned m = stoi(minutes);
	current_time_ = new MyTime(h, m);//time when the user arrives at the starting station

	vector<Path*> all_paths_found = this->findPaths(start_index, end_index, current_time_);
	return all_paths_found;
}

Path* Simulator::firstStrategy(string start, string end, string curr) {
	string hours, minutes;
	unsigned cnt = 0;
	while ((curr[cnt] >= '0') && (curr[cnt] <= '9')) hours.push_back(curr[cnt++]);

	while ((curr[cnt] < '0') || (curr[cnt] > '9')) cnt++;//skip non number characters

	while ((curr[cnt] >= '0') && (curr[cnt] <= '9')) minutes.push_back(curr[cnt++]);

	unsigned h = stoi(hours);
	unsigned m = stoi(minutes);
	current_time_ = new MyTime(h, m);//time when the user arrives at the starting station

	Path* path = (this->findAllPaths(start, end, curr))[0];//first path found in an array of found paths
	this->calculatePathTime(current_time_, path);
	return (this->findAllPaths(start, end, curr))[0];
}

Path* Simulator::secondStrategy(string start_station_code, string end_station_code, string curr_time)
{
	vector<Path*> all_paths_found = this->findAllPaths(start_station_code, end_station_code, curr_time);
	unsigned path_index = 0, min_index = path_index;
	vector<Path*>::iterator all_paths_iter = all_paths_found.begin();

	unsigned start_index, end_index, counter = 0;
	vector<Station*>::iterator st_iter = stations_.begin(); bool found = false;
	while (st_iter != stations_.end()) {
		if ((*st_iter)->getStationCode() == start_station_code) {
			start_index = counter;
			if (found) break;
			found = true;
		}
		if ((*st_iter)->getStationCode() == end_station_code) {
			end_index = counter; if (found) break;
			found = true;
		}
		st_iter++; counter++;
	}
	cout << all_paths_found.size() << endl;
	MyTime* min_time = new MyTime(10000, 0);
	//set minimum path time to be the first found correct path time
	while (all_paths_iter != all_paths_found.end()) {
		cout << "Put broj " << path_index << ' ';
		vector<unsigned> path_stations = (*all_paths_iter)->getStations();
		vector<unsigned> path_lin = (*all_paths_iter)->getLines();
		vector<unsigned>::iterator path_st = path_stations.begin();
		vector<unsigned>::iterator path_lines = path_lin.begin();
		cout << (stations_[*path_st])->getStationCode() << ' '; path_st++;
		while (path_lines != path_lin.end()) {
			cout << bus_lines_[*path_lines]->getBusName() << ' ' << stations_[*path_st]->getStationCode();
			path_lines++; path_st++;
			if (path_lines == path_lin.end()) cout << endl;
			else cout << ' ';
		}

		if (*((*all_paths_iter)->getStations().begin()) == start_index) {
			if (*((*all_paths_iter)->getStations().end() - 1) == end_index) {
				this->calculatePathTime(current_time_, *all_paths_iter);
				min_time = (*all_paths_iter)->getPathTime();//sets min time to the first found path time
				min_index = path_index;
				break;
			}
		}
		all_paths_iter++; path_index++;
	}

	//find minimum time from the rest of the found paths
	while (all_paths_iter != all_paths_found.end()) {
		cout << "Put broj " << path_index << ' ';
		vector<unsigned> path_stations = (*all_paths_iter)->getStations();
		vector<unsigned> path_lin = (*all_paths_iter)->getLines();
		vector<unsigned>::iterator path_st = path_stations.begin();
		vector<unsigned>::iterator path_lines = path_lin.begin();
		cout << (stations_[*path_st])->getStationCode() << ' '; path_st++;
		while (path_lines != path_lin.end()){
			cout << bus_lines_[*path_lines]->getBusName() << ' ' << stations_[*path_st]->getStationCode();
			path_lines++; path_st++;
			if (path_lines == path_lin.end()) cout << endl;
			else cout << ' ';
		}

		if (*((*all_paths_iter)->getStations().begin()) == start_index) {
			if (*((*all_paths_iter)->getStations().end() - 1) == end_index) {
				this->calculatePathTime(current_time_, *all_paths_iter);
				MyTime* path_time = (*all_paths_iter)->getPathTime();
				if (min_time->operator>(path_time)) {
					min_time = path_time;
					min_index = path_index;
				}
			}
		}
		all_paths_iter++; path_index++;
	}
	
	cout << endl << "Indeks puta sa minimalnim vremenom: " << min_index << endl;

	return all_paths_found.at(min_index);//returns a path with the minimum travel time
}

Path* Simulator::thirdStrategy(string start_station_code, string end_station_code, string curr)
{
	string hours, minutes;
	unsigned cnt = 0;
	while ((curr[cnt] >= '0') && (curr[cnt] <= '9')) hours.push_back(curr[cnt++]);

	while ((curr[cnt] < '0') || (curr[cnt] > '9')) cnt++;//skip non number characters

	while ((curr[cnt] >= '0') && (curr[cnt] <= '9')) minutes.push_back(curr[cnt++]);

	unsigned h = stoi(hours);
	unsigned m = stoi(minutes);
	current_time_ = new MyTime(h, m);//time when the user arrives at the starting station

	vector<Path*> all_paths_found = this->findAllPaths(start_station_code, end_station_code, curr);
	Path* pwtlnol = this->findPathMinLines(all_paths_found);
	this->calculatePathTime(current_time_, pwtlnol);
	return pwtlnol;//path with the least number of lines
}

Path* Simulator::findPathMinLines(vector<Path*> found_paths)
{
	//finds a path with minimum different lines 
	unsigned min_lines = bus_lines_.size() + 1;
	vector<Path*>::iterator fp_it = found_paths.begin();
	Path* path_with_least_lines = nullptr;
	while (fp_it != found_paths.end()) {
		unsigned number_of_lines = 0;
		vector<unsigned> path_lines = (*fp_it)->getLines();
		
		vector<unsigned>::iterator pl_it = path_lines.begin();
		vector<unsigned> different_lines = {};

		while (pl_it != path_lines.end()) {
			
			bool added = false;
			vector<unsigned>::iterator different_lines_iter = different_lines.begin();
			while (different_lines_iter != different_lines.end()) {
				if (*pl_it == *different_lines_iter) {
					added = true;
					break;
				}
				different_lines_iter++;
			}
			if (added == false) {
				number_of_lines++;
				different_lines.push_back(*pl_it);
			}
			pl_it++;
		}

		if (number_of_lines < min_lines) {
			path_with_least_lines = *fp_it;
			min_lines = number_of_lines;
		}
		fp_it++;
	}
	return path_with_least_lines;
}

void Simulator::updateNetworkState(MyTime* curr_time, Path* path)
{
	if(!path->getActiveLines().empty())
		path->deleteActiveLines();//deletes all active lines if there are any
	//sets all active lines to their positions in the moment defined by curr_time
	unsigned station_delay = 3;//number of minutes between stations
	MyTime* ct = curr_time;
	vector<BusLine*>::iterator line_it = bus_lines_.begin(); unsigned counter = 0;
	while (line_it != bus_lines_.end()) {
		MyTime* first_departure = (*line_it)->getFirstDepartureTime();
		unsigned delay = (*line_it)->getDelayTime();
		vector<Station*> line_stations = (*line_it)->getLineStations();
		unsigned num_of_stations = line_stations.size();
		//all lines departing before the current moment and not arriving at the first stop before the current moment will be saved into path active lines queue
		if (curr_time->operator>=(first_departure)) {
			MyTime* time_to_pass = curr_time->operator-(first_departure);
			unsigned line_time = 2 * (num_of_stations - 1) * station_delay;//time(in minutes) that it takes for a line to go a full circle

			unsigned time_to_pass_in_mins = (time_to_pass->getHours() * 60 + time_to_pass->getMinutes());
			unsigned number_of_departures = time_to_pass_in_mins / delay + 1;

			MyTime* new_departure_time = first_departure;
			unsigned cnt = number_of_departures;
			while (cnt > 0) {
				MyTime* departure_to_current_time = new MyTime(curr_time->operator-(new_departure_time));
				bool line_active = departure_to_current_time->operator<(this->convertMinsToMyTime(line_time));
				//if line is active find its current station, direction, and minutes passed after last station
				if (line_active) {
					unsigned departure_to_current_mins = departure_to_current_time->getHours() * 60 + departure_to_current_time->getMinutes();
					bool direction = true;
					unsigned new_station_index = departure_to_current_mins / station_delay;
					if (new_station_index >= num_of_stations - 1) {
						new_station_index = 2 * num_of_stations - 2 - new_station_index;
						direction = false;
					}
					unsigned minutes_after_last_station = departure_to_current_mins % station_delay;
					vector<Station*>::iterator st_it = line_stations.begin(); unsigned station_cnt = 0;
					while (st_it != line_stations.end()) {
						if (station_cnt == new_station_index) break;
						st_it++; station_cnt++;
					}
					ActiveLine* new_active = new ActiveLine(*line_it, minutes_after_last_station, direction, *st_it);
					path->addActiveLine(counter, new_active);
				}
				new_departure_time = new_departure_time->operator+(this->convertMinsToMyTime(delay));
				cnt--;
			}
		}
		line_it++; counter++;
	}
}

vector<Path*> Simulator::findPaths(unsigned s1, unsigned s2, MyTime* curr_time)
{
	//s1 - start station, s2 - end station, curr_time - current time
	//uses BFS traversal from the starting node to the end while using a set which saves all the paths that can be made
	//from all saved paths return only paths ending with the end station(s2)
	
	if (s1 == s2) return {};

	bool connected = this->StationsConnected(s1, s2);
	if (!connected) return {};//if there is no path between start and end station return empty vector

	vector<bool> visited;
	unsigned counter = 0;

	//initialize all stations to unvisited
	while (counter < stations_.size()) {
		visited.push_back(false);
		counter++;
	}

	unordered_set<Path*> paths;//all different paths that may be taken from the first station to the last one
	vector<Path*> searched_paths;
	visited[s1] = true;//first node set to be visited is the start station
	queue<unsigned> station_queue;//contains nodes that will be visited next
	station_queue.push(s1);
	while (!station_queue.empty()) {
		unsigned new_station_index = station_queue.front();
		station_queue.pop();

		bool connected = true;
		if (new_station_index != s2)
			connected = this->StationsConnected(new_station_index, s2);
		
		if (connected == true){
			//if the new station is s2 then create a path and add it to the searched_paths array
			if (!paths.empty()) {
				vector<Path*> new_paths;
				unordered_set<Path*>::iterator path_it = paths.begin();
				unsigned path_cnt = 1;
				while (path_it != paths.end()) {
					path_cnt++;
					vector<unsigned> path_st = (*path_it)->getStations();
					vector<unsigned> path_lines = (*path_it)->getLines();
					unsigned last_station = path_st.at(path_st.size() - 1);
					//add the new station and make new paths only if multiple lines exist between last station and the new one
					if (connections_matrix_[last_station][new_station_index] != 0) {
						(*path_it)->addStation(new_station_index);
						vector<unsigned> new_lines = this->linesBetweenStations(stations_[last_station], stations_[new_station_index]);
						vector<unsigned>::iterator new_lines_it = new_lines.begin();
						while (new_lines_it != new_lines.end()) {
							if (new_lines_it == new_lines.begin()) {
								(*path_it)->addLine(*new_lines_it);
								if (new_station_index == s2) {
									Path* searched_path = new Path(*path_it);
									searched_paths.push_back(searched_path);
								}
							}
							else {
								//make a new path here
								Path* new_new_path = new Path(*path_it);
								new_new_path->removeLine();//removes the new line added to the path_it 
								new_new_path->addLine(*new_lines_it);//adds the next line connecting the new station
								new_paths.push_back(new_new_path);
								if (new_station_index == s2) {
									Path* searched_path = new Path(new_new_path);
									searched_paths.push_back(searched_path);
								}
							}
							new_lines_it++;
						}
					}
					else {
						//if the new station is not connected to the last station in a path
						//check other stations in a path and find the one connected to the new station
						unsigned counter = 0;
						Path* new_path = new Path();
						while (counter < path_st.size()) {
							unsigned curr_station = path_st[counter];
							//if station connection is found make a new path(or multiple paths if there is more than one line)
							if (connections_matrix_[curr_station][new_station_index] != 0) {
								new_path->addStation(curr_station);//adds the last station from the old path
								vector<unsigned> new_lines = this->linesBetweenStations(stations_[curr_station], stations_[new_station_index]);
								vector<unsigned>::iterator new_lines_it = new_lines.begin();

								//makes a new path for every line connection and adds it to the searched path if its the end station
								while (new_lines_it != new_lines.end()) {
									if (new_lines_it == new_lines.begin()) {
										new_path->addLine(*new_lines_it);//new line connection to the new station
										new_path->addStation(new_station_index);//new station
										new_paths.push_back(new_path);
										if (new_station_index == s2) {
											Path* searched_path = new Path(new_path);
											searched_paths.push_back(searched_path);
										}
									}
									else {
										//make a new path here
										Path* new_new_path = new Path(*path_it);
										new_new_path->removeLine();//removes the new line added to the new_path
										new_new_path->addLine(*new_lines_it);
										new_paths.push_back(new_new_path);
										if (new_station_index == s2) {
											Path* searched_path = new Path(new_new_path);
											searched_paths.push_back(searched_path);
										}
									}
									new_lines_it++;
								}
								break;//found the new node and made all new paths
							}
							else {
								//follows the path by adding stations and lines
								new_path->addStation(curr_station);
								if (counter < path_st.size() - 1) {
									unsigned curr_line = path_lines[counter];
									new_path->addLine(curr_line);
								}
							}
							counter++;
						}
					}
					path_it++;
				}

				//adds all new paths to the set of all paths
				vector<Path*>::iterator new_paths_it = new_paths.begin();
				while (new_paths_it != new_paths.end()) {
					paths.insert(*new_paths_it++);
				}

			}
			else {
				//create the first path containing only the start station
				MyTime* start_time = new MyTime(0, 0);
				Path* root_path = new Path();
				root_path->addStation(new_station_index);
				this->updateNetworkState(curr_time, root_path);//sets all active lines to the correct positions in the current moment
				paths.insert(root_path);
			}
		}

		for (unsigned i = 0; i < stations_.size(); i++)
		{
			if (connections_matrix_[i][new_station_index] != 0) {
				if (!visited[i]) {
					visited[i] = true;
					station_queue.push(i);//push node children in the queue
				}
			}
		}
	}
	return searched_paths;//returns all possible paths starting from s1 and ending with s2
}

void Simulator::calculatePathTime(MyTime* curr_time, Path* path)
{
	//calculates time it takes to go from the departure at the first station on the path to the last station
	MyTime* start_time = curr_time;

	vector<unsigned> path_stations = path->getStations();
	vector<unsigned>::iterator path_st_iter = path_stations.begin();

	vector<unsigned> path_lines = path->getLines();
	vector<unsigned>::iterator path_line_iter = path_lines.begin();

	while (path_st_iter + 1 != path_stations.end()) {
		unsigned curr_line = *path_line_iter;
		this->updateNetworkState(curr_time, path);//changes active line positions according to the new current time

		bool not_begin = path_line_iter != path_lines.begin();
		bool same_line = false;
		if (not_begin) {
			same_line = *(path_line_iter - 1) == *path_line_iter;
		}

		if (not_begin && same_line) {
			curr_time = curr_time->operator+(new MyTime(0, 3));//if line hasnt changed just add time needed to travel between 2 stations
		}
		else {
			list<queue <ActiveLine*>> path_active_lines = path->getActiveLines();
			list<queue <ActiveLine*>>::iterator active_lines_iter = path_active_lines.begin();
			while (active_lines_iter != path_active_lines.end()) {
				if (((*active_lines_iter).front()->getActiveLine())->getBusName() == bus_lines_[curr_line]->getBusName()) {
					//if line is found, then go through the queue of active lines and find the one with the minimum waiting time

					//find the path direction between two current stations
					bool path_curr_line_direction = true;
					BusLine* current_line = bus_lines_[curr_line];
					vector<Station*> line_stations = current_line->getLineStations();
					vector<Station*>::iterator line_st_iter = line_stations.begin();
					while (line_st_iter != line_stations.end()) {
						string path_curr_st = stations_[*path_st_iter]->getStationCode();
						string path_next_st = stations_[*(path_st_iter + 1)]->getStationCode();

						string line_curr = (*line_st_iter)->getStationCode();

						if (line_curr == path_curr_st) {
							if (line_st_iter == line_stations.begin()) {
								path_curr_line_direction = true;
								//next station in a path must be the next forward station in a line path
							}
							else if (line_st_iter + 1 == line_stations.end()) {
								path_curr_line_direction = false;
								//next station in a line must be the previous station, so its the only outcome
							}
							else {
								string line_next = (*(line_st_iter + 1))->getStationCode();
								string line_prev = (*(line_st_iter - 1))->getStationCode();
								if (path_next_st == line_next)
									path_curr_line_direction = true;
								else
									path_curr_line_direction = false;
							}
						}
						line_st_iter++;
					}

					//if line is found, find the active line with the minimum waiting time
					queue<ActiveLine*> replace_queue;

					//unsigned mins_min = (*active_lines_iter).front()->calcTimeToArrive(stations_[*path_st_iter], path_curr_line_direction);
					unsigned mins_min = 10000;
					//find minimum waiting time for all active lines of this name
					while (!(*active_lines_iter).empty()) {
						ActiveLine* active_line = (*active_lines_iter).front();
						(*active_lines_iter).pop();

						unsigned wait_mins = active_line->calcTimeToArrive(stations_[*path_st_iter], path_curr_line_direction);
						if (active_line->getMiss() == false) {
							//minutes until the active line arrives at the station
							if (wait_mins < mins_min) mins_min = wait_mins;
						}
						replace_queue.push(active_line);
					}
					(*active_lines_iter).swap(replace_queue);

					//calculate wait time for the first next bus line departure
					MyTime* fd_new = current_line->getFirstDepartureTime();
					MyTime* del = this->convertMinsToMyTime((current_line->getDelayTime()));
					while (fd_new->operator<(curr_time)) {
						fd_new = fd_new->operator+(del);
					}
					MyTime* line_departure_wait_time = fd_new->operator-(curr_time);

					//for the first next bus departure calculate travel time to the current station
					unsigned travel_mins = 0;
					vector<Station*>::iterator line_st_it = line_stations.begin();
					while ((*line_st_it)->getStationCode() != stations_[*path_st_iter]->getStationCode()) {
						travel_mins += 3;
						line_st_it++;
					}
					travel_mins += 3;//adds travel time to the next station
					MyTime* line_travel_time = this->convertMinsToMyTime(travel_mins);//travel time for the next new bus 
					MyTime* line_arrival_time = line_departure_wait_time->operator+(line_travel_time);//final time needed to arrive to the station 
					MyTime* travel_time = this->convertMinsToMyTime(mins_min + 3);//minimal time found using only currently active lines
					
					//finds the minimum between the minimum time for the currently active and the time needed for the next bus to arrive
					if (travel_time->operator>(line_arrival_time)) travel_time = line_arrival_time;

					//adds found time to the current time
					curr_time = curr_time->operator+(travel_time);
					break;//found the line time for this part of the path
				}
				active_lines_iter++;
			}
		}
		path_st_iter++; path_line_iter++;
	}
	path->setPathTime(curr_time->operator-(start_time));
}

unsigned ActiveLine::calcTimeToArrive(Station* nxt_st, bool path_dir)
{
	unsigned time_to_arrive = 3 - time_elapsed_;//time until the next station in the line is arrived
	vector<Station*> line_stations = line_->getLineStations();
	unsigned num_of_stations = line_stations.size(), cnt = 0;
	vector<Station*>::iterator line_st_it = line_stations.begin();
	unsigned current_position = 0, next_position = 0;
	bool found_current = false, found_next = false;

	while (line_st_it != line_stations.end()) {
		if (*(line_st_it) == current_station_) {
			current_position = cnt;
			found_current = true;
		}
		if ((*line_st_it)->getStationCode() == nxt_st->getStationCode()) {
			next_position = cnt;
			found_next = true;
		}
		line_st_it++; cnt++;
	}

	//if line direction is forward
	if (direction_ == true) {
		if (path_dir == true) {
			int stations_to_pass = next_position - current_position - 1;
			if (stations_to_pass > 0) { //1 c 3 n 5
				time_to_arrive += stations_to_pass * 3;
				return time_to_arrive;
			}
			else {//1 n 3 c 5
				missed_ = true;//line will not go in that direction 
				return time_to_arrive;
			}
		}
		else {
			int stations_to_pass = next_position - current_position - 1;
			if (stations_to_pass > 0) { //1 c 3<-n 5,
				unsigned num_of_st = num_of_stations - 1 - (current_position + 1) + num_of_stations - 1 - next_position;
				time_to_arrive += num_of_st * 3;
				return time_to_arrive;
			}
			else {//1<-n 3 c 5
				unsigned stations_to_pass_two_directions = 2 * num_of_stations - 3 - current_position - next_position;
				time_to_arrive += stations_to_pass_two_directions * 3;
				return time_to_arrive;
			}
		}
	}
	else {
		if(path_dir == true){
			int stations_to_pass = next_position - current_position - 1;
			if (stations_to_pass > 0) { //1<-c 3 n->5
				missed_ = true; //the line doesnt go this way
				return time_to_arrive;//returns time but it will not be used in finding path time 
			}
			else {//1 n->3<-c 5
				missed_ = true;
				return time_to_arrive;
			}
		}
		else {
			int stations_to_pass = next_position - current_position - 1;
			if (stations_to_pass > 0) { //1<-c 3<-n 5
				missed_ = true;
				return time_to_arrive;
			}
			else {//1<-n 3<-c 5
				unsigned multiplier = current_position - next_position - 1;
				time_to_arrive += multiplier * 3;
				return time_to_arrive;
			}

		}
	}
}

void Path::addActiveLine(unsigned line, ActiveLine* new_active) {
	BusLine* bus = new_active->getActiveLine();
	string new_active_line_name = bus->getBusName();
	if (active_lines_.empty()) {
		queue<ActiveLine*> new_queue; new_queue.push(new_active);
		active_lines_.emplace_back(new_queue);
	}
	else {
		bool added = false;
		list<queue<ActiveLine*>>::iterator act_line_it = active_lines_.begin();
		while (act_line_it != active_lines_.end()) {
			if (((*act_line_it).front())->getActiveLine()->getBusName() == new_active_line_name) {
				(*act_line_it).push(new_active);
				added = true;
			}
			act_line_it++;
		}

		if(added == false) {
			queue <ActiveLine*> new_queue; new_queue.push(new_active);
			active_lines_.emplace_back(new_queue);
		}
	}
}


void Simulator::writePathToFile(Path* found_path, unsigned s1, unsigned s2)
{
	ofstream output_file;
	output_file.open("putanja_" + stations_.at(s1)->getStationCode() + "_" + stations_.at(s2)->getStationCode() + ".txt", ios::out);

	vector<unsigned> path_stations = found_path->getStations();
	vector<unsigned> path_lines = found_path->getLines();
	vector<unsigned>::iterator path_st_iter = path_stations.begin();
	vector<unsigned>::iterator path_line_iter = path_lines.begin();

	MyTime* pt = found_path->getPathTime();
	unsigned h = pt->getHours();
	unsigned m = pt->getMinutes();

	if ((h < 10) && (m < 10)) {
		output_file << "path time: 0" << h << ':' << '0' << m << endl;
	}
	else if ((h < 10) && !(m < 10)) {
		output_file << "path time: 0" << h << ':' << m << endl;
	}
	else if (!(h < 10) && (m < 10)) {
		output_file << "path time: " << h << ':' << '0' << m << endl;
	}
	else {
		output_file << "path time: " << h << m << endl;
	}

	while (path_line_iter != path_lines.end()) {
		bool same_line = false, begin = true;
		if (path_line_iter != path_lines.begin()) {
			begin = false;
			if (*path_line_iter == *(path_line_iter - 1)) {
				same_line = true;
			}
		}
		if (same_line == true) {
			output_file << ' ' << stations_[*(path_st_iter + 1)]->getStationCode();
		}
		else {
			if (begin == true) {
				output_file << bus_lines_[*path_line_iter]->getBusName() << "->" << endl;
				output_file << stations_[*path_st_iter]->getStationCode() << ' ' << stations_[*(path_st_iter + 1)]->getStationCode();
			}
			else {
				string last_line_name = bus_lines_[*(path_line_iter - 1)]->getBusName();
				string next_line_name = bus_lines_[*path_line_iter]->getBusName();
				output_file << endl << last_line_name << "->" << next_line_name << endl;
				output_file << stations_[*path_st_iter]->getStationCode() << ' ' << stations_[*(path_st_iter + 1)]->getStationCode();
			}
		}
		path_st_iter++; path_line_iter++;
	}
	output_file.close();
}




