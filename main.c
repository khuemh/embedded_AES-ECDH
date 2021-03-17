#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "ecdh.h"
#include "pRNG.h"
#include "aes.h"
#include "hexPrint.h"


/***************************************************************
 *  GLOBAL VARIABLES DECLARATION
***************************************************************/
prng_t prng_ctx;


/***************************************************************
 *  FUNCTIONS DECLARATION
***************************************************************/
static void ecdh_demo(void);
static void aes_ecb_demo(void);
static void protocol_demo(void);


/***************************************************************
 *  MAIN FUNCTION
***************************************************************/
int main(int argc, char *argv[])
{
  printf("\n------------- TESTING ECDH -------------\n\n");
  int i;
  int ncycles = 1;

  if (argc > 1)
  {
    ncycles = atoi(argv[1]);
  }

  for (i = 0; i < ncycles; ++i)
  {
    ecdh_demo();
  }

  printf("\n------------- TESTING AES128 -------------\n\n");
  aes_ecb_demo();

  printf("\n ------------- TESTING WSNs Protocol -------------\n\n");
  protocol_demo();
  return 0;
  
}


/***************************************************************
 *  SUB-FUNCTIONs
***************************************************************/

/* DEMO of WSNs protocol */
static void protocol_demo(void)
{
  //--- KEY EXCHANGE ---//
      //--- INIT ---//
  static uint8_t pubKey_A[ECC_PUB_KEY_SIZE];
  static uint8_t prvKey_A[ECC_PRV_KEY_SIZE];
  static uint8_t secret_A[ECC_PUB_KEY_SIZE];
  static uint8_t pubKey_B[ECC_PUB_KEY_SIZE];
  static uint8_t prvKey_B[ECC_PRV_KEY_SIZE];
  static uint8_t secret_B[ECC_PUB_KEY_SIZE];
  uint32_t i;

  /* Initialize and seed random number generator */
  static int initialized = 0;
  if (!initialized)
  {
    prng_init(&prng_ctx, (0xbad ^ 0xc0ffee ^ 42) | 0xcafebabe | 666);
    initialized = 1;
  }

      //--- ALICE SIDE ---//
  for (i = 0; i < ECC_PRV_KEY_SIZE; ++i)
  {
    prvKey_A[i] = prng_next(&prng_ctx);
  }
  assert(ecdh_generate_keys(pubKey_A, prvKey_A));

  printf("[A Public KEY]\t");
  printHex_str(pubKey_A, 48); printf("\n");
  printf("[A Private KEY]\t");
  printHex_str(prvKey_A, 24); printf("\n");

      //--- BOB SIDE ---//
  for (i = 0; i < ECC_PRV_KEY_SIZE; ++i)
  {
    prvKey_B[i] = prng_next(&prng_ctx);
  }
  assert(ecdh_generate_keys(pubKey_B, prvKey_B));

  printf("\n[B Public KEY]\t");
  printHex_str(pubKey_B, 48); printf("\n");
  printf("[B Private KEY]\t");
  printHex_str(prvKey_B, 24); printf("\n");

      //--- COMMON CALCULATION ---//
  /* Alice calculation */
  assert(ecdh_shared_secret(prvKey_A, pubKey_B, secret_A));

  printf("\n[A Secret]\t");
  printHex_str(secret_A, 48); printf("\n");

  /* Bob calculation */
  assert(ecdh_shared_secret(prvKey_B, pubKey_A, secret_B));

  printf("[B Secret]\t");
  printHex_str(secret_B, 48); printf("\n\n");

  //--- TRANSFER DATA ---//
  uint8_t symKey[16];
  uint8_t plain_text[32] = {  (uint8_t) 0x6b, (uint8_t) 0xc1, (uint8_t) 0xbe, (uint8_t) 0xe2, (uint8_t) 0x2e, (uint8_t) 0x40, (uint8_t) 0x9f, (uint8_t) 0x96, (uint8_t) 0xe9, (uint8_t) 0x3d, (uint8_t) 0x7e, (uint8_t) 0x11, (uint8_t) 0x73, (uint8_t) 0x93, (uint8_t) 0x17, (uint8_t) 0x2a,
                              (uint8_t) 0xae, (uint8_t) 0x2d, (uint8_t) 0x8a, (uint8_t) 0x57, (uint8_t) 0x1e, (uint8_t) 0x03, (uint8_t) 0xac, (uint8_t) 0x9c, (uint8_t) 0x9e, (uint8_t) 0xb7, (uint8_t) 0x6f, (uint8_t) 0xac, (uint8_t) 0x45, (uint8_t) 0xaf, (uint8_t) 0x8e, (uint8_t) 0x51 };
  uint8_t cipher_text[32];
  uint8_t decrypted_plain_text[32];
  for (uint8_t i = 0; i < 16; i++)
  {
    symKey[i] = secret_B[i];
  }

  printf("[Symmetry KEY]\t");
  printHex_str(symKey, 16);
  printf("\n");
  printf("[Plain Text]\t");
  printHex_str(plain_text, 32);
  printf("\n");

  struct AES_ctx ctx;
  AES_init_ctx(&ctx, symKey);

  printf("\nEncrypting...\n");
  printf("[Cipher Text]\t");
  memcpy(cipher_text, plain_text, 32);
  for (int i = 0; i < 2; ++i)
  {
    AES_ECB_encrypt(&ctx, cipher_text + (i * 16));
    printHex_str(cipher_text + (i * 16), 16);
  }
  printf("\n");

  printf("\nDecrypting...\n");
  printf("[Plain Text]\t");
  memcpy(decrypted_plain_text, cipher_text, 32);
  for (int i = 0; i < 2; ++i)
  {
    AES_ECB_decrypt(&ctx, decrypted_plain_text + (i * 16));
    printHex_str(decrypted_plain_text + (i * 16), 16);
  }
  printf("\n");
}


/* DEMO Elliptic Curve Diffie-Hellman Key Exchange - ECDH */
static void ecdh_demo(void)
{
  static uint8_t puba[ECC_PUB_KEY_SIZE];
  static uint8_t prva[ECC_PRV_KEY_SIZE];
  static uint8_t seca[ECC_PUB_KEY_SIZE];
  static uint8_t pubb[ECC_PUB_KEY_SIZE];
  static uint8_t prvb[ECC_PRV_KEY_SIZE];
  static uint8_t secb[ECC_PUB_KEY_SIZE];
  uint32_t i;

  /* 0. Initialize and seed random number generator */
  static int initialized = 0;
  if (!initialized)
  {
    prng_init(&prng_ctx, (0xbad ^ 0xc0ffee ^ 42) | 0xcafebabe | 666);
    initialized = 1;
  }

  /* 1. Alice picks a (secret) random natural number 'a', calculates P = a * g and sends P to Bob. */
  for (i = 0; i < ECC_PRV_KEY_SIZE; ++i)
  {
    prva[i] = prng_next(&prng_ctx);
  }
  assert(ecdh_generate_keys(puba, prva));
  // DEBUG PRINT !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  printf("[A Public KEY]\t");
  printHex_str(puba, 48); printf("\n");
  printf("[A Private KEY]\t");
  printHex_str(prva, 24); printf("\n");

  /* 2. Bob picks a (secret) random natural number 'b', calculates Q = b * g and sends Q to Alice. */
  for (i = 0; i < ECC_PRV_KEY_SIZE; ++i)
  {
    prvb[i] = prng_next(&prng_ctx);
  }
  assert(ecdh_generate_keys(pubb, prvb));
  // DEBUG PRINT !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  printf("\n[B Public KEY]\t");
  printHex_str(pubb, 48); printf("\n");
  printf("[B Private KEY]\t");
  printHex_str(prvb, 24); printf("\n");

  /* 3. Alice calculates S = a * Q = a * (b * g). */
  assert(ecdh_shared_secret(prva, pubb, seca));
  // DEBUG PRINT !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  printf("\n[A Secret]\t");
  printHex_str(seca, 48); printf("\n");

  /* 4. Bob calculates T = b * P = b * (a * g). */
  assert(ecdh_shared_secret(prvb, puba, secb));
  // DEBUG PRINT !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  printf("\n[B Secret]\t");
  printHex_str(secb, 48); printf("\n");

  /* 5. Assert equality, i.e. check that both parties calculated the same value. */
  for (i = 0; i < ECC_PUB_KEY_SIZE; ++i)
  {
    assert(seca[i] == secb[i]);
  }
  // printf("\n[Key Exchange Successfully!]\n\n");
}

/* DEMO AES128 ECB mode*/
static void aes_ecb_demo(void)
{
  // uint8_t i;
  // 128bit key
  uint8_t key[16] =        {  (uint8_t) 0x2b, (uint8_t) 0x7e, (uint8_t) 0x15, (uint8_t) 0x16, (uint8_t) 0x28, (uint8_t) 0xae, (uint8_t) 0xd2, (uint8_t) 0xa6, (uint8_t) 0xab, (uint8_t) 0xf7, (uint8_t) 0x15, (uint8_t) 0x88, (uint8_t) 0x09, (uint8_t) 0xcf, (uint8_t) 0x4f, (uint8_t) 0x3c };
  uint8_t plain_text[32] = {  (uint8_t) 0x6b, (uint8_t) 0xc1, (uint8_t) 0xbe, (uint8_t) 0xe2, (uint8_t) 0x2e, (uint8_t) 0x40, (uint8_t) 0x9f, (uint8_t) 0x96, (uint8_t) 0xe9, (uint8_t) 0x3d, (uint8_t) 0x7e, (uint8_t) 0x11, (uint8_t) 0x73, (uint8_t) 0x93, (uint8_t) 0x17, (uint8_t) 0x2a,
                              (uint8_t) 0xae, (uint8_t) 0x2d, (uint8_t) 0x8a, (uint8_t) 0x57, (uint8_t) 0x1e, (uint8_t) 0x03, (uint8_t) 0xac, (uint8_t) 0x9c, (uint8_t) 0x9e, (uint8_t) 0xb7, (uint8_t) 0x6f, (uint8_t) 0xac, (uint8_t) 0x45, (uint8_t) 0xaf, (uint8_t) 0x8e, (uint8_t) 0x51 };
  uint8_t cipher_text[32];
  uint8_t decrypted_plain_text[32];
  printf("[KEY]\t\t");
  printHex_str(key, 16);
  printf("\n");
  printf("[Plain Text]\t");
  printHex_str(plain_text, 32);
  printf("\n");

  struct AES_ctx ctx;
  AES_init_ctx(&ctx, key);

  printf("\nEncryting...\n");
  printf("[Cipher Text]\t");
  memcpy(cipher_text, plain_text, 32);
  for (int i = 0; i < 2; ++i)
  {
    AES_ECB_encrypt(&ctx, cipher_text + (i * 16));
    printHex_str(cipher_text + (i * 16), 16);
  }
  printf("\n");

  printf("\nDecryting...\n");
  printf("[Plain Text]\t");
  memcpy(decrypted_plain_text, cipher_text, 32);
  for (int i = 0; i < 2; ++i)
  {
    AES_ECB_decrypt(&ctx, decrypted_plain_text + (i * 16));
    printHex_str(decrypted_plain_text + (i * 16), 16);
  }
  printf("\n"); 
}
