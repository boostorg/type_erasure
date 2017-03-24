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
#include <boost/type_erasure/tuple.hpp>
#include <boost/type_erasure/builtin.hpp>
#include <boost/type_erasure/operators.hpp>
#include <boost/type_erasure/any_cast.hpp>
#include <boost/type_erasure/relaxed.hpp>
#include <boost/mpl/vector.hpp>

#define BOOST_TEST_MAIN
#include <boost/test/unit_test.hpp>

using namespace boost::type_erasure;

template<class T = _self>
struct common : ::boost::mpl::vector<
    copy_constructible<T>,
    typeid_<T>
> {};

#ifndef BOOST_NO_CXX11_RVALUE_REFERENCES

template<class T = _self>
struct movable_common : ::boost::mpl::vector<
    destructible<T>,
    constructible<T(T&&)>,
    typeid_<T>
> {};

template<class T = _self, class U = T>
using move_assignable = ::boost::type_erasure::assignable<T, U&&>;

#endif

BOOST_AUTO_TEST_CASE(test_basic)
{
    typedef ::boost::mpl::vector<common<>, assignable<> > test_concept;
    any<test_concept> x(1);
    int* ip = any_cast<int*>(&x);
    any<test_concept> y(2);
    x = y;
    BOOST_CHECK_EQUAL(any_cast<int>(x), 2);
    // make sure that we're actually using assignment
    // of the underlying object, not copy and swap.
    BOOST_CHECK_EQUAL(any_cast<int*>(&x), ip);
}

BOOST_AUTO_TEST_CASE(test_basic_relaxed)
{
    typedef ::boost::mpl::vector<common<>, assignable<>, relaxed > test_concept;
    any<test_concept> x(1);
    int* ip = any_cast<int*>(&x);
    any<test_concept> y(2);
    x = y;
    BOOST_CHECK_EQUAL(any_cast<int>(x), 2);
    // make sure that we're actually using assignment
    // of the underlying object, not copy and swap.
    BOOST_CHECK_EQUAL(any_cast<int*>(&x), ip);
}

BOOST_AUTO_TEST_CASE(test_relaxed_no_copy)
{
    typedef ::boost::mpl::vector<
        destructible<>,
        typeid_<>,
        assignable<>,
        relaxed
    > test_concept;
    any<test_concept> x(1);
    int* ip = any_cast<int*>(&x);
    any<test_concept> y(2);
    x = y;
    BOOST_CHECK_EQUAL(any_cast<int>(x), 2);
    // make sure that we're actually using assignment
    // of the underlying object, not copy and swap.
    BOOST_CHECK_EQUAL(any_cast<int*>(&x), ip);
    // assigning raw values is not supported although "relaxed" is in concept,
    // because "assignable" is in concept, too.
    //x = 3.4;  // compile-error
    //BOOST_CHECK_EQUAL(any_cast<double>(x), 3.4);
}

BOOST_AUTO_TEST_CASE(test_relaxed_no_copy_no_assign)
{
    typedef ::boost::mpl::vector<
        destructible<>,
        typeid_<>,
        relaxed
    > test_concept;
    any<test_concept> x(1);
    BOOST_CHECK_EQUAL(any_cast<int>(x), 1);
    // assigning raw values is supported because "relaxed" is in concept.
    x = 2.3;
    BOOST_CHECK_EQUAL(any_cast<double>(x), 2.3);
}

BOOST_AUTO_TEST_CASE(test_relaxed_no_assign)
{
    typedef ::boost::mpl::vector<
        common<>,
        relaxed
    > test_concept;
    any<test_concept> x(1);
    any<test_concept> y(2);
    x = y;
    BOOST_CHECK_EQUAL(any_cast<int>(x), 2);
    // assigning raw values is supported because "relaxed" is in concept.
    x = 3.4;
    BOOST_CHECK_EQUAL(any_cast<double>(x), 3.4);
}

BOOST_AUTO_TEST_CASE(test_dynamic_fallback)
{
    typedef ::boost::mpl::vector<common<>, assignable<>, relaxed> test_concept;
    any<test_concept> x(1);
    any<test_concept> y(2.0);
    x = y;
    BOOST_CHECK_EQUAL(any_cast<double>(x), 2.0);
}

BOOST_AUTO_TEST_CASE(test_dynamic_fail)
{
    typedef ::boost::mpl::vector<destructible<>, typeid_<>, assignable<>, relaxed> test_concept;
    any<test_concept> x(1);
    any<test_concept> y(2.0);
    BOOST_CHECK_THROW(x = y, bad_function_call);
}

#ifndef BOOST_NO_CXX11_RVALUE_REFERENCES

template<class T>
struct move_only
{
    explicit move_only(T n)
        : value(n) {}
    move_only(move_only&& other)
        : value(other.value) { other.value = 0; }
    move_only& operator=(move_only&& other)
        { value = other.value; other.value = 0; return *this; }
    T value;
private:
    move_only(const move_only&);
    move_only& operator=(const move_only&);
};

template<class T>
struct move_construct_only
{
    explicit move_construct_only(T n)
        : value(n) {}
    move_construct_only(move_construct_only&& other)
        { value = other.value; other.value = 0; }
    T value;
private:
    move_construct_only(const move_construct_only&);
    move_construct_only& operator=(move_construct_only&&);
    move_construct_only& operator=(const move_construct_only&);
};

template<class T>
struct move_assign_only
{
    explicit move_assign_only(T n)
        : value(n) {}
    move_assign_only& operator=(move_assign_only&& other)
        { value = other.value; other.value = 0; return *this; }
    T value;
private:
    move_assign_only(move_assign_only&&);
    move_assign_only(const move_assign_only&);
    move_assign_only& operator=(const move_assign_only&);
};

//template<class T>
//struct copy_only
//{
//    explicit copy_only(T n)
//        : value(n) {}
//    copy_only(const copy_only& other)
//        : value(other.value) { other.value = 0; }
//    copy_only& operator=(const copy_only& other)
//        { value = other.value; return *this; }
//    T value;
//private:
//    copy_only(copy_only&&);
//    copy_only& operator=(copy_only&&);
//};

template<class T>
struct copy_construct_only
{
    explicit copy_construct_only(T n)
        : value(n) {}
    copy_construct_only(const copy_construct_only& other)
        { value = other.value; }
    T value;
private:
    copy_construct_only(copy_construct_only&&);
    copy_construct_only& operator=(copy_construct_only&&);
    copy_construct_only& operator=(const copy_construct_only&);
};

//template<class T>
//struct copy_assign_only
//{
//    explicit copy_assign_only(T n)
//        : value(n) {}
//    copy_assign_only& operator=(const copy_assign_only& other)
//        : { value = other.value; return *this; }
//    T value;
//private:
//    copy_assign_only(copy_assign_only&&);
//    copy_assign_only(const copy_assign_only&);
//    copy_assign_only& operator=(copy_assign_only&&);
//};

template<class T>
struct copy_construct_move_assign
{
    explicit copy_construct_move_assign(T n)
        : value(n) {}
    copy_construct_move_assign(const copy_construct_move_assign& other)
        : value(other.value) {}
    copy_construct_move_assign& operator=(copy_construct_move_assign&& other)
        { value = other.value; other.value = 0; return *this; }
    T value;
private:
    copy_construct_move_assign(copy_construct_move_assign&&);
    copy_construct_move_assign& operator=(const copy_construct_move_assign&);
};

template<class T>
struct copy_and_move
{
    explicit copy_and_move(T n)
        : value(n) {}
    copy_and_move(copy_and_move&& other)
        : value(other.value) { other.value = 0; }
    copy_and_move(const copy_and_move& other)
        : value(other.value) {}
    copy_and_move& operator=(int&& other)
        { value = other; other = 0; return *this; }
    copy_and_move& operator=(copy_and_move&& other)
        { value = other.value; other.value = 0; return *this; }
    copy_and_move& operator=(const copy_and_move& other)
        { value = other.value; return *this; }
    T value;
    copy_and_move& operator=(const int& other)
        { value = other; return *this; }
};

BOOST_AUTO_TEST_CASE(test_copy_constructable_move_assignable_basic)
{
    typedef ::boost::mpl::vector<
        common<>,
        move_assignable<>
    > test_concept;
    typedef copy_construct_move_assign<int> test_type;
    test_type source_x(1);
    test_type source_y(2);

    any<test_concept> x(source_x);
    test_type* ip = any_cast<test_type*>(&x);
    any<test_concept> y(source_y);
    x = std::move(y);
    BOOST_CHECK_EQUAL(any_cast<test_type>(x).value, 2);
    // make sure that we're actually using assignment
    // of the underlying object, not copy and swap.
    BOOST_CHECK_EQUAL(any_cast<test_type*>(&x), ip);
}

BOOST_AUTO_TEST_CASE(test_copy_constructable_move_assignable_basic_relaxed)
{
    typedef ::boost::mpl::vector<
        common<>,
        move_assignable<>,
        relaxed
    > test_concept;
    typedef copy_construct_move_assign<int> test_type;
    test_type source_x(1);
    test_type source_y(2);

    any<test_concept> x(source_x);
    test_type* ip = any_cast<test_type*>(&x);
    any<test_concept> y(source_y);
    x = std::move(y);
    BOOST_CHECK_EQUAL(any_cast<test_type>(x).value, 2);
    // make sure that we're actually using assignment
    // of the underlying object, not copy and swap.
    BOOST_CHECK_EQUAL(any_cast<test_type*>(&x), ip);
}

BOOST_AUTO_TEST_CASE(test_copy_constructable_move_and_copy_assignable_basic)
{
    typedef ::boost::mpl::vector<
        common<>,
        assignable<>,
        move_assignable<>
    > test_concept;
    typedef copy_and_move<int> test_type;
    test_type source_x(1);
    test_type source_y(2);
    test_type source_z(3);

    any<test_concept> x(source_x);
    test_type* ip = any_cast<test_type*>(&x);

    any<test_concept> y(source_y);
    x = y;
    BOOST_CHECK_EQUAL(any_cast<test_type>(x).value, 2);
    // make sure that we're actually using assignment
    // of the underlying object, not copy and swap.
    BOOST_CHECK_EQUAL(any_cast<test_type*>(&x), ip);

    any<test_concept> z(source_z);
    x = std::move(z);
    BOOST_CHECK_EQUAL(any_cast<test_type>(x).value, 3);
    // make sure that we're actually using assignment
    // of the underlying object, not copy and swap.
    BOOST_CHECK_EQUAL(any_cast<test_type*>(&x), ip);

    // no matching (move-)assignment operator and cannot use
    // copy-construction of the underlying object and swap,
    // because "relaxed" is not in concept.
    //x = any<test_concept>(4.5);  // undefined behavior (UB)
    //double check_val = 4.5;
    //BOOST_CHECK_EQUAL(any_cast<test_type>(x).value,          // UB
    //                  *reinterpret_cast<int*>(&check_val));  // UB
    //BOOST_CHECK_THROW(any_cast<double>(x), bad_any_cast);
}

BOOST_AUTO_TEST_CASE(test_copy_constructable_move_and_copy_assignable_basic_relaxed)
{
    typedef ::boost::mpl::vector<
        common<>,
        assignable<>,
        move_assignable<>,
        relaxed
    > test_concept;
    typedef copy_and_move<int> test_type;
    test_type source_x(1);
    test_type source_y(2);
    test_type source_z(3);

    any<test_concept> x(source_x);
    test_type* ip = any_cast<test_type*>(&x);

    any<test_concept> y(source_y);
    x = y;
    BOOST_CHECK_EQUAL(any_cast<test_type>(x).value, 2);
    // make sure that we're actually using assignment
    // of the underlying object, not copy and swap.
    BOOST_CHECK_EQUAL(any_cast<test_type*>(&x), ip);

    any<test_concept> z(source_z);
    x = std::move(z);
    BOOST_CHECK_EQUAL(any_cast<test_type>(x).value, 3);
    // make sure that we're actually using assignment
    // of the underlying object, not copy and swap.
    BOOST_CHECK_EQUAL(any_cast<test_type*>(&x), ip);

    // uses copy-construction of the underlying object and swap,
    // because "relaxed" is in concept and the stored types are
    // different ('test_type' and 'int').
    x = any<test_concept>(4);
    BOOST_CHECK_THROW(any_cast<const test_type&>(x), bad_any_cast);
    BOOST_CHECK_EQUAL(any_cast<const int&>(x), 4);
    // verify, that we're actually using copy-construction
    // of the underlying object and swap.
    BOOST_CHECK_NE(static_cast<void*>(any_cast<int*>(&x)), static_cast<void*>(ip));

    // assigning raw values is not supported although "relaxed" is in concept,
    // because "assignable"/"move_assignable" is in concept, too.
    //x = 5.6;  // compile-error
    //BOOST_CHECK_EQUAL(any_cast<double>(x), 5.6);
}

BOOST_AUTO_TEST_CASE(test_movable_basic)
{
    typedef ::boost::mpl::vector<
        movable_common<>,
        move_assignable<>
    > test_concept;
    typedef move_only<int> test_type;
    test_type source_x(1);
    test_type source_y(2);

    any<test_concept> x(std::move(source_x));
    test_type* ip = any_cast<test_type*>(&x);
    any<test_concept> y(std::move(source_y));
    x = std::move(y);
    BOOST_CHECK_EQUAL(any_cast<const test_type&>(x).value, 2);
    // make sure that we're actually using assignment
    // of the underlying object, not copy and swap.
    BOOST_CHECK_EQUAL(any_cast<test_type*>(&x), ip);
}

BOOST_AUTO_TEST_CASE(test_movable_basic_relaxed)
{
    typedef ::boost::mpl::vector<
        movable_common<>,
        move_assignable<>,
        relaxed
    > test_concept;
    typedef move_only<int> test_type;
    test_type source_x(1);
    test_type source_y(2);

    any<test_concept> x(std::move(source_x));
    test_type* ip = any_cast<test_type*>(&x);
    any<test_concept> y(std::move(source_y));
    x = std::move(y);
    BOOST_CHECK_EQUAL(any_cast<const test_type&>(x).value, 2);
    // make sure that we're actually using assignment
    // of the underlying object, not copy and swap.
    BOOST_CHECK_EQUAL(any_cast<test_type*>(&x), ip);
}

BOOST_AUTO_TEST_CASE(test_move_constructible_only_relaxed)
{
    typedef ::boost::mpl::vector<
        movable_common<>,
        relaxed
    > test_concept;
    typedef move_construct_only<int> test_type;
    test_type source_x(1);
    test_type source_y(2);
    typedef move_only<int> test_type2;
    test_type2 source_z(3);

    any<test_concept> x(std::move(source_x));
    test_type* ip = any_cast<test_type*>(&x);
    any<test_concept> y(std::move(source_y));
    x = std::move(y);
    BOOST_CHECK_EQUAL(any_cast<const test_type&>(x).value, 2);
    // verify, that we're actually using move-construction
    // of the underlying object and swap.
    BOOST_CHECK_NE(any_cast<test_type*>(&x), ip);

    any<test_concept> z(std::move(source_z));
    x = std::move(z);
    BOOST_CHECK_EQUAL(any_cast<const test_type2&>(x).value, 3);
}

BOOST_AUTO_TEST_CASE(test_move_assignable_only)
{
    typedef ::boost::mpl::vector<
        destructible<>,
        typeid_<>,
        constructible<_self(int)>,
        move_assignable<>
    > test_concept;
    typedef move_assign_only<int> test_type;
    typedef ::boost::mpl::map< ::boost::mpl::pair<_self, test_type> > type;

    any<test_concept> x(make_binding<type>(), 1);
    test_type* ip = any_cast<test_type*>(&x);
    any<test_concept> y(make_binding<type>(), 2);
    x = std::move(y);
    BOOST_CHECK_EQUAL(any_cast<const test_type&>(x).value, 2);
    // make sure that we're actually using assignment
    // of the underlying object, not copy and swap.
    BOOST_CHECK_EQUAL(any_cast<test_type*>(&x), ip);

    // assigning raw values is not supported because "relaxed" is not in concept.
    //x = test_type(3);
}

BOOST_AUTO_TEST_CASE(test_move_assignable_only_relaxed)
{
    typedef ::boost::mpl::vector<
        destructible<>,
        typeid_<>,
        constructible<_self(int)>,
        move_assignable<>,
        relaxed
    > test_concept;
    typedef move_assign_only<int> test_type;
    typedef ::boost::mpl::map< ::boost::mpl::pair<_self, test_type> > type;

    any<test_concept> x(make_binding<type>(), 1);
    test_type* ip = any_cast<test_type*>(&x);
    any<test_concept> y(make_binding<type>(), 2);
    x = std::move(y);
    BOOST_CHECK_EQUAL(any_cast<const test_type&>(x).value, 2);
    // make sure that we're actually using assignment
    // of the underlying object, not copy and swap.
    BOOST_CHECK_EQUAL(any_cast<test_type*>(&x), ip);

    // assigning raw values is not supported although "relaxed" is in concept,
    // because (move_)assignable is in concept, too.
    //x = test_type(3);
}

BOOST_AUTO_TEST_CASE(test_copyable_relaxed_no_assign)
{
    typedef ::boost::mpl::vector<
        common<>,
        relaxed
    > test_concept;
    typedef copy_construct_only<int> test_type;
    test_type source_x(1);
    test_type source_y(2);

    any<test_concept> x(source_x);
    any<test_concept> y(source_y);
    x = std::move(y);
    BOOST_CHECK_EQUAL(any_cast<const test_type&>(x).value, 2);
}

BOOST_AUTO_TEST_CASE(test_movable_relaxed_no_assign)
{
    typedef ::boost::mpl::vector<
        movable_common<>,
        relaxed
    > test_concept;
    typedef move_construct_only<int> test_type;
    test_type source_x(1);
    test_type source_y(2);

    any<test_concept> x(std::move(source_x));
    any<test_concept> y(std::move(source_y));
    x = std::move(y);
    BOOST_CHECK_EQUAL(any_cast<const test_type&>(x).value, 2);
}

BOOST_AUTO_TEST_CASE(test_copyable_dynamic_fallback)
{
    typedef ::boost::mpl::vector<
        common<>,
        move_assignable<>,
        relaxed
    > test_concept;
    typedef copy_construct_move_assign<int>    test_type1;
    typedef copy_construct_move_assign<double> test_type2;
    test_type1 source_x(1);
    test_type2 source_y(2.5);

    any<test_concept> x(source_x);
    any<test_concept> y(source_y);
    x = std::move(y);
    BOOST_CHECK_THROW(any_cast<const test_type1&>(x), bad_any_cast);
    BOOST_CHECK_EQUAL(any_cast<const test_type2&>(x).value, 2.5);
}

BOOST_AUTO_TEST_CASE(test_movable_dynamic_fallback)
{
    typedef ::boost::mpl::vector<
        movable_common<>,
        move_assignable<>,
        relaxed
    > test_concept;
    typedef move_only<int>    test_type1;
    typedef move_only<double> test_type2;
    test_type1 source_x(1);
    test_type2 source_y(2.5);

    any<test_concept> x(std::move(source_x));
    any<test_concept> y(std::move(source_y));
    x = std::move(y);
    BOOST_CHECK_THROW(any_cast<const test_type1&>(x), bad_any_cast);
    BOOST_CHECK_EQUAL(any_cast<const test_type2&>(x).value, 2.5);
}

BOOST_AUTO_TEST_CASE(test_dynamic_fail2)
{
    typedef ::boost::mpl::vector<
        destructible<>,
        typeid_<>,
        move_assignable<>,
        relaxed
    > test_concept;
    typedef move_only<int>    test_type1;
    typedef move_only<double> test_type2;
    test_type1 source_x(1);
    test_type2 source_y(2.5);

    any<test_concept> x(std::move(source_x));
    any<test_concept> y(std::move(source_y));
    BOOST_CHECK_THROW(x = std::move(y), bad_function_call);
}

#endif

BOOST_AUTO_TEST_CASE(test_basic_int)
{
    typedef ::boost::mpl::vector<common<>, assignable<_self, int> > test_concept;
    any<test_concept> x(1);
    int* ip = any_cast<int*>(&x);
    x = 2;
    BOOST_CHECK_EQUAL(any_cast<int>(x), 2);
    // make sure that we're actually using assignment
    // of the underlying object, not copy and swap.
    BOOST_CHECK_EQUAL(any_cast<int*>(&x), ip);
}

BOOST_AUTO_TEST_CASE(test_basic_relaxed_int)
{
    typedef ::boost::mpl::vector<common<>, assignable<_self, int>, relaxed > test_concept;
    any<test_concept> x(1);
    int* ip = any_cast<int*>(&x);
    x = 2;
    BOOST_CHECK_EQUAL(any_cast<int>(x), 2);
    // make sure that we're actually using assignment
    // of the underlying object, not copy and swap.
    BOOST_CHECK_EQUAL(any_cast<int*>(&x), ip);
}

BOOST_AUTO_TEST_CASE(test_relaxed_no_copy_int)
{
    typedef ::boost::mpl::vector<
        destructible<>,
        typeid_<>,
        assignable<_self, int>,
        relaxed
    > test_concept;
    any<test_concept> x(1);
    int* ip = any_cast<int*>(&x);
    x = 2;
    BOOST_CHECK_EQUAL(any_cast<int>(x), 2);
    // make sure that we're actually using assignment
    // of the underlying object, not copy and swap.
    BOOST_CHECK_EQUAL(any_cast<int*>(&x), ip);
}

BOOST_AUTO_TEST_CASE(test_relaxed_no_assign_int)
{
    typedef ::boost::mpl::vector<
        common<>,
        relaxed
    > test_concept;
    any<test_concept> x(1);
    x = 2;
    BOOST_CHECK_EQUAL(any_cast<int>(x), 2);
}

#ifndef BOOST_NO_CXX11_RVALUE_REFERENCES

struct move_only_assign_int
{
    explicit move_only_assign_int(int i)
        : value(i) {}
    move_only_assign_int(move_only_assign_int&& other)
        : value(other.value) { other.value = 0; }
    move_only_assign_int& operator=(move_only_assign_int&& other)
        { value = other.value; other.value = 0; return *this; }
    move_only_assign_int& operator=(int&& i)
        { value = i; return *this; }
    int value;
private:
    move_only_assign_int(const move_only_assign_int&);
    move_only_assign_int& operator=(const move_only_assign_int&);
};

struct only_assign_int
{
    explicit only_assign_int(int i)
        : value(i) {}
    only_assign_int& operator=(int&& i)
        { value = i; return *this; }
    int value;
private:
    only_assign_int(only_assign_int&&);
    only_assign_int(const only_assign_int&);
    only_assign_int& operator=(only_assign_int&&);
    only_assign_int& operator=(const only_assign_int&);
};

struct move_and_copy_assign_int
{
    explicit move_and_copy_assign_int(int i)
        : value(i) {}
    move_and_copy_assign_int& operator=(move_and_copy_assign_int&& other)
        { value = other.value; other = 0; return *this; }
    move_and_copy_assign_int& operator=(const move_and_copy_assign_int& other)
        { value = other.value; return *this; }
    move_and_copy_assign_int& operator=(int&& i)
        { value = i; return *this; }
    move_and_copy_assign_int& operator=(const int& i)
        { value = i; return *this; }
    int value;
private:
    move_and_copy_assign_int(move_and_copy_assign_int&&);
    move_and_copy_assign_int(const move_and_copy_assign_int&);
};

struct move_and_copy_construct_assign_int
{
    explicit move_and_copy_construct_assign_int(int i)
        : value(i) {}
    move_and_copy_construct_assign_int(move_and_copy_construct_assign_int&& other)
        : value(other.value) { other.value = 0; }
    move_and_copy_construct_assign_int(const move_and_copy_construct_assign_int& other)
        : value(other.value) {}
    move_and_copy_construct_assign_int& operator=(move_and_copy_construct_assign_int&& other)
        { value = other.value; other = 0; return *this; }
    move_and_copy_construct_assign_int& operator=(const move_and_copy_construct_assign_int& other)
        { value = other.value; return *this; }
    move_and_copy_construct_assign_int& operator=(int&& i)
        { value = i; return *this; }
    move_and_copy_construct_assign_int& operator=(const int& i)
        { value = i; return *this; }
    int value;
};

struct copy_only_assign_int
{
    explicit copy_only_assign_int(int i)
        : value(i) {}
    copy_only_assign_int(const copy_only_assign_int& other)
        : value(other.value) {}
    copy_only_assign_int& operator=(const copy_only_assign_int& other)
        { value = other.value; return *this; }
    copy_only_assign_int& operator=(int&& i)
        { value = i; return *this; }
    copy_only_assign_int& operator=(const int& i)
        { value = i; return *this; }
    int value;
private:
    copy_only_assign_int(copy_only_assign_int&&);
    copy_only_assign_int& operator=(copy_only_assign_int&&);
};

struct copy_only_assign_double
{
    explicit copy_only_assign_double(double i)
        : value(i) {}
    copy_only_assign_double(const copy_only_assign_double& other)
        : value(other.value) {}
    copy_only_assign_double& operator=(const copy_only_assign_double& other)
        { value = other.value; return *this; }
    copy_only_assign_double& operator=(double&& i)
        { value = i; return *this; }
    copy_only_assign_double& operator=(const double& i)
        { value = i; return *this; }
    double value;
private:
    copy_only_assign_double(copy_only_assign_double&&);
    copy_only_assign_double& operator=(copy_only_assign_double&&);
};

BOOST_AUTO_TEST_CASE(test_movable_basic_int)
{
    typedef ::boost::mpl::vector<
        movable_common<>,
        move_assignable<_self, int>
    > test_concept;
    typedef move_only_assign_int test_type;
    test_type source_x(1);

    any<test_concept> x(std::move(source_x));
    test_type* ip = any_cast<test_type*>(&x);
    x = std::move(2);
    BOOST_CHECK_EQUAL(any_cast<const test_type&>(x).value, 2);
    // make sure that we're actually using assignment
    // of the underlying object, not copy and swap.
    BOOST_CHECK_EQUAL(any_cast<test_type*>(&x), ip);
}

BOOST_AUTO_TEST_CASE(test_movable_basic_relaxed_int)
{
    typedef ::boost::mpl::vector<
        movable_common<>,
        move_assignable<_self, int>,
        relaxed
    > test_concept;
    typedef move_only_assign_int test_type;
    test_type source_x(1);

    any<test_concept> x(std::move(source_x));
    test_type* ip = any_cast<test_type*>(&x);
    x = std::move(2);
    BOOST_CHECK_EQUAL(any_cast<const test_type&>(x).value, 2);
    // make sure that we're actually using assignment
    // of the underlying object, not copy and swap.
    BOOST_CHECK_EQUAL(any_cast<test_type*>(&x), ip);
}

BOOST_AUTO_TEST_CASE(test_relaxed_no_constructible_int)
{
    typedef ::boost::mpl::vector<
        destructible<>,
        typeid_<>,
        constructible<_self(int)>,
        move_assignable<_self, int>,
        relaxed
    > test_concept;
    typedef only_assign_int test_type;

    typedef ::boost::mpl::map< ::boost::mpl::pair<_self, test_type> > type;
    any<test_concept> x(make_binding<type>(), 1);
    BOOST_CHECK_EQUAL(any_cast<const test_type&>(x).value, 1);
    test_type* ip = any_cast<test_type*>(&x);
    x = std::move(2);
    BOOST_CHECK_EQUAL(any_cast<const test_type&>(x).value, 2);
    // make sure that we're actually using assignment
    // of the underlying object, not copy and swap.
    BOOST_CHECK_EQUAL(any_cast<test_type*>(&x), ip);
}

BOOST_AUTO_TEST_CASE(test_movable_relaxed_no_assign_int)
{
    typedef ::boost::mpl::vector<
        movable_common<>,
        relaxed
    > test_concept;
    typedef move_only_assign_int test_type;
    test_type source_x(1);

    any<test_concept> x(std::move(source_x));
    x = test_type(2);
    BOOST_CHECK_EQUAL(any_cast<const test_type&>(x).value, 2);
}

BOOST_AUTO_TEST_CASE(test_move_and_copy_assignable_int)
{
    typedef ::boost::mpl::vector<
        destructible<>,
        typeid_<>,
        constructible<_self(int)>,
        assignable<_self, _self>,
        move_assignable<_self, _self>,
        assignable<_self, int>,
        move_assignable<_self, int>,
        relaxed
    > test_concept;
    typedef move_and_copy_assign_int test_type;
    typedef ::boost::mpl::map< ::boost::mpl::pair<_self, test_type> > type;

    any<test_concept> x(make_binding<type>(), 1);
    BOOST_CHECK_EQUAL(any_cast<const test_type&>(x).value, 1);

    any<test_concept> y(make_binding<type>(), 2);
    x = y;
    BOOST_CHECK_EQUAL(any_cast<const test_type&>(x).value, 2);

    any<test_concept> z(make_binding<type>(), 3);
    x = std::move(z);
    BOOST_CHECK_EQUAL(any_cast<const test_type&>(x).value, 3);

    x = 4;
    BOOST_CHECK_EQUAL(any_cast<const test_type&>(x).value, 4);

    x = std::move(5);
    BOOST_CHECK_EQUAL(any_cast<const test_type&>(x).value, 5);
}

BOOST_AUTO_TEST_CASE(test_move_and_copy_assignable_copy_construct_int)
{
    typedef ::boost::mpl::vector<
        common<>,
        assignable<_self, _self>,
        move_assignable<_self, _self>,
        assignable<_self, int>,
        move_assignable<_self, int>,
        relaxed
    > test_concept;
    typedef move_and_copy_construct_assign_int test_type;
    test_type source_x(1);
    test_type source_y(2);
    test_type source_z(3);

    any<test_concept> x(source_x);
    BOOST_CHECK_EQUAL(any_cast<const test_type&>(x).value, 1);
    test_type* ip = any_cast<test_type*>(&x);

    any<test_concept> y(source_y);
    x = y;
    BOOST_CHECK_EQUAL(any_cast<const test_type&>(x).value, 2);
    // make sure that we're actually using assignment
    // of the underlying object, not copy and swap.
    BOOST_CHECK_EQUAL(any_cast<test_type*>(&x), ip);

    any<test_concept> z(source_z);
    x = std::move(z);
    BOOST_CHECK_EQUAL(any_cast<const test_type&>(x).value, 3);
    // make sure that we're actually using move-assignment
    // of the underlying object, not copy and swap.
    BOOST_CHECK_EQUAL(any_cast<test_type*>(&x), ip);

    x = 4;
    BOOST_CHECK_EQUAL(any_cast<const test_type&>(x).value, 4);
    // make sure that we're actually using assignment
    // of the underlying object, not copy and swap.
    BOOST_CHECK_EQUAL(any_cast<test_type*>(&x), ip);

    x = std::move(5);
    BOOST_CHECK_EQUAL(any_cast<const test_type&>(x).value, 5);
    // make sure that we're actually using move-assignment
    // of the underlying object, not copy and swap.
    BOOST_CHECK_EQUAL(any_cast<test_type*>(&x), ip);
}

#if !defined(__GNUC__) || (__GNUC__ * 10000 + __GNUC_MINOR__ * 100 + __GNUC_PATCHLEVEL__) >= 40900
// Older GCC (up to 4.8.x) cannot handle the following two test-cases (due to ambiguity errors).

BOOST_AUTO_TEST_CASE(test_assignable_with_conversion)
{
    typedef ::boost::mpl::vector<
        destructible<>,
        typeid_<>,
        assignable<_self, int>,
        relaxed
    > test_concept;
    typedef move_and_copy_construct_assign_int test_type1;
    typedef copy_only_assign_int               test_type2;
    typedef copy_only_assign_double            test_type3;

    test_type1 source_x(1);
    any<test_concept> x(source_x);
    BOOST_CHECK_EQUAL(any_cast<const test_type1&>(x).value, 1);

    // assigning raw values is supported because "relaxed" is in concept.
    test_type2 source_y(2);
    x = source_y;
    BOOST_CHECK_EQUAL(any_cast<const test_type2&>(x).value, 2);

    // move-assigning raw values is supported because "relaxed" is in concept.
    x = test_type1(3);
    BOOST_CHECK_EQUAL(any_cast<const test_type1&>(x).value, 3);

    // assigning int is supported because appropriate "assignable" is in concept.
    int int4 = 4;
    x = int4;
    BOOST_CHECK_EQUAL(any_cast<const test_type1&>(x).value, 4);
    int int5 = 5;
    x = std::move(int5);
    BOOST_CHECK_EQUAL(any_cast<const test_type1&>(x).value, 5);

    // assigning double (which is implicitly convertible to int) is supported
    // because appropriate "assignable" is in concept.
    double double6 = 6.5;
    x = double6;  // truncates value
    BOOST_CHECK_EQUAL(any_cast<const test_type1&>(x).value, 6);
    double double7 = 7.5;
    x = std::move(double7);  // truncates value
    BOOST_CHECK_EQUAL(any_cast<const test_type1&>(x).value, 7);

    // assigning raw values is supported because "relaxed" is in concept.
    test_type3 source_z(8.5);
    x = source_z;
    BOOST_CHECK_EQUAL(any_cast<const test_type3&>(x).value, 8.5);

    // assigning double (which is implicitly convertible to int) is supported
    // because appropriate "assignable" is in concept.
    double double9 = 9.5;
    x = double9;  // truncates value
    BOOST_CHECK_EQUAL(any_cast<const test_type3&>(x).value, 9.0);
    double double10 = 10.5;
    x = std::move(double10);  // truncates value
    BOOST_CHECK_EQUAL(any_cast<const test_type3&>(x).value, 10.0);
}

BOOST_AUTO_TEST_CASE(test_move_assignable_with_conversion)
{
    typedef ::boost::mpl::vector<
        destructible<>,
        typeid_<>,
        move_assignable<_self, int>,
        relaxed
    > test_concept;
    typedef move_and_copy_construct_assign_int test_type1;
    typedef copy_only_assign_int               test_type2;
    typedef copy_only_assign_double            test_type3;

    test_type1 source_x(1);
    any<test_concept> x(source_x);
    BOOST_CHECK_EQUAL(any_cast<const test_type1&>(x).value, 1);

    // assigning raw values is supported because "relaxed" is in concept.
    test_type2 source_y(2);
    x = source_y;
    BOOST_CHECK_EQUAL(any_cast<const test_type2&>(x).value, 2);

    // move-assigning raw values is supported because "relaxed" is in concept.
    x = test_type1(3);
    BOOST_CHECK_EQUAL(any_cast<const test_type1&>(x).value, 3);

    // move-assigning int is supported because appropriate "move_assignable" is in concept.
    int int4 = 4;
    x = std::move(int4);
    BOOST_CHECK_EQUAL(any_cast<const test_type1&>(x).value, 4);
    // copy-assigning int is assigning a raw_value.
    int int5 = 5;
    x = int5;
    BOOST_CHECK_EQUAL(any_cast<const int&>(x), 5);
    x = source_x;  // reset.

    // move-assigning double (which is implicitly convertible to int) is supported
    // because appropriate "move_assignable" is in concept.
    double double6 = 6.5;
    x = std::move(double6);  // truncates value
    BOOST_CHECK_EQUAL(any_cast<const test_type1&>(x).value, 6);
    // copy-assigning double is assigning a raw_value.
    double double7 = 7.5;
    x = double7;
    BOOST_CHECK_EQUAL(any_cast<const double&>(x), 7.5);

    // assigning raw values is supported because "relaxed" is in concept.
    test_type3 source_z(8.5);
    x = source_z;
    BOOST_CHECK_EQUAL(any_cast<const test_type3&>(x).value, 8.5);

    // move-assigning double (which is implicitly convertible to int) is supported
    // because appropriate "move_assignable" is in concept.
    double double9 = 9.5;
    x = std::move(double9);  // truncates value
    BOOST_CHECK_EQUAL(any_cast<const test_type3&>(x).value, 9.0);
    // copy-assigning double is assigning a raw_value.
    double double10 = 10.5;
    x = double10;
    BOOST_CHECK_EQUAL(any_cast<const double&>(x), 10.5);
}
#endif

#endif

BOOST_AUTO_TEST_CASE(test_basic_ref)
{
    typedef ::boost::mpl::vector<common<>, assignable<> > test_concept;
    int i = 1;
    any<test_concept, _self&> x(i);
    any<test_concept> y(2);
    x = y;
    BOOST_CHECK_EQUAL(i, 2);
}

BOOST_AUTO_TEST_CASE(test_basic_relaxed_ref)
{
    typedef ::boost::mpl::vector<common<>, assignable<>, relaxed > test_concept;
    int i = 1;
    any<test_concept, _self&> x(i);
    any<test_concept> y(2);
    x = y;
    BOOST_CHECK_EQUAL(i, 2);
}

BOOST_AUTO_TEST_CASE(test_relaxed_no_assign_ref)
{
    typedef ::boost::mpl::vector<
        common<>,
        relaxed
    > test_concept;
    int i = 1;
    any<test_concept, _self&> x(i);
    any<test_concept> y(2);
    x = y;
    BOOST_CHECK_EQUAL(i, 1);
    BOOST_CHECK_EQUAL(any_cast<int*>(&x), any_cast<int*>(&y));
}

BOOST_AUTO_TEST_CASE(test_dynamic_fallback_ref)
{
    typedef ::boost::mpl::vector<common<>, assignable<>, relaxed> test_concept;
    int i = 1;
    any<test_concept, _self&> x(i);
    any<test_concept> y(2.0);
    x = y;
    BOOST_CHECK_EQUAL(any_cast<double>(x), 2.0);
}
