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
#include <boost/type_erasure/tuple.hpp>
#include <boost/mpl/vector.hpp>
#include <iostream>

namespace mpl = boost::mpl;
using namespace boost::type_erasure;

void multi1() {
    //[multi1
    /*`
        Operations can have more than one __any argument.
        Let's change the example from the previous section
        to use binary addition, instead of increment.
    */
    typedef any<
        mpl::vector<
            copy_constructible<>,
            typeid_<>,
            addable<>,
            ostreamable<>
        >
    > any_type;
    any_type x(10);
    any_type y(7);
    any_type z(x + y);
    std::cout << z << std::endl; // prints 17
    /*`
        The underlying types of the arguments of `+` must match or the
        behavior is undefined.
    */
    //]
}

void multi2() {
    //[multi2
    /*`
        __addable`<>` requires the types of the arguments to be
        the same.  We can also capture relationships among several types.
        To do this we'll need to identify each type with a
        __placeholder.
    */
    int array[5];

    typedef mpl::vector<
        copy_constructible<_a>,
        copy_constructible<_b>,
        typeid_<_a>,
        addable<_a, _b, _a>
    > requirements;

    /*`
        Also, we can no longer capture the variables
        independently.
        ``
            any<requirements, _a> ptr(&array[0]); // illegal
        ``
        This doesn't work because the library needs
        to know the type that _b binds to when it
        captures the concept bindings.  We need to
        specify the bindings of both placeholders
        when we construct the __any.
     */

    typedef mpl::map<mpl::pair<_a, int*>, mpl::pair<_b, int> > types; 
    any<requirements, _a> ptr(&array[0], static_binding<types>());
    any<requirements, _b> idx(2, static_binding<types>());
    any<requirements, _a> x(ptr + idx);
    // x now holds array + 2

    /*`
        Here the arguments of `+` are no longer the same.
        What we require is that the dynamic bindings of
        the two arguments to `+` must map the placeholders
        `_a` and `_b` to the same types.

        We can also use __tuple to avoid having to
        write out the map out explicitly.
     */
    tuple<requirements, _a, _b> t(&array[0], 2);
    any<requirements, _a> y(get<0>(t) + get<1>(t));
    //]
}


//[multi
//` (For the source of the examples in this section see
//` [@boost:/libs/type_erasure/example/multi.cpp multi.cpp])
//` [multi1]
//` [multi2]
//]
