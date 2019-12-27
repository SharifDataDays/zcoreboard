#include <iostream>

#include "service.hpp"

using namespace std;

int main(int argc, char *argv[]) {
	ScoreboardService *ss;
	if (argc == 2)
		ss = ScoreboardService::getInstance(argv[1]);
	else
		ss = ScoreboardService::getInstance();
	ss->run();
	return 0;
}
