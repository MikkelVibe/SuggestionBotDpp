#include <dpp/dpp.h>
#include "mysql_connection.h"
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/prepared_statement.h>

#pragma once
class Database
{
	public:
		// Database handling
		void connect_to_database();

		int find_suggestion_in_database(std::string url);
		boolean is_suggestion_in_database(std::string url);
		void add_suggestion_to_database(std::string url, std::string description, std::string creater_url);

		void add_user_to_database(int count, std::string userID, boolean hasVotedUp, boolean hasVotedDown, int suggestionID);
		std::vector<int> find_users_in_suggestion(int suggestionID);
};