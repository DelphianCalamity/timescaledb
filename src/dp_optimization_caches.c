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

static Cache* dp_optimization_results_caches_current[NUM_QUERIES];
static Cache* dp_optimization_distances_caches_current[NUM_QUERIES];
static CachesMap caches_map;


DPOptimizationCaches
_dp_optimization_caches_add_get(int64 queryId)
{
    DPOptimizationCaches caches;
    for (int i=0; i<caches_map.counter; i++) 
	{
		if (queryId == caches_map.entries[i])
        {
            caches.dp_optimization_results_cache = dp_optimization_results_cache_get(i);
            caches.dp_optimization_distances_cache = dp_optimization_distances_cache_get(i);
            return caches;
        }         
    }

	caches_map.entries[caches_map.counter] = queryId;
	dp_optimization_results_caches_current[caches_map.counter] = dp_optimization_results_cache_create();
    dp_optimization_distances_caches_current[caches_map.counter] = dp_optimization_distances_cache_create();

    caches.dp_optimization_results_cache = dp_optimization_results_caches_current[caches_map.counter];
    caches.dp_optimization_distances_cache = dp_optimization_distances_caches_current[caches_map.counter];

    caches_map.counter++;

    return caches;
}

void
_dp_optimization_caches_init(void)
{
	caches_map.counter = 0;
}

void
_dp_optimization_caches_fini(void)
{
	for (int i=0; i<caches_map.counter; i++) 
	{
		ts_cache_invalidate(dp_optimization_results_caches_current[i]);
        ts_cache_invalidate(dp_optimization_distances_caches_current[i]);
	}
	caches_map.counter = 0;
}