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
#include "hypertable_cache.h"
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

static ExecutorEnd_hook_type prev_ExecutorEnd = NULL;

static void
timescaledb_executor_end_hook(QueryDesc *queryDesc)
{
    Assert(queryDesc != NULL);

    printf("lalala\n");
    if (prev_ExecutorEnd)
        prev_ExecutorEnd(queryDesc);
    else
        standard_ExecutorEnd(queryDesc);
}

void _executor_init(void)
{	
    prev_ExecutorEnd = ExecutorEnd_hook;
    ExecutorEnd_hook = timescaledb_executor_end_hook;
}

void _executor_fini(void) 
{
    ExecutorEnd_hook = prev_ExecutorEnd;

}

