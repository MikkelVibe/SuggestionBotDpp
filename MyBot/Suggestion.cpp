#include "Suggestion.h"
#include <dpp/dpp.h>

Suggestion* Suggestion::instance = nullptr;;

Suggestion::Suggestion() {
    
}

Suggestion *Suggestion::get_instance() {
    if (instance == nullptr) {
        instance = new Suggestion();
    }
    return instance;
}


dpp::message Suggestion::create_message(std::string description, std::string formatted_username, dpp::snowflake channelid, int votes) {
    dpp::embed embed = dpp::embed()
        .set_color(dpp::colors::sti_blue)
        .set_description("A new suggestion has been submitted! React below to vote.")
        .add_field(
            "Submitter",
            formatted_username
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

dpp::message Suggestion::create_approved_suggestion(dpp::snowflake approvedChannelID, std::string description, dpp::user creator, int votes) {
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

    return msg;
}
