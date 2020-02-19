#include "task.hpp"

Json::value Task::as_json() {
	Json::value task;
	task["id"] = id;
	task["name"] = name;
	return task;
}

