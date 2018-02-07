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
#include <boost/type_erasure/builtin.hpp>
#include <boost/type_erasure/function.hpp>
#include <boost/mpl/vector.hpp>

#define BOOST_TEST_MAIN
#include <boost/test/unit_test.hpp>

using namespace boost::type_erasure;

struct model {
    explicit model(int v) : val(v) {}
    int f1() { return val; }
    int f1(int i) { return val + i; }
    int val;
};

BOOST_TYPE_ERASURE_FUNCTION(f1)

BOOST_AUTO_TEST_CASE(test_global_has_f1_0) {
    typedef ::boost::mpl::vector<
        has_member_f1<int()>,
        copy_constructible<> > concept_type;
    model m(10);
    any<concept_type> x(m);
    BOOST_CHECK_EQUAL(x.f1(), 10);
}

namespace ns1 { namespace ns2 {
BOOST_TYPE_ERASURE_FUNCTION(f1)
}}

BOOST_AUTO_TEST_CASE(test_ns_has_f1_0) {
    typedef ::boost::mpl::vector<
        ns1::ns2::has_member_f1<int()>,
        copy_constructible<> > concept_type;
    model m(10);
    any<concept_type> x(m);
    BOOST_CHECK_EQUAL(x.f1(), 10);
}

struct model_const {
    explicit model_const(int v) : val(v) {}
    int f1() const { return val; }
    int f1(int i) const { return val + i; }
    int val;
};

BOOST_AUTO_TEST_CASE(test_global_has_f1_0_const) {
    typedef ::boost::mpl::vector<
        ns1::ns2::has_member_f1<int() const>,
        copy_constructible<> > concept_type;
    model_const m(10);
    any<concept_type> x(m);
    BOOST_CHECK_EQUAL(x.f1(), 10);
}

BOOST_AUTO_TEST_CASE(test_global_has_f1_0_void) {
    typedef ::boost::mpl::vector<
        has_member_f1<void()>,
        copy_constructible<> > concept_type;
    model m(10);
    any<concept_type> x(m);
    x.f1();
}

BOOST_AUTO_TEST_CASE(test_global_has_f1_overload) {
    typedef ::boost::mpl::vector<
        has_member_f1<int()>,
        has_member_f1<int(int)>,
        copy_constructible<> > concept_type;
    model m(10);
    any<concept_type> x(m);
    BOOST_CHECK_EQUAL(x.f1(), 10);
    BOOST_CHECK_EQUAL(x.f1(5), 15);
}

BOOST_AUTO_TEST_CASE(test_global_has_f1_overload_const) {
    typedef ::boost::mpl::vector<
        has_member_f1<int() const>,
        has_member_f1<int(int) const>,
        copy_constructible<> > concept_type;
    model_const m(10);
    any<concept_type> x(m);
    BOOST_CHECK_EQUAL(x.f1(), 10);
    BOOST_CHECK_EQUAL(x.f1(5), 15);
}

struct model_overload_const_non_const {
    int f1() { return 1; }
    int f1() const { return 2; }
};

BOOST_AUTO_TEST_CASE(test_global_has_f1_overload_const_non_const) {
    typedef ::boost::mpl::vector<
        has_member_f1<int()>,
        has_member_f1<int() const>,
        copy_constructible<> > concept_type;
    model_overload_const_non_const m;
    any<concept_type> x1(m);
    BOOST_CHECK_EQUAL(x1.f1(), 1);
    const any<concept_type> x2(m);
    BOOST_CHECK_EQUAL(x2.f1(), 2);
}

#ifndef BOOST_NO_CXX11_RVALUE_REFERENCES

BOOST_AUTO_TEST_CASE(test_global_has_f1_rv) {
    typedef ::boost::mpl::vector<
        has_member_f1<int(int&&)>,
        copy_constructible<> > concept_type;
    model m(10);
    any<concept_type> x(m);
    BOOST_CHECK_EQUAL(x.f1(5), 15);
}

BOOST_AUTO_TEST_CASE(test_global_has_f1_rv_const) {
    typedef ::boost::mpl::vector<
        has_member_f1<int(int&&) const>,
        copy_constructible<> > concept_type;
    model_const m(10);
    const any<concept_type> x(m);
    BOOST_CHECK_EQUAL(x.f1(5), 15);
}

#endif

// free functions

int f1(model& m) { return m.val; }
int f1(model& m, int i) { return m.val + i; }

BOOST_AUTO_TEST_CASE(test_global_has_free_f1_1) {
    typedef ::boost::mpl::vector<
        has_f1<int(_self&)>,
        copy_constructible<> > concept_type;
    model m(10);
    any<concept_type> x(m);
    BOOST_CHECK_EQUAL(f1(x), 10);
}

BOOST_AUTO_TEST_CASE(test_ns_has_free_f1_1) {
    typedef ::boost::mpl::vector<
        ns1::ns2::has_f1<int(_self&)>,
        copy_constructible<> > concept_type;
    model m(10);
    any<concept_type> x(m);
    BOOST_CHECK_EQUAL(f1(x), 10);
}

int f1(const model_const& m) { return m.val; }
int f1(const model_const& m, int i) { return m.val + i; }

BOOST_AUTO_TEST_CASE(test_global_has_free_f1_1_const) {
    typedef ::boost::mpl::vector<
        ns1::ns2::has_f1<int(const _self&)>,
        copy_constructible<> > concept_type;
    model_const m(10);
    const any<concept_type> x(m);
    BOOST_CHECK_EQUAL(f1(x), 10);
}

BOOST_AUTO_TEST_CASE(test_global_has_free_f1_1_void) {
    typedef ::boost::mpl::vector<
        has_f1<void(_self&)>,
        copy_constructible<> > concept_type;
    model m(10);
    any<concept_type> x(m);
    f1(x);
}

BOOST_AUTO_TEST_CASE(test_global_has_free_f1_overload) {
    typedef ::boost::mpl::vector<
        has_f1<int(_self&)>,
        has_f1<int(_self&, int)>,
        copy_constructible<> > concept_type;
    model m(10);
    any<concept_type> x(m);
    BOOST_CHECK_EQUAL(f1(x), 10);
    BOOST_CHECK_EQUAL(f1(x, 5), 15);
}

BOOST_AUTO_TEST_CASE(test_global_has_free_f1_overload_const) {
    typedef ::boost::mpl::vector<
        has_f1<int(const _self&)>,
        has_f1<int(const _self&, int)>,
        copy_constructible<> > concept_type;
    model_const m(10);
    const any<concept_type> x(m);
    BOOST_CHECK_EQUAL(f1(x), 10);
    BOOST_CHECK_EQUAL(f1(x, 5), 15);
}

#ifndef BOOST_NO_CXX11_RVALUE_REFERENCES

BOOST_AUTO_TEST_CASE(test_global_has_free_f1_rv) {
    typedef ::boost::mpl::vector<
        has_f1<int(_self&&, int&&)>,
        copy_constructible<> > concept_type;
    model_const m(10);
    any<concept_type> x(m);
    BOOST_CHECK_EQUAL(f1(std::move(x), 5), 15);
}

#endif
