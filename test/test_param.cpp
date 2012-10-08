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
#include <boost/type_erasure/param.hpp>
#include <boost/type_erasure/builtin.hpp>

using namespace boost::type_erasure;

template<int N>
struct size { typedef char (&type)[N]; };

// lvalues
extern any<copy_constructible<>, _self> a1;
extern const any<copy_constructible<>, _self> a2;
extern any<copy_constructible<>, _self&> a3;
extern const any<copy_constructible<>, _self&> a4;
extern any<copy_constructible<>, const _self&> a5;
extern const any<copy_constructible<>, const _self&> a6;

// rvalues
any<copy_constructible<>, _self> a7();
const any<copy_constructible<>, _self> a8();
any<copy_constructible<>, _self&> a9();
const any<copy_constructible<>, _self&> a10();
any<copy_constructible<>, const _self&> a11();
const any<copy_constructible<>, const _self&> a12();

extern int i;

size<1>::type f1(param<copy_constructible<>, _self&>);
size<2>::type f1(...);

void test_ref() {
    BOOST_STATIC_ASSERT(sizeof(f1(a1)) == 1);
    BOOST_STATIC_ASSERT(sizeof(f1(a2)) == 2);
    BOOST_STATIC_ASSERT(sizeof(f1(a3)) == 1);
    BOOST_STATIC_ASSERT(sizeof(f1(a4)) == 1);
    BOOST_STATIC_ASSERT(sizeof(f1(a5)) == 2);
    BOOST_STATIC_ASSERT(sizeof(f1(a6)) == 2);
    
    BOOST_STATIC_ASSERT(sizeof(f1(a7())) == 2);
    BOOST_STATIC_ASSERT(sizeof(f1(a8())) == 2);
    BOOST_STATIC_ASSERT(sizeof(f1(a9())) == 1);
    BOOST_STATIC_ASSERT(sizeof(f1(a10())) == 1);
    BOOST_STATIC_ASSERT(sizeof(f1(a11())) == 2);
    BOOST_STATIC_ASSERT(sizeof(f1(a12())) == 2);

    BOOST_STATIC_ASSERT(sizeof(f1(i)) == 2);
    BOOST_STATIC_ASSERT(sizeof(f1(1)) == 2);
}

size<1>::type f2(param<copy_constructible<>, const _self&>);
size<2>::type f2(...);

void test_cref() {
    BOOST_STATIC_ASSERT(sizeof(f2(a1)) == 1);
    BOOST_STATIC_ASSERT(sizeof(f2(a2)) == 1);
    BOOST_STATIC_ASSERT(sizeof(f2(a3)) == 1);
    BOOST_STATIC_ASSERT(sizeof(f2(a4)) == 1);
    BOOST_STATIC_ASSERT(sizeof(f2(a5)) == 1);
    BOOST_STATIC_ASSERT(sizeof(f2(a6)) == 1);
    
    BOOST_STATIC_ASSERT(sizeof(f2(a7())) == 1);
    BOOST_STATIC_ASSERT(sizeof(f2(a8())) == 1);
    BOOST_STATIC_ASSERT(sizeof(f2(a9())) == 1);
    BOOST_STATIC_ASSERT(sizeof(f2(a10())) == 1);
    BOOST_STATIC_ASSERT(sizeof(f2(a11())) == 1);
    BOOST_STATIC_ASSERT(sizeof(f2(a12())) == 1);

    BOOST_STATIC_ASSERT(sizeof(f2(i)) == 2);
    BOOST_STATIC_ASSERT(sizeof(f2(1)) == 2);
}

size<1>::type f3(param<copy_constructible<>, _self>);
size<2>::type f3(...);

void test_val() {
    BOOST_STATIC_ASSERT(sizeof(f3(a1)) == 1);
    BOOST_STATIC_ASSERT(sizeof(f3(a2)) == 1);
    BOOST_STATIC_ASSERT(sizeof(f3(a3)) == 1);
    BOOST_STATIC_ASSERT(sizeof(f3(a4)) == 1);
    BOOST_STATIC_ASSERT(sizeof(f3(a5)) == 1);
    BOOST_STATIC_ASSERT(sizeof(f3(a6)) == 1);
    
    BOOST_STATIC_ASSERT(sizeof(f3(a7())) == 1);
    BOOST_STATIC_ASSERT(sizeof(f3(a8())) == 1);
    BOOST_STATIC_ASSERT(sizeof(f3(a9())) == 1);
    BOOST_STATIC_ASSERT(sizeof(f3(a10())) == 1);
    BOOST_STATIC_ASSERT(sizeof(f3(a11())) == 1);
    BOOST_STATIC_ASSERT(sizeof(f3(a12())) == 1);

    BOOST_STATIC_ASSERT(sizeof(f3(i)) == 2);
    BOOST_STATIC_ASSERT(sizeof(f3(1)) == 2);
}

#ifndef BOOST_NO_FUNCTION_REFERENCE_QUALIFIERS

size<1>::type f4(param<copy_constructible<>, _self&&>);
size<2>::type f4(...);

void test_rref() {
    BOOST_STATIC_ASSERT(sizeof(f4(a1)) == 2);
    BOOST_STATIC_ASSERT(sizeof(f4(a2)) == 2);
    BOOST_STATIC_ASSERT(sizeof(f4(a3)) == 2);
    BOOST_STATIC_ASSERT(sizeof(f4(a4)) == 2);
    BOOST_STATIC_ASSERT(sizeof(f4(a5)) == 2);
    BOOST_STATIC_ASSERT(sizeof(f4(a6)) == 2);
    
    BOOST_STATIC_ASSERT(sizeof(f4(a7())) == 1);
    BOOST_STATIC_ASSERT(sizeof(f4(a8())) == 1);
    BOOST_STATIC_ASSERT(sizeof(f4(a9())) == 2);
    BOOST_STATIC_ASSERT(sizeof(f4(a10())) == 2);
    BOOST_STATIC_ASSERT(sizeof(f4(a11())) == 2);
    BOOST_STATIC_ASSERT(sizeof(f4(a12())) == 2);

    BOOST_STATIC_ASSERT(sizeof(f4(i)) == 2);
    BOOST_STATIC_ASSERT(sizeof(f4(1)) == 2);
}

#endif
