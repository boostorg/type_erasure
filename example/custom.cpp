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
    The first argument is `push_back`, since we want to inject a member
    into every __any that uses the `push_back` concept.  The second argument,
    Base, is used by the library to chain multiple uses of __concept_interface
    together.  We have to inherit from it publicly.  Other than
    that we can ignore it.  The third argument is the placeholder
    that represents this any.  If someone used `push_back<_c, _b>`,
    we only want to insert a `push_back` member in the container,
    not the value type.  Thus, the third argument is the container
    placeholder.
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
