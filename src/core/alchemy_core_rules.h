/*
 * alchemy_core_rules.h — Alchemy Core Engineering Rules
 *
 * ═══════════════════════════════════════════════════════════════════
 * IMMUTABLE RULES — Highest Priority, Do Not Modify
 * ═══════════════════════════════════════════════════════════════════
 *
 * Source: github.com/addyosmani/agent-skills
 * Adaptation: C99 + HTML format, integrated into Alchemy identity layer.
 *
 * These rules govern ALL development decisions in Alchemy.
 * They are the "production-grade engineering discipline" layer —
 * above all other prompts, unchangeable by anyone including the user.
 *
 * ─── Why this exists ───
 * AI coding agents default to the shortest path — skipping specs,
 * tests, security reviews. These rules enforce the same discipline
 * senior engineers bring to production code. "Seems right" is never
 * sufficient — there must be evidence.
 *
 * ─── 6 Phases ───
 * DEFINE → PLAN → BUILD → VERIFY → REVIEW → SHIP
 *
 * ─── 3 Immutable Behaviors ───
 * 1. Surface Assumptions — before acting, state what you assume
 * 2. Manage Confusion — stop on ambiguity, never guess silently
 * 3. Verify, Don't Assume — evidence required, never "seems right"
 */

#ifndef ALCHEMY_CORE_RULES_H
#define ALCHEMY_CORE_RULES_H

#ifdef __cplusplus
extern "C" {
#endif

/* ── phases ────────────────────────────────────────────────────── */

#define CORE_RULES_PHASES 6
#define CORE_RULES_MAX    4096

/**
 * core_rules_prompt — Generate the full rules system prompt in HTML.
 *
 * The output is a self-contained HTML <section> block designed to be
 * injected into the identity dashboard (Level 2.5).
 *
 * Rules are organized by phase with data-* attributes for LLM parsing.
 *
 * Returns bytes written to out, or -1 on error.
 */
int core_rules_prompt(char *out, int max_out);

/**
 * core_rules_json — Generate rules as JSON-LD for LLM consumption.
 * Compact machine-readable version of all rules.
 */
int core_rules_json(char *out, int max_out);

#ifdef __cplusplus
}
#endif
#endif /* ALCHEMY_CORE_RULES_H */
