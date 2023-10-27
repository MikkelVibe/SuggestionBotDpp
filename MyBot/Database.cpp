#include "Database.h"

const std::string server = "tcp://127.0.0.1:3306";
const std::string username = "root";
const std::string password = "";

sql::Driver* driver;
sql::Connection* con;
sql::Statement* stmt;
sql::PreparedStatement* pstmt;
sql::ResultSet* result;
sql::Statement* stmt;


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

int get_max_id_suggestions() {
	pstmt = con->prepareStatement("SELECT MAX(iduser) FROM suggestions;");
	result = pstmt->executeQuery();

	return result->getInt(1);
}

void Database::add_suggestion_to_database(std::string url, std::string description, std::string creater_url) {
	pstmt = con->prepareStatement("INSERT INTO suggestions(id, url, description, creater_discord_id) VALUES(?,?,?,?)");

	pstmt->setInt(1, get_max_id_suggestions()+1);
	pstmt->setString(2, url);
	pstmt->setString(3, description);
	pstmt->setString(4, creater_url);
	pstmt->setInt(5, 0);
}

int Database::find_suggestion_in_database(std::string url) {
	int temp = -1;

	pstmt = con->prepareStatement("SELECT * FROM suggestions;");
	result = pstmt->executeQuery();

	boolean found = false;
	while (!found && result->next()) {
		if (result->getString(2) == url) {
			found = true;
			temp = result->getInt(1);
		}
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

// USERS

int Database::get_max_id_users() {
	pstmt = con->prepareStatement("SELECT MAX(id) FROM user;");
	result = pstmt->executeQuery();

	return result->getInt(1);
}

void Database::add_user_to_database(int count, std::string userID, std::string discordName, boolean hasVotedUp, boolean hasVotedDown, int suggestionID) {
	pstmt = con->prepareStatement("INSERT INTO user(iduser, discord_id, discord_username, hasVotedUp, hasVotedDown, suggestion_id) VALUES(?, ?, ?, ?, ?, ?)");

	pstmt->setInt(1, get_max_id_users()+1);
	pstmt->setString(2, userID);
	pstmt->setString(3, discordName);
	pstmt->setBoolean(4, hasVotedUp);
	pstmt->setBoolean(5, hasVotedDown);
	pstmt->setInt(6, suggestionID);
}

std::vector<int> Database::find_users_in_suggestion(int suggestionID) {
	std::vector<int> returnVector;

	pstmt = con->prepareStatement("SELECT * FROM user;");
	result = pstmt->executeQuery();

	while (result->next()) {
		if (result->getInt(6) == suggestionID) {
			returnVector.push_back(result->getInt(6));
		}
	}
	return returnVector;
}


boolean Database::user_has_vote_up(dpp::user user) {
	boolean result = false;
	if (has_user(user)) {
		result = get_user_in_list(user)->get_reacted_up();
	}
	return result;
}
boolean Database::user_has_vote_down(dpp::user user) {
	boolean result = false;
	if (has_user(user)) {
		result = get_user_in_list(user)->get_reacted_down();
	}
	return result;
}

// SERVER CONFIGS
