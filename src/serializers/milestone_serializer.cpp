#include "milestone_serializer.hpp"

MilestoneSerializer::MilestoneSerializer(Milestone *milestone) {
	this->milestone = milestone;
}

Json::Value MilestoneSerializer::serialize() {
	Json::Value milestone;
	milestone["id"] = this->milestone->id;
	milestone["name"] = this->milestone->name;
	return milestone;
}

