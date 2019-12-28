#ifndef SCOREBOARD_SERVICE_H
#define SCOREBOARD_SERVICE_H

#include <map>
#include <vector>
#include "task.hpp"
#include "milestone.hpp"
#include "scoreboard.hpp"

class ScoreboardService {
	private:
		static ScoreboardService *instance;
		ScoreboardService();
		ScoreboardService(char *config_file);

	public:
		map<int, int> milestones_map,
			tasks_map;
		vector<Task> tasks;
		vector<Milestone> milestones;
		vector<Scoreboard> milestone_scoreboards;
		Scoreboard general_scoreboard;

		static ScoreboardService* getInstance();
		static ScoreboardService* getInstance(char *config_file);

		void run();

		int add_task(Task &task);
		int add_milestone(Milestone &ms);
		void add_task_to_milestone(int task_id, int milestone_id);
		int add_team(string team_name);
		void update_score(string team_name, int task_id, int new_score);
};

#endif
