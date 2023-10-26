#include <dpp/dpp.h>

#pragma once
class VoteUser
{
	private:
		boolean hasReactedUp;
		boolean hasReactedDown;

		dpp::user userDpp;

	public:
		boolean get_reacted_up();
		boolean get_reacted_down();

		void update_react_up();
		void update_react_down();

		std::string get_user_url();

		VoteUser(dpp::user user);
		VoteUser();
};

