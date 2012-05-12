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
#include <boost/type_erasure/binding_of.hpp>
#include <boost/mpl/vector.hpp>
#include <boost/tuple/tuple.hpp>
#include <vector>

#define BOOST_TEST_MAIN
#include <boost/test/unit_test.hpp>

using namespace boost::type_erasure;

template<class T = _self>
struct common : ::boost::mpl::vector<
    copy_constructible<T>,
    typeid_<T>
> {};

BOOST_AUTO_TEST_CASE(test_unary)
{
    typedef ::boost::mpl::vector<
        common<_a>,
        constructible<_b(_a)>,
        destructible<_b>,
        typeid_<_b>
    > test_concept;
    int i = 1;
    double d = 10;
    tuple<test_concept, _a, _b&> t(i, d);
    any<test_concept, _b> x(get<0>(t));
    BOOST_CHECK_EQUAL(any_cast<double>(x), 1);
}

BOOST_AUTO_TEST_CASE(test_unary_no_const)
{
    typedef ::boost::mpl::vector<
        constructible<_b(_a&)>,
        destructible<_b>,
        typeid_<_b>
    > test_concept;
    typedef boost::mpl::map<
        boost::mpl::pair<_a, int>,
        boost::mpl::pair<_b, boost::tuple<int&> >
    > types;
    int i = 1;
    any<test_concept, _a&> x(i, make_binding<types>());
    any<test_concept, _b> y(x);
    BOOST_CHECK_EQUAL(&any_cast<boost::tuple<int&> >(y).get<0>(), &i);
    any<test_concept, _b> z(binding_of(x), x);
    BOOST_CHECK_EQUAL(&any_cast<boost::tuple<int&> >(z).get<0>(), &i);
}

BOOST_AUTO_TEST_CASE(test_from_int)
{
    typedef ::boost::mpl::vector<
        common<_a>,
        constructible<_a(std::size_t)>
    > test_concept;
    binding<test_concept> types =
        make_binding<
            ::boost::mpl::map<
                ::boost::mpl::pair<_a, std::vector<int> >
            >
        >();
    any<test_concept, _a> x(types, 10);
    std::vector<int> vec(any_cast<std::vector<int> >(x));
    BOOST_CHECK_EQUAL(vec.size(), 10u);
}

BOOST_AUTO_TEST_CASE(test_default)
{
    typedef ::boost::mpl::vector<
        common<_a>,
        constructible<_a()>
    > test_concept;
    binding<test_concept> types =
        make_binding<
            ::boost::mpl::map<
                ::boost::mpl::pair<_a, std::vector<int> >
            >
        >();
    any<test_concept, _a> x(types);
    std::vector<int> vec(any_cast<std::vector<int> >(x));
    BOOST_CHECK_EQUAL(vec.size(), 0u);
}

BOOST_AUTO_TEST_CASE(test_overload)
{
    typedef ::boost::mpl::vector<
        common<_a>,
        common<_b>,
        constructible<_a(_b)>,
        constructible<_a(std::size_t)>
    > test_concept;
    typedef ::boost::mpl::map<
        ::boost::mpl::pair<_a, std::vector<int> >,
        ::boost::mpl::pair<_b, std::size_t>
    > types;
    binding<test_concept> table = make_binding<types>();
    any<test_concept, _b> x(static_cast<std::size_t>(10), make_binding<types>());
    any<test_concept, _a> y(x);
    any<test_concept, _a> z(table, 17);
    std::vector<int> vec1(any_cast<std::vector<int> >(y));
    BOOST_CHECK_EQUAL(vec1.size(), 10u);
    std::vector<int> vec2(any_cast<std::vector<int> >(z));
    BOOST_CHECK_EQUAL(vec2.size(), 17u);
}


template<class T>
T as_rvalue(const T& arg) { return arg; }
template<class T>
const T& as_const(const T& arg) { return arg; }

BOOST_AUTO_TEST_CASE(test_from_int_with_binding)
{
    typedef ::boost::mpl::vector<common<> > test_concept;
    any<test_concept> x(4, make_binding<boost::mpl::map<boost::mpl::pair<_self, int> > >());
    BOOST_CHECK_EQUAL(any_cast<int>(x), 4);
}

BOOST_AUTO_TEST_CASE(test_copy)
{
    typedef ::boost::mpl::vector<common<> > test_concept;
    any<test_concept> x(4);
    BOOST_CHECK_EQUAL(any_cast<int>(x), 4);
    any<test_concept> y(x);
    BOOST_CHECK_EQUAL(any_cast<int>(y), 4);
    any<test_concept> z = as_rvalue(x);
    BOOST_CHECK_EQUAL(any_cast<int>(z), 4);
    any<test_concept> w = as_const(x);
    BOOST_CHECK_EQUAL(any_cast<int>(w), 4);
}

BOOST_AUTO_TEST_CASE(test_convert)
{
    typedef ::boost::mpl::vector<common<>, incrementable<> > src_concept;
    typedef ::boost::mpl::vector<common<> > dst_concept;
    any<src_concept> x(4);
    BOOST_CHECK_EQUAL(any_cast<int>(x), 4);
    any<dst_concept> y(x);
    BOOST_CHECK_EQUAL(any_cast<int>(y), 4);
    any<dst_concept> z = as_rvalue(x);
    BOOST_CHECK_EQUAL(any_cast<int>(z), 4);
    any<dst_concept> w = as_const(x);
    BOOST_CHECK_EQUAL(any_cast<int>(w), 4);
}

BOOST_AUTO_TEST_CASE(test_rebind)
{
    typedef ::boost::mpl::vector<common<> > src_concept;
    typedef ::boost::mpl::vector<common<_a> > dst_concept;
    any<src_concept> x(4);
    BOOST_CHECK_EQUAL(any_cast<int>(x), 4);
    any<dst_concept, _a> y(x);
    BOOST_CHECK_EQUAL(any_cast<int>(y), 4);
    any<dst_concept, _a> z = as_rvalue(x);
    BOOST_CHECK_EQUAL(any_cast<int>(z), 4);
    any<dst_concept, _a> w = as_const(x);
    BOOST_CHECK_EQUAL(any_cast<int>(w), 4);
}

BOOST_AUTO_TEST_CASE(test_rebind_and_convert)
{
    typedef ::boost::mpl::vector<common<>, incrementable<> > src_concept;
    typedef ::boost::mpl::vector<common<_a> > dst_concept;
    any<src_concept> x(4);
    BOOST_CHECK_EQUAL(any_cast<int>(x), 4);
    any<dst_concept, _a> y(x);
    BOOST_CHECK_EQUAL(any_cast<int>(y), 4);
    any<dst_concept, _a> z = as_rvalue(x);
    BOOST_CHECK_EQUAL(any_cast<int>(z), 4);
    any<dst_concept, _a> w = as_const(x);
    BOOST_CHECK_EQUAL(any_cast<int>(w), 4);
}

BOOST_AUTO_TEST_CASE(test_rebind_and_convert_with_binding)
{
    typedef ::boost::mpl::vector<common<>, incrementable<> > src_concept;
    typedef ::boost::mpl::vector<common<_a> > dst_concept;
    typedef ::boost::mpl::map<boost::mpl::pair<_a, _self> > map;
    typedef ::boost::mpl::map<boost::mpl::pair<_a, int> > types;

    binding<dst_concept> table(make_binding<types>());
    
    any<src_concept> x(4);
    BOOST_CHECK_EQUAL(any_cast<int>(x), 4);
    any<dst_concept, _a> y(x, make_binding<map>());
    BOOST_CHECK_EQUAL(any_cast<int>(y), 4);
    any<dst_concept, _a> z(x, table);
    BOOST_CHECK_EQUAL(any_cast<int>(z), 4);
    
    any<dst_concept, _a> cy(as_const(x), make_binding<map>());
    BOOST_CHECK_EQUAL(any_cast<int>(cy), 4);
    any<dst_concept, _a> cz(as_const(x), table);
    BOOST_CHECK_EQUAL(any_cast<int>(cz), 4);
}

BOOST_AUTO_TEST_CASE(test_copy_from_ref)
{
    typedef ::boost::mpl::vector<common<> > test_concept;
    int i = 4;
    any<test_concept, _self&> x(i);
    BOOST_CHECK_EQUAL(any_cast<int>(x), 4);
    any<test_concept> y(x);
    BOOST_CHECK_EQUAL(any_cast<int>(y), 4);
    any<test_concept> z = as_rvalue(x);
    BOOST_CHECK_EQUAL(any_cast<int>(z), 4);
    any<test_concept> w = as_const(x);
    BOOST_CHECK_EQUAL(any_cast<int>(w), 4);
}

BOOST_AUTO_TEST_CASE(test_convert_from_ref)
{
    typedef ::boost::mpl::vector<common<>, incrementable<> > src_concept;
    typedef ::boost::mpl::vector<common<> > dst_concept;
    int i = 4;
    any<src_concept, _self&> x(i);
    BOOST_CHECK_EQUAL(any_cast<int>(x), 4);
    any<dst_concept> y(x);
    BOOST_CHECK_EQUAL(any_cast<int>(y), 4);
    any<dst_concept> z = as_rvalue(x);
    BOOST_CHECK_EQUAL(any_cast<int>(z), 4);
    any<dst_concept> w = as_const(x);
    BOOST_CHECK_EQUAL(any_cast<int>(w), 4);
}

BOOST_AUTO_TEST_CASE(test_rebind_from_ref)
{
    typedef ::boost::mpl::vector<common<> > src_concept;
    typedef ::boost::mpl::vector<common<_a> > dst_concept;
    int i = 4;
    any<src_concept, _self&> x(i);
    BOOST_CHECK_EQUAL(any_cast<int>(x), 4);
    any<dst_concept, _a> y(x);
    BOOST_CHECK_EQUAL(any_cast<int>(y), 4);
    any<dst_concept, _a> z = as_rvalue(x);
    BOOST_CHECK_EQUAL(any_cast<int>(z), 4);
    any<dst_concept, _a> w = as_const(x);
    BOOST_CHECK_EQUAL(any_cast<int>(w), 4);
}

BOOST_AUTO_TEST_CASE(test_rebind_and_convert_from_ref)
{
    typedef ::boost::mpl::vector<common<>, incrementable<> > src_concept;
    typedef ::boost::mpl::vector<common<_a> > dst_concept;
    int i = 4;
    any<src_concept, _self&> x(i);
    BOOST_CHECK_EQUAL(any_cast<int>(x), 4);
    any<dst_concept, _a> y(x);
    BOOST_CHECK_EQUAL(any_cast<int>(y), 4);
    any<dst_concept, _a> z = as_rvalue(x);
    BOOST_CHECK_EQUAL(any_cast<int>(z), 4);
    any<dst_concept, _a> w = as_const(x);
    BOOST_CHECK_EQUAL(any_cast<int>(w), 4);
}

BOOST_AUTO_TEST_CASE(test_rebind_and_convert_with_binding_from_ref)
{
    typedef ::boost::mpl::vector<common<>, incrementable<> > src_concept;
    typedef ::boost::mpl::vector<common<_a> > dst_concept;
    typedef ::boost::mpl::map<boost::mpl::pair<_a, _self> > map;
    typedef ::boost::mpl::map<boost::mpl::pair<_a, int> > types;

    binding<dst_concept> table(make_binding<types>());
    
    int i = 4;
    any<src_concept, _self&> x(i);
    BOOST_CHECK_EQUAL(any_cast<int>(x), 4);
    any<dst_concept, _a> y(x, make_binding<map>());
    BOOST_CHECK_EQUAL(any_cast<int>(y), 4);
    any<dst_concept, _a> z(x, table);
    BOOST_CHECK_EQUAL(any_cast<int>(z), 4);
    
    any<dst_concept, _a> cy(as_const(x), make_binding<map>());
    BOOST_CHECK_EQUAL(any_cast<int>(cy), 4);
    any<dst_concept, _a> cz(as_const(x), table);
    BOOST_CHECK_EQUAL(any_cast<int>(cz), 4);
}

BOOST_AUTO_TEST_CASE(test_copy_from_cref)
{
    typedef ::boost::mpl::vector<common<> > test_concept;
    int i = 4;
    any<test_concept, const _self&> x(i);
    BOOST_CHECK_EQUAL(any_cast<int>(x), 4);
    any<test_concept> y(x);
    BOOST_CHECK_EQUAL(any_cast<int>(y), 4);
    any<test_concept> z = as_rvalue(x);
    BOOST_CHECK_EQUAL(any_cast<int>(z), 4);
    any<test_concept> w = as_const(x);
    BOOST_CHECK_EQUAL(any_cast<int>(w), 4);
}

BOOST_AUTO_TEST_CASE(test_convert_from_cref)
{
    typedef ::boost::mpl::vector<common<>, incrementable<> > src_concept;
    typedef ::boost::mpl::vector<common<> > dst_concept;
    int i = 4;
    any<src_concept, const _self&> x(i);
    BOOST_CHECK_EQUAL(any_cast<int>(x), 4);
    any<dst_concept> y(x);
    BOOST_CHECK_EQUAL(any_cast<int>(y), 4);
    any<dst_concept> z = as_rvalue(x);
    BOOST_CHECK_EQUAL(any_cast<int>(z), 4);
    any<dst_concept> w = as_const(x);
    BOOST_CHECK_EQUAL(any_cast<int>(w), 4);
}

BOOST_AUTO_TEST_CASE(test_rebind_from_cref)
{
    typedef ::boost::mpl::vector<common<> > src_concept;
    typedef ::boost::mpl::vector<common<_a> > dst_concept;
    int i = 4;
    any<src_concept, const _self&> x(i);
    BOOST_CHECK_EQUAL(any_cast<int>(x), 4);
    any<dst_concept, _a> y(x);
    BOOST_CHECK_EQUAL(any_cast<int>(y), 4);
    any<dst_concept, _a> z = as_rvalue(x);
    BOOST_CHECK_EQUAL(any_cast<int>(z), 4);
    any<dst_concept, _a> w = as_const(x);
    BOOST_CHECK_EQUAL(any_cast<int>(w), 4);
}

BOOST_AUTO_TEST_CASE(test_rebind_and_convert_from_cref)
{
    typedef ::boost::mpl::vector<common<>, incrementable<> > src_concept;
    typedef ::boost::mpl::vector<common<_a> > dst_concept;
    int i = 4;
    any<src_concept, const _self&> x(i);
    BOOST_CHECK_EQUAL(any_cast<int>(x), 4);
    any<dst_concept, _a> y(x);
    BOOST_CHECK_EQUAL(any_cast<int>(y), 4);
    any<dst_concept, _a> z = as_rvalue(x);
    BOOST_CHECK_EQUAL(any_cast<int>(z), 4);
    any<dst_concept, _a> w = as_const(x);
    BOOST_CHECK_EQUAL(any_cast<int>(w), 4);
}

BOOST_AUTO_TEST_CASE(test_rebind_and_convert_with_binding_from_cref)
{
    typedef ::boost::mpl::vector<common<>, incrementable<> > src_concept;
    typedef ::boost::mpl::vector<common<_a> > dst_concept;
    typedef ::boost::mpl::map<boost::mpl::pair<_a, _self> > map;
    typedef ::boost::mpl::map<boost::mpl::pair<_a, int> > types;

    binding<dst_concept> table(make_binding<types>());
    
    int i = 4;
    any<src_concept, const _self&> x(i);
    BOOST_CHECK_EQUAL(any_cast<int>(x), 4);
    any<dst_concept, _a> y(x, make_binding<map>());
    BOOST_CHECK_EQUAL(any_cast<int>(y), 4);
    any<dst_concept, _a> z(x, table);
    BOOST_CHECK_EQUAL(any_cast<int>(z), 4);
    
    any<dst_concept, _a> cy(as_const(x), make_binding<map>());
    BOOST_CHECK_EQUAL(any_cast<int>(cy), 4);
    any<dst_concept, _a> cz(as_const(x), table);
    BOOST_CHECK_EQUAL(any_cast<int>(cz), 4);
}
