#ifndef SCOREBOARD_H
#define SCOREBOARD_H

#include <vector>
#include <map>
#include <string>
#include <jsoncpp/json/json.h>

#include "milestone.hpp"

using namespace std;

#define POOL_CAPACITY 200

class Scoreboard {
public:
	vector<float> total_scores;
	map<string, int> teams_map;
	vector<string> teams;
	map<int, int> tasks_map;
	vector<int> tasks;
	vector<vector<float> > tasks_scores;
	vector<int> order;
	Milestone milestone;

	Scoreboard(Milestone &);

	void remove(int);
	void insert(int);
	void reset_order(void);
	
	int get_team_rank(int);
	int get_team_id(string, bool);
	int add_team(string);
	float get_score(int, int);
	int get_task_id(int);
	int get_teams_count(void);
	void add_task(int);

	void update_score(string, int, float);
	Json::Value get_team_info(string);
	Json::Value get_scoreboard(int, int);
};

#endif
