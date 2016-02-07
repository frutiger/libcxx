//===----------------------------------------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is dual licensed under the MIT and the University of Illinois Open
// Source Licenses. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

// <memory>

// template <class OuterAlloc, class... InnerAllocs>
//   class scoped_allocator_adaptor

// template <class T, class... Args> void construct(T* p, Args&& args);
// template <class T1, class T2, class... Args1, class... Args2>
//     void construct(pair<T1, T2>* p, piecewise_construct t, tuple<Args1...> x,
//                    tuple<Args2...> y);
// template <class T1, class T2>
//     void construct(pair<T1, T2>* p);
// template <class T1, class T2, class U, class V>
//     void construct(pair<T1, T2>* p, U&& x, V&& y);
// template <class T1, class T2, class U, class V>
//     void construct(pair<T1, T2>* p, const pair<U, V>& x);
// template <class T1, class T2, class U, class V>
//     void construct(pair<T1, T2>* p, pair<U, V>&& x);

#include <functional>
#include <list>
#include <scoped_allocator>
#include <cassert>

#include "instrumentingallocators.h"

namespace scoped {

template <typename T>
using IA1 = std::scoped_allocator_adaptor<IA1<T>>;

template <typename T>
using List1 = std::list<T, IA1<T>>;

}

template <typename List>
struct NextList {
    typedef typename std::list<List, std::scoped_allocator_adaptor<IA1<List>>> type;
};

template <typename BaseType, std::size_t depth>
struct MakeList {
    typedef typename MakeList<typename NextList<BaseType>::type, depth - 1>::type type;
};

template <typename BaseType>
struct MakeList<BaseType, 0> {
    typedef BaseType type;
};

int main()
{
    Instrumentation instrumentation;
    {
        typedef scoped::List1<int> List;

        List::allocator_type alloc(&instrumentation);
        List list(alloc);
        list.emplace_back();
        instrumentation.checkAllocsIncreased();
        list.emplace_back(list.back());
        instrumentation.checkAllocsIncreased();
    }
    assert(instrumentation.allocs_.size() == 0);
    {
        typedef scoped::List1<scoped::List1<int>> List;

        List::allocator_type alloc(&instrumentation);
        List list(alloc);
        list.emplace_back();
        instrumentation.checkAllocsIncreased();
        list.emplace_back(list.back());
        instrumentation.checkAllocsIncreased();
    }
    assert(instrumentation.allocs_.size() == 0);
    {
        typedef scoped::List1<std::function<int ()>> List;

        List::allocator_type alloc(&instrumentation);
        List list(alloc);
        list.emplace_back();
        instrumentation.checkAllocsIncreased();
        list.emplace_back(list.back());
        instrumentation.checkAllocsIncreased();
    }
    assert(instrumentation.allocs_.size() == 0);
    {
        typedef MakeList<int, 1>::type List;

        IA1<char> subAlloc(&instrumentation);
        std::scoped_allocator_adaptor<IA1<char>> alloc(subAlloc);
        List list(alloc);
        list.emplace_back();
        instrumentation.checkAllocsIncreased();
        list.emplace_back(list.back());
        instrumentation.checkAllocsIncreased();
    }
}

