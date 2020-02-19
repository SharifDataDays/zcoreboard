#ifndef TASK_H
#define TASK_H

#include <string>

using namespace std;

struct Task {
	int id;
	string name;
	vector<int> milestones;

	Json::value as_json();

};

#endif

