#include <dpp/dpp.h>
#include "VoteUser.h"

class Suggestion {
	private:
        int votes;
        dpp::message messageOfSuggestion;

        std::vector<VoteUser> users;

        dpp::user creator;

        dpp::snowflake channelid;

        std::string description;

	public:
        Suggestion();

        dpp::message get_message();
        
        std::string get_description();

        void set_message(dpp::message newMessage);

        int get_votes();

        void subtract_vote(dpp::user user);
        void add_vote(dpp::user user);

        void add_user(VoteUser userToAdd);

        VoteUser* get_user_in_list(dpp::user user);

        boolean has_user(dpp::user user);

        boolean user_has_vote_up(dpp::user user);
        boolean user_has_vote_down(dpp::user user);

        Suggestion(std::string newDescription, dpp::user newCreator, dpp::snowflake newChannel);

        dpp::message create_approved_suggestion(dpp::snowflake approvedChannelID);
        dpp::message create_message();
};
