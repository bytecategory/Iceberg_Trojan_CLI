#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <openssl/des.h>
#include <zlib.h>
void reverse_bytes(uint8_t *data, size_t len) {
    for (size_t i = 0; i < len; i++) {
        uint8_t b = data[i];
        uint8_t reversed = 0;
        for (int j = 0; j < 8; j++) {
            reversed |= ((b >> j) & 1) << (7 - j);
        }
        data[i] = reversed;
    }
}
void get_new_key(const uint8_t *key, size_t key_len, uint8_t *new_key) {
    memset(new_key, 0, 8);
    for (size_t i = 0; i < key_len; i++) {
        new_key[i % 8] ^= key[i];
    }
}
size_t padding(const uint8_t *data, size_t len, uint8_t **padded) {
    size_t padded_len = ((len + 7) / 8) * 8;
    *padded = (uint8_t *)calloc(padded_len, 1);
    memcpy(*padded, data, len);
    return padded_len;
}
void append_len(const uint8_t *data, size_t len, uint8_t **result, size_t *result_len) {
    *result_len = len + 4;
    *result = (uint8_t *)malloc(*result_len);
    uint32_t length = (uint32_t)len;
    memcpy(*result, &length, 4);  
    memcpy(*result + 4, data, len);
}
void remove_len(const uint8_t *data, size_t len, uint8_t **result, size_t *result_len) {
    if (len < 4) {
        *result = NULL;
        *result_len = 0;
        return;
    }
    uint32_t orig_len;
    memcpy(&orig_len, data, 4);
    *result_len = orig_len;
    *result = (uint8_t *)malloc(*result_len);
    memcpy(*result, data + 4, *result_len);
}
uint8_t* e_des_encrypt(const uint8_t *plain, size_t plain_len, const uint8_t *key, size_t key_len, size_t *cipher_len) {
    uint8_t new_key[8];
    get_new_key(key, key_len, new_key);
    reverse_bytes(new_key, 8);
    uint8_t *with_len;
    size_t with_len_size;
    append_len(plain, plain_len, &with_len, &with_len_size);
    uint8_t *padded;
    size_t padded_len = padding(with_len, with_len_size, &padded);
    free(with_len);
    DES_key_schedule schedule;
    DES_set_key_unchecked((const_DES_cblock *)new_key, &schedule);
    uint8_t *cipher = (uint8_t *)malloc(padded_len);
    for (size_t i = 0; i < padded_len; i += 8) {
        DES_ecb_encrypt((const_DES_cblock *)(padded + i), 
                        (DES_cblock *)(cipher + i), 
                        &schedule, 
                        DES_ENCRYPT);
    }
    free(padded);
    *cipher_len = padded_len;
    return cipher;
}
uint8_t* e_des_decrypt(const uint8_t *cipher, size_t cipher_len, const uint8_t *key, size_t key_len, size_t *plain_len) {
    uint8_t new_key[8];
    get_new_key(key, key_len, new_key);
    reverse_bytes(new_key, 8);
    DES_key_schedule schedule;
    DES_set_key_unchecked((const_DES_cblock *)new_key, &schedule);
    uint8_t *decrypted = (uint8_t *)malloc(cipher_len);
    for (size_t i = 0; i < cipher_len; i += 8) {
        DES_ecb_encrypt((const_DES_cblock *)(cipher + i), 
                        (DES_cblock *)(decrypted + i), 
                        &schedule, 
                        DES_DECRYPT);
    }   
    uint8_t *result;
    remove_len(decrypted, cipher_len, &result, plain_len);
    free(decrypted);
    return result;
}
int main(int argc, char *argv[]) {
    uint8_t plain[] = {
        0x4d, 0x69, 0x63, 0x72, 0x6f, 0x73, 0x6f, 0x66, 0x74, 0x57, 0x65, 0x62,
        0xa4, 0xf1, 0xa8, 0xe4, 0xa4, 0xf1, 0x30, 0xa4, 0xf1, 0xa8, 0xe4, 0xa4,
        0xf1, 0x30, 0xa4, 0xf1, 0xa8, 0xe4, 0xa4, 0xf1, 0x30, 0xa4, 0xf1, 0xa8,
        0xe4, 0xa4, 0xf1, 0x30, 0xa4, 0xf1, 0xa8, 0xe4, 0xa4, 0xf1, 0x30, 0xa4,
        0xf1, 0xa8, 0xe4, 0xa4, 0xf1, 0x30, 0xa4, 0xf1, 0xa8, 0xe4, 0xa4, 0xf1,
        0x30, 0xa4, 0xf1, 0xa8, 0xe4, 0xa4, 0xf1, 0x30, 0xa4, 0xf1, 0xa8, 0xe4,
        0xa4, 0xf1, 0x30, 0xa4, 0xf1, 0xa8, 0xe4, 0xa4, 0xf1, 0x30
    };
    size_t plain_len = sizeof(plain);
    uint8_t key[] = "Microsoft";
    size_t key_len = strlen((char *)key);
    size_t cipher_len;
    uint8_t *cipher = e_des_encrypt(plain, plain_len, key, key_len, &cipher_len);
    uLongf compressed_len = compressBound(cipher_len);
    uint8_t *compressed = (uint8_t *)malloc(compressed_len);
    compress(compressed, &compressed_len, cipher, cipher_len);
    free(cipher);
    uint8_t header[] = {0x0d, 0x0f, 0x3e, 0x03, 0x08, 0x01, 0x00, 0x00};
    size_t data_len = sizeof(header) + compressed_len;
    uint8_t *data = (uint8_t *)malloc(data_len);
    memcpy(data, header, sizeof(header));
    memcpy(data + sizeof(header), compressed, compressed_len);
    free(compressed);
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8080);
    inet_pton(AF_INET, argv[1], &server_addr.sin_addr);
    connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr));
    send(sock, data, data_len, 0);
    free(data);
    uint8_t buffer[1024];
    while (1) {
        ssize_t received = recv(sock, buffer, sizeof(buffer), 0);
        if (received <= 0) {
            break;
        }
        int zlib_start = -1;
        for (ssize_t i = 0; i < received - 1; i++) {
            if (buffer[i] == 0x78 && buffer[i+1] == 0x9c) {
                zlib_start = i;
                break;
            }
        }
        if (zlib_start != -1) {  
            uLongf decompressed_len = 10240;  
            uint8_t *decompressed = (uint8_t *)malloc(decompressed_len);
            if (uncompress(decompressed, &decompressed_len, 
                          buffer + zlib_start, received - zlib_start) == Z_OK) {
                size_t plaintext_len;
                uint8_t *plaintext = e_des_decrypt(decompressed, decompressed_len, 
                                                   key, key_len, &plaintext_len);
                if (plaintext) { 
                    fwrite(plaintext, 1, plaintext_len, stdout);
                    printf("\n");
                    free(plaintext);
                }
            }
            free(decompressed);
        }
    }
    close(sock);
    return 0;
}
