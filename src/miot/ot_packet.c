#include <string.h>
#include <stdlib.h>
#include <stdint.h>

#include "tiny_malloc.h"
#include "tiny_log.h"
#include "tiny_base.h"
#include "tiny_socket.h"


#include "ot_packet.h"
#include "miio_util.h"
#include "aes.h"


#define TAG "OtPacket"

uint64_t htobe64(uint64_t v) {
    union { uint32_t lv[2]; uint64_t llv; } u;
    u.lv[0] = htonl(v >> 32);
    u.lv[1] = htonl(v & 0xFFFFFFFFULL);
    return u.llv;
}

uint64_t be64toh(uint64_t v) {
    union { uint32_t lv[2]; uint64_t llv; } u;
    u.llv = v;
    return ((uint64_t)ntohl(u.lv[0]) << 32) | (uint64_t)ntohl(u.lv[1]);
}


static TinyRet ot_encrypt_packet(char **buffer, int *len, MiioCtx* miio)
{
	struct ot_packet *packet = (struct ot_packet *)(*buffer);
	struct ot_packet_header *header = &packet->header;
	int pad_len;
	unsigned char pad_char;
	char *new;
	int new_len;
	int i;
	
	const char* key = miio->dev_info.key;
	uint8_t mykey[AES_BLOCK_SIZE], iv[AES_BLOCK_SIZE];
	uint8_t tmpbuf[AES_BLOCK_SIZE * 2];
	char* encbuf = NULL;

	if (packet == NULL)
		return -1;

	pad_len = 16 - (*len)%16;
	pad_char = (unsigned char) pad_len;
	new_len = *len + pad_len;
	*len = new_len;

	new = tiny_realloc(*buffer, new_len);
	if (new == NULL) {
		LOG_E(TAG, "tiny_realloc error");
		return TINY_RET_E_OUT_OF_MEMORY;
	}
	*buffer = new;
	packet = (struct ot_packet *)(*buffer);
	header = &packet->header;

	for (new = *buffer + new_len - 1; pad_len; pad_len--) {
		*new = pad_char;
		new--;
	}

	/* encrypt */


	myMD5(key, AES_BLOCK_SIZE, mykey);
	memcpy(tmpbuf, mykey, AES_BLOCK_SIZE);
	memcpy(tmpbuf + AES_BLOCK_SIZE, key, AES_BLOCK_SIZE);
	myMD5(tmpbuf, sizeof(tmpbuf), iv);

	encbuf = tiny_malloc(new_len - sizeof(struct ot_packet_header));
	if (encbuf == NULL)
	{
		LOG_E(TAG, "tiny_malloc for payload error");
		tiny_free(*buffer);
		return TINY_RET_E_OUT_OF_MEMORY;
	}
	//assert(encbuf != NULL);
	AES128_CBC_encrypt_buffer(encbuf, packet->payload, new_len - sizeof(struct ot_packet_header),
				  mykey, iv);
	memcpy(packet->payload, encbuf, new_len - sizeof(struct ot_packet_header));
	tiny_free(encbuf);

	header->version[0] = '!';
	header->version[1] = '1';

	header->did = htobe64(miio->dev_info.did);
	header->length = htons(new_len);
	header->timestamp = htonl(timestamp() + miio->timestamp_diff);

	memcpy(header->md5, key, AES_BLOCK_SIZE);
	myMD5((unsigned char *)packet, new_len, mykey);
	memcpy(header->md5, mykey, AES_BLOCK_SIZE);

	return TINY_RET_OK;
}

static TinyRet OtPacket_Construct_Header(MiioCtx* miio, int pack_len, struct ot_packet_header* header)
{
	uint8_t md5[AES_BLOCK_SIZE];

	RETURN_VAL_IF_FAIL(header, TINY_RET_E_ARG_INVALID);
	
	header->version[0] = '!';
	header->version[1] = '1';

	header->did = htobe64(miio->dev_info.did);
	header->length = htons(pack_len);
	header->timestamp = htonl(timestamp() + miio->timestamp_diff);

	memcpy(header->md5,  miio->dev_info.key, AES_BLOCK_SIZE);
	myMD5((unsigned char *)header, pack_len, md5);
	memcpy(header->md5, md5, AES_BLOCK_SIZE);

	return TINY_RET_OK;
}

char *OtPacket_Encode(char *payload, int *ret_length, MiioCtx* miio)
{
	char *buffer;
	int length = sizeof(struct ot_packet_header) + strlen(payload) + 1;

	buffer = tiny_malloc(length);
	if (buffer == NULL) {
		LOG_E(TAG, "ot_construct_packet malloc error");
		return NULL;
	}

	memset(buffer, 0, length);

	if (payload == NULL || strlen(payload) == 0)
	{
		LOG_D(TAG, "report keepalive packet");
		if(RET_SUCCEEDED(OtPacket_Construct_Header(miio, OT_PACKET_HEADER_LEN, (struct ot_packet_header*)buffer)))
		{
			*ret_length = OT_PACKET_HEADER_LEN;
			return buffer;
		}
		else
		{
			*ret_length = 0;
			tiny_free(buffer);
			return NULL;
		}
	}

	
	memcpy(buffer + sizeof(struct ot_packet_header), payload, strlen(payload));
	if (RET_SUCCEEDED(ot_encrypt_packet(&buffer, &length, miio)))
	{
	    *ret_length = length;
	    return buffer;
	}
	*ret_length = 0;
	return NULL;
}

int OtPacket_Decode(char* data, int len, MiioCtx* miio)
{
	struct ot_packet *packet;
	uint8_t md5_orig[AES_BLOCK_SIZE];
	uint8_t md5_now[AES_BLOCK_SIZE];
	struct ot_packet_header *header;

	uint32_t count;
	int done = 0;

	uint8_t mykey[AES_BLOCK_SIZE], iv[AES_BLOCK_SIZE];
	uint8_t tmpbuf[AES_BLOCK_SIZE * 2];
	char* decbuf = NULL;
	unsigned char *pad_p;
	uint8_t pad_len = 0;
	
	header = (struct ot_packet_header *) data;

	/* Basic verify before we trust the length */
	if (header->version[0] != '!' ||
	    header->version[1] != '1') {
		LOG_E(TAG, "OT protocol version not match.\n");
		//dump_header((unsigned char *)header, sizeof(struct ot_packet_header));
		return -1;
	}

	if (len < sizeof(struct ot_packet_header)) {
		LOG_E(TAG, "OT packet length not correct: %d.\n", len);
		//dump_header((unsigned char *)header, sizeof(struct ot_packet_header));
		return -1;
	}

	if (len == sizeof(struct ot_packet_header)) {
/*
		if (header->did == OT_DID_REQUEST) {
			int ts = ntohl(header->timestamp);

			log_printf(LOG_DEBUG, "Time sync msg from server\n");
			report_synctime_ack(ts);
			return 0;
		} else {
			log_printf(LOG_DEBUG, "Ping - Pong msg from server\n");
			miio.ping_pong = true;
			return 0;
		}
*/
		LOG_W(TAG, "no payload, maybe synctime_ack or ping_pang");
		return 0;
	}

	if (be64toh(header->did) != miio->dev_info.did) {
		LOG_E(TAG, "OT device id not match.\n");
		//dump_header((unsigned char *)header, sizeof(struct ot_packet_header));
		return -1;
	}

	packet = (struct ot_packet *) data;

	/* check MD5 */
	memcpy(md5_orig, header->md5, sizeof(md5_orig));
	memcpy(header->md5, miio->dev_info.key, sizeof(header->md5));
	myMD5((unsigned char *)packet, len, md5_now);
	if (memcmp(md5_orig, md5_now, sizeof(md5_orig)) != 0) {
		LOG_E(TAG, "OT MD5 not match.\n");
		//dump_header((unsigned char *)header, sizeof(struct ot_packet_header));
		return -1;
	}

	/* decrypt */
	myMD5(miio->dev_info.key, sizeof(miio->dev_info.key), mykey);
	memcpy(tmpbuf, mykey, AES_BLOCK_SIZE);
	memcpy(tmpbuf + AES_BLOCK_SIZE, miio->dev_info.key, AES_BLOCK_SIZE);
	myMD5(tmpbuf, sizeof(tmpbuf), iv);

	decbuf = tiny_malloc(len - sizeof(struct ot_packet_header));
	if (decbuf == NULL)
	{
		LOG_E(TAG, "tiny_malloc for decbuf error");
		return -1;
	}
	//assert(decbuf != NULL);
	AES128_CBC_decrypt_buffer(decbuf, packet->payload, len - sizeof(struct ot_packet_header),
				  mykey, iv);
	memcpy(packet->payload, decbuf, len - sizeof(struct ot_packet_header));
	free(decbuf);

	/* remove padding */
	pad_p = (unsigned char *)packet + len - 1;
	pad_len = *pad_p;

	if (pad_len > 16) {
		LOG_E(TAG, "OT padding panic, pad_len: %d\n", pad_len);
		return -1;
	}

	len -= sizeof(struct ot_packet_header);
	len -= pad_len;

	for (; pad_len > 0; pad_len--) {
		*pad_p = '\0';
		pad_p--;
	}

	/* hand over to local process */
	//cloud_handle_msg(packet->payload, length);

	return len ;
}



