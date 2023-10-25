#include <dpp/dpp.h>
#include "VoteUser.h"

boolean VoteUser::getReactedUp() {
	return hasReactedUp;
}
boolean VoteUser::getReactedDown() {
	return hasReactedDown;
}

std::string VoteUser::getUserUrl() {
	return userDpp.get_url();
}

void VoteUser::updateReactUp() {
	if (!hasReactedUp) {
		hasReactedUp = true;
	}
	else {
		hasReactedUp = false;
	}
}

void VoteUser::updateReactDown() {
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

