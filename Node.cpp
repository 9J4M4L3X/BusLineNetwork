#include "Node.h"

ChildLine* Node::getChild(string station_code)
{
	vector<ChildLine*>::iterator children_iter = children_paths_.begin();
	while (children_iter != children_paths_.end()) {
		if ((*children_iter++)->getNode()->getStation()->getStationCode() == station_code) return *children_iter;
	}
	return nullptr;
}
