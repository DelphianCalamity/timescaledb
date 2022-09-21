/*
 * This file and its contents are licensed under the Apache License 2.0.
 * Please see the included NOTICE for copyright information and
 * LICENSE-APACHE for a copy of the license.
 */
#ifndef TIMESCALEDB_DP_OPTIMIZATION_DISTANCES_CACHE_H
#define TIMESCALEDB_DP_OPTIMIZATION_DISTANCES_CACHE_H

#include <postgres.h>
#include <catalog/namespace.h>

#include "dp_optimization_caches.h"

Cache *
dp_optimization_distances_cache_create(void);

extern TSDLLEXPORT List *
	ts_dp_optimization_distances_cache_get_entry(Cache *cache,  char *key, bool *found);

extern TSDLLEXPORT void
	ts_dp_optimization_distances_cache_write(Cache *distances_cache, Cache *results_cache, char *key, float result);

extern TSDLLEXPORT List *
	ts_dp_optimization_distances_cache_write_entry(Cache *cache, char *key, bool *found);

// extern TSDLLEXPORT List *get_substitutes(Blocks blocks, float k);

extern void ts_dp_optimization_distances_cache_invalidate_callback(Cache **cache);

extern TSDLLEXPORT Cache* dp_optimization_distances_cache_get(int i);

#endif /* TIMESCALEDB_DP_OPTIMIZATION_DISTANCES_CACHE_H */
