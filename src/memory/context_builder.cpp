// src/memory/context_builder.cpp — YOU IMPLEMENT. Spec §4.4.

#include "memory/context_builder.h"
#include "util/min_heap.h"

#include <utility>
#include <vector>

ContextPlan select_context(const std::vector<ContextItem>& items,
                           std::size_t budget, std::size_t keep_recent) {
    ContextPlan plan;
    std::vector<bool> keep(items.size(), true);

    MinHeap<std::pair<int, std::size_t>> heap;

    for (std::size_t i = 0; i < items.size(); ++i) {
        plan.total_tokens += items[i].tokens;

        // Check if this is one of the recent messages
        bool recent = keep_recent >= items.size() || i >= items.size() - keep_recent;

        // Protected messages cannot be removed
        if (items[i].role == Role::System || items[i].pinned || recent) {
            continue;
        }
        heap.push({drop_rank(items[i].role), i});
    }

    // Remove messages until we fit within the budget
    while (plan.total_tokens > budget && !heap.empty()) {
        std::size_t index = heap.pop().second;

        keep[index] = false;
        plan.total_tokens -= items[index].tokens;
    }

    // Save the indices of messages we kept
    for (std::size_t i = 0; i < items.size(); ++i) {
        if (keep[i]) {
            plan.keep.push_back(i);
        }
    }

    plan.over_budget = plan.total_tokens > budget;

    return plan;
}
