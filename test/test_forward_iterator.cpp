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
#include <boost/type_erasure/builtin.hpp>
#include <boost/type_erasure/operators.hpp>
#include <boost/type_erasure/any_cast.hpp>
#include <boost/type_erasure/iterator.hpp>
#include <boost/type_erasure/tuple.hpp>
#include <boost/mpl/vector.hpp>
#include <boost/concept_check.hpp>

#define BOOST_TEST_MAIN
#include <boost/test/unit_test.hpp>

using namespace boost::type_erasure;

BOOST_AUTO_TEST_CASE(test_basic)
{
    typedef boost::mpl::vector<forward_iterator<_self, int> > test_concept;
    std::vector<int> vec(10);
    any<test_concept> x(vec.begin());
    any<test_concept> y(vec.end());

    for(int i = 0; x != y; ++x, ++i) {
        *x = i;
    }
    int expected[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
    BOOST_CHECK_EQUAL_COLLECTIONS(vec.begin(), vec.end(), &expected[0], &expected[0] + 10);

    BOOST_MPL_ASSERT((boost::is_same<any<test_concept>::value_type, int>));
    BOOST_MPL_ASSERT((boost::is_same<any<test_concept>::reference, int&>));
    BOOST_MPL_ASSERT((boost::is_same<any<test_concept>::pointer, int*>));
    BOOST_MPL_ASSERT((boost::is_same<any<test_concept>::difference_type, std::ptrdiff_t>));
    BOOST_MPL_ASSERT((boost::is_same<any<test_concept>::iterator_category, std::forward_iterator_tag>));
}

BOOST_AUTO_TEST_CASE(test_relaxed)
{
    typedef boost::mpl::vector<forward_iterator<_self, int>, relaxed_match> test_concept;
    std::vector<int> vec(10);
    any<test_concept> x(vec.begin());
    any<test_concept> y(vec.end());

    for(int i = 0; x != y; ++x, ++i) {
        *x = i;
    }
    int expected[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
    BOOST_CHECK_EQUAL_COLLECTIONS(vec.begin(), vec.end(), &expected[0], &expected[0] + 10);

    BOOST_MPL_ASSERT((boost::is_same<any<test_concept>::value_type, int>));
    BOOST_MPL_ASSERT((boost::is_same<any<test_concept>::reference, int&>));
    BOOST_MPL_ASSERT((boost::is_same<any<test_concept>::pointer, int*>));
    BOOST_MPL_ASSERT((boost::is_same<any<test_concept>::difference_type, std::ptrdiff_t>));
    BOOST_MPL_ASSERT((boost::is_same<any<test_concept>::iterator_category, std::forward_iterator_tag>));

    BOOST_CONCEPT_ASSERT((boost::ForwardIterator<any<test_concept> >));
}
