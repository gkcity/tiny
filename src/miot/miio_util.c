#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdarg.h>
#include <stdint.h>
#include <time.h>
#include <errno.h>
#include "miio_util.h"
//#include "miio_json.h"
//#include "miio_mqtt.h"
#include "LibMd5.h"
#include "tiny_log.h"
#include "tiny_socket.h"

#ifdef ENABLE_MOSQUITTO
#include "mosquitto.h"
struct mosquitto;
#endif

#define TAG "miio_util"
/**
 * Note: caller make sure dest is big enough and end with '\0'
 */
void rand_str(char *dest, size_t length)
{
	char charset[] = "0123456789"
		"abcdefghijklmnopqrstuvwxyz"
		"ABCDEFGHIJKLMNOPQRSTUVWXYZ";

	srand(get_micro_second());
	while (length-- > 0) {
		size_t index = (double) rand() / RAND_MAX * (sizeof charset - 1);
		*dest++ = charset[index];
	}
}

uint64_t get_micro_second(void)
{
	struct timespec ts;
	clock_gettime(CLOCK_MONOTONIC, &ts);
	return (uint64_t)ts.tv_sec * 1000000 + ts.tv_nsec / 1000;
}

unsigned int timestamp(void)
{
	return get_micro_second() / 1000000;
}

void str2hex(char *ds, unsigned char *bs, unsigned int n)
{
	int i;
	for (i = 0; i < n; i++)
		sprintf(ds + 2 * i, "%02x", bs[i]);

}

void hex2str(char *buf, int buflen, const char *hex, int hexlen)
{
	char substr[3];
	int i;

	if (buflen < hexlen / 2) {
		LOG_W(TAG, "buflen(%d) might not be enough for the hexlen(%d) convertion.\n", buflen, hexlen);
	}

	memset(buf, 0, buflen);

	for (i = 0; i < buflen &&  i * 2 < hexlen; i++) {
		memcpy(substr, hex + i * 2, 2);
		substr[2] = '\0';
		buf[i] = (char)strtoul(substr, NULL, 16);
	}

	return;
}

void callback_queue_add(struct callback **head, struct callback *c)
{
	c->next = *head;
	*head = c;
	return;
}

void callback_queue_del(struct callback **head, struct callback *c)
{
	struct callback *node, *prev;

	if (c == NULL || *head == NULL)
		return;

	prev = *head;
	for (node = *head; node != NULL;) {
		if (node->id == c->id) {
			if (prev == node)
				*head = node->next;
			else
				prev->next = node->next;
			break;
		}
		prev = node;
		node = node->next;
	}

	if (node != NULL)
		free(node);
	return;
}

int callback_queue_issue(struct callback **head, int id, void *new_data)
{
	void (* func)(void *save_data, void *new_data);
	struct callback *node, *prev;

	if (*head  == NULL)
		return -1;

	prev = *head;
	for (node = *head; node != NULL;) {
		if (node->id == id) {
			if (prev == node)
				*head = node->next;
			else
				prev->next = node->next;
			break;
		}
		prev = node;
		node = node->next;
	}

	if (node == NULL)
		return -1;

	func = node->func;
	func(node->save_data, new_data);

	free(node);

	return 0;
}

/**
 * check whether network config finished.
 *
 * @return true if network config-ed, false if not
 */
/* 
static bool wifi_config_done(char *datadir)
{
	struct stat st;
	char path[PATH_MAX];
	int ret = -1;

	ret = snprintf(path, sizeof(path), "%s%s", datadir, WIFI_CONF_FILE);
	if (ret < 0) {
		return false;
	} else if (ret >= sizeof(path)) {
		return false;
	} else {
		path[ret] = '\0';
	}

	if (stat(path, &st) < 0)
		return false;

	return true;
}
*/
/**
 * Check to see if OTD can trust peer or not.
 *
 * If smart device had been binded to a certain mobile, then OTD will not
 * trust any other mobile any more.
 *
 * @return true if OTD do not trust peer, false otherwise
 */
/*
bool otd_donot_trust_peer(struct miio_data_struct *miio)
{
	struct stat st;
	char path[PATH_MAX];
	int ret;

	ret = snprintf(path, sizeof(path), "%s%s", miio->datadir, OTD_DONOT_TRUST_PEER_FILE);
	if (ret < 0) {
		return true;
	} else if (ret >= sizeof(path)) {
		return true;
	} else {
		path[ret] = '\0';
	}

	if (stat(path, &st) < 0)
		return false;

	return true;
}
*/
/**
 * Get system uptime.
 */
long get_uptime(void)
{
/*
	struct sysinfo s;

	memset(&s, 0, sizeof(sysinfo));
	if (sysinfo(&s) < 0) {
		log_printf(LOG_WARNING, "get uptime fail, %m");
		return 0;
	}

	return s.uptime;
*/
	struct timespec ts;
	clock_gettime(CLOCK_MONOTONIC, &ts);
	return ts.tv_sec;

}


/*
 * Return a random number between low and high inclusive.
 */
int rand_range(int low, int high)
{
	int limit = high - low;
	int divisor = RAND_MAX/(limit+1);
	int retval;

	do {
		retval = rand() / divisor;
	} while (retval > limit);

	return retval + low;
}

int generate_random_id(void)
{
	srand(get_micro_second());

	return rand_range(MSG_ID_MIN, MSG_ID_MAX);
}

/*
 * Broadcast to other processes in local device
 */
/*
void method_local_broadcast_msg(struct miio_data_struct *miio, char *buf)
{
	int i;

	if (miio == NULL)
		return;

#ifdef ENABLE_MOSQUITTO
	mosquitto_publish(miio->mosq, NULL, MQTT_TOPIC_MIIO_COMMAND, strlen(buf), buf, 2, false);
#endif
	for (i = 0; i < miio->client_count; i++) {
		if (miio->clientfds[i] == miio->ot_agent_internalfd)
			continue;
		general_send_one(miio->clientfds[i], buf, strlen(buf));
	}

	return;
}
*/
/* Convert a struct sockaddr address to a string, IPv4 and IPv6
 * char str[INET6_ADDRSTRLEN]
 */
int get_ip_str(const struct sockaddr *sa, char *s, size_t maxlen)
{
	switch(sa->sa_family) {
        case AF_INET:
		inet_ntop(AF_INET, &(((struct sockaddr_in *)sa)->sin_addr),
			  s, maxlen);
		break;

        case AF_INET6:
		inet_ntop(AF_INET6, &(((struct sockaddr_in6 *)sa)->sin6_addr),
			  s, maxlen);
		break;

        default:
		strncpy(s, "Unknown AF", maxlen);
		return -1;
	}

	return 0;
}

/* Get port number */
uint16_t get_port(const struct sockaddr *sa)
{
	unsigned short port;

	switch(sa->sa_family) {
        case AF_INET:
		port = ((struct sockaddr_in *)sa)->sin_port;
		break;

        case AF_INET6:
		port = ((struct sockaddr_in6 *)sa)->sin6_port;
		break;

        default:
		port = -1;
		return port;
	}

	return ntohs(port);
}

void myMD5(const unsigned char *data, unsigned long n, unsigned char *digest)
{
	MD5_CTX c;

	MD5_Init(&c);
	MD5_Update(&c, (void *)data, (uint32_t)n);
	MD5_Final(digest, &c);
}

log_level_t g_loglevel  = LOG_DEBUG;

#ifndef __ANDROID_LOG__
void log_printf(log_level_t level, const char *fmt, ...)
{
	char buf[80];
	time_t now;
	va_list ap;
	struct tm *p;
	char *slevel;

	if (level <= g_loglevel) {
		switch (level) {
		case LOG_DEBUG   : slevel = "[DEBUG]"; break;
		case LOG_WARNING : slevel = "[WARNING]"; break;
		case LOG_INFO    : slevel = "[INFO]"; break;
		case LOG_ERROR   : slevel = "[ERROR]"; break;
		}

		now = time(NULL);
		p = localtime(&now);
		strftime(buf, 80, "[%Y%m%d %H:%M:%S]", p);

		va_start(ap, fmt);
		printf("%s %s ", buf, slevel);
		vprintf(fmt, ap);
		va_end(ap);
	}
}
#endif

/*
 * wait sock connect timeout
 */
/*
int wait_sock_timeout(int sock, long timeout, int r, int w)
{
	struct timeval tv = {0,0};
	fd_set fdset;
	fd_set *rfds, *wfds;
	int n, so_error;
	unsigned so_len;

	FD_ZERO(&fdset);
	FD_SET(sock, &fdset);
	tv.tv_sec = timeout;
	tv.tv_usec = 0;

	if (r) rfds = &fdset; else rfds = NULL;
	if (w) wfds = &fdset; else wfds = NULL;

	n = select(sock + 1, rfds, wfds, NULL, &tv);
	switch (n) {
	case 0:
		errno = ETIMEDOUT;
		return -errno;
	case -1:
		return -errno;
	default:
		so_len = sizeof(so_error);
		so_error = 0;
		getsockopt(sock, SOL_SOCKET, SO_ERROR, &so_error, &so_len);
		if (so_error == 0)
			return 0;
		errno = so_error;
		return -errno;
	}
}

int timer_setup(void)
{
	int fd;

	fd = timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);
	if (fd < 0) {
		perror("timerfd_create");
		return fd;
	}

	return fd;
}

int timer_start(int fd, int first_expire, int interval)
{
	struct itimerspec new_value;

	new_value.it_value.tv_sec = first_expire / 1000;
	new_value.it_value.tv_nsec = first_expire % 1000 * 1000000;

	new_value.it_interval.tv_sec = interval / 1000;
	new_value.it_interval.tv_nsec = interval % 1000 * 1000000;

	if (timerfd_settime(fd, 0, &new_value, NULL) == -1) {
		perror("timerfd_settime");
		return -1;
	}

	return 0;
}

int general_send_one(int sock, char *buf, int size)
{
	ssize_t sent = 0;
	ssize_t ret;

	if (size == 0) {
		log_printf(LOG_ERROR, "%s size == 0\n", __func__);
		return 0;
	}

	if (buf != NULL) log_printf(LOG_INFO, "%s,%s about to send %s:%d to %d\n", __FILE__, __func__, buf, size, sock);
	while (size > 0) {
		//ret = send(sock, buf + sent, size, MSG_DONTWAIT);
		ret = write(sock, buf + sent, size);
		if (ret < 0) {
			log_printf(LOG_ERROR, "%s,%s: send error: %m\n", __FILE__, __func__);
			return ret;
		}

		if (ret < size)
			log_printf(LOG_WARNING, "Partial written\n");

		sent += ret;
		size -= ret;
	}

	return ret;
}
*/
/*
int ot_agent_fd_close(struct miio_data_struct *p_miio, int fd)
{
	int n, found;
*/
	/* remove from pollfds[] */
/*	found = 0;
	for (n = 0; n < p_miio->count_pollfds; n++) {
		if (p_miio->pollfds[n].fd == fd) {
			found = 1;
			while (n < p_miio->count_pollfds) {
				p_miio->pollfds[n] = p_miio->pollfds[n + 1];
				n++;
			}
		}
	}

	if (found)
		p_miio->count_pollfds--;
	else
		log_printf(LOG_WARNING, "client fd (%d) not in pollfds.\n", fd);
*/
	/* remove from clientfds[] and close */
/*	found = 0;
	for (n = 0; n < p_miio->client_count; n++) {
		if (p_miio->clientfds[n] == fd) {
			found = 1;
			while (n < p_miio->client_count) {
				p_miio->clientfds[n] = p_miio->clientfds[n + 1];
				n++;
			}
		}
	}

	if (found)
		p_miio->client_count--;
	else
		log_printf(LOG_WARNING, "client fd (%d) not in clientfds.\n", fd);

	log_printf(LOG_WARNING, "miio.so close fd: %d\n", fd);
	close(fd);
}
*/

void xor_encrypt(const char *key, char *string, int n)
{
	int i;
	int len = strlen(key);
	for (i = 0 ; i < n ; i++) {
		string[i] = string[i] ^ key[i % len];
	}
}

/**
 * key: key used to encrypt data;
 * in: input data;
 * in_len: input data length;
 * out: output data;
 * out_len: output data length, initialized as out buffer length, and will
 *	be changed to reflect actual output data length;
 *
 * Note: out_len must be long enough: at least (2*in_len + 1)
 */
int data_encrypt(const char *key, const char *in, int in_len,
			char *out, int *out_len)
{
	int i;
	char *tmp;

	if (*out_len < 2 * in_len + 1)
		return -1;
	if (key == NULL || in == NULL || out == NULL || in_len == 0)
		return -1;

	tmp = malloc(in_len);
	if (tmp == NULL)
		return -1;
	memcpy(tmp, in, in_len);
	xor_encrypt(key, tmp, in_len);
	for (i = 0; i < in_len; i++)
		tmp[i] -= 15;
	str2hex(out, tmp, in_len);
	*out_len = 2 * in_len;

	return 0;
}

/**
 * key: key used to decrypt data;
 * in: input data;
 * in_len: input data length;
 * out: output data;
 * out_len: output data length, initialized as out buffer length, and will
 *	be changed to reflect actual output data length;
 *
 * Note: out_len must be long enough: at least in_len / 2
 */
int data_decrypt(const char *key, const char *in, int in_len,
			char *out, int *out_len)
{
	int i;

	if (*out_len < in_len / 2)
		return -1;
	if (in_len % 2 != 0)
		return -1;
	if (key == NULL || in == NULL || out == NULL || in_len == 0)
		return -1;

	hex2str(out, *out_len, in, in_len);
	*out_len = in_len / 2;
	for (i = 0; i < *out_len; i++)
		out[i] += 15;
	xor_encrypt(key, out, *out_len);

	return 0;
}
