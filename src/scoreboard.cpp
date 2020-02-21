#include "scoreboard.hpp"
#include "errors.hpp"
#include "service.hpp"
#include "serializers/task_serializer.hpp"
#include "serializers/milestone_serializer.hpp"
#include "serializers/task_list_serializer.hpp"
#include "utils.hpp"
#include <algorithm>

Scoreboard::Scoreboard(Milestone &ms) {
	this->milestone = ms;
}

void Scoreboard::remove(int team_id) {
	for (int i = 0; i < this->order.size(); ++i)
		if (this->order[i] == team_id)
			this->order.erase(this->order.begin() + i), --i;
}

void Scoreboard::insert(int team_id) {
	this->order.push_back(team_id);
	this->reset_order();
}

void Scoreboard::reset_order(void) {
	auto &scores = this->total_scores;
	sort(this->order.begin(), this->order.end(), [&scores](int i, int j) { return scores[i] > scores[j];});
}

int Scoreboard::get_team_rank(int team_id) {
	if (team_id == -1)
		return this->get_teams_count();
	for (int i = 0; i < this->order.size(); ++i)
		if (this->total_scores[this->order[i]] == this->total_scores[team_id])
			return i;
	return -1;
}

int Scoreboard::get_team_id(string team_name, bool create) {
	if (this->teams_map.count(team_name) == 0) {
		if (create)
			return this->add_team(team_name);
		else
			return -1;
	}
	return this->teams_map[team_name];
}

int Scoreboard::add_team(string team_name) {
	int team_id = this->teams.size();
	this->teams.push_back(team_name);
	this->teams_map[team_name] = team_id;

	this->tasks_scores.push_back(vector<float>(this->tasks.size()));
	this->total_scores.push_back(0.);
	this->order.push_back(team_id);

	return team_id;
}

float Scoreboard::get_score(int team_id, int task_id) {
	if (team_id == -1)
		return 0.;
	return tasks_scores[team_id][task_id];
}

int Scoreboard::get_task_id(int task_id) {
	if (this->tasks_map.count(task_id))
		return this->tasks_map[task_id];
	throw TaskNotFoundError();
}

int Scoreboard::get_teams_count(void) {
	return this->teams.size();
}

void Scoreboard::add_task(int task_id) {
	if (this->tasks_map.count(task_id))
		throw TaskDuplicateIdError();
	int new_task_id = this->tasks.size();
	this->tasks_map[task_id] = new_task_id;
	this->tasks.push_back(task_id);
	this->milestone.add_task(task_id);
}

void Scoreboard::update_score(string team_name, int task_id, float new_score) {
	task_id = this->get_task_id(task_id);
	int team_id = this->get_team_id(team_name, true);
	if (team_id == -1)
		throw TeamNotFoundError();
	
	this->remove(team_id);
	this->total_scores[team_id] += new_score - this->tasks_scores[team_id][task_id];
	this->tasks_scores[team_id][task_id] = new_score;
	this->insert(team_id);
}

Json::Value Scoreboard::get_team_info(string team_name) {
	auto ss = ScoreboardService::getInstance();
	Json::Value root;
	root["milestone"] = MilestoneSerializer(&this->milestone).serialize();
	int team_id = this->get_team_id(team_name, false);
	for (int i = 0; i < this->tasks.size(); ++i) {
		auto &task = ss->tasks[this->tasks[i]];
		root["tasks"][i] = TaskSerializer(&task).serialize();
		root["scores"][i] = this->get_score(team_id, i);
	}
	root["rank"] = 1 + this->get_team_rank(team_id);
	return root;
}

Json::Value Scoreboard::get_scoreboard(int start_index, int end_index) {
	end_index = min(end_index, (int)this->order.size());
	if (end_index <= start_index or start_index >= this->teams.size())
		return Json::Value(Json::arrayValue);

	auto ss = ScoreboardService::getInstance();

	vector<Task*> tasks_list;
	for (auto task_id : this->tasks)
		tasks_list.push_back(&ss->tasks[task_id]);

	Json::Value scoreboard;
	scoreboard["milestone"] = MilestoneSerializer(&this->milestone).serialize();
	scoreboard["tasks"] = TaskListSerializer(&tasks_list).serialize();
	for (int i = start_index; i < end_index; i++) {
		int team_id = this->order[i];
		Json::Value current_team;
		current_team["rank"] = i + 1;
		current_team["total_score"] = score_to_string(this->total_scores[team_id]);
		current_team["name"] = this->teams[team_id];
		for (int j = 0; j < this->tasks.size(); j++)
			current_team["scores"][j] = score_to_string(this->tasks_scores[team_id][j]);
		scoreboard["scoreboard"][i] = current_team;
	}
	return scoreboard;
}

