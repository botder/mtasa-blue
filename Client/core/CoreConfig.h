/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  PURPOSE:     Base configuration
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "version.h"

#define BLUE_VERSION_STRING   "Multi Theft Auto v" MTA_DM_BUILDTAG_LONG
#define BLUE_COPYRIGHT_STRING "Copyright (C) 2003 - %BUILD_YEAR% Multi Theft Auto"

// Configuration file path (relative to MTA install directory)
#define MTA_CONFIG_PATH            "mta/config/coreconfig.xml"
#define MTA_SERVER_CACHE_PATH      "mta/config/servercache.xml"
#define MTA_CONSOLE_LOG_PATH       "mta/logs/console.log"
#define MTA_CONSOLE_INPUT_LOG_PATH "mta/logs/console-input.log"
#define CONFIG_ROOT                "mainconfig"
#define CONFIG_NODE_CVARS          "settings"            // cvars node
#define CONFIG_NODE_KEYBINDS       "binds"               // keybinds node
#define CONFIG_NODE_JOYPAD         "joypad"
#define CONFIG_NODE_UPDATER        "updater"
#define CONFIG_NODE_SERVER_INT     "internet_servers"                   // backup of last successful master server list query
#define CONFIG_NODE_SERVER_FAV     "favourite_servers"                  // favourite servers list node
#define CONFIG_NODE_SERVER_REC     "recently_played_servers"            // recently played servers list node
#define CONFIG_NODE_SERVER_OPTIONS "serverbrowser_options"              // saved options for the server browser
#define CONFIG_NODE_SERVER_SAVED   "server_passwords"                   // This contains saved passwords (as appose to save_server_passwords which is a setting)
#define CONFIG_NODE_SERVER_HISTORY "connect_history"
#define CONFIG_INTERNET_LIST_TAG   "internet_server"
#define CONFIG_FAVOURITE_LIST_TAG  "favourite_server"
#define CONFIG_RECENT_LIST_TAG     "recently_played_server"
#define CONFIG_HISTORY_LIST_TAG    "connected_server"
#define IDT_TIMER1                 1234
