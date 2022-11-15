/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <aos/aos.h>
#include <aos/debug.h>
#include <uservice/uservice.h>
#include <devices/netdrv.h>

#include <lwip/apps/dhcps.h>
#include <lwip/netifapi.h>
// #include <lwip_netconf.h>
#include <lwip/dns.h>

#include <netif/ethernet.h>
#include <devices/wifi.h>
#include <devices/hal/wifi_impl.h>

#include <wifimanager.h>

#define TAG "xr829"


#define MAX_AP_RECORD 100
#define IEEE80211_FCTL_TODS 0x0100
#define IEEE80211_FCTL_FROMDS 0x0200
#define IEEE80211_FTYPE_MGMT 0x0000
#define IEEE80211_FTYPE_CTL 0x0004
#define IEEE80211_FTYPE_DATA 0x0008
#define ETH_ALEN 6

#if !defined(MIN)
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#endif

typedef struct {
    aos_dev_t   device;
    uint8_t mode;
    void (*write_event)(aos_dev_t *dev, int event_id, void *priv);
    void *priv;
} wifi_dev_t;

//static const char *TAG = "RTL8723DS OPS";

//advise: hal functions using global variable here may be deleted
static wifi_lpm_mode_t       g_wifi_lpm_mode;
// static rtw_country_code_t    g_country;
// static wifi_promiscuous_cb_t g_monitor_cb;
// static wifi_mgnt_cb_t        g_monitor_mgnt_cb;
static wifi_event_func *     g_evt_func;
static wifi_config_t         g_config;
static aos_dev_t *           wifi_evt_dev;
// static wifi_ap_record_t *    ap_records;
// static int                   scan_count;
static aw_wifi_mode_t        wifi_mode = AW_WIFI_MODE_STA;
static int                   wifi_disconnected_times;
static int                   sta_task_running;
// static int                   wifi_open_mode;
static int                   wifi_crashed;
static int                   connect_failed_reason;
static int                   g_xr829_inited;
static int                   wifi_status = 0;

// static struct netif xnetif[2];

extern void dhcps_deinit(void);
extern struct netif *g_wlan_netif;

// void dhcps_set_addr_pool(int addr_pool_set, ip_addr_t *addr_pool_start, ip_addr_t *addr_pool_end);
// static void xr829_wifi_handshake_done_hdl(char *buf, int buf_len, int flags, void *userdata);
// static void xr829_wifi_connected_hdl(char *buf, int buf_len, int flags, void *userdata);
// void        xr829_scan_report_hdl(char *buf, int buf_len, int flags, void *userdata);
// static void xr829_scan_done_hdl(char *buf, int buf_len, int flags, void *userdata);


void xr829_set_invalid()
{
    wifi_crashed = 1;
}


#define DRIVER_INVALID_RETURN_VAL \
    do { \
        if (wifi_crashed) \
            return -1; \
    } while (0)

#define DRIVER_INVALID_RETURN \
    do { \
        if (wifi_crashed) \
            return; \
    } while (0)


#define RTL8723DS_NET_DRIVER
#ifdef RTL8723DS_NET_DRIVER

int xr829_set_mac_addr(aos_dev_t *dev, const uint8_t *mac)
{
    /** write this mac address will write to efuse, forbidden this */
    return -1;

#if 0
    char mac_str[18] = {0};
    sLOGD(TAG, mac_str, MAC_FMT, mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    return wifi_set_mac_address(mac_str);
#endif
}


int xr829_get_mac_addr(aos_dev_t *dev, uint8_t *mac)
{
    DRIVER_INVALID_RETURN_VAL;

    // int mac_tmp[6];
    // char mac_str[18] = {0};
    
    // int ret = wifi_get_mac_address(mac_str);

    // sscanf(mac_str, "%02x:%02x:%02x:%02x:%02x:%02x",
    //        &mac_tmp[0], &mac_tmp[1], &mac_tmp[2],
    //        &mac_tmp[3], &mac_tmp[4], &mac_tmp[5]);

    char *mac_tmp = (char *)aw_wifi_get_mac();
    mac[0] = mac_tmp[0];
    mac[1] = mac_tmp[1];
    mac[2] = mac_tmp[2];
    mac[3] = mac_tmp[3];
    mac[4] = mac_tmp[4];
    mac[5] = mac_tmp[5];


    return 0;
}

int xr829_set_dns_server(aos_dev_t *dev, ip_addr_t ipaddr[], uint32_t num)
{
    int n, i;

    n = MIN(num, DNS_MAX_SERVERS);

    for (i = 0; i < n; i ++) {
        dns_setserver(i, &ipaddr[i]);
    }

    return n;
}

int xr829_get_dns_server(aos_dev_t *dev, ip_addr_t ipaddr[], uint32_t num)
{
    int n, i;

    n = MIN(num, DNS_MAX_SERVERS);

    for (i = 0; i < n; i ++) {
        if (!ip_addr_isany(dns_getserver(i))) {
            memcpy(&ipaddr[i], dns_getserver(i), sizeof(ip_addr_t));
        } else {
            return i;
        }
    }

    return n;
}

int xr829_set_hostname(aos_dev_t *dev, const char *name)
{
#if LWIP_NETIF_HOSTNAME
    struct netif *netif = g_wlan_netif;
    netif_set_hostname(netif, name);
    return 0;
#else
    return -1;
#endif
}

const char *xr829_get_hostname(aos_dev_t *dev)
{
#if LWIP_NETIF_HOSTNAME
    struct netif *netif = g_wlan_netif;
    return netif_get_hostname(netif);
#else
    return NULL;
#endif
}

void net_status_callback(struct netif *netif)
{
    if (!ip_addr_isany(ip_2_ip4(&netif->ip_addr))) {
        event_publish(EVENT_NET_GOT_IP, NULL);
    }
}

static int xr829_start_dhcp(aos_dev_t *dev)
{
    DRIVER_INVALID_RETURN_VAL;

    // struct netif *netif = g_wlan_netif; //netif_find("en0");
    // aos_check_return_einval(netif);

    // if (!netif_is_link_up(netif)) {
    //     return -1;
    // }

    // netif_set_ipaddr(netif, NULL);
    // netif_set_netmask(netif, NULL);
    // netif_set_gw(netif, NULL);

    // netif_set_status_callback(netif, net_status_callback);

    // dhcp_start

    // return netifapi_dhcp_start(netif);
    return 0;
}

static int xr829_stop_dhcp(aos_dev_t *dev)
{
    DRIVER_INVALID_RETURN_VAL;

    // struct netif *netif = g_wlan_netif; //netif_find("en0");
    // aos_check_return_einval(netif);

    // netifapi_dhcp_stop(netif);

    return 0;
}

static int xr829_set_ipaddr(aos_dev_t *dev, const ip_addr_t *ipaddr, const ip_addr_t *netmask,
                                const ip_addr_t *gw)
{
    return -1;
}

static int xr829_get_ipaddr(aos_dev_t *dev, ip_addr_t *ipaddr, ip_addr_t *netmask, ip_addr_t *gw)
{
    DRIVER_INVALID_RETURN_VAL;

    struct netif *netif = g_wlan_netif; //netif_find("en0");
    aos_check_return_einval(netif && ipaddr && netmask && gw);

    ip_addr_copy(*(ip4_addr_t *)ip_2_ip4(ipaddr), *netif_ip_addr4(netif));
    ip_addr_copy(*(ip4_addr_t *)ip_2_ip4(gw), *netif_ip_gw4(netif));
    ip_addr_copy(*(ip4_addr_t *)ip_2_ip4(netmask), *netif_ip_netmask4(netif));

    return 0;
}

int xr829_subscribe(aos_dev_t *dev, uint32_t event, event_callback_t cb, void *param)
{
    return -1;
}

int xr829_ping_remote(aos_dev_t *dev, int type, char *remote_ip)
{
    return -1;
}

static net_ops_t xr829_net_driver = {
    .set_mac_addr   = xr829_set_mac_addr,
    .get_mac_addr   = xr829_get_mac_addr,
    .set_dns_server = xr829_set_dns_server,
    .get_dns_server = xr829_get_dns_server,
    .set_hostname   = xr829_set_hostname,
    .get_hostname   = xr829_get_hostname,
    .start_dhcp     = xr829_start_dhcp,
    .stop_dhcp      = xr829_stop_dhcp,
    .set_ipaddr     = xr829_set_ipaddr,
    .get_ipaddr     = xr829_get_ipaddr,
    .subscribe      = xr829_subscribe,
    .ping           = xr829_ping_remote,
};

#endif

#define RTL8723DS_WIFI_DRIVER
#ifdef RTL8723DS_WIFI_DRIVER

// static void wifi_promisc_hdl(u8 *in_buf, unsigned int buf_len, void *userdata)
// {
//     ieee80211_frame_info_t *    info = (ieee80211_frame_info_t *)userdata;
//     wifi_promiscuous_pkt_t *    buf  = aos_malloc(sizeof(wifi_promiscuous_pkt_t) + buf_len);
//     wifi_promiscuous_pkt_type_t type = WIFI_PKT_MISC;

//     buf->rx_ctrl.rssi    = info->rssi;
//     buf->rx_ctrl.sig_len = buf_len;
//     memcpy(buf->payload, in_buf, buf_len);

//     if ((info->i_fc & 0xc) == IEEE80211_FTYPE_DATA) {
//         type = WIFI_PKT_DATA;
//     } else if ((info->i_fc & 0xc) == IEEE80211_FTYPE_MGMT) {
//         type = WIFI_PKT_MGMT;
//     } else if ((info->i_fc & 0xc) == IEEE80211_FTYPE_CTL) {
//         type = WIFI_PKT_CTRL;
//     }

//     g_monitor_cb(buf, type);
//     aos_free(buf);
// }

static void wifi_set_sta_mode()
{
    // int mode = 0;
    //Check if in AP mode
    //wext_get_mode(WLAN0_NAME, &mode);
    DRIVER_INVALID_RETURN;

    return;
}





// int xr829_get_security(char *ssid, rtw_security_t *security_type)
// {
//     return -1;
// }

#define MAX_SCAN_RESULTS_NUM 32

#define DHCPC_TRYIES          3
#define DHCPC_TIMEOUT         180000
#define DHCPC_CONNECT_TIMEOUT 180000

#include <lwip/tcpip.h>
#include <lwip/netif.h>
#include <lwip/dhcp.h>
#include <lwip/inet.h>
// #include <net_ctrl/net_ctrl.h>
#include <wifimanager.h>
#include <unistd.h>
// #include <hal_cmd.h>

static const char* wifi_state_to_str(int sta)
{
	switch(sta) {
		case WIFI_DISCONNECTED:
            event_publish(EVENT_WIFI_LINK_DOWN, NULL);
            wifi_status = 0;
			return "WIFI_DISCONNECTED";
		case WIFI_SCAN_STARTED:
			return "WIFI_SCAN_STARTED";
		case WIFI_SCAN_FAILED:
			return "WIFI_SCAN_FAILED";
		case WIFI_NETWORK_NOT_FOUND:
            event_publish(EVENT_WIFI_LINK_DOWN, (void *)NET_DISCON_REASON_WIFI_NOEXIST);
            wifi_status = 0;
			return "WIFI_NETWORK_NOT_FOUND";
		case WIFI_ASSOCIATING:
			return "WIFI_ASSOCIATING";
		case WIFI_AUTH_REJECT:
            event_publish(EVENT_WIFI_LINK_DOWN, NULL);
            wifi_status = 0;
			return "WIFI_AUTH_REJECT";
		case WIFI_AUTH_TIMEOUT:
            event_publish(EVENT_WIFI_LINK_DOWN, (void *)NET_DISCON_REASON_WIFI_TIMEOUT);
            wifi_status = 0;
			return "WIFI_AUTH_TIMEOUT";
		case WIFI_HANDSHAKE_FAILED:
			return "WIFI_HANDSHAKE_FAILED";
		case WIFI_CONNECTED:
            // event_publish(EVENT_WIFI_LINK_UP, NULL);
			return "WIFI_CONNECTED";
		case WIFI_CONN_TIMEOUT:
			return "WIFI_CONN_TIMEOUT";
		case DHCP_START_FAILED:
			return "DHCP_START_FAILED";
		case DHCP_TIMEOUT:
			return "DHCP_TIMEOUT";
		case DHCP_SUCCESS:
            event_publish(EVENT_NET_GOT_IP, NULL);
            wifi_status = 1;
			return "DHCP_SUCCESS";
		default:
			return "UNKNOWN";
	}
}

static void wifi_status_change(aw_wifi_status_change_t *wifi_status)
{
	if(strncmp(wifi_status->status,"connect",7) == 0) {
        // event_publish(EVENT_WIFI_LINK_UP, NULL);
		// LOGD(TAG, "wifi connect successful:%s\r\n",wifi_status->ssid);
    }
	else {
        event_publish(EVENT_WIFI_LINK_DOWN, NULL);
		LOGD(TAG, "wifi connect failed.\r\n");
    }
}

static void wifi_state_change(aw_wifi_state_t state)
{   
    wifi_state_to_str(state);
	// LOGD(TAG, "wifi state:%s\r\n",wifi_state_to_str(state));
}

static void wifi_hw_status(aw_wifi_hw_status_t hw_status)
{
	LOGD(TAG, "wifi %s\r\n",hw_status?"up":"down");
}

static void wifi_msg_callback(aw_wifi_msg_data_t *p_msg)
{
	switch(p_msg->id) {
		case WIFI_MSG_ID_WIFI_HW_STATUS:
			wifi_hw_status(p_msg->data.wlan_status);
			break;
		case WIFI_MSG_ID_WIFI_STATE:
			wifi_state_change(p_msg->data.wlan_status);
			break;
		case WIFI_MSG_ID_NETWORK_STATUS:
			wifi_status_change(p_msg->data.network_status_change);
			break;
		default:
			LOGD(TAG, "Not support.!\r\n");
			break;
	}
}

static int dhcp_handle(int tries,int timeout_ms,void *arg)
{
	struct netif *net_if = NULL;
	struct dhcp *dhcp_data = NULL;
	uint32_t addr_val = 0;

	net_if = g_wlan_netif;

	if(net_if == NULL) {
		LOGD(TAG, "get net interface failed\r\n");
		return -1;
	}

	if(dhcp_start(net_if) != 0) {
		LOGD(TAG, "star dhcp failed.");
		return -1;
	}

    dhcp_data = netif_dhcp_data(net_if);
    if (NULL == dhcp_data) {
		LOGD(TAG, "LwIP_DHCP: dhcp data failed.");
		return -1;
	}

    while (1) {
		/* If DHCP stopped by wifi_disconn_hdl*/
		if(dhcp_data->state == 0) {
		    LOGD(TAG, "LwIP_DHCP: dhcp stop.");
			return -1;
		}

		/* Read the new IP address */
		addr_val = net_if->ip_addr.addr;

	    // Assigned
		if (addr_val != 0) {
		    //  LOGD(TAG, "Dhcp got ip %hu.%hu.%hu.%hu", 
			// 		ip4_addr1_16(netif_ip4_addr(net_if)), 
			// 		ip4_addr2_16(netif_ip4_addr(net_if)), 
			// 		ip4_addr3_16(netif_ip4_addr(net_if)), 
			// 		ip4_addr4_16(netif_ip4_addr(net_if)));
			return 0;
		} else {
			/* DHCP timeout */
			// LOGD(TAG, "dhcp tries time: %d\r\n",dhcp_data->tries);
			if (dhcp_data->tries > tries) {
				/* Stop DHCP */
				dhcp_stop(net_if);
				LOGD(TAG, "dhcp timeout, stop dhcp");
				return -1;
			} else {
				aos_msleep(1500);
			}
		}
    }

	return 0;
}

void aw_wifi_task(void *arg)
{
    DRIVER_INVALID_RETURN;

    // int            ret    = 0;
    wifi_config_t *config = (wifi_config_t *)arg;

    // char            *password;
    // int              password_len;
    // int              key_id;


    aw_attr_t attr = {
    .dhcp_cb = dhcp_handle,
    .msg_cb  = wifi_msg_callback,
    .dhcp_tries = DHCPC_TRYIES,
    .dhcp_timeout = DHCPC_TIMEOUT,
    .connect_timeout = DHCPC_CONNECT_TIMEOUT,
    };

    aw_wifi_init(&attr);

    sta_task_running        = 1;
    wifi_disconnected_times = 0;
    connect_failed_reason   = 0;

    aw_wifi_on(AW_WIFI_MODE_STA);

    aw_wifi_connect(config->ssid, config->password);

    sta_task_running        = 0;
}

/*
    return---- 0:succese, -1: fail
*/
int wifi_start_softap(wifi_config_t *config)
{
    DRIVER_INVALID_RETURN_VAL;

//     int timeout = 20;

//     ip_addr_t     ipaddr;
//     ip_addr_t     netmask;
//     ip_addr_t     gw;
//     ip_addr_t     ipaddr_pool_start;
//     ip_addr_t     ipaddr_pool_end;
//     struct netif *pnetif = g_wlan_netif;


//     IP4_ADDR(&ipaddr, 192, 168, 1, 1);
//     IP4_ADDR(&netmask, 255, 255, 255, 0);
//     IP4_ADDR(&gw, 192, 168, 1, 1);
//     netif_set_addr(pnetif, &ipaddr, &netmask, &gw);
    
//     dhcps_deinit();
//     IP4_ADDR(&ipaddr_pool_start, 192, 168, 1, 100);
//     IP4_ADDR(&ipaddr_pool_end, 192, 168, 1, 115); /** limited to only 16 client */
//     dhcps_set_addr_pool(1, &ipaddr_pool_start, &ipaddr_pool_end); 
//     dhcps_init(pnetif);


//     int channel = 6;
//     int ret     = 0;
    
// 	if((rltk_wlan_running(WLAN0_IDX) != 0) ||
// 		(rltk_wlan_running(WLAN1_IDX) != 0)) {
// 		/** disable auto reconnect */
// #if CONFIG_AUTO_RECONNECT
//         wifi_set_autoreconnect(RTW_AUTORECONNECT_DISABLE);
// #endif
//     }

// #if CONFIG_AUTO_RECONNECT
//     wifi_wait_auto_reconnect();
// #endif

//     /** avoid wifi_off send wifi disconnect event */
//     wifi_unreg_event_handler(WIFI_EVENT_DISCONNECT, xr829_wifi_disconn_hdl);
//     wifi_unreg_event_handler(WIFI_EVENT_FOURWAY_HANDSHAKE_DONE, xr829_wifi_handshake_done_hdl);
//     wifi_unreg_event_handler(WIFI_EVENT_CONNECT, xr829_wifi_connected_hdl);

//     aw_wifi_off();
//     rtw_mdelay_os(20);

//     if (wifi_on(RTW_MODE_AP) < 0) {
//         LOGE(TAG, "Wifi on failed!");
//         return -1;
//     }

//     //  wifi_disable_powersave();//add to close powersave
//     if (channel > 14) {
//         LOGE(TAG, "Invalid Channel,plese check!\r\r\n");
//         channel = 1;
//     }

//     if (strlen(config->password) > 0) {
//         /** Start AP with WPA/WPA2 */
//         if (wifi_start_ap((char *)config->ssid, RTW_SECURITY_WPA2_AES_PSK, (char *)config->password,
//                           strlen((const char *)config->ssid),
//                           strlen((const char *)config->password), channel) != RTW_SUCCESS) {
//             LOGE(TAG, "wifi start ap mode failed!\n\r");
//             return -1;
//         }
//     } else {
//         /** Start AP with open mode */
//         if (wifi_start_ap((char *)config->ssid, RTW_SECURITY_OPEN, NULL,
//                           strlen((const char *)config->ssid), 0, channel) != RTW_SUCCESS) {
//             LOGE(TAG, "wifi start ap mode failed!\n\r");
//             return -1;
//         }
//     }

//     while (1) {
//         char essid[33];

//         if (wext_get_ssid(WLAN0_NAME, (unsigned char *)essid) > 0) {
//             if (strcmp((const char *)essid, (const char *)config->ssid) == 0) {
//                 LOGD(TAG, "%s started\r\n", config->ssid);
//                 ret = 0;
//                 break;
//             }
//         }

//         if (timeout == 0) {
//             LOGE(TAG, "Start AP timeout!\n\r");
//             ret = -1;
//             break;
//         }

//         aos_msleep(1000);
//         timeout--;
//     }
    
//     netif_set_link_up(g_wlan_netif);

    // return ret;
    return 0;
}

/**
    The wifi mode is controlled by the global variable 'wifi_mode'
*/
static int xr829_set_mode(aos_dev_t *dev, wifi_mode_t mode)
{
    DRIVER_INVALID_RETURN_VAL;

    aw_wifi_mode_t rtw_mode;

    switch (mode) {
        case WIFI_MODE_NULL:
            rtw_mode = AW_WIFI_MODE_INVALID;
            break;

        case WIFI_MODE_STA:
            rtw_mode = AW_WIFI_MODE_STA;
            break;

        case WIFI_MODE_AP:
            rtw_mode = AW_WIFI_MODE_HOSTAP;
            break;

        case WIFI_MODE_P2P:
        case WIFI_MODE_APSTA:
        case WIFI_MODE_MAX:
        default:
            rtw_mode = AW_WIFI_MODE_INVALID;
            LOGE(TAG, "ERR mode\r\n");
    }

    wifi_mode = rtw_mode;

    aw_wifi_off();
    aos_msleep(20);
    aw_wifi_on(wifi_mode);

    return 0;
}

static int xr829_get_mode(aos_dev_t *dev, wifi_mode_t *mode)
{
    DRIVER_INVALID_RETURN_VAL;

    switch ((int)wifi_mode) {
        case AW_WIFI_MODE_INVALID:
            *mode = WIFI_MODE_NULL;
            break;

        case AW_WIFI_MODE_STA:
            *mode = WIFI_MODE_STA;
            break;

        case AW_WIFI_MODE_HOSTAP:
            *mode = WIFI_MODE_AP;
            break;

    }

    return 0;
}

static int xr829_install_event_cb(aos_dev_t *dev, wifi_event_func *evt_func)
{
    DRIVER_INVALID_RETURN_VAL;

    g_evt_func   = evt_func;
    wifi_evt_dev = dev;

    return -1;
}

static int xr829_init(aos_dev_t *dev)
{
    //wifi_on();
    return 0;
}

int xr829_deinit(aos_dev_t *dev)
{
    DRIVER_INVALID_RETURN_VAL;

    while (sta_task_running) {
#if CONFIG_AUTO_RECONNECT
        wifi_set_autoreconnect(RTW_AUTORECONNECT_DISABLE);
#endif
        LOGW(TAG, "%s Wait STA Task Exit!\r\n", __FUNCTION__);
        aos_msleep(1000);
    }
    /** avoid wifi_off send wifi disconnect event */
    // wifi_unreg_event_handler(WIFI_EVENT_DISCONNECT, xr829_wifi_disconn_hdl);
    // wifi_unreg_event_handler(WIFI_EVENT_FOURWAY_HANDSHAKE_DONE, xr829_wifi_handshake_done_hdl);
    // wifi_unreg_event_handler(WIFI_EVENT_CONNECT, xr829_wifi_connected_hdl);
    netif_set_link_down(g_wlan_netif);
    aw_wifi_off();
    return 0;
}

int xr829_start(aos_dev_t *dev, wifi_config_t *config)
{
    DRIVER_INVALID_RETURN_VAL;

    if (config->mode == WIFI_MODE_STA) {
        if (sta_task_running) {
            LOGD(TAG, "ERROR: STA Task Already starting!\r\n");
            return -1;
        }
        memcpy(&g_config, config, sizeof(wifi_config_t));
        aos_task_t task_handle;
        sta_task_running        = 1;
        aos_task_new_ext(&task_handle, "aw_wifi_task", aw_wifi_task, &g_config, 1024 * 6, 35);
    } else if (config->mode == WIFI_MODE_AP) {
        LOGD(TAG, "start softap!\r\n");
        while (sta_task_running) {
            LOGW(TAG, "%s Wait STA Task Exit!\r\n", __FUNCTION__);
            aos_msleep(1000);
        }
        if (wifi_start_softap(config) == 0) {
            LOGI(TAG, "SoftAP UP\r\n");
            //m->ev_cb->stat_chg(m, NOTIFY_AP_UP, NULL);
        } else {
            LOGI(TAG, "SoftAP DOWN\r\n");
            //m->ev_cb->stat_chg(m, NOTIFY_AP_UP_FAILED, NULL);
        }
    } else {
        LOGD(TAG, "Not supported wifi mode!\r\n");
    }

    return 0;
}

int xr829_stop(aos_dev_t *dev)
{
    DRIVER_INVALID_RETURN_VAL;

    netif_set_link_down(g_wlan_netif);
    aw_wifi_off();
    return 0;
}

int xr829_reset(aos_dev_t *dev)
{
    DRIVER_INVALID_RETURN_VAL;

//     while (sta_task_running) {
// #if CONFIG_AUTO_RECONNECT
//         wifi_set_autoreconnect(RTW_AUTORECONNECT_DISABLE);
// #endif
//         LOGW(TAG, "%s Wait STA Task Exit!\r\n", __FUNCTION__);
//         aos_msleep(1000);
//     }
//     /** avoid wifi_off send wifi disconnect event */
//     wifi_unreg_event_handler(WIFI_EVENT_HANDSHAKE_FAILED, xr829_handshake_failed_hdl);
//     wifi_unreg_event_handler(WIFI_EVENT_DISCONNECT, xr829_wifi_disconn_hdl);
//     wifi_unreg_event_handler(WIFI_EVENT_FOURWAY_HANDSHAKE_DONE, xr829_wifi_handshake_done_hdl);
//     wifi_unreg_event_handler(WIFI_EVENT_CONNECT, xr829_wifi_connected_hdl);
//     netif_set_link_down(g_wlan_netif);

    if (g_xr829_inited) {
        aw_wifi_off();
        aos_msleep(20);
        aw_wifi_on(wifi_mode);
    }
    g_xr829_inited = 0;
    return 0;
}

/** conf APIs */
int xr829_set_lpm(aos_dev_t *dev, wifi_lpm_mode_t mode)
{
    DRIVER_INVALID_RETURN_VAL;

    // if (mode == WIFI_LPM_NONE) {

    //     if (g_wifi_lpm_mode == WIFI_LPM_KEEP_LINK) {
    //         rtw_resume(0);
    //     } else if (g_wifi_lpm_mode == WIFI_LPM_KEEP_SOCKET) {
    //         wifi_disable_powersave();
    //     }
    // } else if (mode == WIFI_LPM_KEEP_SOCKET) {
    //     if (g_wifi_lpm_mode == WIFI_LPM_NONE) {
    //         wifi_enable_powersave();
    //     } else if (g_wifi_lpm_mode == WIFI_LPM_KEEP_LINK) {
    //         rtw_resume(0);
    //         wifi_enable_powersave();
    //     }
    // } else if (mode == WIFI_LPM_KEEP_LINK) {

    //     rtw_suspend(0);

    // } else {
    //     return -1;
    // }

    g_wifi_lpm_mode = mode;

    return 0;
}

int xr829_get_lpm(aos_dev_t *dev, wifi_lpm_mode_t *mode)
{
    *mode = g_wifi_lpm_mode;
    return 0;
}

int xr829_set_protocol(aos_dev_t *dev, uint8_t protocol_bitmap)
{
    DRIVER_INVALID_RETURN_VAL;

    // rtw_network_mode_t mode = RTW_NETWORK_B;

    // if (protocol_bitmap & (WIFI_PROTOCOL_11B | WIFI_PROTOCOL_11G | WIFI_PROTOCOL_11N)) {
    //     mode = RTW_NETWORK_BGN;
    // } else if (protocol_bitmap & (WIFI_PROTOCOL_11B | WIFI_PROTOCOL_11G)) {
    //     mode = RTW_NETWORK_BG;
    // } else if (protocol_bitmap & (WIFI_PROTOCOL_11B)) {
    //     mode = RTW_NETWORK_B;
    // }

    // wifi_set_network_mode(mode);
    return 0;
}

int xr829_get_protocol(aos_dev_t *dev, uint8_t *protocol_bitmap)
{
    DRIVER_INVALID_RETURN_VAL;

    // rtw_network_mode_t mode = RTW_NETWORK_B;
    // wifi_get_network_mode(&mode);

    // switch (mode) {
    //     case RTW_NETWORK_B:
    //         *protocol_bitmap = (WIFI_PROTOCOL_11B);
    //         break;

    //     case RTW_NETWORK_BG:
    //         *protocol_bitmap = (WIFI_PROTOCOL_11B | WIFI_PROTOCOL_11G);
    //         break;

    //     case RTW_NETWORK_BGN:
    //         *protocol_bitmap = (WIFI_PROTOCOL_11B | WIFI_PROTOCOL_11G | WIFI_PROTOCOL_11N);
    //         break;
    // }

    return 0;
}

int xr829_set_country(aos_dev_t *dev, wifi_country_t country)
{
    DRIVER_INVALID_RETURN_VAL;

    // rtw_country_code_t rtw_country_code;

    // switch (country) {
    //     case WIFI_COUNTRY_CN:
    //         rtw_country_code = RTW_COUNTRY_CN;
    //         break;

    //     case WIFI_COUNTRY_JP:
    //         rtw_country_code = RTW_COUNTRY_JP;
    //         break;

    //     case WIFI_COUNTRY_US:
    //         rtw_country_code = RTW_COUNTRY_US;
    //         break;

    //     case WIFI_COUNTRY_EU:
    //         rtw_country_code = RTW_COUNTRY_EU;
    //         break;

    //     default:
    //         rtw_country_code = RTW_COUNTRY_CN;
    // }

    // g_country = rtw_country_code;
    // return wifi_set_country(rtw_country_code);
    return 0;
}

int xr829_get_country(aos_dev_t *dev, wifi_country_t *country)
{
    DRIVER_INVALID_RETURN_VAL;

    // switch (g_country) {
    //     case RTW_COUNTRY_CN:
    //         *country = WIFI_COUNTRY_CN;
    //         break;

    //     case RTW_COUNTRY_JP:
    //         *country = WIFI_COUNTRY_JP;
    //         break;

    //     case RTW_COUNTRY_US:
    //         *country = WIFI_COUNTRY_US;
    //         break;

    //     case RTW_COUNTRY_EU:
    //         *country = WIFI_COUNTRY_EU;
    //         break;

    //     default:
    //         *country = WIFI_COUNTRY_CN;
    // }

    return 0;
}

/**
    When wifi is disconnected, judge if reconnect wifi
*/
int xr829_set_auto_reconnect(aos_dev_t *dev, bool en)
{
    DRIVER_INVALID_RETURN_VAL;

#if CONFIG_AUTO_RECONNECT
    if (en)
        wifi_set_autoreconnect(RTW_AUTORECONNECT_DISABLE);
    else
        wifi_set_autoreconnect(RTW_AUTORECONNECT_FINITE);

    return 0;
#else
    return -1;
#endif
}

int xr829_get_auto_reconnect(aos_dev_t *dev, bool *en)
{
    DRIVER_INVALID_RETURN_VAL;

#if CONFIG_AUTO_RECONNECT
    unsigned char enable;

    wifi_get_autoreconnect(&enable);
    if (enable != RTW_AUTORECONNECT_DISABLE)
        *en = 1;
    else
        *en = 0;
    return 0;
#else
    return -1;
#endif
}

int xr829_power_on(aos_dev_t *dev)
{
    DRIVER_INVALID_RETURN_VAL;

    return 0;
}

int xr829_power_off(aos_dev_t *dev)
{
    aw_wifi_off();
    return 0;
}


/**
    TBD, scan with scan config
*/
int xr829_start_scan(aos_dev_t *dev, wifi_scan_config_t *config, bool block)
{
    int ret = 0;
    DRIVER_INVALID_RETURN_VAL;

    //only support block mode now
    if (block == 0)
        return -1;

    wifi_set_sta_mode();

#if 0
    if ((ret = wifi_scan_networks(wifi_scan_result_handler, NULL)) != RTW_SUCCESS) {
        LOGD(TAG, "[ATWS]ERROR: wifi scan failed\n\r");
        goto exit;
    }
#else
	aw_wifi_scan_results_t scan_result[MAX_SCAN_RESULTS_NUM];
    LOGD(TAG, "start scan \r\r\n");
    ret = aw_wifi_scan(scan_result,MAX_SCAN_RESULTS_NUM);
    for(int i=0; i>=0 && i<ret;i++) {
        LOGD(TAG, "%2d bssid: %02X:%02X:%02X:%02X:%02X:%02X   ssid:%-24.24s "
                "ch: %-2d   rssi: %2d   key_mgmt: %2d\r\n",i,
                scan_result[i].bssid[0],scan_result[i].bssid[1],scan_result[i].bssid[2],
                scan_result[i].bssid[3],scan_result[i].bssid[4],scan_result[i].bssid[5],
                scan_result[i].ssid,scan_result[i].channel,scan_result[i].rssi,
                scan_result[i].key);
    }
#endif

    //exit:
    return ret;
}

int xr829_sta_get_link_status(aos_dev_t *dev, wifi_ap_record_t *ap_info)
{
    DRIVER_INVALID_RETURN_VAL;

    if (wifi_status) {
        ap_info->link_status = WIFI_STATUS_LINK_UP;
    } else {
        ap_info->link_status = WIFI_STATUS_LINK_DOWN;
        return 0;
    }

    aw_wifi_network_info_t info;
    aw_wifi_get_network_info(&info);
    strcpy((char *)ap_info->ssid, (char *)info.ssid);
    memcpy(ap_info->bssid, info.bssid, 6);
    ap_info->channel = info.ch;
    ap_info->rssi    = info.rssi;

    return 0;
}

int xr829_ap_get_sta_list(aos_dev_t *dev, wifi_sta_list_t *sta)
{
    DRIVER_INVALID_RETURN_VAL;

    // int i;
    // struct {
    //     int       count;
    //     rtw_mac_t mac_list[AP_STA_NUM];
    // } client_info;

    // client_info.count = AP_STA_NUM;
    // wifi_get_associated_client_list(&client_info, sizeof(client_info));

    // LOGD(TAG, "\n\rAssociated Client List:");
    // LOGD(TAG, "\n\r==============================");

    // sta->num = client_info.count;

    // if (client_info.count == 0) {
    //     LOGD(TAG, "\n\rClient Num: 0\n\r");
    // } else {
    //     LOGD(TAG, "\n\rClient Num: %d", client_info.count);

    //     for (i = 0; i < client_info.count; i++) {
    //         if (i >= HAL_WIFI_MAX_CONN_NUM) {
    //             LOGD(TAG, "WiFi connection exceeds max number of HAL\r\n");
    //             break;
    //         }

    //         memcpy(sta->sta[i].mac, client_info.mac_list[i].octet, 6);

    //         LOGD(TAG, "\n\rClient [%d]:", i);
    //         LOGD(TAG, "\n\r\tMAC => " MAC_FMT "", MAC_ARG(client_info.mac_list[i].octet));
    //     }

    //     LOGD(TAG, "\n\r");
    // }

    return 0;
}

// static int frame_handler(const unsigned char* frame_buf, unsigned int frame_len)
// {
// #ifdef WIFI_PORTING_DISABLED
//     return 0;
// #endif

//     g_monitor_mgnt_cb((uint8_t*)frame_buf, frame_len);

//     return 0;
// }

int xr829_start_mgnt_monitor(aos_dev_t *dev, wifi_mgnt_cb_t cb)
{
    DRIVER_INVALID_RETURN_VAL;

// #if CONFIG_AUTO_RECONNECT
//     wifi_set_autoreconnect(RTW_AUTORECONNECT_DISABLE);
//     wifi_wait_auto_reconnect();
// #endif
//     while (sta_task_running) {
//         LOGI(TAG, "%s Wait STA Task Exit!\r\n", __FUNCTION__);
//         aos_msleep(1000);
//     }

    // g_monitor_mgnt_cb = cb;

    // wext_enable_rawdata_recv(WLAN0_NAME, (void*)frame_handler);

    return 0;
}

int xr829_stop_mgnt_monitor(aos_dev_t *dev)
{
    DRIVER_INVALID_RETURN_VAL;

    // wext_disable_rawdata_recv(WLAN0_NAME);

    return 0;
}

int xr829_start_monitor(aos_dev_t *dev, wifi_promiscuous_cb_t cb)
{
    DRIVER_INVALID_RETURN_VAL;


    return 0;
}

int xr829_stop_monitor(aos_dev_t *dev)
{
    DRIVER_INVALID_RETURN_VAL;


    return 0;
}

int xr829_set_channel(aos_dev_t *dev, uint8_t primary, wifi_second_chan_t second)
{
    DRIVER_INVALID_RETURN_VAL;

    // return wifi_set_channel(primary);
    return -1;
}

int xr829_get_channel(aos_dev_t *dev, uint8_t *primary, wifi_second_chan_t *second)
{
    DRIVER_INVALID_RETURN_VAL;

    // int channel;
    // if (wifi_get_channel(&channel) == 0) {
    //     *primary = channel;
    //     return 0;
    // } else {
    //     return -1;
    // }
    return -1;
}

static int xr829_send_80211_raw_frame(aos_dev_t *dev, void *buffer, uint16_t len)
{
    DRIVER_INVALID_RETURN_VAL;

    // int         ret    = 0;
    // const char *ifname = WLAN0_NAME;

    // ret = wext_send_mgnt(ifname, (char *)buffer, len, 1);
    return -1;
}

static wifi_driver_t xr829_wifi_driver = {
    /** basic APIs */

    .init             = xr829_init,
    .deinit           = xr829_deinit,
    .reset            = xr829_reset,
    .set_mode         = xr829_set_mode,
    .get_mode         = xr829_get_mode,
    .install_event_cb = xr829_install_event_cb,

    .set_protocol        = xr829_set_protocol,
    .get_protocol        = xr829_get_protocol,
    .set_country         = xr829_set_country,
    .get_country         = xr829_get_country,
    .set_mac_addr        = xr829_set_mac_addr,
    .get_mac_addr        = xr829_get_mac_addr,
    .set_auto_reconnect  = xr829_set_auto_reconnect,
    .get_auto_reconnect  = xr829_get_auto_reconnect,
    .set_lpm             = xr829_set_lpm,
    .get_lpm             = xr829_get_lpm,
    .power_on            = xr829_power_on,
    .power_off           = xr829_power_off,
    .start_scan          = xr829_start_scan,
    .start               = xr829_start,
    .stop                = xr829_stop,
    .sta_get_link_status = xr829_sta_get_link_status,
    .ap_get_sta_list     = xr829_ap_get_sta_list,

    /** promiscuous APIs */
    .start_monitor        = xr829_start_monitor,
    .stop_monitor         = xr829_stop_monitor,
    .start_mgnt_monitor   = xr829_start_mgnt_monitor,
    .stop_mgnt_monitor    = xr829_stop_mgnt_monitor,
    .send_80211_raw_frame = xr829_send_80211_raw_frame,
    .set_channel          = xr829_set_channel,
    .get_channel          = xr829_get_channel,

    .set_smartcfg = NULL,
};

#endif

#define RTL8723DS_DRIVER
#ifdef RTL8723DS_DRIVER
static aos_dev_t *xr829_dev_init(driver_t *drv, void *config, int id)
{
    aos_dev_t *dev = device_new(drv, sizeof(wifi_dev_t), id);

    return dev;
}

static void xr829_dev_uninit(aos_dev_t *dev)
{
    aos_check_param(dev);

    device_free(dev);
}

static int xr829_dev_open(aos_dev_t *dev)
{
    return 0;
}

static int xr829_dev_close(aos_dev_t *dev)
{
    return 0;
}
#endif

static netdev_driver_t xr829_driver = {
    .drv =
        {
            .name   = "wifi",
            .init   = xr829_dev_init,
            .uninit = xr829_dev_uninit,
            .open   = xr829_dev_open,
            .close  = xr829_dev_close,
        },
    .link_type = NETDEV_TYPE_WIFI,
    .net_ops   = &xr829_net_driver,
    .link_ops  = &xr829_wifi_driver,
};

#if 0
static void xr829_cut_off_gpio(int gpio)
{
    LOGD(TAG, "Turn off %d\r\n", gpio);
    drv_pinmux_config(gpio, PIN_FUNC_GPIO);
    gpio_pin_handle_t pin = csi_gpio_pin_initialize(gpio, NULL);
    csi_gpio_pin_config_mode(pin, GPIO_MODE_PULLNONE);
    csi_gpio_pin_config_direction(pin, GPIO_DIRECTION_INPUT);
    //csi_gpio_pin_write(pin, 1);
}
#endif

/**
 * @brief  register wifi driver of rtl8723 
 * @param  [in] config
 * @return  
 */
void wifi_xr829_register(void *config)
{
    // LwIP_Init();

    // wifi_manager_init();

    driver_register(&xr829_driver.drv, NULL, 0);
}

// void xr829_wifi_test()
// {
//     while (1) {

//         int total = 0, used = 0, mfree = 0, peak = 0;
//         aos_get_mminfo(&total, &used, &mfree, &peak);
//         LOGD(TAG, "                total   used   free   peak \r\r\n");
//         LOGD(TAG, "memory usage: %7d%7d%7d%7d\r\n\r\r\n", total, used, mfree, peak);

//         aw_wifi_off();
//         rtw_mdelay_os(20);
//         wifi_on(RTW_MODE_STA);

//         aos_msleep(5000);
//     }
// }
