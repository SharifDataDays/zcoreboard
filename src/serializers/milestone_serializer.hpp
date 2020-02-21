#ifndef MILESTONE_SERIALIZER_H
#define MILESTONE_SERIALIZER_H

#include "base_serializer.hpp"
#include "../milestone.hpp"

class MilestoneSerializer : public BaseSerializer {
public:
	Milestone *milestone;

	MilestoneSerializer(Milestone *milestone);
	Json::Value serialize();
};

#endif
