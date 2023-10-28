#include "MyBot.h"
#include <dpp/dpp.h>

#include "Database.h"

const std::string BOT_TOKEN = "MTE2NTY2NTMwNzAxNzM2NzU2Mg.GNAeFy.xiEQhj4wZoh3crGTqdSXxVGZV4lLJtACsZ5KWU";

std::vector<Suggestion> suggestions;
std::vector<dpp::snowflake> roles;

dpp::snowflake suggestionChannel; 
dpp::snowflake approveChannel;
dpp::snowflake rolePermission;

MyBot myBot;

Database database;

boolean isConfigured = false;

MyBot::MyBot() {

}



dpp::snowflake get_suggestion_channel() {
	return suggestionChannel;
}

std::string get_channel_url(std::string channelid, std::string guildid) {
	return "https://discord.com/channels/" + guildid + "/" + channelid;
}

std::string get_role_url(std::string roleid) {
	return "<@&" + roleid + ">";
}

void MyBot::set_temp_suggestion(Suggestion* toSet) {
	suggestionTemp = *toSet;
}

Suggestion* MyBot::get_temp_suggestion() {
	return &suggestionTemp;
}

boolean has_role(dpp::guild_member guildMember) {
	boolean found = false;
	int i = 0;
	while (!found && i < guildMember.get_roles().size()) {
		dpp::snowflake role = guildMember.get_roles()[i];
		if (role.str()._Equal(rolePermission.str())) {
			found = true;
		} 
		else {
			i++;
		}
	}
	return found;
}

int main()
{
	// Connect to database

	database.connect_to_database();

	/* Create bot cluster */
	dpp::cluster bot(BOT_TOKEN);
	
	/* Output simple log messages to stdout */
	bot.on_log(dpp::utility::cout_logger());

	/* Handle slash command */
	bot.on_slashcommand([&bot](const dpp::slashcommand_t& event) {
		if (isConfigured) {
			if (event.command.get_command_name() == "suggest") {
				std::string suggestionString = std::get<std::string>(event.get_parameter("suggestion"));
				dpp::user userInQuestion = event.command.get_issuing_user();

				std::cout << get_channel_url(suggestionChannel.str(), event.command.get_guild().id.str());

				Suggestion newSuggestion(suggestionString, userInQuestion, suggestionChannel);

				myBot.set_temp_suggestion(&newSuggestion);

				std::cout << newSuggestion.get_description();
				
				//is saved here
				bot.message_create(newSuggestion.create_message(), [&newSuggestion, &bot](const dpp::confirmation_callback_t& callback) {
					if (callback.is_error()) {
						std::cout << callback.get_error().message << std::endl;
					}
					else {
						myBot.create_and_add_to_list(callback.get<dpp::message>());
					}
				});
				event.reply(dpp::message("Suggestion created in: " + get_channel_url(suggestionChannel.str(), event.command.get_guild().id.str())).set_flags(dpp::m_ephemeral));
			}
		}
		else if (event.command.get_command_name() == "config") {
			if (!isConfigured) {
				isConfigured = true;
			}
			rolePermission = std::get<dpp::snowflake>(event.get_parameter("role"));
			suggestionChannel = std::get<dpp::snowflake>(event.get_parameter("suggestionchannel"));
			approveChannel = std::get<dpp::snowflake>(event.get_parameter("approvechannel"));

			dpp::embed embed = dpp::embed()
				.set_color(dpp::colors::sti_blue)
				.set_description("Updated config to")
				.set_footer(
					dpp::embed_footer()
					.set_text("SuggestionBot")
				)
				.add_field(
					"Required role",
					get_role_url(rolePermission.str())
				)
				.add_field(
					"Output suggestion to",
					get_channel_url(suggestionChannel.str(), event.command.get_guild().id.str())
				)
				.add_field(
					"Output approved suggestion to",
					get_channel_url(approveChannel.str(), event.command.get_guild().id.str())
				)
				.set_timestamp(time(0));

			dpp::message message = dpp::message(event.command.channel_id, embed);
			event.reply((message).set_flags(dpp::m_ephemeral));
		}
		else {
			dpp::message message = dpp::message("Please configure the bot before creating suggestions. Use /config to configure the bot.");
			event.reply((message).set_flags(dpp::m_ephemeral));
		}
	});
	
    bot.on_button_click([&bot](const dpp::button_click_t& event) {
        /* Button clicks are still interactions, and must be replied to in some form to
         * prevent the "this interaction has failed" message from Discord to the user.
         */
		std::string urlOfEvent = event.command.msg.get_url();


		if (!database.is_suggestion_in_database(urlOfEvent)) {
			return;
		}

		std::string eventID = event.custom_id;

		int suggestionID = database.find_suggestion_in_database(urlOfEvent);
		
		std::string userClick = event.command.get_issuing_user().id.str();

		sql::SQLString userClickSQL = userClick.c_str();

		if (eventID == "upvote") {
			if (!database.user_has_vote_down(userClickSQL, suggestionID)) {
				database.subtract_vote(userClickSQL, suggestionID);
			}



			suggestion->add_vote(*userClick);

			dpp::message mess = nullptr;
			if (!suggestion->get_user_in_list(*userClick)->get_reacted_up()) {
				mess = dpp::message("Vote confirmed");
			}
			else {
				mess = dpp::message("Vote removed");
			}
			dpp::message messageLocation = suggestion->get_message();
			dpp::message newMessage = suggestion->create_message();

			newMessage.id = messageLocation.id;
			newMessage.channel_id = messageLocation.channel_id;

			bot.message_edit(newMessage);

			event.reply(mess.set_flags(dpp::m_ephemeral));
		}
		else if (eventID == "downvote") {

			if (!suggestion->user_has_vote_up(*userClick) && suggestion->has_user(*userClick)) {
				suggestion->add_vote(*userClick);
			}
			suggestion->subtract_vote(*userClick);

			dpp::message mess = nullptr;
			if (!suggestion->get_user_in_list(*userClick)->get_reacted_down()) {
				mess = dpp::message("Vote confirmed");
			}
			else {
				mess = dpp::message("Vote removed");
			}

			dpp::message messageLocation = suggestion->get_message();
			dpp::message newMessage = suggestion->create_message();

			newMessage.id = messageLocation.id;
			newMessage.channel_id = messageLocation.channel_id;

			bot.message_edit(newMessage);

			event.reply(mess.set_flags(dpp::m_ephemeral));
		}
		else if (eventID == "approve") {
			const dpp::guild_member& issuer = event.command.member;

			dpp::message mes = nullptr;

			if (has_role(issuer)) {
				dpp::message currentMessage = event.command.get_context_message();
				bot.message_delete(currentMessage.id, currentMessage.channel_id);

				Suggestion* suggestionToGet = myBot.find_suggestion_match(currentMessage.get_url());

				dpp::message approvedSuggestion = suggestionToGet->create_approved_suggestion(approveChannel);

				bot.message_create(approvedSuggestion);

				mes = dpp::message("Suggestion approved. Sent to: " + get_channel_url(approveChannel.str(), event.command.get_guild().id.str()));
			}
			else {
				mes = dpp::message("You don't have permission to approve suggestions.");
			}
			event.reply(mes.set_flags(dpp::m_ephemeral));
		}
		
    });

	bot.on_ready([&bot](const dpp::ready_t& event) {
		if (dpp::run_once<struct register_bot_commands>()) {
			dpp::slashcommand commandPermissions("config", "Set permissions and output channel", bot.me.id);

			commandPermissions.add_option(dpp::command_option(dpp::co_role, "role", "Tag the role that can approve suggestions", true));
			commandPermissions.add_option(dpp::command_option(dpp::co_channel, "suggestionchannel", "Tag the channel where suggestions are sent", true));
			commandPermissions.add_option(dpp::command_option(dpp::co_channel, "approvechannel", "Tag the channel where approved suggestions are sent", true));

			bot.global_command_create(commandPermissions);

			dpp::slashcommand commandSuggest("suggest", "create a suggestion", bot.me.id);
			commandSuggest.add_option(dpp::command_option(dpp::co_string, "suggestion", "Type your suggestion here", true));
			bot.global_command_create(commandSuggest);
		}
	});

	bot.start(dpp::st_wait);

	return 0;
}