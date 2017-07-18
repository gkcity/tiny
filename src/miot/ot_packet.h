#ifndef __OT_PACKET_H__
#define __OT_PACKET_H__

#include <stdlib.h>
#include <stdint.h>

#include "miio.h"

#define OT_DID_REQUEST			0xFFFFFFFFFFFFFFFFLL
#define OT_TOKEN_REQUEST		"\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF"
#define OT_TS_REQUEST			"\xFF\xFF\xFF\xFF"
#define OT_PACKET_HEADER_LEN     32

TINY_BEGIN_DECLS
;
struct ot_packet_header {
	uint8_t version[2];
	uint16_t length;
	uint64_t did;
	uint32_t timestamp;
	uint8_t md5[AES_BLOCK_SIZE];
}__attribute__((packed));

struct ot_packet {
	struct ot_packet_header header;
	unsigned char payload[];
};


typedef struct ot_packet_header OtPacketHeader;
typedef struct ot_packet OtPacket;

/* build a ot packet */
char* OtPacket_Encode(char* msg, int* len, MiioCtx* miio);

int OtPacket_Decode(char* data, int len, MiioCtx* miio);

TINY_END_DECLS

#endif
