/*
 * alchemy_skill_evolve.h — Alchemy Skill Evolution Engine
 *
 * 5-axis skill assessment (0-10) + Dreyfus level mapping + auto-evolution.
 * 
 * Reference frameworks:
 *   - Dreyfus Model of Skill Acquisition (5+1 stages)
 *   - CMMI Capability Maturity Model (5 levels)
 *   - Rasch Model (item response theory, logit scale)
 *   - Learning Curve: S(t) = 10 * (1 - e^(-λ*N))
 */

#ifndef ALCHEMY_SKILL_EVOLVE_H
#define ALCHEMY_SKILL_EVOLVE_H

#include <stdbool.h>
#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ─── 5-Axis Scores (0.0 - 2.0 each → 0-10 total) ─── */
typedef struct {
    float accuracy;      /* 정확도: intended vs actual outcome */
    float efficiency;    /* 효율성: time/tokens/resources optimized */
    float consistency;   /* 일관성: same input → same output rate */
    float adaptability;  /* 적응성: edge case handling */
    float recovery;      /* 회복력: graceful error handling */
} skill_scores_t;

/* ─── Evolution Record (one entry per evolution step) ─── */
typedef struct {
    char timestamp[32];
    char description[256];    /* what changed */
    float old_score;          /* previous total score */
    float new_score;          /* new total score */
    char trigger[64];         /* what triggered this evolution */
} skill_evolve_entry_t;

#define SKILL_EVOLVE_LOG_MAX 32

/* ─── Dreyfus Levels ─── */
typedef enum {
    DREYFUS_NOVICE           = 1,  /* 0.0 - 2.0 */
    DREYFUS_ADVANCED_BEGINNER = 2,  /* 2.1 - 4.0 */
    DREYFUS_COMPETENT        = 3,  /* 4.1 - 6.0 */
    DREYFUS_PROFICIENT       = 4,  /* 6.1 - 8.0 */
    DREYFUS_EXPERT           = 5,  /* 8.1 - 9.5 */
    DREYFUS_MASTER           = 6   /* 9.6 - 10.0 */
} dreyfus_level_t;

/* ─── Skill Metadata (embedded in .skill file frontmatter) ─── */
typedef struct {
    char      name[64];
    char      description[256];
    int       dreyfus_level;        /* 1-6 */
    float     total_score;          /* 0.0 - 10.0 */
    skill_scores_t scores;          /* per-axis */
    int       use_count;            /* cumulative uses */
    int       success_count;        /* successful uses */
    int       fail_count;           /* failed uses */
    char      version[16];          /* "0.0.1" etc */
    int       version_patch;        /* patch increment */
    int       version_minor;        /* minor increment */
    int       version_major;        /* major increment */
    bool      is_master;            /* true when score >= 9.6 */
    skill_evolve_entry_t log[SKILL_EVOLVE_LOG_MAX];
    int       log_count;
    time_t    last_used;
    time_t    created_at;
} skill_meta_t;

/* ═══════════════════════════════════════════════════════════════
 * API
 * ═══════════════════════════════════════════════════════════════ */

/* Initialize skill metadata with defaults */
void skill_meta_init(skill_meta_t *meta, const char *name, const char *desc);

/* Calculate total score from 5-axis scores */
float skill_calc_total(const skill_scores_t *scores);

/* Map total score (0-10) to Dreyfus level (1-6) */
int  skill_score_to_level(float total);

/* Map Dreyfus level + patch to version string "X.Y.Z" */
void skill_level_to_version(int level, int patch, char *out, int max_out);

/* Parse version string to level (e.g. "0.1.0" → 10) */
int  skill_version_to_level(const char *version);

/* 
 * Record one skill usage and update scores.
 * Returns new total score, or -1 on error.
 */
float skill_record_use(skill_meta_t *meta, bool success, float time_seconds);

/*
 * Evolve: apply one improvement to the skill.
 * Updates scores based on success/failure pattern.
 * Returns 0 on success, -1 if already master.
 */
int skill_evolve(skill_meta_t *meta, const char *trigger_desc);

/*
 * Check if skill is Master (total >= 9.6 or level == 6)
 */
bool skill_is_master(const skill_meta_t *meta);

/*
 * Generate evolution log as JSON string (for dashboard)
 */
int skill_evolve_log_json(const skill_meta_t *meta, char *out, int max_out);

/*
 * Generate skill scores as JSON
 */
int skill_scores_json(const skill_scores_t *scores, char *out, int max_out);

/* Return Dreyfus level name string */
const char *skill_level_name(int level);

#ifdef __cplusplus
}
#endif

#endif /* ALCHEMY_SKILL_EVOLVE_H */
