#ifndef SIMULATOR_H_
#define SIMULATOR_H_

#include"Node.h"

class Path;

class Simulator {
public:
	Simulator() { connections_matrix_ = nullptr; bus_lines_ = {}; stations_ = {}; }

	void readStations(const string& station_file);
	void readBusLines(const string& bus_line_file);

	void writeBusLineInfo(string bus_line_name);
	void writeStationInfo(string station_code);
	void writeBusLineStats(string bus_line_name);
	void writeConnections();
	//finds all the lines that have at least one station that the bus_line has
	vector<BusLine*> findSimilarBusLines(BusLine* bus_line);
	int** matrixMultiplication(int**, int**, unsigned);
	int** matrixPower(int**, unsigned);
	bool StationsConnected(unsigned s1, unsigned s2);

	vector<unsigned> linesBetweenStations(Station* st1, Station* st2);//returns all bus lines connecting two connected stations
	unsigned findStationIndex(Station* station);//finds the station position in a vector of stations
	unsigned findLineIndex(BusLine* lines);
	MyTime* convertMinsToMyTime(unsigned mins);

	void simulate(string start_station_code, string end_station_code, string curr_time, int strategy_number);
	
	Path* firstStrategy(string start_station_code, string end_station_code, string curr_time);//forms any path from station start to station end
	Path* secondStrategy(string start_station_code, string end_station_code, string curr_time);//forms a path with a minimum number of stations between start and end
	Path* thirdStrategy(string start_station_code, string end_station_code, string curr_time);//forms a path with a minimum number of different bus lines from start to end
	
	Path* findPathMinLines(vector<Path*> found_paths);//finds a path with the least number of different lines
	vector<Path*> findAllPaths(string, string, string);//only calls the findPaths function which returns all paths found from start to end station in the current time
	vector<Path*> findPaths(unsigned s1, unsigned s2, MyTime* curr_time);

	void updateNetworkState(MyTime* curr_time, Path*);//all active lines initialize their positions and directions 
	void calculatePathTime(MyTime*, Path*);//sets path time needed to pass until the last station
	
	//writes a path specified by start and end station
	void writePathToFile(Path* found_path, unsigned s1, unsigned s2);
	
protected:
	vector<Station*> stations_;
	vector<BusLine*> bus_lines_;
	MyTime* current_time_;
	int** connections_matrix_;//when two vertices(stations) are connected matrix cell holds a number of edges(bus lines) between them
};

class ActiveLine : protected Simulator {
public:
	ActiveLine() { line_ = nullptr; direction_ = true; current_station_ = nullptr; time_elapsed_ = 0; missed_ = false; }
	ActiveLine(BusLine* line, unsigned minutes_after_last_station = 0, bool direction = true, Station* current_station = nullptr, bool missed = false) :
		line_(line), time_elapsed_(minutes_after_last_station), direction_(direction), current_station_(current_station), missed_(missed) {}
	~ActiveLine() { delete line_; delete current_station_; }

	BusLine* getActiveLine() { return line_; }
	bool getDirection() { return direction_; }
	Station* getCurrentStation() { return current_station_; }
	unsigned minsPassedAfterCurrStation() { return time_elapsed_; }
	unsigned calcTimeToArrive(Station* next_station, bool path_line_dir);//calculates the time of waiting for the active bus line to arrive to the station
	bool getMiss() { return missed_; }
private:
	BusLine* line_;
	bool direction_;//if true, line direction is forward
	Station* current_station_;
	unsigned time_elapsed_;//time that the line has been traveling since arriving at the current station
	bool missed_;//only true if direction is backward and the current station is before the next station in a path
};

class Path : public Simulator{
public:
	Path() { path_time_ = new MyTime(0, 0); stations_ = {}; path_lines_ = {}; }
	
	Path(MyTime* path_time, vector<unsigned> stations = {}, vector<unsigned> lines = {}) :
		path_time_(path_time), path_stations_(stations), path_lines_(lines) {}
	
	Path(Path* path) {
		path_time_ = path->getPathTime();
		path_stations_ = path->getStations();
		path_lines_ = path->getLines();
		active_lines_ = path->getActiveLines();
	}

	vector<unsigned> getStations() { return path_stations_; }
	vector<unsigned> getLines() { return path_lines_; }
	MyTime* getPathTime() { return path_time_; }
	list<queue <ActiveLine*>> getActiveLines() { return active_lines_; }
	
	void addStation(unsigned station) { path_stations_.push_back(station); }
	void addLine(unsigned line) { path_lines_.push_back(line); }
	void removeLine() { path_lines_.pop_back(); }
	void addActiveLine(unsigned line, ActiveLine* new_active);
	void setPathTime(MyTime* path_time) { path_time_ = path_time; }
	void deleteActiveLines() { active_lines_ = {}; }
private:
	MyTime* path_time_;//time between the first station on a path and the last station in a path
	vector<unsigned> path_stations_;
	vector<unsigned> path_lines_;
	list<queue <ActiveLine*>> active_lines_;//active lines change positions each time a station is passed in a path
};


#endif // !SIMULATOR_H_
