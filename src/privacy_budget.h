#ifndef TIMESCALEDB_PRIVACY_BUDGET_H
#define TIMESCALEDB_PRIVACY_BUDGET_H

#define INITIAL_BUDGET 10

#include <stdbool.h>

typedef double float8;

typedef struct PrivacyBudget
{
    float8 initial_budget;
    float8 reserved_budget;
	float8 available_budget;
} PrivacyBudget;

void ts_privacy_budget_init(PrivacyBudget *privacy_budget);
bool ts_privacy_budget_is_exhausted(const PrivacyBudget *privacy_budget);
PrivacyBudget* ts_copy_privacy_budget(PrivacyBudget *privacy_budget);


#endif /* TIMESCALEDB_PRIVACY_BUDGET_H */