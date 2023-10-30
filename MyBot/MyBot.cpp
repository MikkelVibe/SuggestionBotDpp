#include "MyBot.h"
#include <dpp/dpp.h>
#include "Suggestion.h"
#include "Database.h"

std::string BOT_TOKEN;


std::vector<dpp::snowflake> roles;

MyBot myBot;

Database database;

int guildid;

dpp::message getMessage;

MyBot::MyBot() {

}

void set_message(dpp::message toSet) {
	getMessage = toSet;
}

dpp::message get_message() {
	return getMessage;
}


int get_temp_guild_id() {
	return guildid;
}

void set_temp_guild_id(int number) {
	guildid = number;
}

void add_suggestion(std::string url, std::string description, std::string creater_url, int configID) {
	database.add_suggestion_to_database(url, description, creater_url, configID);
}

std::string get_channel_url(std::string channelid, std::string guildid) {
	return "https://discord.com/channels/" + guildid + "/" + channelid;
}

std::string get_role_url(std::string roleid) {
	return "<@&" + roleid + ">";
}

boolean has_role(dpp::guild_member guildMember, int configID) {
	boolean found = false;
	int i = 0;
	while (!found && i < guildMember.get_roles().size()) {
		dpp::snowflake role = guildMember.get_roles()[i];
		if (role.str()._Equal(database.get_role_id(configID))) {
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

	/* Get token from file and create bot cluster */
	std::string filename = "C:\\Users\\Mikkel\\Documents\\GitHub\\SuggestionBotDpp\\token.txt";
	std::string file_contents;

	std::ifstream file(filename);

	if (file.is_open()) {
		std::getline(file, file_contents);
		file.close();
	}
	else {
		std::cerr << "Error: Unable to open the file." << std::endl;
	}
	BOT_TOKEN = file_contents;

	dpp::cluster bot(BOT_TOKEN);
	
	/* Output simple log messages to stdout */
	bot.on_log(dpp::utility::cout_logger());

	/* Handle slash command */
	bot.on_slashcommand([&bot](const dpp::slashcommand_t& event) {
		if (database.find_config(event.command.get_guild().id.str()) != -1) {
			if (event.command.get_command_name() == "suggest") {
				std::string suggestionString = std::get<std::string>(event.get_parameter("suggestion"));
				dpp::user user = event.command.get_issuing_user();

				set_temp_guild_id(database.find_config(event.command.get_guild().id.str()));
				
				bot.message_create(Suggestion::create_message(suggestionString, user.format_username(), database.get_suggest_channel_id(get_temp_guild_id()), 0), [&suggestionString, &user](const dpp::confirmation_callback_t& callback) {
					if (callback.is_error()) {
						std::cout << callback.get_error().message << std::endl;
					}
					else {
						add_suggestion(callback.get<dpp::message>().get_url(), suggestionString, user.id.str(), get_temp_guild_id());
					}
				});
				event.reply(dpp::message("Suggestion created in: " + get_channel_url(database.get_suggest_channel_id(database.find_config(event.command.get_guild().id.str())), event.command.get_guild().id.str())).set_flags(dpp::m_ephemeral));
			}
		}
		if (event.command.get_command_name() == "config") {
			dpp::snowflake rolePermission = std::get<dpp::snowflake>(event.get_parameter("role"));
			dpp::snowflake suggestionChannel = std::get<dpp::snowflake>(event.get_parameter("suggestionchannel"));
			dpp::snowflake approveChannel = std::get<dpp::snowflake>(event.get_parameter("approvechannel"));

			std::string description;

			std::string guildId = event.command.get_guild().id.str();

			if (database.find_config(guildId) == -1) {
				database.add_config(guildId, suggestionChannel.str(), approveChannel.str(), rolePermission.str());
				description = "Created config with";
			}
			else {
				

				std::vector<int> locations = database.different_value_locations(guildId, suggestionChannel.str(), approveChannel.str(), rolePermission.str());

				int configDBID = database.find_config(guildId);

				for (int location : locations) {
					if (location == 1) {
						database.update_config_suggest_channel_id(suggestionChannel.str(), configDBID);
					}
					else if (location == 2) {
						database.update_config_approve_channel_id(approveChannel.str(), configDBID);
					}
					else if (location == 3) {
						database.update_config_role_id(rolePermission.str(), configDBID);
					}
				}
				description = "Updated config to";
			}

			dpp::embed embed = dpp::embed()
				.set_color(dpp::colors::sti_blue)
				.set_description(description)
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
			event.reply("Error: suggestion not in database");
			return;
		}

		std::string eventID = event.custom_id;

		int suggestionID = database.find_suggestion_in_database(urlOfEvent);
		
		dpp::user userIssuing = event.command.get_issuing_user();

		sql::SQLString userClick = userIssuing.id.str().c_str();

		dpp::message mess = nullptr;

		std::string description = database.get_description(suggestionID);
		dpp::user* user = dpp::find_user(dpp::snowflake(database.get_creator_discord_id(suggestionID)));
		dpp::channel* channel_id = dpp::find_channel(database.get_suggest_channel_id(database.find_config(event.command.get_guild().id.str())));

		if (eventID == "upvote") {
			database.add_vote(userIssuing, suggestionID);

			if (database.user_has_vote_up(userClick, suggestionID)) {
				mess = dpp::message("Vote confirmed");
			}
			else {
				mess = dpp::message("Vote removed");
			}
			set_message(Suggestion::create_message(description, user->format_username(), channel_id->id.str(), database.get_votes(suggestionID)));
			dpp::snowflake messageid = dpp::snowflake(database.get_message_url(suggestionID).substr(68, 19));
			dpp::snowflake channelid = dpp::snowflake(database.get_message_url(suggestionID).substr(48, 19));

			bot.message_get(messageid, channelid, [&bot](const dpp::confirmation_callback_t& callback) {
				if (callback.is_error()) {
					std::cout << callback.get_error().message << "<--- error callback" << std::endl;
				}
				else {
					dpp::message newMessage = get_message();
					dpp::message msg = callback.get<dpp::message>();
					newMessage.set_channel_id(msg.channel_id);
					newMessage.id = msg.id;

					bot.message_edit(newMessage);
				}
			});
			event.reply(mess.set_flags(dpp::m_ephemeral));
		}
		else if (eventID == "downvote") {
			database.subtract_vote(userIssuing, suggestionID);

			if (database.user_has_vote_down(userClick, suggestionID)) {
				mess = dpp::message("Vote confirmed");
			}
			else {
				mess = dpp::message("Vote removed");
			}
			set_message(Suggestion::create_message(description, user->format_username(), channel_id->id.str(), database.get_votes(suggestionID)));
			dpp::snowflake messageid = dpp::snowflake(database.get_message_url(suggestionID).substr(68, 19));
			dpp::snowflake channelid = dpp::snowflake(database.get_message_url(suggestionID).substr(48, 19));

			bot.message_get(messageid, channelid, [&bot](const dpp::confirmation_callback_t& callback) {
				if (callback.is_error()) {
					std::cout << callback.get_error().message << "<--- error callback" << std::endl;
				}
				else {
					dpp::message newMessage = get_message();
					dpp::message msg = callback.get<dpp::message>();
					newMessage.set_channel_id(msg.channel_id);
					newMessage.id = msg.id;

					bot.message_edit(newMessage);
				}
			});
			event.reply(mess.set_flags(dpp::m_ephemeral));
		}
		else if (eventID == "approve") {
			const dpp::guild_member& issuer = event.command.member;

			if (has_role(issuer, database.find_config(event.command.get_guild().id.str()))) {
				dpp::message currentMessage = event.command.get_context_message();

				bot.message_delete(currentMessage.id, currentMessage.channel_id);

				std::string description = database.get_description(suggestionID);
				dpp::user* user = dpp::find_user(database.get_creator_discord_id(suggestionID));
				dpp::channel* channel_id = dpp::find_channel(database.get_approve_id(database.find_config(event.command.get_guild().id.str())));

				dpp::message approvedSuggestion = Suggestion::create_approved_suggestion(channel_id->id.str(), description, *user, database.get_votes(suggestionID));

				bot.message_create(approvedSuggestion);

				mess = dpp::message("Suggestion approved. Sent to: " + get_channel_url(database.get_approve_id(database.find_config(event.command.get_guild().id.str())), event.command.get_guild().id.str()));

				database.delete_suggestion(suggestionID);
			}
			else {
				mess = dpp::message("You don't have permission to approve suggestions.");
			}
			event.reply(mess.set_flags(dpp::m_ephemeral));
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