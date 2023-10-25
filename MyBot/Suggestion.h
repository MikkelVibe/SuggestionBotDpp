#include <dpp/dpp.h>
#include "VoteUser.h"

class Suggestion {
	private:
        int votes;
        std::string messageURL;
        
        std::vector<VoteUser> users;

        dpp::user creator;
        dpp::snowflake channel;
        std::string description;

	public:
        std::string getMessageURL();

        void setMessage(std::string newMessage);

        int getVotes();

        void subtractVote(dpp::user user);
        void addVote(dpp::user user);

        void addUser(VoteUser userToAdd);

        VoteUser* getUserInList(dpp::user user);

        boolean hasUser(dpp::user user);

        boolean userHasVoteUp(dpp::user user);
        boolean userHasVoteDown(dpp::user user);

        Suggestion(std::string description, dpp::user creator);

        dpp::message createMessage();
};
