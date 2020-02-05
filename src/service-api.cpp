#include "service.hpp"

#include <memory>
#include <cstdlib>
#include <functional>
#include <restbed>
#include <iostream>
#include <jsoncpp/json/json.h>

#include "milestone.hpp"
#include "errors.hpp"

#define PORT 8000
#define MAX_SCOREBOARD_PAGE_SIZE 50

using namespace std;
using namespace restbed;

void get_method_handler(const shared_ptr<Session> session) {
	session->close( OK, "Password Protected Hello, World!", { { "Content-Length", "32" } } );
}

void add_milestone_handler(const shared_ptr<Session> session) {
	auto request = session->get_request();
	size_t request_content_length = stoi(request->get_header("Content-Length", "0"));
	session->fetch(request_content_length, [request](const std::shared_ptr<restbed::Session> session, const restbed::Bytes & body)
			{
			string data = string((char*)body.data(), body.size());
			Json::Reader reader;
			Json::Value root;
			if (!reader.parse(data, root)) {
				session->close(UNPROCESSABLE_ENTITY, "Bad input :(", {{"Content-Length", "12"}});
				return;
			}

			Milestone ms;
			ms.id = root["ms_id"].asInt();
			ms.name = root["ms_name"].asString();
			ScoreboardService::getInstance()->add_milestone(ms);
			return session->close(OK, "Done!", {{"Content-Length", "5"}});
			}
			);
}

void add_task_handler(const shared_ptr<Session> session) {
	auto request = session->get_request();
	size_t request_content_length = stoi(request->get_header("Content-Length", "0"));
	session->fetch(request_content_length, [request](const std::shared_ptr<restbed::Session> session, const restbed::Bytes & body)
			{
			string data = string((char*)body.data(), body.size());
			Json::Reader reader;
			Json::Value root;
			if (!reader.parse(data, root)) {
				session->close(UNPROCESSABLE_ENTITY, "Bad input :(", {{"Content-Length", "12"}});
				return;
			}

			auto ss = ScoreboardService::getInstance();
			Task task;
			task.id = root["task_id"].asInt();
			task.name = root["task_name"].asString();
			ss->add_task(task);
			return session->close(OK, "Done!", {{"Content-Length", "5"}});
			}
			);
}

void add_task_to_milestone_handler(const shared_ptr<Session> session) {
	auto request = session->get_request();
	size_t request_content_length = stoi(request->get_header("Content-Length", "0"));
	session->fetch(request_content_length, [request](const std::shared_ptr<restbed::Session> session, const restbed::Bytes & body)
			{
			string data = string((char*)body.data(), body.size());
			Json::Reader reader;
			Json::Value root;
			if (!reader.parse(data, root)) {
				session->close(UNPROCESSABLE_ENTITY, "Bad input :(", {{"Content-Length", "12"}});
				return;
			}

			auto ss = ScoreboardService::getInstance();
			int task_id = root["task_id"].asInt();
			int milestone_id = root["ms_id"].asInt();
			ss->add_task_to_milestone(task_id, milestone_id);
			return session->close(OK, "Done!", {{"Content-Length", "5"}});
			}
			);
}

void update_score_handler(const shared_ptr<Session> session)
{
	auto request = session->get_request();
	size_t request_content_length = stoi(request->get_header("Content-Length", "0"));
	session->fetch(request_content_length, [request](const std::shared_ptr<restbed::Session> session, const restbed::Bytes & body)
			{
			string data = string((char*)body.data(), body.size());
			Json::Reader reader;
			Json::Value root;
			if (!reader.parse(data, root)) {
				session->close(UNPROCESSABLE_ENTITY, "Bad input :(", {{"Content-Length", "12"}});
				return;
			}

			auto ss = ScoreboardService::getInstance();
			int task_id = root["task_id"].asInt();
			string team_name = root["team_name"].asString();
			float new_score = root["score"].asFloat();
			ss->update_score(team_name, task_id, new_score);
			return session->close(OK, "Done!", {{"Content-Length", "5"}});
			}
			);
}

void view_scoreboard_handler(const shared_ptr<Session> session)
{
	auto req = session->get_request();
	int start_index = stoi(req->get_query_parameter("start_index"));
	int end_index = stoi(req->get_query_parameter("end_index"));
	int ms_id = stoi(req->get_query_parameter("ms_id"));

	auto ss = ScoreboardService::getInstance();
	string result;
	if (end_index - start_index > MAX_SCOREBOARD_PAGE_SIZE)
		throw LongRangeError();
	result = ss->get_scoreboard(start_index, end_index, ms_id);
	session->close(OK, result, {{"Content-Length", to_string(result.size())}, {"Content-Type", "application/json"}});
}

void team_score_handler(const shared_ptr<Session> session)
{
	auto req = session->get_request();
	string team_name = req->get_query_parameter("team_name");
	string ms_str = req->get_query_parameter("ms_id");

	auto ss = ScoreboardService::getInstance();
	string result;
	if (ms_str == "")
		result = ss->get_team_info(team_name, -1);
	else
		result = ss->get_team_info(team_name, stoi(ms_str));
	session->close(OK, result, {{"Content-Length", to_string(result.size())}, {"Content-Type", "application/json"}});
}

void milestone_info_handler(const shared_ptr<Session> session) {
	auto req = session->get_request();
	int ms_id = stoi(req->get_query_parameter("ms_id"));
	Json::Value result;
	result["teams_count"] = ScoreboardService::getInstance()->get_teams_count(ms_id);
	Json::FastWriter writer;
	string result_string = writer.write(result);
	session->close(OK, result_string, {{"Content-Length", to_string(result_string.size())}, {"Content-Type", "application/json"}});
}

void error_handler(const int, const exception& e, const shared_ptr<Session> session)
{
	if (session->is_closed())
		return;
	const BaseError *err = dynamic_cast<const BaseError*>(&e);
	int status;
	const char* message;
	if (err == nullptr)
		status = 500, message = InternalServerError().details;
	else
		status = err->status, message = err->details;
	Json::Value result;
	result["status_code"] = status;
	result["details"] = string(message);

	Json::FastWriter writer;
	string result_string = writer.write(result);

	session->close(OK, result_string, {{"Content-Length", to_string(result_string.size())}, {"Content-Type", "application/json"}});
}

void ScoreboardService::run()
{
	auto add_ms = make_shared<Resource>();
	add_ms->set_path("/add_ms");
	add_ms->set_method_handler("POST", add_milestone_handler);

	auto add_task = make_shared<Resource>();
	add_task->set_path("/add_task");
	add_task->set_method_handler("POST", add_task_handler);

	auto add_task_to_milestone = make_shared<Resource>();
	add_task_to_milestone->set_path("/add_task_to_milestone");
	add_task_to_milestone->set_method_handler("POST", add_task_to_milestone_handler);

	auto update_score = make_shared<Resource>();
	update_score->set_path("/update_score");
	update_score->set_method_handler("POST", update_score_handler);

	auto view_scoreboard = make_shared<Resource>();
	view_scoreboard->set_path("/scoreboard");
	view_scoreboard->set_method_handler("GET", view_scoreboard_handler);

	auto team_score = make_shared<Resource>();
	team_score->set_path("/team_score");
	team_score->set_method_handler("GET", team_score_handler);

	auto milestone_info = make_shared<Resource>();
	milestone_info->set_path("/milestone_info");
	milestone_info->set_method_handler("GET", milestone_info_handler);

	auto settings = make_shared<Settings>();
	settings->set_port(PORT);
	settings->set_default_header("Connection", "close");

	Service service;
	service.publish(add_ms);
	service.publish(add_task);
	service.publish(add_task_to_milestone);
	service.publish(update_score);
	service.publish(view_scoreboard);
	service.publish(team_score);
    service.publish(milestone_info);

	service.set_error_handler(error_handler);

	service.start(settings);

	return;
}
