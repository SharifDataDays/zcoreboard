#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <jsoncpp/json/json.h>
#include "service.hpp"
#include "task.hpp"
#include "milestone.hpp"
#include "errors.hpp"
#include "utils.hpp"

using namespace std;

ScoreboardService* ScoreboardService::instance = nullptr;

ScoreboardService::ScoreboardService() {
}

ScoreboardService::ScoreboardService(char *config_file) {
	//	ifstream fin(config_file);
	//	int n;
	//	fin >> n;
	//	for (int i = 0; i < n; ++i) {
	//		Milestone ms;
	//		fin >> ms.id;
	//		getline(fin, ms.name);
	//		fin >> ms.tasks_count;
	//		this->add_milestone(ms);
	//		for (int j = 0; j < ms.tasks_count; ++j) {
	//			Task t;
	//			fin >> t.id;
	//			t.milestone_id = i;
	//			getline(fin, t.name);
	//			this->add_task(t);
	//		}
	//	}
	//	
	//	int updates;
	//	fin >> updates;
	//	for (int i = 0; i < updates; i++) {
	//		//TODO continue this part
	//	}
	//	fin.close();
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
	if (this->tasks_map.count(task.id) != 0)
		throw TaskDuplicateIdError();
	int tid = this->tasks.size();
	this->tasks_map[task.id] = tid;
	this->tasks.push_back(task);
	return tid;
}

int ScoreboardService::add_milestone(Milestone &ms) {
	if (this->milestones_map.count(ms.id) != 0)
		throw MilestoneDuplicateIdError();
	int msid = this->scoreboards.size();
	this->milestones_map[ms.id] = msid;
	this->scoreboards.push_back(Scoreboard(ms));
	return msid;
}

void ScoreboardService::add_task_to_milestone(int task_id, int milestone_id) {
	task_id = this->get_task_id(task_id);
	milestone_id = this->get_milestone_id(milestone_id);
	this->tasks[task_id].milestones.push_back(milestone_id);
	this->scoreboards[milestone_id].add_task(task_id);
}

void ScoreboardService::update_score(string team_name, int task_id, float new_score) {
	task_id = this->get_task_id(task_id);
	auto &task = this->tasks[task_id];
	for (auto ms_id : task.milestones)
		this->scoreboards[ms_id].update_score(team_name, task_id, new_score);
}

string ScoreboardService::get_scoreboard(int start_index, int end_index, int milestone_id) {
	milestone_id = this->get_milestone_id(milestone_id);
	Json::Value root = scoreboards[milestone_id].get_scoreboard(start_index - 1, end_index);
	return Json::FastWriter().write(root);
}

string ScoreboardService::get_team_info(string team_name, int milestone_id) {
	milestone_id = this->get_milestone_id(milestone_id);
	auto &milestone = this->scoreboards[milestone_id];
	Json::Value root = milestone.get_team_info(team_name);
	return Json::FastWriter().write(root);
}

int ScoreboardService::get_teams_count(int ms_id) {
	ms_id = this->get_milestone_id(ms_id);
	return this->scoreboards[ms_id].get_teams_count();
}

int ScoreboardService::get_milestone_id(int ms_id) {
	if (this->milestones_map.count(ms_id) == 0)
		throw MilestoneNotFoundError();
	return this->milestones_map[ms_id];
}

int ScoreboardService::get_task_id(int task_id) {
	if (this->tasks_map.count(task_id) == 0)
		throw TaskNotFoundError();
	return this->tasks_map[task_id];
}

