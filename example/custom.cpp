// Boost.TypeErasure library
//
// Copyright 2011 Steven Watanabe
//
// Distributed under the Boost Software License Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// $Id$

#include <boost/type_erasure/any.hpp>
#include <boost/type_erasure/concept_interface.hpp>
#include <boost/type_erasure/rebind_any.hpp>
#include <vector>

namespace mpl = boost::mpl;
using namespace boost::type_erasure;

//[custom1
/*`
    Let's define a concept to allow push_back on a sequence.
    To do this, we create a class template with a template
    parameter for each argument, and a static member function
    called `apply` that calls `push_back`.
*/

template<class C, class T>
struct push_back
{
    static void apply(C& cont, const T& arg) { cont.push_back(arg); }
};
//]

//[custom3
/*`
    This works, but we'd really like to call `c.push_back(10)`.
    We can add members to __any by specializing __concept_interface.
*/
namespace boost {
namespace type_erasure {
template<class C, class T, class Base>
struct concept_interface< ::push_back<C, T>, Base, C> : Base
{
    void push_back(typename rebind_any<Base, const T&>::type arg)
    { call(::push_back<C, T>(), *this, arg); }
};
}
}
//]

void custom2() {
    //[custom2
    /*`
        Now, we can use this in an __any using
        __call to dispatch the operation.
    */
    std::vector<int> vec;
    any<push_back<_self, int>, _self&> c(vec);
    int i = 10;
    call(push_back<_self, int>(), c, i);
    // vec is [10].
    //]
}

void custom4() {
    //[custom4
    /*`
        Note the use of __rebind_any to determine the argument
        type.  We could just use `T`, but that would fail when
        `T` is a placeholder.  __rebind_any does the work to
        determine the correct type in that case.  Our example
        now becomes
    */
    std::vector<int> vec;
    any<push_back<_self, int>, _self&> c(vec);
    c.push_back(10);
    /*`
        which is what we want.
    */
    //]
}

//[custom
//` (For the source of the examples in this section see
//` [@boost:/libs/type_erasure/example/custom.cpp custom.cpp])
//` [custom1]
//` [custom2]
//` [custom3]
//` [custom4]
//]
