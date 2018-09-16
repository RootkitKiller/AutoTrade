#pragma once
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
/* #include <string.h> */
#include <stdint.h>	//  Added by RKW, needed for types uint8_t, uint32_t; requires C99 compiler
typedef struct {
	uint8_t		hash[32];	// Changed by RKW, unsigned char becomes uint8_t
	uint32_t	buffer[16];	// Changed by RKW, unsigned long becomes uint32_t
	uint32_t	state[8];	// Changed by RKW, unsinged long becomes uint32_t
	uint8_t		length[8];	// Changed by RKW, unsigned char becomes uint8_t
} sha256;
void sha256_initialize(sha256 *sha);
void sha256_update(sha256 *sha,
	const uint8_t *message,
	uint32_t length);
void sha256_finalize(sha256 *sha,
	const uint8_t *message,
	uint32_t length);
void sha256_get(uint8_t hash[32],
	const uint8_t *message,
	int length);
typedef struct _hmac_sha256 {
	uint8_t	digest[32];	// Changed by RKW, unsigned char becomes uint_8
	uint8_t	key[64];	// Changed by RKW, unsigned char becomes uint_8
	sha256	sha;
} hmac_sha256;
void hmac_sha256_initialize(hmac_sha256 *hmac,
	const uint8_t *key, int length);
void hmac_sha256_update(hmac_sha256 *hmac,
	const uint8_t *message, int length);
void hmac_sha256_finalize(hmac_sha256 *hmac,
	const uint8_t *message, int length);
void hmac_sha256_get(uint8_t digest[32],
	const uint8_t *message, int message_length,
	const uint8_t *key, int key_length);
//int main1(int argc, const char *const *argv, unsigned char *key, unsigned char *block);













