/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  PURPOSE:     Server browser configuration
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

// Master server list URL
#define SERVER_LIST_MASTER_URL "http://master.multitheftauto.com/ase/mta/"

// Query response data buffer
#define SERVER_LIST_QUERY_BUFFER 4096

// Master server list timeout (in ms)
#define SERVER_LIST_MASTER_TIMEOUT 10000

// Maximum amount of server queries per pulse (so the list gradually streams in)
#define SERVER_LIST_QUERIES_PER_PULSE 2

// LAN packet broadcasting interval (in ms)
#define SERVER_LIST_BROADCAST_REFRESH 2000

// Timeout for one server in the server list to respond to a query (in ms)
#define SERVER_LIST_ITEM_TIMEOUT 8000

// Amount of server lists/tabs (ServerBrowserType)
#define SERVER_BROWSER_TYPE_COUNT 4

// Amount of search types
#define SERVER_BROWSER_SEARCH_TYPE_COUNT 2

// Server browser list update interval (in ms)
#define SERVER_BROWSER_UPDATE_INTERVAL 1000
