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

// pointer allocate(size_type n, const_void_pointer hint);

#include <scoped_allocator>
#include <cassert>

#include "allocators.h"

int main()
{
#ifndef _LIBCPP_HAS_NO_RVALUE_REFERENCES

    {
        typedef std::scoped_allocator_adaptor<A1<int>> A;
        A a;
        A1<int>::allocate_called = false;
        a.allocate(10, (const void*)0);
        assert(A1<int>::allocate_called == 10);
    }
    {
        typedef std::scoped_allocator_adaptor<A1<int>, A2<int>> A;
        A a;
        A1<int>::allocate_called = false;
        a.allocate(10, (const void*)10);
        assert(A1<int>::allocate_called == 10);
    }
    {
        typedef std::scoped_allocator_adaptor<A1<int>, A2<int>, A3<int>> A;
        A a;
        A1<int>::allocate_called = false;
        a.allocate(10, (const void*)20);
        assert(A1<int>::allocate_called == 10);
    }

    {
        typedef std::scoped_allocator_adaptor<A2<int>> A;
        A a;
        A2<int>::allocate_called = (const void*)50;
        a.allocate(10, (const void*)0);
        assert(A2<int>::allocate_called == (const void*)0);
    }
    {
        typedef std::scoped_allocator_adaptor<A2<int>, A2<int>> A;
        A a;
        A2<int>::allocate_called = (const void*)50;
        a.allocate(10, (const void*)10);
        assert(A2<int>::allocate_called == (const void*)10);
    }
    {
        typedef std::scoped_allocator_adaptor<A2<int>, A2<int>, A3<int>> A;
        A a;
        A2<int>::allocate_called = (const void*)50;
        a.allocate(10, (const void*)20);
        assert(A2<int>::allocate_called == (const void*)20);
    }
#endif  // _LIBCPP_HAS_NO_RVALUE_REFERENCES
}
