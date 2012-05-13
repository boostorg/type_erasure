// Boost.TypeErasure library
//
// Copyright 2012 Steven Watanabe
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
#include <boost/type_erasure/deduced.hpp>
#include <boost/type_erasure/same_type.hpp>
#include <boost/pointee.hpp>
#include <boost/mpl/vector.hpp>
#include <iostream>

namespace mpl = boost::mpl;
using namespace boost::type_erasure;

//[associated1
/*`
    Associated types are defined using the __deduced
    template.  __deduced is just like an ordinary
    placeholder, except that the type that it binds
    to is determined by calling a metafunction and
    does not need to be specified explicitly.

    For example, we can define a concept for
    holding any iterator, raw pointer, or
    smart pointer as follows:
*/

template<class T>
struct pointee
{
    typedef typename mpl::eval_if<is_placeholder<T>,
        mpl::identity<void>,
        boost::pointee<T>
    >::type type;
};

template<class T = _self>
struct pointer :
    mpl::vector<
        copy_constructible<T>,
        dereferenceable<deduced<pointee<T> >&, T>
    >
{
    typedef deduced<pointee<T> > element_type;
};

//]

void basic1() {
    //[associated2
    /*`
        Now when we construct `x`, `pointer<>::element_type` is
        deduced as `boost::pointee<int*>::type` which is `int`.
    */
    int i = 10;
    any<
        mpl::vector<
            pointer<>,
            typeid_<pointer<>::element_type>
        >
    > x(&i);
    int j = boost::type_erasure::any_cast<int>(*x); // j == i
    //]
}

void basic2() {
    //[associated3
    /*`
        Referring to the full name of the associated type
        can be cumbersome when it's used many times.  Also,
        sometimes we want to require that the associated
        type be a specific type.  Both of these can be
        solved using the __same_type concept.  Here we
        create an any that can hold any pointer whose
        element type is `int`.
    */
    int i = 10;
    any<
        mpl::vector<
            pointer<>,
            same_type<pointer<>::element_type, int>
        >
    > x(&i);
    std::cout << *x << std::endl; // prints 10
    //]
}

//[associated
//` (For the source of the examples in this section see
//` [@boost:/libs/type_erasure/example/associated.cpp associated.cpp])
//` [associated1]
//` [associated2]
//` [associated3]
//]
