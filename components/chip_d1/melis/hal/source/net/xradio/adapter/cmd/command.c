/*
 * Copyright (C) 2017 XRADIO TECHNOLOGY CO., LTD. All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *    1. Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *    2. Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the
 *       distribution.
 *    3. Neither the name of XRADIO TECHNOLOGY CO., LTD. nor the names of
 *       its contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#if 0
#include "cmd_util.h"
#include "cmd_wlan.h"
#include "cmd_ifconfig.h"
#include "cmd_iperf.h"
#include "cmd_lmac.h"
#include "cmd_wlancmd.h"
#ifdef __CONFIG_ETF
#include "cmd_etf.h"
#endif

/*
 * net commands
 */
static const struct cmd_data g_net_cmds[] = {
#ifndef __CONFIG_ETF
	{ "mode",		cmd_wlan_mode_exec },
	{ "ap", 		cmd_wlan_ap_exec },
	{ "sta",		cmd_wlan_sta_exec },
	{ "ifconfig",	cmd_ifconfig_exec },
	{ "lmac",		cmd_lmac_exec },
	{ "wlan",		cmd_wlan_exec },
#else
	{ "etf",		cmd_etf_exec },
#endif
};

static enum cmd_status cmd_net_exec(char *cmd)
{
	return cmd_exec(cmd, g_net_cmds, cmd_nitems(g_net_cmds));
}

#define MSH_CMD_BUF_SIZE	256

static char msh_cmd_buf[MSH_CMD_BUF_SIZE];

static void msh_net_exec(int argc, char *argv[])
{
	int i, len;
	int left = MSH_CMD_BUF_SIZE;
	char *ptr = msh_cmd_buf;

	for (i = 1; i < argc && left >= 2; ++i) {
		len = cmd_strlcpy(ptr, argv[i], left);
		ptr += len;
		left -= len;
		if (i < argc - 1 && left >= 2) {
			*ptr++ = ' ';
			*ptr = '\0';
			left -= 1;
		}
	}
	CMD_DBG("net cmd: %s\n", msh_cmd_buf);
	cmd_net_exec(msh_cmd_buf);
}

FINSH_FUNCTION_EXPORT_CMD(msh_net_exec,xradio, xradio testcmd);
#endif