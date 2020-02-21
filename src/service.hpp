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
		vector<Scoreboard> scoreboards;

		static ScoreboardService* getInstance();
		static ScoreboardService* getInstance(char *config_file);

		void run();

		int add_task(Task &task);
		int add_milestone(Milestone &ms);
		void add_task_to_milestone(int task_id, int milestone_id);
		int add_team(string team_name);
		void update_score(string team_name, int task_id, float new_score);

		string get_scoreboard(int start_index, int end_index, int milestone_id);
		string get_team_info(string team_name, int milestone_id);
		int get_teams_count(int ms_id);

		int get_team_id(string team_name, bool create);
		int get_milestone_id(int ms_id);
		int get_task_id(int task_id);
};

#endif
