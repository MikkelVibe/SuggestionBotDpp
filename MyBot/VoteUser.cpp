#include <dpp/dpp.h>
#include "VoteUser.h"

boolean VoteUser::get_reacted_up() {
	return hasReactedUp;
}
boolean VoteUser::get_reacted_down() {
	return hasReactedDown;
}

std::string VoteUser::get_user_url() {
	return userDpp.get_url();
}

void VoteUser::update_react_up() {
	if (!hasReactedUp) {
		hasReactedUp = true;
	}
	else {
		hasReactedUp = false;
	}
}

void VoteUser::update_react_down() {
	if (!hasReactedDown) {
		hasReactedDown = true;
	}
	else {
		hasReactedDown = false;
	}
}

VoteUser::VoteUser(dpp::user user) {
	userDpp = user;
}

VoteUser::VoteUser() {
	hasReactedDown = false;
	hasReactedUp = false;
}

