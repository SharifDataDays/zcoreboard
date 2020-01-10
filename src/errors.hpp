#ifndef ERRORS_H
#define ERRORS_H

#include <exception>

class BaseError : public std::exception {
public:
	uint16_t status;
	const char* details;
	const char* what()
	{
		return details;
	}
};

class TaskNotFoundError : public BaseError {
public:
	TaskNotFoundError()
	{
		status = 404;
		details = "task not found";
	}
};

class MilestoneNotFoundError : public BaseError {
public:
	MilestoneNotFoundError()
	{
		status = 404;
		details = "milestone not found";
	}
};

class TeamNotFoundError : public BaseError {
	public:
	TeamNotFoundError()
	{
		status = 404;
		details = "team not found";
	}
};

class TaskDuplicateIdError : public BaseError {
public:
	TaskDuplicateIdError()
	{
		status = 400;
		details = "already have a problem with this id";
	}
};

class MilestoneDuplicateIdError : public BaseError {
public:
	MilestoneDuplicateIdError()
	{
		status = 400;
		details = "already have a milestone with this id";
	}
};

class LongRangeError : public BaseError {
public:
	LongRangeError()
	{
		status = 400;
		details = "requested range is too large";
	}
};

class InternalServerError : public BaseError {
public:
	InternalServerError()
	{
		status = 500;
		details = "internal server error";
	}
};

#endif

