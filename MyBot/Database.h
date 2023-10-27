#include <dpp/dpp.h>
#include "mysql_connection.h"
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/prepared_statement.h>

#pragma once
class Database
{
	private:
		int get_max_id_suggestions();
		int get_max_id_users();


	public:
		// Database handling
		void connect_to_database();

		// Suggestions
		int find_suggestion_in_database(std::string url);
		boolean is_suggestion_in_database(std::string url);
		void add_suggestion_to_database(std::string url, std::string description, std::string creater_url);

		// Users
		void add_user_to_database(std::string userID, std::string discordName, boolean hasVotedUp, boolean hasVotedDown, int suggestionID);
		std::vector<int> find_users_in_suggestion(int suggestionID);
		boolean user_has_vote_up(int idInDB);
		boolean user_has_vote_down(int idInDB);

		// Config
};