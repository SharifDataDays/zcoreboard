#include "task_serializer.hpp"

TaskSerializer::TaskSerializer(Task *task) {
	this->task = task;
}

Json::Value TaskSerializer::parse() {
	Json::Value task;
	task["id"] = this->task->id;
	task["name"] = this->task->name;
	return task;
}

