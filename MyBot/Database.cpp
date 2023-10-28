#include "Database.h"


const std::string server = "tcp://127.0.0.1:3306";
const std::string username = "root";
const std::string password = "Gse26fvrGse26fvr%";

sql::Driver* driver;
sql::Connection* con;
sql::Statement* stmt;
sql::PreparedStatement* pstmt;
sql::ResultSet* result;
sql::Statement* stmt;


sql::SQLString convert_string_to_sqlstring(std::string string) {
	return string.c_str();
}

// SUGGESTIONS

void Database::connect_to_database() {
	try
	{
		driver = get_driver_instance();
		con = driver->connect(server, username, password);
		std::cout << "Successfully connected to the database " << std::endl;
		con->setSchema("testdb");
	}
	catch (sql::SQLException e)
	{
		std::cout << "Could not connect to server. Error message: " << e.what() << std::endl;
		system("pause");
		exit(1);
	}
}

int Database::get_max_id_suggestions() {
	int resultInt = -1;

	pstmt = con->prepareStatement("SELECT MAX(iduser) FROM suggestions;");
	result = pstmt->executeQuery();

	if (result->next()) {
		resultInt = result->getInt(1);
	}

	return resultInt;
}

void Database::add_suggestion_to_database(std::string url, std::string description, std::string creater_url) {
	pstmt = con->prepareStatement("INSERT INTO suggestions(id, url, description, creater_discord_id) VALUES(?,?,?,?)");

	pstmt->setInt(1, get_max_id_suggestions()+1);
	pstmt->setString(2, url);
	pstmt->setString(3, description);
	pstmt->setString(4, creater_url);
	pstmt->setInt(5, 0);

	pstmt->execute();
}

int Database::find_suggestion_in_database(std::string url) {
	int temp = -1;

	pstmt = con->prepareStatement("SELECT * FROM suggestions WHERE url = " + url);
	result = pstmt->executeQuery();

	if (result->next()) {
		temp = result->getInt(1);
	}

	return temp;
}

boolean Database::is_suggestion_in_database(std::string url) {
	boolean b = false;
	if (find_suggestion_in_database(url) != -1) {
		b = true;
	}
	return b;
}

void Database::add_vote(dpp::user user, int suggestionIDDB) {
	sql::SQLString userSqlString = convert_string_to_sqlstring(user.id.str());

	if (!is_user_in_suggestion(userSqlString, suggestionIDDB)) {
		add_user_to_database(user.id.str(), user.format_username(), false, false, suggestionIDDB);
	}

	if (!user_has_vote_up(userSqlString, suggestionIDDB)) {
		update_react_up(userSqlString, suggestionIDDB);

		pstmt = con->prepareStatement("SELECT * FROM suggestions WHERE id = " + suggestionIDDB);
		result = pstmt->executeQuery();

		int currentVotes = result->getInt(5) + 1;

		pstmt = con->prepareStatement("UPDATE suggestions SET votes = (?) WHERE id = (?)");
		pstmt->setInt(1, currentVotes);
		pstmt->setInt(2, suggestionIDDB);
		result = pstmt->executeQuery();
	}
	else {
		int currentVotes = result->getInt(5) - 1;

		pstmt = con->prepareStatement("UPDATE suggestions SET votes = (?) WHERE id = (?)");
		pstmt->setInt(1, currentVotes);
		pstmt->setInt(2, suggestionIDDB);
		result = pstmt->executeQuery();

		update_react_down(userSqlString, suggestionIDDB);
	}
}

void Database::subtract_vote(dpp::user user, int suggestionIDDB) {

	sql::SQLString userSqlString = convert_string_to_sqlstring(user.id.str());

	if (!is_user_in_suggestion(userSqlString, suggestionIDDB)) {
		add_user_to_database(user.id.str(), user.format_username(), false, false, suggestionIDDB);
	}

	if (!user_has_vote_down(userSqlString, suggestionIDDB)) {
		update_react_down(userSqlString, suggestionIDDB);

		pstmt = con->prepareStatement("SELECT * FROM suggestions WHERE id = " + suggestionIDDB);
		result = pstmt->executeQuery();

		int currentVotes = result->getInt(5) -1;

		pstmt = con->prepareStatement("UPDATE suggestion SET votes = (?) WHERE id = (?)");
		pstmt->setInt(1, currentVotes);
		pstmt->setInt(2, suggestionIDDB);
		result = pstmt->executeQuery(); 
	}
	else {
		int currentVotes = result->getInt(5) + 1;

		pstmt = con->prepareStatement("UPDATE suggestion SET votes = (?) WHERE id = (?)");
		pstmt->setInt(1, currentVotes);
		pstmt->setInt(2, suggestionIDDB);
		result = pstmt->executeQuery();

		update_react_down(userSqlString, suggestionIDDB);
	}
}

// USERS

int Database::get_max_id_users() {
	pstmt = con->prepareStatement("SELECT MAX(iduser) FROM user;");
	result = pstmt->executeQuery();

	return result->getInt(1);
}

void Database::add_user_to_database(std::string userID, std::string discordName, boolean hasVotedUp, boolean hasVotedDown, int suggestionIDDB) {
	pstmt = con->prepareStatement("INSERT INTO user(iduser, discord_id, discord_username, hasVotedUp, hasVotedDown, suggestion_id) VALUES(?, ?, ?, ?, ?, ?)");

	pstmt->setInt(1, get_max_id_users()+1);
	pstmt->setString(2, userID);
	pstmt->setString(3, discordName);
	pstmt->setBoolean(4, hasVotedUp);
	pstmt->setBoolean(5, hasVotedDown);
	pstmt->setInt(6, suggestionIDDB);

	pstmt->execute();
}

std::vector<int> Database::find_users_in_suggestion(int suggestionID) {
	std::vector<int> returnVector;

	pstmt = con->prepareStatement("SELECT * FROM user WHERE suggestion_id = " + suggestionID);
	result = pstmt->executeQuery();

	while (result->next()) {
		returnVector.push_back(result->getInt(1));
	}
	return returnVector;
}

int Database::find_user(sql::SQLString discorduserid, int suggestionDBID) {
	int i = -1;

	pstmt = con->prepareStatement("SELECT * FROM user WHERE discord_id = " + discorduserid);
	result = pstmt->executeQuery();

	if (result->next()) {
		i = result->getInt(1);
	}

	return i;
}

boolean Database::is_user_in_suggestion(sql::SQLString discorduserid, int suggestionDBID) {
	boolean result = false;
	if (find_user(discorduserid, suggestionDBID) != -1) {
		result = true;
	}
	return result;
}

boolean Database::user_has_vote_up(sql::SQLString discorduserid, int suggestionDBID) {
	boolean resultBoolean = false;
	
	pstmt = con->prepareStatement("SELECT * FROM user WHERE iduser = " + find_user(discorduserid, suggestionDBID));
	result = pstmt->executeQuery();

	if (result->getBoolean(4)) {
		resultBoolean = true;
	} 
	else {
		resultBoolean = false;
	}
	return resultBoolean;
}

boolean Database::user_has_vote_down(sql::SQLString discorduserid, int suggestionDBID) {
	boolean resultBoolean = false;

	pstmt = con->prepareStatement("SELECT * FROM user WHERE iduser = " + find_user(discorduserid, suggestionDBID));
	result = pstmt->executeQuery();

	if (result->getBoolean(5)) {
		resultBoolean = true;
	}
	else {
		resultBoolean = false;
	}
	return resultBoolean;
}

void Database::update_react_down(sql::SQLString discorduserid, int suggestionDBID) {
	if (user_has_vote_down(discorduserid, suggestionDBID)) {
		pstmt = con->prepareStatement("UPDATE user SET hasVotedDown = true WHERE iduser = " + find_user(discorduserid, suggestionDBID));
		result = pstmt->executeQuery();
	}
	else {
		pstmt = con->prepareStatement("UPDATE user SET hasVotedDown = false WHERE iduser = " + find_user(discorduserid, suggestionDBID));
		result = pstmt->executeQuery();
	}
}

void Database::update_react_up(sql::SQLString discorduserid, int suggestionDBID) {
	if (user_has_vote_up(discorduserid, suggestionDBID)) {
		pstmt = con->prepareStatement("UPDATE user SET hasVotedUp = true WHERE iduser = " + find_user(discorduserid, suggestionDBID));
		result = pstmt->executeQuery();
	}
	else {
		pstmt = con->prepareStatement("UPDATE user SET hasVotedUp = false WHERE iduser = " + find_user(discorduserid, suggestionDBID));
		result = pstmt->executeQuery();
	}
}

// SERVER CONFIGS


int get_max_config_id() {
	int resultInt = -1;

	pstmt = con->prepareStatement("SELECT MAX(id) FROM server_config");
	result = pstmt->executeQuery();

	if (result->next()) {
		resultInt = result->getInt(1);
	}

	return resultInt;
}

void Database::add_config(std::string guild_id, std::string suggest_channel_id, std::string approve_channel_id, std::string role_id) {
	pstmt = con->prepareStatement("INSERT INTO server_config(id, guild_id, suggest_channel_id, approve_channel_id, role_id) VALUES(?, ?, ?, ?, ?)");

	pstmt->setInt(1, get_max_config_id() + 1);
	pstmt->setString(2, guild_id);
	pstmt->setString(3, suggest_channel_id);
	pstmt->setString(4, approve_channel_id);
	pstmt->setString(5, role_id);

	pstmt->execute();
}


int Database::find_config(std::string guild_id) {
	int i = -1;

	pstmt = con->prepareStatement("SELECT * FROM server_config WHERE guild_id = (?)");
	pstmt->setString(1, guild_id);

	result = pstmt->executeQuery();

	if (result->next()) {
		i = result->getInt(1);
	}

	return i;
}

std::vector<int> Database::different_value_locations(std::string guild_id, std::string suggest_channel_id, std::string approve_channel_id, std::string role_id) {
	std::vector<int> returnVector;

	pstmt = con->prepareStatement("SELECT * FROM server_config WHERE guild_id = (?)");
	pstmt->setString(1, guild_id);
	result = pstmt->executeQuery();

	if (result->getString(2) == convert_string_to_sqlstring(suggest_channel_id)) {
		returnVector.push_back(2);
	}
	if (result->getString(3) == convert_string_to_sqlstring(approve_channel_id)) {
		returnVector.push_back(3);
	}
	if (result->getString(4) == convert_string_to_sqlstring(role_id)) {
		returnVector.push_back(4);
	}
}

void Database::update_config_suggest_channel_id(std::string suggest_channel_id, int configID) {
	pstmt = con->prepareStatement("UPDATE server_config SET suggest_channel_id = (?) WHERE guild_id = (?)");
	pstmt->setString(1, suggest_channel_id);
	pstmt->setInt(2, configID);

	result = pstmt->executeQuery();
}

void Database::update_config_approve_channel_id(std::string approve_channel_id, int configID) {
	pstmt = con->prepareStatement("UPDATE server_config SET approve_channel_id = (?) WHERE guild_id = (?)");
	pstmt->setString(1, approve_channel_id);
	pstmt->setInt(2, configID);

	result = pstmt->executeQuery();
}

void Database::update_config_role_id(std::string role_id, int configID) {
	pstmt = con->prepareStatement("UPDATE server_config SET role_id = (?) WHERE guild_id = (?)");
	pstmt->setString(1, role_id);
	pstmt->setInt(2, configID);

	result = pstmt->executeQuery();
}