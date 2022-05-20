#ifndef _GRAPH_H_
#define _GRAPH_H_

#include"Station.h"

class Node;

//child node(station) and all the bus lines leading to it
class ChildLine {
public:
	ChildLine() { node_ = nullptr; line_ = {}; }

	Node* getNode() { return node_; }
	vector<BusLine*> getLines() { return line_; }
private:
	Node* node_;
	vector<BusLine*> line_;
};

//station with all its children stations connected by bus lines
class Node {
public:
	Node() { station_ = nullptr; children_paths_ = {}; }
	Node(Station* st): station_(st) {}
	Node(Station* st, vector<ChildLine*> children) : station_(st), children_paths_(children){}

	Station* getStation() { return station_; }
	vector<ChildLine*> getChildren() { return children_paths_; }
	ChildLine* getChild(string station_code);
private:
	Station* station_;//data about the station
	vector<ChildLine*> children_paths_;//stations connected to the node by a bus line
};
#endif // !_GRAPH_H_

