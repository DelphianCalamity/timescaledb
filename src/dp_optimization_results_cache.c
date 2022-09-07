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
#include "dp_optimization_caches.h"
#include "dp_optimization_results_cache.h"
#include "ts_catalog/catalog.h"
#include "cache.h"
#include "dimension.h"

extern Cache* dp_optimization_results_caches_current[NUM_QUERIES];
extern Cache* dp_optimization_distances_caches_current[NUM_QUERIES];
extern CachesMap caches_map;
extern CachesKeys caches_keys;

static void *
dp_optimization_results_cache_get_key(CacheQuery *query)
{
	return (char *) query->data;
}

typedef struct
{	
	char key[KEY_SIZE];		//temp hack
	float result;
} DpOptimizationResultsCacheEntry;


static void *
dp_optimization_results_cache_create_entry(Cache *cache, CacheQuery *query)
{
	return query->result;
}

static bool
ts_dp_optimization_results_cache_valid_result(const void *result)
{
	if (result == NULL)
		return false;
	return ((DpOptimizationResultsCacheEntry *) result) != NULL;
}

Cache *
dp_optimization_results_cache_create(void)
{
	MemoryContext ctx =
		AllocSetContextCreate(CacheMemoryContext, "dp_optimization_results cache", ALLOCSET_DEFAULT_SIZES);

	Cache *cache = MemoryContextAlloc(ctx, sizeof(Cache));

	*cache = (Cache)
	{
		.hctl =
		{
			.keysize = sizeof(char)*KEY_SIZE,
			.entrysize = sizeof(DpOptimizationResultsCacheEntry),
			.hcxt = ctx,
		},
		.name = "dp_optimization_results_cache",
		.numelements = 16,
		.flags = HASH_ELEM | HASH_CONTEXT | HASH_BLOBS,
		.get_key = dp_optimization_results_cache_get_key,
		.create_entry = dp_optimization_results_cache_create_entry,
		.missing_error = NULL,
		.valid_result = ts_dp_optimization_results_cache_valid_result,
	};

	ts_cache_init(cache);
	cache->handle_txn_callbacks = false;

	return cache;
}

void
ts_dp_optimization_results_cache_invalidate_callback(Cache **cache)
{
	ts_cache_invalidate(*cache);
	*cache = dp_optimization_results_cache_create();
}

/* Get dp optimization results cache entry. */
float
ts_dp_optimization_results_cache_get_entry(Cache *cache, char *key, bool *found)
{
	*found = false;
	const unsigned int flags = CACHE_FLAG_MISSING_OK | CACHE_FLAG_NOCREATE;

	CacheQuery query = {
		.flags = flags,
		.data = key,

	};

	DpOptimizationResultsCacheEntry *entry = ts_cache_fetch(cache, &query);
	if (entry != NULL)
	{
		*found = true;
		return entry->result;
	}
	return 0;
}

/* Get dp optimization results cache write entry. */
void
ts_dp_optimization_results_cache_write_entry(Cache *cache, char *key, float result, bool *found)
{
	*found = false;
	const unsigned int flags = CACHE_FLAG_MISSING_OK;

	CacheQuery query = {
		.flags = flags,
		.data = key,
	};
	DpOptimizationResultsCacheEntry *entry = ts_cache_fetch(cache, &query);
	if (entry != NULL) {
		*found = true;
		entry->result = result;

		// char *key_ = malloc(100*sizeof(char));
		// sprintf(key_, "%s", key);
		// Add key to caches keys
		caches_keys.entries[caches_keys.counter] = key;
		caches_keys.counter++;
	}
}

Cache*
dp_optimization_results_cache_get(int i)
{
	return dp_optimization_results_caches_current[i];
}