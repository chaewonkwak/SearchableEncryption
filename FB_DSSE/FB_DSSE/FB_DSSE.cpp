#include "FB_DSSE.h"

FB_DSSE::FB_DSSE(int bitLen) {
	this->bitLen = bitLen;
	BIGNUM* nn = BN_new();
	BIGNUM* two = BN_new();
	BIGNUM* p = BN_new();
	BN_CTX* ctx = BN_CTX_new();
	BN_set_word(two, 2);
	BN_set_word(p, this->bitLen);
	BN_exp(nn, two, p, ctx);
	this->n = BN_dup(nn);

	BN_CTX_free(ctx);
	BN_free(two);
	BN_free(p);
	BN_free(nn);
}
void FB_DSSE::RandomGen(uint8_t bytes[]) {
	for (int i = 0; i < AES_BLOCK_SIZE; i++)
		bytes[i] = (uint8_t) rand();
}
void FB_DSSE::Update(string w, BIGNUM* bs) {

	//init
	BN_CTX* ctx = BN_CTX_new();
	uint8_t derived_key[AES_BLOCK_SIZE];
	memcpy(derived_key, key, AES_BLOCK_SIZE);
	uint8_t iivv[AES_BLOCK_SIZE];
	memcpy(iivv, iv, AES_BLOCK_SIZE);

	// Fk(w)
	uint8_t* s = getBytes(w);
	uint8_t K[sizeof(s)];

	Encrypt(s, sizeof(s), derived_key, iivv, K);

	uint8_t K_w1[sizeof(K) / 2] = { 0, };
	uint8_t K_w2[sizeof(K) / 2] = { 0, };
	copy(K, K + sizeof(K) / 2, K_w1);
	copy(K + sizeof(K) / 2, K + sizeof(K), K_w2);

	// (ST,c)<-CT[w]
	auto it = this->CT.find(w);
	STAndC STAndC_old;
	if (it == this->CT.end()) {
		int c = -1;
		uint8_t STc[AES_BLOCK_SIZE];
		RandomGen(STc);
		STAndC_old.c = c;
		//STAndC_old->STc = STc;
		memcpy(STAndC_old.STc, STc, AES_BLOCK_SIZE);
	}
	else
	{
		STAndC_old = this->CT[w];
	}

	int c = STAndC_old.c + 1;
	uint8_t STc[AES_BLOCK_SIZE];
	RandomGen(STc);
	
	STAndC STAndC_new;//= new STAndC();
	STAndC_new.c = c;
	memcpy(STAndC_new.STc, STc, AES_BLOCK_SIZE);
	//STAndC_new->STc = (uint8_t*)STc;
	
	this->CT[w] = STAndC_new;

	
	// UT <- H1
	uint8_t UT[sizeof(STAndC_new.STc)];
	Encrypt(STAndC_new.STc, sizeof(STAndC_new.STc), K_w1, iivv, UT);

	// C <- H2 xor STc
	uint8_t H2[sizeof(STAndC_new.STc)];
	Encrypt(STAndC_new.STc, sizeof(STAndC_new.STc), K_w1, iivv, H2);
	uint8_t C[sizeof(H2)]; 
	for (int i = 0; i < sizeof(C); i++)
	{
		C[i] = H2[i] ^ (STAndC_old.STc[i]);
	}
	
	// sk <- H3
	uint8_t sk[AES_BLOCK_SIZE];
	Encrypt((uint8_t*)&(c),
		sizeof(sk),
			K_w2, 
			iivv, 
			sk);
	BIGNUM* skBN = BN_new();
	// skBN <- sk
	unsigned long long value;
	memcpy(&value, sk, sizeof(int));
	BN_set_word(skBN, value);

	BN_mod(skBN, skBN, this->n, ctx);

	// e <- Enc(sk, bs, n)
	BIGNUM* e = BN_new();
	BN_copy(e, bs);
	if (BN_is_negative(e))
		BN_add(e, e, this->n);

	BN_mod_add(e, skBN, e, this->n, ctx);

	// EDB[ut] = ct
	string ut = byteToString(UT);
	
	Ciphertext ct;// = { C, e };
	ct.e = e;
	memcpy(ct.stc_pre_enc, C, AES_BLOCK_SIZE);

	EDB[ut] = ct;
	//free
	BN_CTX_free(ctx);

	return;
}

BIGNUM* FB_DSSE::Search(string w) {
	
	//init
	uint8_t derived_key[AES_BLOCK_SIZE];
	memcpy(derived_key, this->key, AES_BLOCK_SIZE);
	uint8_t iivv[AES_BLOCK_SIZE];
	memcpy(iivv, iv, AES_BLOCK_SIZE);
	BN_CTX* ctx = BN_CTX_new();

	// Kw||Kw1 = Fk
	uint8_t* s = getBytes(w);
	uint8_t K[sizeof(s)];
	Encrypt(s, sizeof(s), derived_key, iivv, K);
	uint8_t K_w1[sizeof(K) / 2] = { 0, };
	uint8_t K_w2[sizeof(K) / 2] = { 0, };
	copy(K, K + sizeof(K) / 2, K_w1);
	copy(K + sizeof(K) / 2, K + sizeof(K), K_w2);

	// STc,c<-CT[w]
	STAndC STAndC_Search;
	auto it = CT.find(w);
	if (it == CT.end()) {
		BIGNUM *z = BN_new();
		BN_zero(z);
		return z;
	}
	else {
		STAndC_Search = CT[w];
	}

	// c to 0
	int c = STAndC_Search.c;
	uint8_t ST[AES_BLOCK_SIZE];
	memcpy(ST, STAndC_Search.STc, AES_BLOCK_SIZE);

	BIGNUM* es = BN_new();
	BN_zero(es);
	for (int i = c; i >= 0; i--) {
		// UT <- H1
		uint8_t UT[sizeof(ST)];
		Encrypt(ST, sizeof(ST), K_w1, iivv, UT);

		// (e,C) <- EDB[UT]
		string ut = byteToString(UT);

		auto it = this->EDB.find(ut);
		if (it == this->EDB.end())
			break;
		Ciphertext ct = this->EDB.at(ut);
		this->EDB.erase(this->EDB.find(ut));

		
		// es <- add(es + ei)
		BN_add(es, es, ct.e);
		BN_mod(es, es, n, ctx);

		// ST <- h2 xor C
		uint8_t h2[sizeof(ST)];
		Encrypt(ST, sizeof(ST), K_w1, iivv, h2);
		//ST = ct.stc_pre_enc;
		memcpy(ST, ct.stc_pre_enc, AES_BLOCK_SIZE);
		for (int j = 0; j < sizeof(ST); j++){
			ST[j] ^= h2[j];
		}
	}

	// EDB[UT] <- es
	Ciphertext ct;
	ct.e = es;
	uint8_t tmp[sizeof(ST)];
	Encrypt(ST, sizeof(ST), K_w1, iivv, tmp);
	this->EDB[byteToString(tmp)] =  ct;

	// Sumsk = Sumsk + ski mod n
	BIGNUM* SumBN = BN_new();
	BN_zero(SumBN);
	for (int i = 0; i <= c; i++) {
		uint8_t sk[AES_BLOCK_SIZE];
		Encrypt((uint8_t*) &i,
			sizeof(sk),
			K_w2, 
			iivv, 
			sk);

		BIGNUM* skBN = BN_new();
		unsigned long long value;
		memcpy(&value, sk, sizeof(int));
		BN_set_word(skBN, value);

		BN_mod(skBN, skBN, n, ctx);
		BN_add(SumBN, SumBN, skBN);
		BN_mod(SumBN, SumBN, n, ctx);
	}
	// bs <- Dec
	BN_add(es, es, n);
	BN_sub(es, es, SumBN);
	BN_mod(es, es, n, ctx);
	return es;
}

uint8_t* FB_DSSE::getBytes(string const& s)
{
	unsigned char* bytes;
	return (unsigned char*)s.c_str();
}

string FB_DSSE::byteToString(uint8_t b[])
{
	string s = "";
	for (int i = 0; i < AES_BLOCK_SIZE; i++)
	{
		s += (char)b[i];
	}
	return s;
}

/*
void FB_DSSE::byteToBN(uint8_t b[], BIGNUM* BIG)
{
	int len = AES_BLOCK_SIZE;

	BN_zero(BIG);

	for (int i = 0; i < len; i++)
	{
		BN_mul_word(BIG, sizeof(uint8_t));
		BN_add_word(BIG, (unsigned long long) b[i]);
	}
}
*/
