/*
 * $Id$
 *
 * Copyright (C) 2007 Colin DIDIER
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include "module.h"
#include "signals.h"
#include "chat-protocols.h"
#include "chatnets.h"
#include "servers-setup.h"
#include "channels-setup.h"
#include "settings.h"

#include "xmpp-servers.h"
#include "xmpp-channels.h"
#include "xmpp-commands.h"
#include "xmpp-nicklist.h"
#include "xmpp-ping.h"
#include "xmpp-protocol.h"
#include "xmpp-queries.h"
#include "xmpp-rosters.h"
#include "xmpp-servers-reconnect.h"
#include "xmpp-session.h"
#include "xmpp-settings.h"

static CHATNET_REC *
create_chatnet(void)
{
	return g_new0(CHATNET_REC, 1);
}

static SERVER_SETUP_REC *
create_server_setup(void)
{
	return g_new0(SERVER_SETUP_REC, 1);
}

static SERVER_CONNECT_REC *
create_server_connect(void)
{
	XMPP_SERVER_CONNECT_REC *rec = g_new0(XMPP_SERVER_CONNECT_REC, 1);
	rec->channels_list = NULL;
	return (SERVER_CONNECT_REC *)rec;
}

static CHANNEL_SETUP_REC *
create_channel_setup(void)
{
	return g_new0(CHANNEL_SETUP_REC, 1);
}

static void
destroy_server_connect(SERVER_CONNECT_REC *conn)
{
	XMPP_SERVER_CONNECT_REC *rec = XMPP_SERVER_CONNECT(conn);
	GSList *tmp;

	if (rec == NULL)
		return;

	for (tmp = rec->channels_list; tmp != NULL; tmp = tmp->next)
		g_free(tmp->data);
	g_slist_free(rec->channels_list);
}

void
xmpp_core_init(void)
{
	CHAT_PROTOCOL_REC *rec;

	rec = g_new0(CHAT_PROTOCOL_REC, 1);
	rec->name = "XMPP";
	rec->fullname = "XMPP, Extensible messaging and presence protocol";
	rec->chatnet = "xmppnet";

	rec->case_insensitive = TRUE;

	rec->create_chatnet = create_chatnet;
	rec->create_server_setup = create_server_setup;
	rec->create_server_connect = create_server_connect;
	rec->create_channel_setup = create_channel_setup;
	rec->destroy_server_connect = destroy_server_connect;

	rec->server_init_connect = xmpp_server_init_connect;
	rec->server_connect = (void (*)(SERVER_REC *))xmpp_server_connect;
	rec->channel_create = (CHANNEL_REC *(*)(SERVER_REC *, const char *,
	    const char *, int))xmpp_channel_create;
	rec->query_create = xmpp_query_create;

	chat_protocol_register(rec);
	g_free(rec);

	xmpp_channels_init();
	xmpp_commands_init();
	xmpp_nicklist_init();
	xmpp_ping_init();
	xmpp_protocol_init();
	xmpp_rosters_init();
	xmpp_servers_init();
	xmpp_servers_reconnect_init();
	xmpp_session_init();
	xmpp_settings_init();

	module_register("xmpp", "core");
}

void
xmpp_core_deinit(void) 
{
	/* deinit servers first to disconnecet servers before unloading */
	xmpp_servers_deinit();

	xmpp_channels_deinit();
	xmpp_commands_deinit();
	xmpp_nicklist_deinit();
	xmpp_ping_deinit();
	xmpp_protocol_deinit();
	xmpp_rosters_deinit();
	xmpp_servers_reconnect_deinit();
	xmpp_session_deinit();
	xmpp_settings_deinit();

	signal_emit("chat protocol deinit", 1, chat_protocol_find("XMPP"));
	chat_protocol_unregister("XMPP");
}
