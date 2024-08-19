[![English](https://custom-icon-badges.demolab.com/badge/-English-gray?style=for-the-badge)](README.md) [![Russian](https://custom-icon-badges.demolab.com/badge/-Russian-green?style=for-the-badge)](README.ru.md)

# What does this mod do?
This mod allows owners of [BDS](https://www.minecraft.net/en-us/download/server/bedrock) managed by [LeviLamina](https://github.com/LiteLDev/LeviLamina) create ranks (privileges) with which you can determine the commands available to the player. It also provides formatting for the player's game chat and *scoretag*.
# Mod Management
The mod is controlled by making configuration changes. You can assign a rank to a player both in the console and in the game using the `setrank` command.
## Formatting the chat
For each rank, you can make your own format for sending a message by the player using the rank configuration. To do this, you need to change the *chat* field of the desired rank. The value (text) of this field supports the following variables:
- Rank prefix: *{prefix}*
- Player's nickname: *{playerName}*
- A message sent by the player to the chat: *{message}*
- The dimension in which the player is located: *{dimension}* (you can change the display name of the dimension in the language configuration)
## Formatting of the player's *scoretag*
For each rank, you can make your own player's *scoretag* format. To do this, you need to change the *scoreTag* field of the desired rank. The value (text) of this field supports only one variable *{prefix}* (rank prefix)
# What's under the hood?
And under the hood we have:
* Optimized and high-speed code in *C++*
* Language Configuration Manager (used by [i18n](https://github.com/LiteLDev/LeviLamina/blob/develop/src/ll/api/i18n)). Folder with languages - *languages*
* *JSON* type rank configuration manager. The name of the configuration file is *ranks.json*
* Manager of the main configuration of the *JSON* type mod. The name of the configuration file is *config.json*
* Database management manager of the *SQLite3* type. It stores player data, namely, the player's nickname, his XUID and PlayFabId, as well as his rank
* The main manager of the mod, registering the `addrank` and `setrank` commands, as well as managing the issue of rank to the player
* Hooks, one of which modifies the contents of the `AvailableCommandsPacket` when it is sent
