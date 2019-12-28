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

void ScoreboardService::add_task(Task &task) {
	this->tasks_map[task.id] = this->tasks.size();
	this->tasks.push_back(task);
}

void ScoreboardService::add_milestone(Milestone &ms) {
	this->milestones_map[ms.id] = this->milestones.size();
	this->milestones.push_back(ms);
}

