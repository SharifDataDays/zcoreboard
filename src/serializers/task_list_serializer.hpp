#ifndef LIST_SERIALIZER_H
#define LIST_SERIALIZER_H

#include "base_serializer.hpp"
#include "../task.hpp"

class TaskListSerializer : public BaseSerializer {
public:
	vector<Task *> *entities;

	TaskListSerializer(vector<Task *> *entities);
	Json::Value serialize();
};

#endif
