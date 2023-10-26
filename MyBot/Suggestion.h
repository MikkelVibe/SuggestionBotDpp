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

        dpp::message getMessage();
        
        std::string getDescription();

        void setMessage(dpp::message newMessage);

        int getVotes();

        void subtractVote(dpp::user user);
        void addVote(dpp::user user);

        void addUser(VoteUser userToAdd);

        VoteUser* getUserInList(dpp::user user);

        boolean hasUser(dpp::user user);

        boolean userHasVoteUp(dpp::user user);
        boolean userHasVoteDown(dpp::user user);

        Suggestion(std::string newDescription, dpp::user newCreator, dpp::snowflake newChannel);

        dpp::message createMessage();
};
