#pragma once

extern "C" {
#include "AES.h"
}

#include <openssl/bn.h>
#include "entity.h"
#include <random>
#include <sstream>
#include <iomanip>
#include <unordered_map>
#include <string>
#include <iostream>

class FB_DSSE {
public:
	int bitLen;
	BIGNUM* n;
	uint8_t* key = (uint8_t*)"0123456789123456";
	uint8_t* iv = (uint8_t*)"0123456789123456";
	unordered_map<string, STAndC> CT;
	unordered_map<string, Ciphertext> EDB;


	FB_DSSE(int bitLen);
	void RandomGen(uint8_t bytes[]);
	void Update(string w, BIGNUM* bs);
	BIGNUM* Search(string w);
	uint8_t* getBytes(string const& s);
	string byteToString(uint8_t b[]);
};