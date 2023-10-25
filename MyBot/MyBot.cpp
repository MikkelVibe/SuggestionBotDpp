#include "MyBot.h"
#include <dpp/dpp.h>

const std::string    BOT_TOKEN    = "";
std::vector<Suggestion> suggestions;
std::vector<dpp::snowflake> roles;

dpp::snowflake suggestionChannel; 
dpp::snowflake approveChannel;
dpp::snowflake rolePermission;

MyBot myBot;

void MyBot::addSuggestion(Suggestion toAdd) {
	suggestions.push_back(toAdd);
}

std::string getChannelUrl(std::string channelid, std::string guildid) {
	return "https://discord.com/channels/" + guildid + "/" + channelid;
}

std::string getRoleUrl(std::string roleid) {
	return "<@&" + roleid + ">";
}

Suggestion* MyBot::findSuggestionMatch(std::string url) {
	Suggestion* result = nullptr;
	int i = 0;
	boolean found = false;
	while(!found && i < suggestions.size()) {
		Suggestion* temp = &suggestions[i];
		if (temp->getMessageURL()._Equal(url)) {
			result = temp;
			found = true;
		}
		else {
			i++;
		}
	}
	return result;
}

boolean isInList(std::string url) {
	boolean returnBoolean = false;
	int i = 0;
	while (!returnBoolean && i < suggestions.size()) {
		Suggestion* temp = &suggestions[i];
		if (temp->getMessageURL()._Equal(url)) {
			returnBoolean = true;
		}
		else {
			i++;
		}
	}
	return returnBoolean;
}

boolean hasRole(dpp::guild_member guildMember) {
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
	/* Create bot cluster */
	dpp::cluster bot(BOT_TOKEN);
	
	/* Output simple log messages to stdout */
	bot.on_log(dpp::utility::cout_logger());

	/* Handle slash command */
	bot.on_slashcommand([&bot](const dpp::slashcommand_t& event) {

		if (event.command.get_command_name() == "suggest") {
			Suggestion newSuggestion(std::get<std::string>(event.get_parameter("suggestion")), event.command.get_issuing_user());

			event.reply(newSuggestion.createMessage(newSuggestion.getVotes()), [&newSuggestion, event](const dpp::confirmation_callback_t& callback) {
				if (callback.is_error()) {
					std::cout << callback.get_error().message << std::endl;
				}
				else {
					event.get_original_response([&newSuggestion](const dpp::confirmation_callback_t& callback) {
						if (callback.is_error()) {
							std::cout << callback.get_error().message << std::endl;
						}
						else {
							dpp::message m = callback.get<dpp::message>();
							newSuggestion.setMessage(m.get_url());

							myBot.addSuggestion(newSuggestion);
						}
					});
				}
			});
		}
		if (event.command.get_command_name() == "config") {
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
					getRoleUrl(rolePermission.str())
				)
				.add_field(
					"Output suggestion to",
					getChannelUrl(suggestionChannel.str(), event.command.get_guild().id.str())
				)
				.add_field(
					"Output approved suggestion to",
					getChannelUrl(approveChannel.str(), event.command.get_guild().id.str())
				)
				.set_timestamp(time(0));
				
			dpp::message message = dpp::message(event.command.channel_id, embed);
			event.reply((message).set_flags(dpp::m_ephemeral));
		}
	});
	
    bot.on_button_click([&bot](const dpp::button_click_t& event) {
        /* Button clicks are still interactions, and must be replied to in some form to
         * prevent the "this interaction has failed" message from Discord to the user.
         */
		std::string eventID = event.custom_id;
		
		std::string urlOfEvent = event.command.msg.get_url();

		const dpp::user* userClick = &event.command.get_issuing_user();

		if (isInList(urlOfEvent)) {
			Suggestion* suggestion = myBot.findSuggestionMatch(urlOfEvent);

			if (eventID == "upvote") {
				if (!suggestion->userHasVoteDown(*userClick) && suggestion->hasUser(*userClick)) {
					suggestion->subtractVote(*userClick);
				}

				suggestion->addVote(*userClick);

				dpp::message mess = nullptr;
				if (!suggestion->getUserInList(*userClick)->getReactedUp()) {
					mess = dpp::message("Vote confirmed");
				}
				else {
					mess = dpp::message("Vote removed");
				}

				event.edit_original_response(suggestion->createMessage(suggestion->getVotes()));

				event.reply((mess).set_flags(dpp::m_ephemeral));
			}
			else if (eventID == "downvote") {

				Suggestion* suggestion = myBot.findSuggestionMatch(event.command.msg.get_url());

				if (!suggestion->userHasVoteUp(*userClick) && suggestion->hasUser(*userClick)) {
					suggestion->addVote(*userClick);
				}

				suggestion->subtractVote(*userClick);

				dpp::message mess = nullptr;
				if (!suggestion->getUserInList(*userClick)->getReactedDown()) {
					mess = dpp::message("Vote confirmed");
				}
				else {
					mess = dpp::message("Vote removed");

					event.edit_original_response(suggestion->createMessage(suggestion->getVotes())), [event](const dpp::command_completion_event_t& callback) {
						event.reply("Updated suggestion");

					};



				}
			}
			else if (eventID == "approve") {
				const dpp::guild_member& issuer = event.command.member;

				if (hasRole(issuer)) {
					event.reply();
				}

				event.reply();
			}
		}
		else {
			event.reply("Suggestion not in database");
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