//===----------------------------------------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is dual licensed under the MIT and the University of Illinois Open
// Source Licenses. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef GENERICELEMENT_H
#define GENERICELEMENT_H

#if 0
#include <support/test_macros.h>

static_assert((TEST_STD_VER >= 11, "GenericElement requires C++11 features"));
#endif

#include <memory>
#include <utility>

template <typename T> class Tag {};

template <typename Allocator>
class GenericElement
{
    typedef std::allocator_traits<Allocator> AllocTraits;
    typedef typename AllocTraits::pointer    Pointer;

    Pointer ptr_;
    Allocator alloc_;
public:
    GenericElement() : ptr_(), alloc_() {}

    GenericElement(std::allocator_arg_t, const Allocator& alloc) : ptr_(), alloc_(alloc) {}

    template <typename T, typename... Args>
    GenericElement(std::allocator_arg_t, const Allocator& alloc, Tag<T>, Args&&... args) : ptr_(), alloc_(alloc) {
        emplace<T>(std::forward<Args>(args)...);
    }

    GenericElement(const GenericElement& other) :
        GenericElement(std::allocator_arg,
                       alloc_(AllocTraits::select_on_container_copy_construction(other.alloc))) {
        emplace<GenericElement>(other);
    }

    GenericElement(std::allocator_arg_t, const Allocator& alloc, const GenericElement& other) :
        GenericElement(std::allocator_arg, alloc) {
        emplace<GenericElement>(other);
    }

    GenericElement(GenericElement&& other) : GenericElement(std::allocator_arg, other.alloc) {
        emplace<GenericElement>(std::move(other));
    }

    GenericElement(std::allocator_arg_t, const Allocator& alloc, GenericElement&& other) :
        GenericElement(std::allocator_arg, alloc) {
        emplace<GenericElement>(std::move(other));
    }

    ~GenericElement() {
        if (ptr_) pop();
    }

    GenericElement& operator=(const GenericElement& other) = delete;
    GenericElement& operator=(GenericElement&& other) = delete;

    template <typename T, typename... Args>
    void emplace(Args&&... args) {
        if (ptr_) throw std::bad_alloc();

        using ReboundAlloc = AllocTraits::rebind_alloc<T>;
        using ReboundTraits = AllocTraits::rebind_traits<T>;

        ReboundAlloc elemAlloc(alloc_);
        ptr_ = ReboundTraits::allocate(elemAlloc, 1);
        AllocTraits::construct(alloc_, std::addressof(*ptr_), std::forward<Args>(args)...);
    }

    template <typename T>
    void push(T&& t) {
        emplace<T>(std::forward<T>(t));
    }

    void pop() {
        if (ptr_) {
            AllocTraits::destroy(alloc_, std::addressof(*ptr_));
            AllocTraits::deallocate(alloc_, ptr_, 1);
            ptr_ = nullptr;
        }
    }
};

#endif
