#ifndef TIMESCALEDB_DP_OPTIMIZATION_CACHES
#define TIMESCALEDB_DP_OPTIMIZATION_CACHES

#include "export.h"
#include "cache.h"
#include "export.h"

#define KEY_SIZE 200
#define NUM_QUERIES 100

typedef struct
{
	int64 entries[NUM_QUERIES];
	int counter;
} CachesMap;

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

extern TSDLLEXPORT DPOptimizationCaches _dp_optimization_caches_add_get(int64 queryId);

extern void _dp_optimization_caches_init(void);

extern void _dp_optimization_caches_fini(void);

#endif /* TIMESCALEDB_DP_OPTIMIZATION_CACHES */
