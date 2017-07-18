#ifndef __MIIO_UTIL_H
#define __MIIO_UTIL_H

#include "tiny_base.h"

#include "miio.h"
#include "tiny_socket.h"

#define INT_MAX 0x7ffff

#define MSG_ID_MIN			1000
#define MSG_ID_MAX			INT_MAX

TINY_BEGIN_DECLS

typedef enum
{
	LOG_ERROR = 0,
	LOG_WARNING,
	LOG_INFO,
	LOG_DEBUG,
	LOG_LEVEL_MAX = LOG_DEBUG
} log_level_t;

extern log_level_t g_loglevel;

struct mosquitto;

struct callback {
	int id;
	void (* func)(void *save_data, void *new_data);
	void *save_data;

	struct callback *next;
};

void rand_str(char *dest, size_t length);
uint64_t get_micro_second(void);
unsigned int timestamp(void);
void str2hex(char *ds, unsigned char *bs, unsigned int n);
void hex2str(char *buf, int buflen, const char *hex, int hexlen);
//void callback_queue_add(struct callback **head, struct callback *c);
//void callback_queue_del(struct callback **head, struct callback *c);
//int callback_queue_issue(struct callback **head, int id, void *new_data);
//bool otd_donot_trust_peer(struct miio_data_struct *miio);
long get_uptime(void);
int rand_range(int low, int high);
int generate_random_id(void);
int get_ip_str(const struct sockaddr *sa, char *s, size_t maxlen);
uint16_t get_port(const struct sockaddr *sa);
void method_local_broadcast_msg(struct miio_data_struct *miio, char *buf);
void myMD5(const unsigned char *data, unsigned long n, unsigned char *digest);
void log_printf(log_level_t level, const char *fmt, ...);
int wait_sock_timeout(int sock, long timeout, int r, int w);
int timer_setup(void);
int timer_start(int fd, int first_expire, int interval);
int general_send_one(int sock, char *buf, int size);
int ot_agent_fd_close(struct miio_data_struct *p_miio, int fd);
void xor_encrypt(const char *key, char *string, int n);
int data_encrypt(const char *key, const char *in, int in_len,
		 char *out, int *out_len);
int data_decrypt(const char *key, const char *in, int in_len,
		 char *out, int *out_len);

TINY_END_DECLS

#endif
