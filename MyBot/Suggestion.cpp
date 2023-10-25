#include "Suggestion.h"
#include <dpp/dpp.h>

int Suggestion::getVotes() {
	return votes;
}

std::string Suggestion::getMessageURL() {
    return messageURL;
}

void Suggestion::addVote(dpp::user user) {
    VoteUser* voteUser = nullptr;
	if (hasUser(user)) {
		voteUser = getUserInList(user);
	}
	else {
		addUser(VoteUser(user));
        voteUser = getUserInList(user);
	}

	if (voteUser->getReactedUp()) {
		getUserInList(user)->updateReactUp();
        votes++;
	}
	else {
		votes--;

		getUserInList(user)->updateReactUp();
	}
}
      
void Suggestion::subtractVote(dpp::user user) {
    VoteUser* voteUser = nullptr;
    if (hasUser(user)) {
        voteUser = getUserInList(user);
    }
    else {
        addUser(VoteUser(user));
        voteUser = getUserInList(user);
    }

    if (voteUser->getReactedDown()) {
        voteUser->updateReactDown();
        votes--;
    }
    else {
        votes++;

        voteUser->updateReactDown();
    }
}

void Suggestion::setMessage(std::string newMessage) {
    messageURL = newMessage;
}

void Suggestion::addUser(VoteUser userToAdd) {
    users.push_back(userToAdd);
}

boolean Suggestion::hasUser(dpp::user user) {
    boolean found = false;
    int i = 0;
    while (!found && i < users.size()) {
        if (user.get_url()._Equal(users[i].getUserUrl())) {
            found = true;
        }
        else {
            i++;
        }
    }
    return found;
}

VoteUser* Suggestion::getUserInList(dpp::user user) {
    VoteUser* voteUser = nullptr;
    boolean found = false;
    int i = 0;
    while (!found && i < users.size()) {
        if(user.get_url()._Equal(users[i].getUserUrl())) {
            found = true;
            voteUser = &users[i];

        }
        else {
            i++;
        }
    }
    return voteUser;
}

boolean Suggestion::userHasVoteUp(dpp::user user) {
    boolean result = false;
    if (hasUser(user)) {
        result = getUserInList(user)->getReactedUp();
    }
    return result;
}
boolean Suggestion::userHasVoteDown(dpp::user user) {
    boolean result = false;
    if (hasUser(user)) {
        result = getUserInList(user)->getReactedDown();
    }
    return result;
}

Suggestion::Suggestion(std::string description, dpp::user creator) {
    this->description = description;
    this->creator = creator;
    votes = 0;
    users = std::vector<VoteUser>(5);
}

dpp::message Suggestion::createMessage() {
    dpp::embed embed = dpp::embed()
        .set_color(dpp::colors::sti_blue)
        .set_description("A new suggestion has been submitted! React below to vote.")
        .add_field(
            "Submitter",
            Suggestion::creator.format_username()
        )
        .add_field(
            "Suggestion",
            Suggestion::description,
            true
        )
        .set_footer(
            dpp::embed_footer()
            .set_text("SuggestionBot")
        )
        .set_timestamp(time(0));

    dpp::message msg(channel, embed);

    /* Add an action row, and then a button within the action row. */
    msg.add_component(
        dpp::component().add_component(
            dpp::component()
            .set_label("Upvote")
            .set_type(dpp::cot_button)
            .set_style(dpp::cos_success)
            .set_id("upvote")
        )
        .add_component(
            dpp::component()
            .set_label("Downvote")
            .set_type(dpp::cot_button)
            .set_style(dpp::cos_danger)
            .set_id("downvote")
        )
        .add_component(
            dpp::component()
            .set_label("Approve")
            .set_type(dpp::cot_button)
            .set_style(dpp::cos_primary)
            .set_id("approve")
        )
    );
    
    return msg;
}