/*
 * bcrypt.c — bcrypt password hashing: hash, verify, salt generation.
 *
 * Implements the "$2a$" variant that is compatible with Java's
 * BCryptPasswordEncoder and the OpenBSD reference implementation.
 *
 * Random bytes on Windows come from BCryptGenRandom() (the Windows
 * CNG API).  To avoid a header-name collision with our own bcrypt.h
 * we forward-declare the single function we need instead of
 * #include-ing the Windows <bcrypt.h> header.
 */

#define _CRT_SECURE_NO_WARNINGS
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdio.h>

/* ------------------------------------------------------------------ */
/*  Platform random                                                   */
/* ------------------------------------------------------------------ */

#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
/* Forward-declare BCryptGenRandom so we never #include <bcrypt.h>
   from the Windows SDK (its include-guard collides with ours).       */
typedef PVOID BCRYPT_ALG_HANDLE;
#define BCRYPT_USE_SYSTEM_PREFERRED_RNG 0x00000002
#ifdef __cplusplus
extern "C" {
#endif
__declspec(dllimport) LONG WINAPI BCryptGenRandom(
        BCRYPT_ALG_HANDLE hAlgorithm,
        PUCHAR            pbBuffer,
        ULONG             cbBuffer,
        ULONG             dwFlags);
#ifdef __cplusplus
}
#endif
#else
#include <fcntl.h>
#include <unistd.h>
#endif

/* ------------------------------------------------------------------ */
/*  Blowfish declarations (implemented in blowfish.c)                 */
/* ------------------------------------------------------------------ */

#ifndef BLF_CTX_DEFINED
#define BLF_CTX_DEFINED
typedef struct BlowfishContext {
    uint32_t S[4][256];
    uint32_t P[18];
} blf_ctx;
#endif

#ifndef BCRYPT_HASHSIZE
#define BCRYPT_HASHSIZE 64
#endif

extern void Blowfish_initstate(blf_ctx *);
extern void Blowfish_expandstate(blf_ctx *, const uint8_t *, uint16_t,
                                 const uint8_t *, uint16_t);
extern void Blowfish_expand0state(blf_ctx *, const uint8_t *, uint16_t);
extern void Blowfish_encipher(blf_ctx *, uint32_t *, uint32_t *);
extern void blf_enc(blf_ctx *, uint32_t *, uint16_t);

/* ------------------------------------------------------------------ */
/*  bcrypt custom base-64 encoding / decoding                         */
/* ------------------------------------------------------------------ */

static const char base64_code[] =
    "./ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";

static const int8_t index_64[128] = {
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,  0,  1,
    54, 55, 56, 57, 58, 59, 60, 61, 62, 63, -1, -1, -1, -1, -1, -1,
    -1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15, 16,
    17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, -1, -1, -1, -1, -1,
    -1, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42,
    43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, -1, -1, -1, -1, -1
};

/*
 * Safe character-to-index lookup — implemented as a function rather
 * than a macro to avoid double-evaluation of arguments that carry
 * side effects (e.g. *src++).
 */
static int char64(char c)
{
    unsigned char uc = (unsigned char)c;
    return uc > 127 ? -1 : (int)index_64[uc];
}

/* Encode 3 bytes → 4 base-64 characters. */
static void encode_base64(char *dst, const uint8_t *src, int len)
{
    int i = 0;
    uint8_t c1, c2;

    while (i < len) {
        c1 = src[i++];
        *dst++ = base64_code[(c1 >> 2)];
        c1 = (c1 & 0x03) << 4;
        if (i >= len) {
            *dst++ = base64_code[c1];
            break;
        }
        c2 = src[i++];
        c1 |= (c2 >> 4) & 0x0f;
        *dst++ = base64_code[c1];
        c1 = (c2 & 0x0f) << 2;
        if (i >= len) {
            *dst++ = base64_code[c1];
            break;
        }
        c2 = src[i++];
        c1 |= (c2 >> 6) & 0x03;
        *dst++ = base64_code[c1];
        *dst++ = base64_code[c2 & 0x3f];
    }
    *dst = '\0';
}

/* Decode base-64 characters → bytes (in-place-safe). */
static int decode_base64(uint8_t *dst, size_t dstlen, const char *src)
{
    int state = 0;
    size_t olen = 0;
    int c1, c2;

    while (*src != '\0' && olen < dstlen) {
        c1 = char64(*src);
        if (c1 == -1) break;
        src++;
        switch (state) {
        case 0:
            if (*src == '\0') return -1;
            c2 = char64(*src);
            if (c2 == -1) return -1;
            src++;
            dst[olen++] = (uint8_t)((c1 << 2) | ((c2 & 0x30) >> 4));
            state = 1;
            c1 = c2;
            break;
        case 1:
            c2 = char64(*src);
            if (c2 == -1) {
                dst[olen++] = (uint8_t)((c1 & 0x0f) << 4);
                return (int)olen;
            }
            src++;
            dst[olen++] = (uint8_t)(((c1 & 0x0f) << 4) | ((c2 & 0x3c) >> 2));
            state = 2;
            c1 = c2;
            break;
        case 2:
            c2 = char64(*src);
            if (c2 == -1) {
                dst[olen++] = (uint8_t)((c1 & 0x03) << 6);
            } else {
                src++;
                dst[olen++] = (uint8_t)(((c1 & 0x03) << 6) | c2);
            }
            state = 0;
            break;
        }
    }
    return (int)olen;
}

/* ------------------------------------------------------------------ */
/*  Simpler base-64 decoder specifically for the 22-char salt and     */
/*  the 31-char hash portions of a bcrypt string.                     */
/* ------------------------------------------------------------------ */

static void decode_bcrypt_base64(uint8_t *out, size_t outlen,
                                 const char *src)
{
    int c1, c2, c3, c4;
    size_t i = 0;

    while (i < outlen && *src) {
        c1 = char64(*src++);
        c2 = *src ? char64(*src++) : -1;
        if (c1 == -1 || c2 == -1) break;
        if (i < outlen) out[i++] = (uint8_t)((c1 << 2) | ((c2 & 0x30) >> 4));

        c3 = *src ? char64(*src++) : -1;
        if (c3 == -1) break;
        if (i < outlen) out[i++] = (uint8_t)(((c2 & 0x0f) << 4) | ((c3 & 0x3c) >> 2));

        c4 = *src ? char64(*src++) : -1;
        if (c4 == -1) break;
        if (i < outlen) out[i++] = (uint8_t)(((c3 & 0x03) << 6) | c4);
    }
}

/* ------------------------------------------------------------------ */
/*  bcrypt core: Expensive-key-schedule Blowfish setup + encrypt      */
/* ------------------------------------------------------------------ */

/* The magic string "OrpheanBeholderScryDoubt" as 6 × uint32_t (BE).  */
static const uint32_t ciphertext[6] = {
    0x4f727068, 0x65616e42, 0x65686f6c,
    0x64657253, 0x63727944, 0x6f756274
};

static int bcrypt_hashpass(const char *key, const char *salt,
                           char *encrypted, size_t enclen)
{
    blf_ctx state;
    uint32_t cdata[6];
    uint8_t csalt[16];
    uint32_t rounds;
    int n;
    size_t keylen;
    int logr;

    if (enclen < BCRYPT_HASHSIZE)
        return -1;

    /* -- Parse the salt string ----------------------------------- */
    /* Expected format: $2a$NN$22-base64-chars                      */
    if (salt[0] != '$' || salt[1] != '2')
        return -1;

    /* Accept $2a$, $2b$, $2y$ — they are all treated identically   */
    n = 3; /* skip past the version character */
    if (salt[2] == '$')
        n = 2; /* $2$ — extremely rare, but handle it */
    else if (salt[3] != '$')
        return -1;
    else
        n = 3;

    /* Cost factor — two ASCII digits */
    if (salt[n + 1] < '0' || salt[n + 1] > '9')
        return -1;
    logr = (int)(salt[n + 1] - '0');
    n += 2;
    if (salt[n] != '$') {
        if (salt[n] < '0' || salt[n] > '9')
            return -1;
        logr = logr * 10 + (int)(salt[n] - '0');
        n++;
    }
    if (salt[n] != '$')
        return -1;
    n++;

    if (logr < 4 || logr > 31)
        return -1;

    rounds = 1U << logr;

    /* Decode the 22-char base-64 salt → 16 bytes */
    decode_bcrypt_base64(csalt, sizeof(csalt), salt + n);

    /* Key includes the trailing NUL byte (as per original spec).   */
    keylen = strlen(key) + 1;
    if (keylen > 72) keylen = 72;   /* Blowfish limit */

    /* -- Expensive key schedule ---------------------------------- */
    Blowfish_initstate(&state);
    Blowfish_expandstate(&state, csalt, 16,
                         (const uint8_t *)key, (uint16_t)keylen);
    {
        uint32_t k;
        for (k = 0; k < rounds; k++) {
            Blowfish_expand0state(&state,
                                  (const uint8_t *)key, (uint16_t)keylen);
            Blowfish_expand0state(&state, csalt, 16);
        }
    }

    /* -- Encrypt "OrpheanBeholderScryDoubt" 64 times --------------- */
    memcpy(cdata, ciphertext, sizeof(cdata));
    for (n = 0; n < 64; n++)
        blf_enc(&state, cdata, 3);          /* 3 × 64-bit blocks */

    /* -- Build the output string --------------------------------- */
    {
        /* Re-build the prefix exactly as it appeared in the salt.  */
        /* We always write "$2a$NN$" to stay compatible.            */
        int off;
        uint8_t hashbytes[24];  /* 6×4 = 24 */
        int i;

        /* Convert cdata to big-endian bytes */
        for (i = 0; i < 6; i++) {
            hashbytes[4 * i + 0] = (uint8_t)((cdata[i] >> 24) & 0xFF);
            hashbytes[4 * i + 1] = (uint8_t)((cdata[i] >> 16) & 0xFF);
            hashbytes[4 * i + 2] = (uint8_t)((cdata[i] >>  8) & 0xFF);
            hashbytes[4 * i + 3] = (uint8_t)((cdata[i]      ) & 0xFF);
        }

        off = (int)(salt - salt); /* 0 */
        /* Copy the salt prefix (up to and including the 22 base64 chars) */
        {
            const char *sp = salt;
            int slen = 0;
            /* Walk past "$2X$NN$" */
            while (*sp && slen < 7 + 22) { slen++; sp++; }
            if (slen > (int)enclen - 32) return -1;
            memcpy(encrypted, salt, (size_t)slen);
            off = slen;
        }

        /* Encode 23 bytes of hash (bcrypt only encodes 23 of 24).  */
        encode_base64(encrypted + off, hashbytes, 23);
        encrypted[off + 31] = '\0'; /* 23 bytes → 31 base-64 chars  */
    }

    /* Clear sensitive state */
    memset(&state, 0, sizeof(state));
    memset(cdata,  0, sizeof(cdata));

    return 0;
}

/* ------------------------------------------------------------------ */
/*  Public API                                                        */
/* ------------------------------------------------------------------ */

int bcrypt_hashpw(const char *password, const char *salt,
                  char *hash, size_t hashlen)
{
    return bcrypt_hashpass(password, salt, hash, hashlen);
}

int bcrypt_checkpw(const char *password, const char *hash)
{
    char computed[BCRYPT_HASHSIZE];
    int r;

    r = bcrypt_hashpass(password, hash, computed, sizeof(computed));
    if (r != 0)
        return -1;

    /* Constant-time comparison to avoid timing side-channels.       */
    {
        size_t i, len = strlen(hash);
        uint8_t diff = 0;
        if (len != strlen(computed))
            return -1;
        for (i = 0; i < len; i++)
            diff |= (uint8_t)(hash[i] ^ computed[i]);
        return diff ? -1 : 0;
    }
}

int bcrypt_gensalt(int rounds, char *salt, size_t saltlen)
{
    uint8_t raw[16];
    char encoded[25]; /* 22 chars + NUL */

    if (saltlen < 30)
        return -1;
    if (rounds < 4)  rounds = 4;
    if (rounds > 31) rounds = 31;

    /* Obtain 16 cryptographically-secure random bytes. */
#ifdef _WIN32
    {
        LONG status = BCryptGenRandom(
            NULL, raw, (ULONG)sizeof(raw),
            BCRYPT_USE_SYSTEM_PREFERRED_RNG);
        if (status != 0)    /* NTSTATUS: 0 == STATUS_SUCCESS */
            return -1;
    }
#else
    {
        int fd = open("/dev/urandom", O_RDONLY);
        if (fd < 0) return -1;
        if (read(fd, raw, sizeof(raw)) != sizeof(raw)) {
            close(fd);
            return -1;
        }
        close(fd);
    }
#endif

    encode_base64(encoded, raw, 16);
    encoded[22] = '\0';   /* bcrypt salt is exactly 22 base-64 chars */

    sprintf(salt, "$2a$%02d$%s", rounds, encoded);
    return 0;
}
