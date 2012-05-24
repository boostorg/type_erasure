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
        The operands do not all have to be the same type.
        We can capture the relationships among several types.
        To do this we'll need to identify each type with a
        __placeholder.  Also, we can no longer capture the
        variables independently, since they are connected,
        so we use __tuple to capture them all at once.
    */
    int array[5];

    typedef mpl::vector<
        copy_constructible<_a>,
        copy_constructible<_b>,
        typeid_<_a>,
        addable<_a, _b, _a>
    > requirements;

    tuple<requirements, _a, _b> t(&array[0], 2);
    any<requirements, _a> x(get<0>(t) + get<1>(t));
    // x now holds array + 2
    //]
}


//[multi
//` (For the source of the examples in this section see
//` [@boost:/libs/type_erasure/example/multi.cpp multi.cpp])
//` [multi1]
//` [multi2]
//]
