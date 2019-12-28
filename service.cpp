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
	return tid;
}

int ScoreboardService::add_milestone(Milestone &ms) {
	assert(this->milestones_map.count(ms.id) == 0);
	int msid = this->milestones.size();
	this->milestones_map[ms.id] = msid;
	this->milestones.push_back(ms);
	return msid;
}

void ScoreboardService::add_task_to_milestone(int task_id, int milestone_id) {
	assert(this->tasks_map.count(task_id) != 0);
	assert(this->milestones_map.count(milestone_id) != 0);
	task_id = this->tasks_map[task_id];
	milestone_id = this->milestones_map[milestone_id];
	this->tasks[task_id].milestones.push_back(milestone_id);
	this->milestones[milestone_id].tasks.push_back(task_id);
}

void ScoreboardService::update_score(string team_name, int task_id, int new_score) {
	int team_id = this->add_team(team_name, true);
	task_id = this->tasks_map[task_id];
	// remove team from scoreboards
	// update team's score
	// insert team to scoreboards
}

