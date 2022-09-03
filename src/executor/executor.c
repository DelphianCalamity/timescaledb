#include <postgres.h>
#include <access/tsmapi.h>
#include <access/xact.h>
#include <catalog/namespace.h>
#include <commands/extension.h>
#include <executor/nodeAgg.h>
#include <miscadmin.h>
#include <nodes/makefuncs.h>
#include <nodes/plannodes.h>
#include <optimizer/appendinfo.h>
#include <optimizer/clauses.h>
#include <optimizer/optimizer.h>
#include <optimizer/pathnode.h>
#include <optimizer/paths.h>
#include <optimizer/planner.h>
#include <optimizer/restrictinfo.h>
#include <optimizer/tlist.h>
#include <parser/parsetree.h>
#include <utils/elog.h>
#include <utils/fmgroids.h>
#include <utils/guc.h>
#include <utils/lsyscache.h>
#include <utils/memutils.h>
#include <utils/selfuncs.h>
#include <utils/timestamp.h>

#include "compat/compat-msvc-enter.h"
#include <catalog/pg_constraint.h>
#include <nodes/nodeFuncs.h>
#include <optimizer/cost.h>
#include <optimizer/plancat.h>
#include <parser/analyze.h>
#include <tcop/tcopprot.h>
#include "compat/compat-msvc-exit.h"

#include <math.h>

#include "annotations.h"
#include "chunk.h"
#include "cross_module_fn.h"
#include "dimension.h"
#include "dimension_slice.h"
#include "dimension_vector.h"
#include "extension.h"
#include "func_cache.h"
#include "guc.h"
#include "dp_optimization_results_cache.h"
#include "import/allpaths.h"
#include "license_guc.h"
#include "nodes/chunk_append/chunk_append.h"
#include "nodes/chunk_dispatch_plan.h"
#include "nodes/constraint_aware_append/constraint_aware_append.h"
#include "nodes/hypertable_modify.h"
#include "partitioning.h"
#include "executor/executor.h"
#include "utils.h"

void _executor_init(void);
void _executor_fini(void);

static ExecutorFinish_hook_type prev_ExecutorFinish = NULL;

// Allocates budget from chunks and inserts the result in the dp optimization results cache/distances
static void
timescaledb_executor_finish_hook(QueryDesc *queryDesc)
{
    Assert(queryDesc != NULL);

    if (prev_ExecutorFinish) {
        prev_ExecutorFinish(queryDesc);
    }

	uint64 queryId = queryDesc->plannedstmt->queryId;
    
    // Todo: run only for DP queries; find a way to identify them
    // This is a temporary hack to run only on my experimental query
    if (queryId == 251304559932419399) {
    
        bool found;
        Blocks blocks;
        ListCell *lc;
        RangeTblEntry *rte;
    	List *chunks = NIL;

        // Cache *dp_results_cache = ts_dp_optimization_results_cache_pin();

        // For all chunks involved consume the reserved budget
        foreach (lc, queryDesc->plannedstmt->rtable)
        {        
            rte = lfirst_node(RangeTblEntry, lc);
            Chunk *chunk = ts_chunk_get_by_relid(rte->relid, false);
            if (chunk != NULL)
            {
                ts_chunk_allocate_privacy_budget(chunk, 0.5);
                chunks = lappend(chunks, chunk);
            }
        }

        // Get blocks range to serve as a key in the caches e.g. (1,10)
        Chunk *first_chunk = list_nth(chunks, 0);
        Chunk *last_chunk = list_nth(chunks, chunks->length-1);
        blocks.chunk_id_start = first_chunk->fd.id;
        blocks.chunk_id_end = last_chunk->fd.id;
        
        ts_dp_optimization_results_cache_get_entry(queryId, blocks, &found);
        if (!found)
        {   // If not already in dp optimization results cache add it
            // ts_dp_optimization_results_cache_write_entry(dp_results_cache, queryId, blocks, queryDesc->result, &found);
            ts_dp_optimization_results_cache_write_entry(queryId, blocks, queryDesc->result, &found);

        }
        
        ts_dp_optimization_results_cache_get_entry(queryId, blocks, &found);

        // ts_cache_release(dp_results_cache);
    }
}

void _executor_init(void)
{	
    prev_ExecutorFinish = ExecutorFinish_hook;
    ExecutorFinish_hook = timescaledb_executor_finish_hook;
}

void _executor_fini(void) 
{
    ExecutorFinish_hook = prev_ExecutorFinish;
}

