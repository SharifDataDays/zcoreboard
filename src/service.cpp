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
	for (int i = 0; i < this->teams.size(); ++i)
		this->tasks_scores[i].push_back(0);
	return tid;
}

int ScoreboardService::add_milestone(Milestone &ms) {
	if (this->milestones_map.count(ms.id) != 0)
		throw MilestoneDuplicateIdError();
	int msid = this->milestones.size();
	this->milestones_map[ms.id] = msid;
	this->milestones.push_back(ms);
	this->scoreboards.push_back(Scoreboard(this->teams.size()));
	return msid;
}

void ScoreboardService::add_task_to_milestone(int task_id, int milestone_id) {
	task_id = this->get_task_id(task_id);
	milestone_id = this->get_milestone_id(milestone_id);
	this->tasks[task_id].milestones.push_back(milestone_id);
	this->milestones[milestone_id].tasks.push_back(task_id);
	for (int i = 0; i < this->teams.size(); ++i)
		this->scoreboards[milestone_id].scores[i] += this->tasks_scores[task_id][i];
	this->scoreboards[milestone_id].reset_order();
}

int ScoreboardService::add_team(string team_name) {
	int team_id = this->teams.size();
	this->teams.push_back(team_name);
	this->teams_map[team_name] = team_id;

	this->tasks_scores.push_back(vector<float>(this->tasks.size()));
	for (int i = 0; i < this->milestones.size(); ++i) {
		this->scoreboards[i].scores.push_back(0);
		this->scoreboards[i].insert(team_id);
	}

	return team_id;
}

int ScoreboardService::get_team_id(string team_name, bool create) {
	if (!create and this->teams_map.count(team_name) == 0)
		throw TeamNotFoundError();
	if (this->teams_map.count(team_name) == 0)
		return this->add_team(team_name);
	return this->teams_map[team_name];
}

void ScoreboardService::update_score(string team_name, int task_id, float new_score) {
	int team_id = this->get_team_id(team_name, true);
	task_id = this->get_task_id(task_id);
	float score_diff = new_score - this->tasks_scores[team_id][task_id];
	for (int ms_id : this->tasks[task_id].milestones) {
		this->scoreboards[ms_id].remove(team_id);
		this->scoreboards[ms_id].scores[team_id] += score_diff;
		this->scoreboards[ms_id].insert(team_id);
	}
	this->tasks_scores[team_id][task_id] = new_score;
}

string ScoreboardService::get_scoreboard(int start_index, int end_index, int milestone_id) {
	milestone_id = this->get_milestone_id(milestone_id);
	vector<int> teams = this->scoreboards[milestone_id].get_by_rank(start_index - 1, end_index);
	Json::Value root;
	root["milestone"]["name"] = this->milestones[milestone_id].name;
	root["milestone"]["id"] = this->milestones[milestone_id].id;
	for (int i = 0; i < this->milestones[milestone_id].tasks.size(); ++i) {
		auto &task = this->tasks[this->milestones[milestone_id].tasks[i]];
		Json::Value current_task;
		current_task["id"] = task.id;
		current_task["name"] = task.name;
		root["tasks"][i] = current_task;
	}
	for (int i = 0; i < teams.size(); ++i) {
		int tid = teams[i];
		Json::Value current_team;
		current_team["rank"] = start_index + i;
		current_team["total_score"] = score_to_string(this->scoreboards[milestone_id].scores[tid]);
		current_team["name"] = this->teams[tid];
		for (int j = 0; j < this->milestones[milestone_id].tasks.size(); ++j) {
			auto &task = this->milestones[milestone_id].tasks[j];
			current_team["scores"][j] = score_to_string(this->tasks_scores[tid][j]);
		}
		root["scoreboard"][i] = current_team;
	}
	return Json::FastWriter().write(root);
}

string ScoreboardService::get_team_info(string team_name, int milestone_id) {
	milestone_id = this->get_milestone_id(milestone_id);
	int team_id = this->get_team_id(team_name, false);
	auto &milestone = this->milestones[milestone_id];
	Json::Value root;
	root["milestone"]["name"] = this->milestones[milestone_id].name;
	root["milestone"]["id"] = this->milestones[milestone_id].id;
	for (int i = 0; i < milestone.tasks.size(); ++i) {
		auto &task = this->tasks[milestone.tasks[i]];
		Json::Value current_task;
		current_task["id"] = task.id;
		current_task["name"] = task.name;
		root["tasks"][i] = current_task;
		root["scores"][i] = this->tasks_scores[team_id][milestone.tasks[i]];
	}
	root["rank"] = 1 + this->scoreboards[milestone_id].get_team_rank(team_id);
	return Json::FastWriter().write(root);
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

