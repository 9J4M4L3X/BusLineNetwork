#ifndef _MyTime_H_
#define _MyTime_H_

#include<stack>
#include<unordered_set>
#include<string>
#include<vector>
#include<iostream>
#include<fstream>
#include<sstream>
#include<queue>

using namespace std;

class MyTime {
public:
	MyTime() { hours_ = 0; minutes_ = 0; }
	MyTime(unsigned h, unsigned m): hours_(h), minutes_(m){}
	MyTime(MyTime* t) { hours_ = t->getHours(); minutes_ = t->getMinutes(); }
	~MyTime() { hours_ = 0; minutes_ = 0; }

	MyTime* operator=(MyTime* time) {
		this->hours_ = time->getHours();
		this->minutes_ = time->getMinutes();
	}
	
	MyTime* operator+(MyTime* time) {
		unsigned h = time->getHours() + hours_;
		unsigned m = time->getMinutes() + minutes_;
		if (m >= 60) {
			h += m / 60;
			m = m % 60;
		}
		return new MyTime(h, m);
	}

	MyTime* operator-(MyTime* time) {
		int h = hours_ - time->getHours();
		int m = minutes_ - time->getMinutes();
		if (h >= 0) {
			if (m >= 0) 
				return new MyTime(h, m);
			else {
				if (h == 0) 
					return nullptr;
				else {
					h--;
					m = 60 + m;
					return new MyTime(h, m);
				}
			}
		}
		else return nullptr;
	}

	bool operator>(MyTime* time) {
		if (hours_ > time->getHours()) 
			return true;
		else if (hours_ < time->getHours())
			return false;
		else {
			if (minutes_ > time->getMinutes()) 
				return true;
			else 
				return false;
		}
	}
	bool operator>=(MyTime* time) {
		if (hours_ > time->getHours()) 
			return true;
		else if (hours_ < time->getHours()) 
			return false;
		else {
			if (minutes_ > time->getMinutes()) 
				return true;
			else if (minutes_ == time->getMinutes())
				return true;
			else 
				return false;
		}
	}
	bool operator<=(MyTime* time) {
		if (hours_ < time->getHours())
			return true;
		else if (hours_ > time->getHours())
			return false;
		else {
			if (minutes_ < time->getMinutes())
				return true;
			else if (minutes_ == time->getMinutes())
				return true;
			else
				return false;
		}
	}
	bool operator<(MyTime* time) {
		if (hours_ < time->getHours())
			return true;
		else if (hours_ > time->getHours())
			return false;
		else {
			if (minutes_ < time->getMinutes())
				return true;
			else
				return false;
		}
	}

	unsigned getHours() { return hours_; }
	unsigned getMinutes() { return minutes_; }

private:

	unsigned hours_, minutes_;
};
#endif // !_MyTime_H_