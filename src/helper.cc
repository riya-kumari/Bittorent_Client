#include <openssl/sha.h>


unsigned char* create_hash(char* str){
    unsigned char hash[SHA_DIGEST_LENGTH]; // == 20

    return SHA1(str, sizeof(str) - 1, hash);
}
