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
#include <boost/mpl/vector.hpp>
#include <iostream>

namespace mpl = boost::mpl;
using namespace boost::type_erasure;

void basic1() {
    //[basic1
    /*`
        The main class in the library is __any.  We can just pass
        it an MPL sequence containing all the concepts that we
        wish to use.

        [note The MPL sequence combines multiple concepts.
        In the rare case when we only want one concept, it doesn't
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
        Let's add a few more features.
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

//[basic
//` (For the source of the examples in this section see
//` [@boost:/libs/type_erasure/example/basic.cpp basic.cpp])
//` [basic1]
//` [basic2]
//]
