#pragma once

#include "targets/common/types.h"

#include <cstdint>
#include <vector>

namespace oe {
namespace detail {

template<typename HandleT>
class HandlePool {
public:
    HandleT allocate() {
        if (!freeIndices_.empty()) {
            const uint32_t index = freeIndices_.back();
            freeIndices_.pop_back();
            auto& slot = slots_[index];
            ++slot.generation;
            slot.alive = true;
            return {index, slot.generation};
        }

        const uint32_t index = static_cast<uint32_t>(slots_.size());
        slots_.push_back(Slot{1, true});
        return {index, 1};
    }

    void free(HandleT handle) {
        if (!isValid(handle)) {
            return;
        }
        slots_[handle.index].alive = false;
        freeIndices_.push_back(handle.index);
    }

    [[nodiscard]] bool isValid(HandleT handle) const {
        if (handle.index >= slots_.size()) {
            return false;
        }
        const auto& slot = slots_[handle.index];
        return slot.alive && slot.generation == handle.generation;
    }

    [[nodiscard]] size_t aliveCount() const {
        size_t count = 0;
        for (const auto& slot : slots_) {
            if (slot.alive) {
                ++count;
            }
        }
        return count;
    }

private:
    struct Slot {
        uint32_t generation = 0;
        bool alive = false;
    };

    std::vector<Slot> slots_;
    std::vector<uint32_t> freeIndices_;
};

} // namespace detail
} // namespace oe
