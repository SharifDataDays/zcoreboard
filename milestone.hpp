#ifndef MILESTONE_H
#define MILESTONE_H

#include <string>
#include <vector>

using namespace std;

struct Milestone {
	int id;
	string name;
	vector<int> tasks;
};

#endif

