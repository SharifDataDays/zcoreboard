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

void get_method_handler(const shared_ptr<Session> session)
{
	session->close( OK, "Password Protected Hello, World!", { { "Content-Length", "32" } } );
}

void add_milestone_handler(const shared_ptr<Session> session)
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

			Milestone ms;
			ms.id = root["ms_id"].asInt();
			ms.name = root["ms_name"].asString();
			cout << ms.id << ' ' << ms.name << endl;
			ScoreboardService::getInstance()->add_milestone(ms);
			return session->close(OK, "Done!", {{"Content-Length", "5"}});
			}
			);
}

void add_task_handler(const shared_ptr<Session> session)
{
	session->close( OK, "Pishword Protected Hello, World!", { { "Content-Length", "32" } } );
}

void update_score_handler(const shared_ptr<Session> session)
{
	session->close( OK, "Pishword Protected Hello, World!", { { "Content-Length", "32" } } );
}

void view_scoreboard_handler(const shared_ptr<Session> session)
{
	auto request = session->get_request();
	std::string key = request->get_path_parameter("mag");
	std::string data = "haha";
	std::cout << key << " => " << data << std::endl;
	session->close(restbed::OK, data, { { "Content-Length", std::to_string(data.size()) } });
}

void team_score_handler(const shared_ptr<Session> session)
{
	session->close( OK, "Pishword Protected Hello, World!", { { "Content-Length", "32" } } );
}

void ScoreboardService::run() {
	auto add_ms = make_shared<Resource>();
	add_ms->set_path("/add_ms");
	add_ms->set_method_handler("POST", add_milestone_handler);

	auto add_task = make_shared<Resource>();
	add_task->set_path("/add_task");
	add_task->set_method_handler("POST", add_task_handler);

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
	service.publish(update_score);
	service.publish(view_scoreboard);
	service.publish(team_score);

	service.start(settings);

	return;
}
