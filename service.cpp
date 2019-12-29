#include <iostream>
#include <fstream>
#include <string>
#include "service.hpp"
#include "task.hpp"
#include "milestone.hpp"

using namespace std;

ScoreboardService* ScoreboardService::instance = nullptr;

ScoreboardService::ScoreboardService() {
}

ScoreboardService::ScoreboardService(char *config_file) {
	ifstream fin(config_file);
	int n;
	fin >> n;
	for (int i = 0; i < n; ++i) {
		Milestone ms;
		fin >> ms.id;
		getline(fin, ms.name);
		fin >> ms.tasks_count;
		this->add_milestone(ms);
		for (int j = 0; j < ms.tasks_count; ++j) {
			Task t;
			fin >> t.id;
			t.milestone_id = i;
			getline(fin, t.name);
			this->add_task(t);
		}
	}
	
	int updates;
	fin >> updates;
	for (int i = 0; i < updates; i++) {
		//TODO continue this part
	}
	fin.close();
}

ScoreboardService* ScoreboardService::getInstance() {
	if (instance == nullptr)
		instance = new ScoreboardService();
	return instance;
}

ScoreboardService* ScoreboardService::getInstance(char *config_file) {
	if (instance == nullptr)
		instance = new ScoreboardService(config_file);
	return instance;
}

int ScoreboardService::add_task(Task &task) {
	assert(this->tasks_map.count(task.id) == 0);
	int tid = this->tasks.size();
	this->tasks_map[task.id] = tid;
	this->tasks.push_back(task);
	for (int i = 0; i < this->teams.size(); ++i)
		this->tasks_scores[i].push_back(0);
	return tid;
}

int ScoreboardService::add_milestone(Milestone &ms) {
	assert(this->milestones_map.count(ms.id) == 0);
	int msid = this->milestones.size();
	this->milestones_map[ms.id] = msid;
	this->milestones.push_back(ms);
	this->scoreboards.push_back(Scoreboard(this->teams.size()));
	return msid;
}

void ScoreboardService::add_task_to_milestone(int task_id, int milestone_id) {
	assert(this->tasks_map.count(task_id) != 0);
	assert(this->milestones_map.count(milestone_id) != 0);
	task_id = this->tasks_map[task_id];
	milestone_id = this->milestones_map[milestone_id];
	this->tasks[task_id].milestones.push_back(milestone_id);
	this->milestones[milestone_id].tasks.push_back(task_id);
	for (int i = 0; i < this->teams.size(); ++i)
		this->scoreboards[milestone_id].scores[i] += this->tasks_scores[task_id][i];
	this->scoreboards[milestone_id].reset_order();
}

int ScoreboardService::add_team(string team_name) {
	team_id = this->teams.size();
	this->teams.push_back(team_name);
	this->teams_map[team_name] = team_id;
	
	this->tasks_scores.push_back(vector<float>(this->tasks.size()));
	for (int i = 0; i < this->milestones.size(); ++i)
		this->scoreboards[i].scores.push_back(0);

	return team_id;
}

int ScoreboardService::get_team_id(string team_name, bool create) {
	assert(create or this->teams_map.count(team_name) != 0);
	if (this->teams_map.count(team_name) == 0)
		return this->add_team(team_name);
	return this->teams_map[team_name];
}

void ScoreboardService::update_score(string team_name, int task_id, float new_score) {
	int team_id = this->get_team_id(team_name, true);
	assert(this->tasks_map.count(task_id) != 0);
	task_id = this->tasks_map[task_id];
	float score_diff = new_score - this->tasks_scores[team_id][task_id];
	for (int ms_id : this->tasks[task_id].milestones) {
		this->scoreboards[ms_id].erase(team_id);
		this->scoreboards[ms_id].scores[team_id] += score_diff;
		this->scoreboards[ms_id].insert(team_id);
	}
	this->tasks_scores[team_id][task_id] = new_score;
}

