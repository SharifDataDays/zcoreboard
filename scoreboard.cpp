#include "scoreboard.hpp"
#include <algorithm>

Scoreboard::Scoreboard(int teams_count) {
	this->scores = vector<float>(teams_count);
	for (int i = 0; i < teams_count; ++i)
		this->teams.push_back(i);
}

void Scoreboard::remove(int team_id) {
	for (int i = 0; i < this->teams.size(); ++i)
		if (this->teams[i] == team_id)
			this->teams.erase(this->teams.begin() + i), --i;
}

void Scoreboard::insert(int team_id) {
	this->teams.push_back(team_id);
	this->reset_order();
}

void Scoreboard::reset_order(void) {
	auto &scores = this->scores;
	sort(this->teams.begin(), this->teams.end(), [&scores](int i, int j) { return scores[i] > scores[j];});
}

vector<int> Scoreboard::get_by_rank(int start_index, int end_index) {
	end_index = min(end_index, this->teams.size());
	if (end_index <= start_index or start_index >= this->teams.size())
		return vector<int>();
	return vector<int>(this->teams.begin() + start_index, this->teams.begin() + end_index);
}

int Scoreboard::get_team_rank(int team_id) {
	for (int i = 0; i < this->teams.size(); ++i)
		if (this->scores[this->teams[i]] == this->scores[team_id])
			return i;
	return -1;
}

