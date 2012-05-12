// Boost.TypeErasure library
//
// Copyright 2011 Steven Watanabe
//
// Distributed under the Boost Software License Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// $Id$

#ifndef BOOST_TYPE_ERASURE_RELAXED_MATCH_HPP_INCLUDED
#define BOOST_TYPE_ERASURE_RELAXED_MATCH_HPP_INCLUDED

#include <boost/mpl/vector.hpp>
#include <boost/mpl/bool.hpp>
#include <boost/mpl/is_sequence.hpp>
#include <boost/mpl/find_if.hpp>
#include <boost/mpl/not.hpp>
#include <boost/mpl/end.hpp>
#include <boost/type_traits/is_same.hpp>

namespace boost {
namespace type_erasure {

template<class T>
struct is_relaxed;

namespace detail {

template<class T>
struct is_relaxed_impl :
    ::boost::mpl::not_<
        typename ::boost::is_same<
            typename ::boost::mpl::find_if<
                T,
                ::boost::type_erasure::is_relaxed< ::boost::mpl::_1>
            >::type,
            typename ::boost::mpl::end<T>::type
        >::type
    >::type
{};

}

/**
 * This special concept does not have any behavior by
 * itself.  However it affects the behavior of other
 * concepts.  In the presence of this concept, the
 * requirement that all the arguments to a function
 * must match is relaxed.  Instead, if there is a
 * reasonable default implementation, it will be used,
 * otherwise a @ref bad_function_call exception will
 * be thrown.
 *
 * The following concepts have special behavior with
 * this flag:
 * - @ref assignable "assignable": If the types are not the same,
 *   it will fall back on copy and swap.
 * - @ref equality_comparable "equality_comparable": If the types do not
 *   match, it will return false.
 * - @ref less_than_comparable "less_than_comparable": If the types do not
 *   match, the ordering will be according to
 *   @c std::type_info::before.
 */
struct relaxed_match : ::boost::mpl::vector0<> {};

/**
 * A metafunction indicating whether @c Concept
 * includes @ref relaxed_match.
 */
template<class Concept>
struct is_relaxed :
    ::boost::mpl::eval_if< ::boost::mpl::is_sequence<Concept>,
        ::boost::type_erasure::detail::is_relaxed_impl<Concept>,
        ::boost::mpl::false_
    >::type
{};

/** INTERNAL ONLY */
template<>
struct is_relaxed< ::boost::type_erasure::relaxed_match> :
    ::boost::mpl::true_
{};

}
}

#endif
