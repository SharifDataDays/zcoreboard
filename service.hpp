#ifndef SCOREBOARD_SERVICE_H
#define SCOREBOARD_SERVICE_H

class ScoreboardService {
	private:
		static ScoreboardService *instance;
		ScoreboardService();
		ScoreboardService(char *config_file);

	public:
		static ScoreboardService* getInstance();
		static ScoreboardService* getInstance(char *config_file);

		void run();
};

#endif
