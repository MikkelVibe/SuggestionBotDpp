#include "Suggestion.h"
#include <dpp/dpp.h>

int Suggestion::get_votes() {
	return votes;
}

Suggestion::Suggestion() {

}

dpp::message Suggestion::get_message() {
    return messageOfSuggestion;
}

std::string Suggestion::get_description() {
    return description;
}


void Suggestion::add_vote(dpp::user user) {
    VoteUser* voteUser = nullptr;
	if (has_user(user)) {
		voteUser = get_user_in_list(user);
	}
	else {
		add_user(VoteUser(user));
        voteUser = get_user_in_list(user);
	}

	if (voteUser->get_reacted_up()) {
		get_user_in_list(user)->update_react_up();
        votes++;
	}
	else {
		votes--;

		get_user_in_list(user)->update_react_up();
	}
}
      
void Suggestion::subtract_vote(dpp::user user) {
    VoteUser* voteUser = nullptr;
    if (has_user(user)) {
        voteUser = get_user_in_list(user);
    }
    else {
        add_user(VoteUser(user));
        voteUser = get_user_in_list(user);
    }

    if (voteUser->get_reacted_down()) {
        voteUser->update_react_down();
        votes--;
    }
    else {
        votes++;

        voteUser->update_react_down();
    }
}

void Suggestion::set_message(dpp::message newMessage) {
    messageOfSuggestion = newMessage;
}


void Suggestion::add_user(VoteUser userToAdd) {
    users.push_back(userToAdd);
}

boolean Suggestion::has_user(dpp::user user) {
    boolean found = false;
    int i = 0;
    while (!found && i < users.size()) {
        if (user.get_url()._Equal(users[i].get_user_url())) {
            found = true;
        }
        else {
            i++;
        }
    }
    return found;
}

VoteUser* Suggestion::get_user_in_list(dpp::user user) {
    VoteUser* voteUser = nullptr;
    boolean found = false;
    int i = 0;
    while (!found && i < users.size()) {
        if(user.get_url()._Equal(users[i].get_user_url())) {
            found = true;
            voteUser = &users[i];
        }
        else {
            i++;
        }
    }
    return voteUser;
}

boolean Suggestion::user_has_vote_up(dpp::user user) {
    boolean result = false;
    if (has_user(user)) {
        result = get_user_in_list(user)->get_reacted_up();
    }
    return result;
}
boolean Suggestion::user_has_vote_down(dpp::user user) {
    boolean result = false;
    if (has_user(user)) {
        result = get_user_in_list(user)->get_reacted_down();
    }
    return result;
}

Suggestion::Suggestion(std::string newDescription, dpp::user newCreator, dpp::snowflake newChannelid) {
    description = newDescription;
    creator = newCreator;
    channelid = newChannelid;
    votes = 0;
    users = std::vector<VoteUser>(5);
}

dpp::message Suggestion::create_message() {
    dpp::embed embed = dpp::embed()
        .set_color(dpp::colors::sti_blue)
        .set_description("A new suggestion has been submitted! React below to vote.")
        .add_field(
            "Submitter",
            creator.format_username()
        )
        .add_field(
            "Suggestion",
            description,
            true
        )
        .add_field(
            "Current status",
            std::to_string(votes)
        )
        .set_footer(
            dpp::embed_footer()
            .set_text("SuggestionBot")
        )
        .set_timestamp(time(0));

    dpp::message msg(channelid, embed);

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
    msg.set_channel_id(channelid);

    return msg;
}

dpp::message Suggestion::create_approved_suggestion(dpp::snowflake approvedChannelID) {
    dpp::embed embed = dpp::embed()
        .set_color(dpp::colors::sti_blue)
        .set_description("The following suggestion has been accepted!")
        .add_field(
            "Submitter",
            creator.format_username()
        )
        .add_field(
            "Suggestion",
            description,
            true
        )
        .add_field(
            "Votes of suggestion",
            std::to_string(votes)
        )
        .set_footer(
            dpp::embed_footer()
            .set_text("SuggestionBot")
        )
        .set_timestamp(time(0));

    dpp::message msg(approvedChannelID, embed);

    /* Add an action row, and then a button within the action row. */
    return msg;
}
