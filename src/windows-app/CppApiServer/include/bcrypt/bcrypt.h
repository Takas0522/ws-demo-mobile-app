/*
 * bcrypt.h — Minimal bcrypt password hashing API.
 *
 * Public-domain-style header.  Used by the C++ API Server to verify
 * passwords that were originally hashed by Java's BCryptPasswordEncoder
 * (prefix "$2a$").
 */

#ifndef _BCRYPT_H_
#define _BCRYPT_H_

#include <stddef.h>   /* size_t */

#define BCRYPT_HASHSIZE 64

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Hash `password` with the given `salt` string (e.g. "$2a$10$…22-char…").
 * The resulting hash is written to `hash` (must be at least BCRYPT_HASHSIZE).
 * Returns 0 on success, -1 on error.
 */
int bcrypt_hashpw(const char *password, const char *salt, char *hash, size_t hashlen);

/*
 * Verify `password` against a stored `hash`.
 * Returns  0 on match, -1 on mismatch or error.
 */
int bcrypt_checkpw(const char *password, const char *hash);

/*
 * Generate a random salt string for `rounds` cost factor.
 * `salt` must be at least 30 bytes.  Returns 0 on success, -1 on error.
 */
int bcrypt_gensalt(int rounds, char *salt, size_t saltlen);

#ifdef __cplusplus
}
#endif

#endif /* _BCRYPT_H_ */
