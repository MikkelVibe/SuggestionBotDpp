#include <dpp/dpp.h>

#pragma once
class VoteUser
{
	private:
		boolean hasReactedUp;
		boolean hasReactedDown;

		dpp::user userDpp;

	public:
		boolean getReactedUp();
		boolean getReactedDown();

		void updateReactUp();
		void updateReactDown();

		std::string getUserUrl();

		VoteUser(dpp::user user);
		VoteUser();
};

