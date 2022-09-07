#ifndef TIMESCALEDB_DP_OPTIMIZATION_CACHES
#define TIMESCALEDB_DP_OPTIMIZATION_CACHES

#include "export.h"
#include "cache.h"
#include "export.h"
#include <catalog/namespace.h>

#define KEY_SIZE 200
#define NUM_QUERIES 100
#define NUM_CACHE_KEYS 10000

typedef struct
{
	int64 entries[NUM_QUERIES];
	int counter;
	char *cache_keys[NUM_CACHE_KEYS];
} CachesMap;

typedef struct
{
	char *entries[NUM_CACHE_KEYS];
	int counter;
} CachesKeys;

typedef struct dp_optimization_caches
{
	Cache* dp_optimization_results_cache;
    Cache* dp_optimization_distances_cache;
} DPOptimizationCaches;

typedef struct blocks
{
	int chunk_id_start;
	int chunk_id_end;
} Blocks;

extern TSDLLEXPORT char *get_key(const Blocks blocks);

extern TSDLLEXPORT DPOptimizationCaches dp_optimization_caches_add_get(int64 queryId);

extern void _dp_optimization_caches_init(void);

extern void _dp_optimization_caches_fini(void);

#endif /* TIMESCALEDB_DP_OPTIMIZATION_CACHES */
