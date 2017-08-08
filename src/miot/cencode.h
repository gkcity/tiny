/*
cencode.h - c header for a base64 encoding algorithm

This is part of the libb64 project, and has been placed in the public domain.
For details, see http://sourceforge.net/projects/libb64
*/

#ifndef BASE64_CENCODE_H
#define BASE64_CENCODE_H

#include "tiny_base.h"

TINY_BEGIN_DECLS

typedef enum
{
	step_A, step_B, step_C
} base64_encodestep;

typedef struct
{
	base64_encodestep step;
	signed char result;
	int stepcount;
} base64_encodestate;

void base64_init_encodestate(base64_encodestate* state_in);

signed char base64_encode_value(signed char value_in);

int base64_encode_block(const signed char* plaintext_in, int length_in, signed char* code_out, base64_encodestate* state_in);

int base64_encode_blockend(signed char* code_out, base64_encodestate* state_in);
int base64_encode(const uint8_t *data, uint64_t datalen, signed char *encoded);

TINY_END_DECLS

#endif /* BASE64_CENCODE_H */

