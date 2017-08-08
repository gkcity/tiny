/*
cdecode.h - c header for a base64 decoding algorithm

This is part of the libb64 project, and has been placed in the public domain.
For details, see http://sourceforge.net/projects/libb64
*/

#ifndef BASE64_CDECODE_H
#define BASE64_CDECODE_H

#include <stdint.h>
#include "tiny_base.h"


TINY_BEGIN_DECLS
;
typedef enum
{
	step_a, step_b, step_c, step_d
} base64_decodestep;

typedef struct
{
	base64_decodestep step;
	char plainchar;
} base64_decodestate;

void base64_init_decodestate(base64_decodestate* state_in);

int base64_decode_value(signed char value_in);

int base64_decode_block(const signed char* code_in, const int length_in, signed char* plaintext_out, base64_decodestate* state_in);
int base64_decode(const uint8_t *data, uint64_t datalen, signed char *decoded);

TINY_END_DECLS

#endif /* BASE64_CDECODE_H */

