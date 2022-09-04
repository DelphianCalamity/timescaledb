/*
 * This file and its contents are licensed under the Apache License 2.0.
 * Please see the included NOTICE for copyright information and
 * LICENSE-APACHE for a copy of the license.
 */
#ifndef TIMESCALEDB_DP_OPTIMIZATION_RESULTS_CACHE_H
#define TIMESCALEDB_DP_OPTIMIZATION_RESULTS_CACHE_H

#include <postgres.h>
#include "dp_optimization_caches.h"

Cache* dp_optimization_results_cache_create(void);

extern TSDLLEXPORT void *
	ts_dp_optimization_results_cache_get_entry(Cache *cache, const Blocks blocks, bool *found);
extern TSDLLEXPORT void
	ts_dp_optimization_results_cache_write_entry(Cache *cache, const Blocks blocks, float result, bool *found);

extern void ts_dp_optimization_results_cache_invalidate_callback(Cache **cache);

extern TSDLLEXPORT Cache* dp_optimization_results_cache_get(int i);

#endif /* TIMESCALEDB_DP_OPTIMIZATION_RESULTS_CACHE_H */
