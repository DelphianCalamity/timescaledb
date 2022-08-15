#ifndef TIMESCALEDB_PRIVACY_BUDGET_H
#define TIMESCALEDB_PRIVACY_BUDGET_H

#define INITIAL_BUDGET 10

#include <stdbool.h>

typedef struct PrivacyBudget
{
    float initial_budget;
    float reserved_budget;
	float available_budget;
} PrivacyBudget;

void ts_privacy_budget_init(PrivacyBudget *privacy_budget);
bool ts_privacy_budget_is_exhausted(const PrivacyBudget *privacy_budget);
void ts_privacy_budget_consume(PrivacyBudget *privacy_budget, float val);

#endif /* TIMESCALEDB_PRIVACY_BUDGET_H */