#ifndef TASK_SERIALIZER_H
#define TASK_SERIALIZER_H

#include "base_serializer.hpp"
#include "../task.hpp"

class TaskSerializer : public BaseSerializer {
public:
	Task *task;

	TaskSerializer(Task *task);
	Json::Value parse();
};

#endif
