#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

unsigned long djb2(const char *str) {
    unsigned long hash = 5381;
    int c;

    while ((c = *str++)) {
        hash = ((hash << 5) + hash) + c; // hash * 33 + c
    }
    return hash;
}

unsigned long sdbm(const char *str) {
    unsigned long hash = 0;
    int c;

    while ((c = *str++)) {
        hash = c + (hash << 6) + (hash << 16) - hash;
    }
    return hash;
}

unsigned long jenkins(const char *str) {
    unsigned long hash = 0;
    int c;

    while ((c = *str++)) {
        hash += c;
        hash += (hash << 10);
        hash ^= (hash >> 6);
    }
    hash += (hash << 3);
    hash ^= (hash >> 11);
    hash += (hash << 15);
    return hash;
}

uint32_t murmur3(const char *key, uint32_t seed) {
    uint32_t len = strlen(key);
    uint32_t c1 = 0xcc9e2d51;
    uint32_t c2 = 0x1b873593;
    uint32_t r1 = 15;
    uint32_t r2 = 13;
    uint32_t m = 5;
    uint32_t n = 0xe6546b64;

    uint32_t hash = seed;

    for (uint32_t i = 0; i < len; ++i) {
        uint32_t k = key[i];
        k *= c1;
        k = (k << r1) | (k >> (32 - r1));
        k *= c2;

        hash ^= k;
        hash = (hash << r2) | (hash >> (32 - r2));
        hash = hash * m + n;
    }

    hash ^= len;
    hash ^= (hash >> 16);
    hash *= 0x85ebca6b;
    hash ^= (hash >> 13);
    hash *= 0xc2b2ae35;
    hash ^= (hash >> 16);

    return hash;
}

uint32_t md5_k[] = {
    0xd76aa478, 0xe8c7b756, 0x242070db, 0xc1bdceee,
    0xf57c0faf, 0x4787c62a, 0xa8304613, 0xfd469501,
    0x698098d8, 0x8b44f7af, 0xffff5bb1, 0x895cd7be,
    0x6b901122, 0xfd987193, 0xa679438e, 0x49b40821,
    0xf61e2562, 0xc040b340, 0x265e5a51, 0xe9b6c7aa,
    0xd62f105d, 0x02441453, 0xd8a1e681, 0xe7d3fbc8,
    0x21e1cde6, 0xc33707d6, 0xf4d50d87, 0x455a14ed,
    0xa9e3e905, 0xfcefa3f8, 0x676f02d9, 0x8d2a4c8a,
    0xfffa3942, 0x8771f681, 0x6d9d6122, 0xfde5380c,
    0xa4beea44, 0x4bdecfa9, 0xf6bb4b60, 0xbebfbc70,
    0x289b7ec6, 0xeaa127fa, 0xd4ef3085, 0x04881d05,
    0xd9d4d039, 0xe6db99e5, 0x1fa27cf8, 0xc4ac5665,
    0xf4292244, 0x432aff97, 0xab9423a7, 0xfc93a039,
    0x655b59c3, 0x8f0ccc92, 0xffeff47d, 0x85845dd1,
    0x6fa87e4f, 0xfe2ce6e0, 0xa3014314, 0x4e0811a1,
    0xf7537e82, 0xbd3af235, 0x2ad7d2bb, 0xeb86d391
};

uint32_t md5_r[] = {
    7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22,
    5,  9, 14, 20, 5,  9, 14, 20, 5,  9, 14, 20, 5,  9, 14, 20,
    4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23,
    6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21
};

void md5_init(uint32_t *h0, uint32_t *h1, uint32_t *h2, uint32_t *h3) {
    *h0 = 0x67452301;
    *h1 = 0xefcdab89;
    *h2 = 0x98badcfe;
    *h3 = 0x10325476;
}

uint32_t md5_rotate_left(uint32_t x, uint32_t c) {
    return (x << c) | (x >> (32 - c));
}

void md5_process_block(const uint8_t *block, uint32_t *h0, uint32_t *h1, uint32_t *h2, uint32_t *h3) {
    uint32_t a = *h0, b = *h1, c = *h2, d = *h3, f, g;
    uint32_t w[16];

    for (int i = 0; i < 16; i++) {
        w[i] = (block[i * 4]) + (block[i * 4 + 1] << 8) + (block[i * 4 + 2] << 16) + (block[i * 4 + 3] << 24);
    }

    for (int i = 0; i < 64; i++) {
        if (i < 16) {
            f = (b & c) | ((~b) & d);
            g = i;
        } else if (i < 32) {
            f = (d & b) | ((~d) & c);
            g = (5 * i + 1) % 16;
        } else if (i < 48) {
            f = b ^ c ^ d;
            g = (3 * i + 5) % 16;
        } else {
            f = c ^ (b | (~d));
            g = (7 * i) % 16;
        }

        f = f + a + md5_k[i] + w[g];
        a = d;
        d = c;
        c = b;
        b = b + md5_rotate_left(f, md5_r[i]);
    }

    *h0 += a;
    *h1 += b;
    *h2 += c;
    *h3 += d;
}

uint32_t reverse_bytes(uint32_t val) {
    return ((val >> 24) & 0xFF) |
           ((val >> 8) & 0xFF00) |
           ((val << 8) & 0xFF0000) |
           ((val << 24) & 0xFF000000);
}

void md5(const char *str, uint32_t *out_h0, uint32_t *out_h1, uint32_t *out_h2, uint32_t *out_h3) {
    uint32_t h0, h1, h2, h3;
    md5_init(&h0, &h1, &h2, &h3);

    uint64_t len = strlen(str);
    uint64_t bit_len = len * 8;
    int new_len = ((((len + 8) / 64) + 1) * 64) - 8;
    uint8_t *msg = (uint8_t *)calloc(new_len + 64, 1);
    memcpy(msg, str, len);
    msg[len] = 128;
    memcpy(msg + new_len, &bit_len, 8);

    for (int offset = 0; offset < new_len; offset += 64) {
        md5_process_block(msg + offset, &h0, &h1, &h2, &h3);
    }

    free(msg);

    *out_h0 = reverse_bytes(h0);
    *out_h1 = reverse_bytes(h1);
    *out_h2 = reverse_bytes(h2);
    *out_h3 = reverse_bytes(h3);
}

void print_md5(uint32_t h0, uint32_t h1, uint32_t h2, uint32_t h3) {
    printf("%08x%08x%08x%08x\n", h0, h1, h2, h3);
}

void uint32_to_hex_string(uint32_t value, char *output) {
    snprintf(output, 9, "%08x", value);
}

void md5_to_string(uint32_t h0, uint32_t h1, uint32_t h2, uint32_t h3, char *output) {
    uint32_t values[] = {h0, h1, h2, h3};
    for (int i = 0; i < 4; i++) {
        snprintf(output + i * 8, 9, "%08x", values[i]);
    }
}

int main() {
    const char *test_str = "zxcv2121";
    uint32_t h0, h1, h2, h3;
    md5(test_str, &h0, &h1, &h2, &h3);
    char md5_str[32];
    md5_to_string(h0, h1, h2, h3, md5_str);
    printf("MD5: %s\n", md5_str);
    printf("DJB2: %lu\n", djb2(test_str));
    printf("SDBM: %lu\n", sdbm(test_str));
    printf("Jenkins: %lu\n", jenkins(test_str));
    printf("Murmur3: %u\n", murmur3(test_str, 42));
    return 0;
}
