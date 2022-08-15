#include "privacy_budget.h"
#include <postgres.h>

void ts_privacy_budget_init(PrivacyBudget *privacy_budget) {
    privacy_budget->initial_budget = INITIAL_BUDGET;
    privacy_budget->available_budget = INITIAL_BUDGET;
    privacy_budget->reserved_budget = 0.0;
}

void ts_privacy_budget_consume(PrivacyBudget *privacy_budget, float8 val) {
    privacy_budget->available_budget -= val;
}

bool ts_privacy_budget_is_exhausted(const PrivacyBudget *privacy_budget) {
    if (privacy_budget->available_budget > 0.0001) {
        return false;
    }
    return true;
}

PrivacyBudget* ts_copy_privacy_budget(PrivacyBudget *privacy_budget) {
    PrivacyBudget * pb = palloc(sizeof(PrivacyBudget));
    pb->initial_budget = privacy_budget->initial_budget;
    pb->available_budget = privacy_budget->available_budget;
    pb->reserved_budget = privacy_budget->reserved_budget;
    return pb;
}