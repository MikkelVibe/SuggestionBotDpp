#include <dpp/dpp.h>

#pragma once
class Suggestion {
    private:
        Suggestion();

        static Suggestion* instance;
	public:
        static Suggestion* get_instance();

        static dpp::message create_approved_suggestion(dpp::snowflake approvedChannelID, std::string description, dpp::user creator, int votes);

        static dpp::message create_message(std::string description, std::string formatted_username, dpp::snowflake channelid, int votes);
};
