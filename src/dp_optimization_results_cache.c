/*
 * This file and its contents are licensed under the Apache License 2.0.
 * Please see the included NOTICE for copyright information and
 * LICENSE-APACHE for a copy of the license.
 */
#include <postgres.h>
#include <catalog/namespace.h>
#include <utils/catcache.h>
#include <utils/lsyscache.h>
#include <utils/builtins.h>

#include "errors.h"
#include "dp_optimization_results_cache.h"
#include "ts_catalog/catalog.h"
#include "cache.h"
#include "scanner.h"
#include "dimension.h"
#include "ts_catalog/tablespace.h"
// #include <string.h>
// #include <stdlib.h>

#define KEY_SIZE 200
static void *dp_optimization_results_cache_create_entry(Cache *cache, CacheQuery *query);
static void dp_optimization_results_cache_missing_error(const Cache *cache, const CacheQuery *query);


typedef struct DpOptimizationResultsCacheQuery
{
	CacheQuery q;
	int64 queryId;
	Blocks blocks;
	char key[KEY_SIZE];
} DpOptimizationResultsCacheQuery;

static void *
dp_optimization_results_cache_get_key(CacheQuery *query)
{
	return &((DpOptimizationResultsCacheQuery *) query)->key;
}

typedef struct
{
	void *result;
} DpOptimizationResultsCacheEntry;

static bool
dp_optimization_results_cache_valid_result(const void *result)
{
	if (result == NULL)
		return false;
	return ((DpOptimizationResultsCacheEntry *) result)->result != NULL;
}

static Cache *
dp_optimization_results_cache_create()
{
	MemoryContext ctx =
		AllocSetContextCreate(CacheMemoryContext, "dp_optimization_results cache", ALLOCSET_DEFAULT_SIZES);

	Cache *cache = MemoryContextAlloc(ctx, sizeof(Cache));
	Cache		template =
	{
		.hctl =
		{
			.keysize = KEY_SIZE,
			.entrysize = sizeof(DpOptimizationResultsCacheEntry),
			.hcxt = ctx,
		},
		.name = "dp_optimization_results_cache",
		.numelements = 16,
		.flags = HASH_ELEM | HASH_CONTEXT | HASH_BLOBS,
		.get_key = dp_optimization_results_cache_get_key,
		.create_entry = dp_optimization_results_cache_create_entry,
		.missing_error = dp_optimization_results_cache_missing_error,
		.valid_result = dp_optimization_results_cache_valid_result,
	};

	*cache = template;

	ts_cache_init(cache);

	return cache;
}

static Cache *dp_optimization_results_cache_current = NULL;


static void *
dp_optimization_results_cache_create_entry(Cache *cache, CacheQuery *query)
{
	DpOptimizationResultsCacheEntry *cache_entry = query->result;
	return cache_entry->result == NULL ? NULL : cache_entry;
}

static void
dp_optimization_results_cache_missing_error(const Cache *cache, const CacheQuery *query)
{
}


void
ts_dp_optimization_results_cache_invalidate_callback(void)
{
	ts_cache_invalidate(dp_optimization_results_cache_current);
	dp_optimization_results_cache_current = dp_optimization_results_cache_create();
}

/* Get dp optimization results cache entry. If the entry is not in the cache, add it. */
void *
ts_dp_optimization_results_cache_get_entry(Cache *const cache, const int64 queryid, const Blocks blocks, const unsigned int flags)
{
	DpOptimizationResultsCacheQuery query = {
		.q.flags = flags,
		.queryId = queryid,
		.blocks = blocks,
	};
	sprintf(query.key, "%ld", queryid);
	sprintf(query.key+strlen(query.key), "%d", blocks.chunk_id_start);
	sprintf(query.key+strlen(query.key), "%d", blocks.chunk_id_end);

	DpOptimizationResultsCacheEntry *entry = ts_cache_fetch(cache, &query.q);
	Assert((flags & CACHE_FLAG_MISSING_OK) ? true : (entry != NULL && entry->result != NULL));
	return entry == NULL ? NULL : entry->result;
}

extern TSDLLEXPORT Cache *
ts_dp_optimization_results_cache_pin()
{
	return ts_cache_pin(dp_optimization_results_cache_current);
}

void
_dp_optimization_results_cache_init(void)
{
	CreateCacheMemoryContext();
	dp_optimization_results_cache_current = dp_optimization_results_cache_create();
}

void
_dp_optimization_results_cache_fini(void)
{
	ts_cache_invalidate(dp_optimization_results_cache_current);
}




// typedef struct DpOptimizationDistancesCacheQuery
// {
// 	CacheQuery q;
// 	int64 queryId;
// 	Blocks blocks_1;
// 	Blocks blocks_2;
// 	// Oid relid;
// 	const char *schema;
// 	const char *table;
// } DpOptimizationDistancesCacheQuery;
