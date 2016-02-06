//===----------------------------------------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is dual licensed under the MIT and the University of Illinois Open
// Source Licenses. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef INSTRUMENTINGALLOCATORS_H
#define INSTRUMENTINGALLOCATORS_H

#include <cstdlib>
#include <unordered_map>

struct Instrumentation {
    typedef std::unordered_map<void *, size_t> Allocations;

    Allocations allocs_;
    size_t      numAllocs_            = 0;
    size_t      numDeallocs_          = 0;
    size_t      numAllocsLastChecked_ = 0;

    ~Instrumentation() {
        assert(allocs_.empty());
    }

    void checkAllocsIncreased() {
        assert(numAllocs_ > numAllocsLastChecked_);
        numAllocsLastChecked_ = numAllocs_;
    }
};

template <typename T>
struct IA1 {
    Instrumentation *instrumentation_;

    typedef T value_type;

    explicit IA1(Instrumentation *instrumentation) :
        instrumentation_(instrumentation) {}

    template <typename U>
    IA1(const IA1<U>& other) : instrumentation_(other.instrumentation_) {}

    T *allocate(size_t n) {
        void *result = std::malloc(sizeof(T) * n);
        assert(instrumentation_->allocs_.find(result) ==
               instrumentation_->allocs_.end());
        instrumentation_->allocs_[result] = n;
        ++instrumentation_->numAllocs_;
        return static_cast<T *>(result);
    }

    void deallocate(T *ptr, size_t n) {
        auto alloc = instrumentation_->allocs_.find(ptr);
        assert(alloc != instrumentation_->allocs_.end());
        assert(alloc->second == n);
        instrumentation_->allocs_.erase(alloc);
        ++instrumentation_->numDeallocs_;
        std::free(ptr);
    }
};

template <typename T, typename U>
bool operator==(const IA1<T>& lhs, const IA1<U>& rhs) {
    return lhs.instrumentation_ == rhs.instrumentation_;
}

template <typename T, typename U>
bool operator!=(const IA1<T>& lhs, const IA1<U>& rhs) {
    return !(lhs == rhs);
}

#endif
