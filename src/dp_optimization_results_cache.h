/*
 * This file and its contents are licensed under the Apache License 2.0.
 * Please see the included NOTICE for copyright information and
 * LICENSE-APACHE for a copy of the license.
 */
#ifndef TIMESCALEDB_DP_OPTIMIZATION_RESULTS_CACHE_H
#define TIMESCALEDB_DP_OPTIMIZATION_RESULTS_CACHE_H

#include <postgres.h>

#include "export.h"
#include "cache.h"
#include "hypertable.h"
#include "export.h"

typedef struct blocks
{
	int chunk_id_start;
	int chunk_id_end;
} Blocks;


extern TSDLLEXPORT void *ts_dp_optimization_results_cache_get_entry(Cache *const cache,  const int64 queryid, const Blocks blocks,
															 const unsigned int flags);

extern void ts_dp_optimization_results_cache_invalidate_callback(void);

extern TSDLLEXPORT Cache *ts_dp_optimization_results_cache_pin(void);

extern void _dp_optimization_results_cache_init(void);
extern void _dp_optimization_results_cache_fini(void);

#endif /* TIMESCALEDB_DP_OPTIMIZATION_RESULTS_CACHE_H */
