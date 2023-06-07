#pragma once
#include <string>
#include <unordered_map>
#include <tuple>
#include "AES.h"
#include <openssl/bn.h>

using namespace std;
typedef unsigned char byte;

struct STAndC {
public:
	uint8_t STc[AES_BLOCK_SIZE];
	int c;
};

struct Ciphertext
{
public:
	uint8_t stc_pre_enc[AES_BLOCK_SIZE];
	BIGNUM *e;
};

/*
class UContent;
class Ciphertext;
class AllParam;
class PublicParam;
class STAndC;
class Token;
class SecretKey;
class BUContent;
class BCiphertext;
class BPublicParam;
class BAllParam;
*/