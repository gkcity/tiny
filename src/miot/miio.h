#ifndef __MIIO_H__
#define __MIIO_H__

#include <stdlib.h>
#include <stdint.h>
#include "tiny_base.h"

#ifndef AES_BLOCK_SIZE
#define AES_BLOCK_SIZE	16
#endif

#define OT_MAX_PAYLOAD			1024	/* comply with cloud/server implementation */
#define TIMER_INTERVAL			5000	/* 5s */
#define TIMER_CONNECT_RETRY		10000	/* 10s */
#define TIMER_KEEPALIVE_INTERVAL	15000	/* 15s */
#define TIMER_SYNC_SHORT_INTERVAL	5000	/* 5s interval when first report _otc.info */
#define TIMER_SYNC_INTERVAL		1800000	/* 30 min sync _otc.info */
#define ACK_TIMEOUT			3000	/* 3s wait for server ack and retry */

#define MIIO_DATA_DIR_DEFAULT		"/etc/miio/"
#define MIIO_DEVICE_CONFIG_FILE		"device.conf"
#define MIIO_DEVICE_TOKEN_FILE		"device.token"
#define WIFI_CONF_FILE			"wifi.conf"
#define OTD_DONOT_TRUST_PEER_FILE	"otd_donot_trust_peer"

#define SMART_CONN_PORT	54321
#define OT_SERVER_PORT	54322
#define OT_TCP_HOST	"ott.io.mi.com"
#define OT_TCP_PORT	"80"
#define OT_UDP_HOST	"ot.io.mi.com"
#define OT_UDP_PORT	"8053"

#define VENDOR_SIZE	64
#define VERSION_SIZE	64
#define PARTNER_ID_SIZE 64
#define MODEL_SIZE	24
#define MAX_POLL_FDS	10
#define MAX_BUFLEN	512

#define CLIENT_MAX	5

#define MAGIC_DID_TYPE1 0	/* type 1: special did indicates this device
				   does not need smart wifi connect, but need
				   to register {did, key} from xiaomi cloud. */
#define MAGIC_DID_TYPE2 1	/* type 2: special did indicates this device
				   needs smart wifi connect, as well as
				   register {did, key} from xiaomi cloud. */

TINY_BEGIN_DECLS

//struct mdnsd;

struct _MiioCloudCtx;
typedef struct _MiioCloudCtx MiioCloudCtx;

typedef struct _miio_dev_info {
	uint64_t did;
	uint8_t vendor[VENDOR_SIZE];
	uint8_t key[AES_BLOCK_SIZE];
	uint8_t token[AES_BLOCK_SIZE];
	uint8_t iv[AES_BLOCK_SIZE];
	uint8_t model[MODEL_SIZE];
	uint8_t mac[18];
	uint8_t ip[16];
	uint8_t wpa_state;
} MiioDevInfo;

typedef enum _miio_state {
	STATE_DEVICE_INIT,
	STATE_DIDKEY_REQ1,
	STATE_DIDKEY_REQ2,
	STATE_DIDKEY_DONE,
	STATE_TOKEN_DONE,
	STATE_WIFI_AP_MODE,
	STATE_WIFI_STA_MODE,
//	STATE_CLOUD_TRYING,
//	STATE_CLOUD_CONNECTED,
//	STATE_CLOUD_CONNECT_RETRY,
} MiioState;

typedef struct miio_data_struct {
	MiioState state;

	/* For some devices, we need to req {did,key} from server. */
	uint32_t didkeyreq_timestamp;
	uint32_t didkeyreq_interval;

	bool enckey;
	bool encdata;
	bool force_exit;
	bool ping_pong;	/* Ping pong packet to check network disconnection */
	unsigned int timestamp_sync; /* timestamp when we last sync, (s) */
	int timestamp_diff; /* timestamp_sync + timestamp_diff = server_time */
	int server_noack; /* count of noack from server */

	unsigned char *datadir;
	//struct mdnsd *mdns;

#ifdef ENABLE_MOSQUITTO
	struct mosquitto *mosq;
#endif

	/* device specific */
	MiioDevInfo dev_info;
	char *info_str;
}MiioCtx;

TINY_END_DECLS

#endif

