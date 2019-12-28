#include "service.hpp"

#include <memory>
#include <cstdlib>
#include <functional>
#include <restbed>
#include <iostream>
#include <jsoncpp/json/json.h>

#include "milestone.hpp"

#define PORT 8080

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
			cout << ms.id << ' ' << ms.name << endl;
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
			cout << task.id << ' ' << task.name << endl;
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
			int ms_id = root["ms_id"].asString();
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
	string ms_str = req->get_query_parameter("ms_id");

	auto ss = ScoreboardService::getInstance();
	string result;
	if (end_index - start_index > MAX_SCOREBOARD_PAGE_SIZE) {
		session->close(BAD_REQUEST, "Bazeh be in bozorgi? Gandesho dar avordi!", {{"Content-Length", "41"}});
		return;
	}
	if (ms_str == "") {
		result = ss->get_scoreboard(start_index, end_index, -1);
	else
		result = ss->get_scoreboard(start_index, end_index, stoi(ms_str));
	session->close(OK, result, {{"Content-Length", to_string(result.size())}, {"Content-Type", "application/json"}});
}

void team_score_handler(const shared_ptr<Session> session)
{
	auto req = session->get_request();
	int team_id = stoi(req->get_query_parameter("team_id"));
	string ms_str = req->get_query_parameter("ms_id");

	auto ss = ScoreboardService::getInstance();
	string result;
	if (ms_str == "") {
		result = ss->get_team_info(team_id, -1);
	else
		result = ss->get_team_info(team_id, stoi(ms_str));
	session->close(OK, result, {{"Content-Length", to_string(result.size())}, {"Content-Type", "application/json"}});
}

void ScoreboardService::run() {
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

	service.start(settings);

	return;
}
