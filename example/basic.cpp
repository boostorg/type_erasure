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
#include <boost/type_erasure/any_cast.hpp>
#include <boost/type_erasure/builtin.hpp>
#include <boost/type_erasure/operators.hpp>
#include <boost/type_erasure/member.hpp>
#include <boost/type_erasure/free.hpp>
#include <boost/mpl/vector.hpp>
#include <iostream>

namespace mpl = boost::mpl;
using namespace boost::type_erasure;

void basic1() {
    //[basic1
    /*`
        The main class in the library is __any.  We can just pass
        it an MPL sequence specifying all the requirements on
        the types that it can hold.

        [note The MPL sequence combines multiple concepts.
        In the rare case when we only want a single concept, it doesn't
        need to be wrapped in an MPL sequence.]
    */
    any<mpl::vector<copy_constructible<>, typeid_<> > > x(10);
    int i = any_cast<int>(x); // i == 10
    /*`
        __copy_constructible allows us to copy and destroy the
        object.  __typeid_ provides run-time type information
        so that we can use __any_cast.
     */
    //]
}

void basic2() {
    //[basic2
    /*`
        Now, this example doesn't do very much.  `x` is approximately
        equivalent to a [@boost:/libs/any/index.html boost::any].
        We can make it more interesting by adding some operators,
        such as `operator++` and `operator<<`
    */
    any<
        mpl::vector<
            copy_constructible<>,
            typeid_<>,
            incrementable<>,
            ostreamable<>
        >
    > x(10);
    ++x;
    std::cout << x << std::endl; // prints 11
    //]
}

//[basic3
/*`
    The library provides a full set of operators, but this
    obviously won't cover all use cases;  we often need to
    define our own requirements.  Let's take the `push_back`
    method, defined by several STL containers.
*/

BOOST_TYPE_ERASURE_MEMBER((has_push_back), push_back, 1)

void append_many(any<has_push_back<void(int)>, _self&> container) {
    for(int i = 0; i < 10; ++i)
        container.push_back(i);
}

/*`
    There are a few things to note about this.  First,
    we use the macro __BOOST_TYPE_ERASURE_MEMBER to
    define a concept called `has_push_back` for a
    member function called `push_back` which takes
    one argument.  When we use `has_push_back`, we have to
    give it the signature of the function, `void(int)`.
    This means that we expect to find a function
    that looks like:

    ``
    void push_back(int);
    ``

    Thus, we could call `append_many` with `std::vector<int>`,
    `std::list<int>`, or `std::vector<long>` (because `int` is
    convertible to `long`), but not `std::list<std::string>`
    or `std::set<int>`.

    Also, note the use of `_self&` as the second argument of
    __any.  `_self` is a __placeholder.  By using it here,
    we indicate that the __any stores a reference
    to another object instead of owning its own object.
*/

/*`
    For free functions, we can use __BOOST_TYPE_ERASURE_FREE.
*/

BOOST_TYPE_ERASURE_FREE((has_swap), swap, 2);
template<class T = _self>
struct swappable : mpl::vector<has_swap<void(T&, T&)> > {};

/*`
    We use the __placeholder `_self` here to indicate which arguments
    of `swap` should be any's.  When we use swap, we want it
    to look like `has_swap<void(_self&, _self&)>`, since `swap`
    takes two arguments of the same type by reference.  Since
    the signature of swap always looks like this, we define
    `swappable<>` as a convenient short-cut.
*/

//]

//[basic
//` (For the source of the examples in this section see
//` [@boost:/libs/type_erasure/example/basic.cpp basic.cpp])
//` [basic1]
//` [basic2]
//` [basic3]
//]
