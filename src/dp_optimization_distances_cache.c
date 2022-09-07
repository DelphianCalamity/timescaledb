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
#include "dp_optimization_distances_cache.h"

#include "ts_catalog/catalog.h"
#include "cache.h"
#include "dimension.h"

#define ABS(N) ((N<0)?(-N):(N))

extern Cache* dp_optimization_results_caches_current[NUM_QUERIES];
extern Cache* dp_optimization_distances_caches_current[NUM_QUERIES];
extern CachesMap caches_map;
extern CachesKeys caches_keys;

static void *
dp_optimization_distances_cache_get_key(CacheQuery *query)
{
	return (char *) query->data;
}

typedef struct
{
	char *key;
	float distance;
} DpOptimizationDistancesCacheListEntry;

typedef struct
{
	char key[KEY_SIZE];		// temp hack
	List *entries;

} DpOptimizationDistancesCacheList;

static void *
dp_optimization_distances_cache_create_entry(Cache *cache, CacheQuery *query)
{
	DpOptimizationDistancesCacheList *entry = query->result;
	entry->entries = NIL;
	return entry;
}

static bool
ts_dp_optimization_distances_cache_valid_result(const void *result)
{
	if (result == NULL)
		return false;
	return ((DpOptimizationDistancesCacheList *) result) != NULL;
}

Cache *
dp_optimization_distances_cache_create(void)
{
	MemoryContext ctx =
		AllocSetContextCreate(CacheMemoryContext, "dp_optimization_distances cache", ALLOCSET_DEFAULT_SIZES);

	Cache *cache = MemoryContextAlloc(ctx, sizeof(Cache));

	*cache = (Cache)
	{
		.hctl =
		{
			.keysize = sizeof(char)*KEY_SIZE,
			.entrysize = sizeof(DpOptimizationDistancesCacheList),
			.hcxt = ctx,
		},
		.name = "dp_optimization_distances_cache",
		.numelements = 16,
		.flags = HASH_ELEM | HASH_CONTEXT | HASH_BLOBS,
		.get_key = dp_optimization_distances_cache_get_key,
		.create_entry = dp_optimization_distances_cache_create_entry,
		.missing_error = NULL,
		.valid_result = ts_dp_optimization_distances_cache_valid_result,
	};

	ts_cache_init(cache);
	cache->handle_txn_callbacks = false;

	return cache;
}

void
ts_dp_optimization_distances_cache_invalidate_callback(Cache **cache)
{
	ts_cache_invalidate(*cache);
	*cache = dp_optimization_distances_cache_create();
}


/* Get dp optimization results cache entry. */
List *
ts_dp_optimization_distances_cache_get_entry(Cache *cache,  char *key, bool *found)
{
	*found = false;
	const unsigned int flags = CACHE_FLAG_MISSING_OK | CACHE_FLAG_NOCREATE;

	CacheQuery query = {
		.flags = flags,
		.data = key,

	};

	DpOptimizationDistancesCacheList *entry = ts_cache_fetch(cache, &query);
	if (entry != NULL)
	{
		*found = true;
		return entry->entries;
	}
	return 0;
}

void
ts_dp_optimization_distances_cache_write(Cache *distances_cache, Cache *results_cache, char *curr_key, float curr_result)
{
	char *key;
	List *entries;
	bool found;
	float distance, result;
	DpOptimizationDistancesCacheListEntry *entry;

	for (int i=0; i<caches_keys.counter; i++)
	{
		key = caches_keys.entries[i];
		if (curr_key == key)
			continue;

		result = ts_dp_optimization_results_cache_get_entry(results_cache, key, &found);
        if (!found) 
			elog(ERROR, "key not found");

		// Calculate distance
		distance = ABS(curr_result-result);
		
		// 1) Insert <curr_key : (key,distance)> in cache 
		entries = ts_dp_optimization_distances_cache_get_entry(distances_cache, curr_key, &found);
		if (!found) 
			entries = ts_dp_optimization_distances_cache_write_entry(distances_cache, curr_key, &found);
		// Create list entry and append to the list
		entry = malloc(sizeof(DpOptimizationDistancesCacheListEntry));
		entry->key = key;
		entry->distance = distance;
		entries = lappend(entries, entry);

		// 2) Insert <key : (curr_key,distance)> in cache 
		entries = ts_dp_optimization_distances_cache_get_entry(distances_cache, key, &found);
		if (!found) 
			entries = ts_dp_optimization_distances_cache_write_entry(distances_cache, key, &found);
		// Create list entry and append to the list
		entry = malloc(sizeof(DpOptimizationDistancesCacheListEntry));
		entry->key = curr_key;
		entry->distance = distance;
		entries = lappend(entries, entry);
	}
}

/* Get dp optimization results cache write entry. */
List *
ts_dp_optimization_distances_cache_write_entry(Cache *cache, char* key, bool *found)
{
	*found = false;
	const unsigned int flags = CACHE_FLAG_MISSING_OK;

	CacheQuery query = {
		.flags = flags,
		.data = key,
	};
	DpOptimizationDistancesCacheList *entry = ts_cache_fetch(cache, &query);
	if (entry != NULL) {
		*found = true;
		return entry->entries;
	}
	return NULL;
}

Cache*
dp_optimization_distances_cache_get(int i)
{
	return dp_optimization_distances_caches_current[i];
}