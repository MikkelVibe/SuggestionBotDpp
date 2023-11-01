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
		int get_max_id_config();

		sql::ResultSet* run_sql_command(std::string sqlString);
		sql::ResultSet* run_sql_command_with_int(std::string sqlString, int input);
		sql::ResultSet* run_sql_command_with_string(std::string sqlString, std::string input);

	public:
		// Database handling
		void connect_to_database();

		// Suggestions
		int find_suggestion_in_database(std::string url);
		boolean is_suggestion_in_database(std::string url);
		void add_suggestion_to_database(std::string url, std::string description, std::string creater_url, int configID);
		void add_vote(dpp::user user, int suggestionIDDB);
		void subtract_vote(dpp::user user, int suggestionIDDB);
		std::string get_description(int suggestionINDB);
		std::string get_creator_discord_id(int suggestionINDB);
		int get_votes(int suggestionINDB);
		std::string get_message_url(int suggestionINDB);
		void delete_suggestion(int suggestionINDB);

		// Users
		void add_user_to_database(std::string userID, std::string discordName, boolean hasVotedUp, boolean hasVotedDown, int suggestionID);
		std::vector<int> find_users_in_suggestion(int suggestionID);
		boolean user_has_vote_up(std::string discorduserid, int suggestionDBID);
		boolean user_has_vote_down(std::string discorduserid, int suggestionDBID);
		int find_user(std::string discorduserid, int suggestionDBID);
		boolean is_user_in_suggestion(std::string discorduseridDB, int suggestionDBID);

		void update_react_up(std::string discorduserid, int suggestionDBID);
		void update_react_down(std::string discorduserid, int suggestionDBID);

		// Configs
		void add_config(std::string guild_id, std::string suggest_channel_id, std::string approve_channel_id, std::string role_id);
		int find_config(std::string guild_id);
		
		std::vector<int> different_value_locations(std::string guild_id, std::string suggest_channel_id, std::string approve_channel_id, std::string role_id);

		void update_config_suggest_channel_id(std::string suggest_channel_id, int configID);
		void update_config_approve_channel_id(std::string approve_channel_id, int configID);
		void update_config_role_id(std::string role_id, int configID);

		std::string get_suggest_channel_id(int configID);
		std::string get_approve_id(int configID);
		std::string get_role_id(int configID);
};