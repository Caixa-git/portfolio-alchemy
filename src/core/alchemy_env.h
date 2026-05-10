/*
 * alchemy_env.h — .env file loader
 *
 * Loads KEY=VALUE pairs from a .env file into environment variables
 * so getenv() works transparently afterwards.
 *
 * Search order:
 *   1. $ALCHEMY_DIR/.env   (default: ~/.alchemy/.env)
 *   2. ./alchemy/.env       (project root relative)
 *   3. ./.env               (CWD)
 */

#ifndef ALCHEMY_ENV_H
#define ALCHEMY_ENV_H

#include <stdbool.h>

/* ── API ─────────────────────────────────────── */

/* Load .env from default search paths.
 * Can be called multiple times — later loads do NOT override existing vars.
 * Returns number of variables loaded, or -1 on error. */
int alchemy_env_load(void);

/* Load .env from a specific path.
 * Returns number of variables loaded, or -1 if file not found. */
int alchemy_env_load_path(const char *path, bool overwrite);

#endif /* ALCHEMY_ENV_H */
