#ifndef SCOREBOARD_SERVICE_H
#define SCOREBOARD_SERVICE_H

#include "task.hpp"
#include "milestone.hpp"

class ScoreboardService {
	private:
		static ScoreboardService *instance;
		ScoreboardService();
		ScoreboardService(char *config_file);

	public:
		static ScoreboardService* getInstance();
		static ScoreboardService* getInstance(char *config_file);

		void run();

		void add_task(Task &task, int milestone_id);
		void add_milestone(Milestone &ms, int milestone_id);
};

#endif
