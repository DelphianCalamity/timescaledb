#include "privacy_budget.h"

void ts_privacy_budget_init(PrivacyBudget *privacy_budget) {
    privacy_budget->initial_budget = INITIAL_BUDGET;
    privacy_budget->available_budget = INITIAL_BUDGET;
    privacy_budget->reserved_budget = 0.0;
}

void ts_privacy_budget_consume(PrivacyBudget *privacy_budget, float val) {
    privacy_budget->available_budget -= val;
}

bool ts_privacy_budget_is_exhausted(const PrivacyBudget *privacy_budget) {
    if (privacy_budget->available_budget > 0.0001) {
        return false;
    }
    return true;
}