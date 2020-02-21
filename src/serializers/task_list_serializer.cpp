#include "task_list_serializer.hpp"
#include "task_serializer.hpp"

TaskListSerializer::TaskListSerializer(vector<Task *> *entities) {
	this->entities = entities;
}

Json::Value TaskListSerializer::serialize() {
	Json::Value list;
	for (int i = 0; i < this->entities->size(); i++)
		list[i] = TaskSerializer(this->entities->at(i)).serialize();
	return list;
}

