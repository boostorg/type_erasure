// Boost.TypeErasure library
//
// Copyright 2012 Steven Watanabe
//
// Distributed under the Boost Software License Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// $Id$

#if !defined(BOOST_PP_IS_ITERATING)

#ifndef BOOST_TYPE_ERASURE_DETAIL_INSTANTIATE_HPP_INCLUDED
#define BOOST_TYPE_ERASURE_DETAIL_INSTANTIATE_HPP_INCLUDED

#include <boost/mpl/transform.hpp>
#include <boost/mpl/size.hpp>
#include <boost/mpl/at.hpp>
#include <boost/preprocessor/cat.hpp>
#include <boost/preprocessor/iteration/iterate.hpp>
#include <boost/preprocessor/repetition/repeat.hpp>
#include <boost/preprocessor/repetition/enum.hpp>
#include <boost/preprocessor/repetition/enum_params.hpp>
#include <boost/type_erasure/detail/normalize.hpp>
#include <boost/type_erasure/detail/rebind_placeholders.hpp>

namespace boost {
namespace type_erasure {
namespace detail {

template<int N>
struct make_instantiate_concept_impl;

template<class Concept, class Map>
struct make_instantiate_concept {
    typedef typename ::boost::type_erasure::detail::normalize_concept<
        Concept>::type normalized;
    typedef typename ::boost::type_erasure::detail::get_placeholder_normalization_map<
        Concept
    >::type placeholder_subs;

    typedef typename ::boost::mpl::transform<
        normalized,
        ::boost::type_erasure::detail::rebind_placeholders<
            ::boost::mpl::_1,
            typename ::boost::type_erasure::detail::add_deductions<
                Map,
                placeholder_subs
            >::type
        >
    >::type bound_concept;
    typedef typename ::boost::type_erasure::detail::make_instantiate_concept_impl<
        (::boost::mpl::size<bound_concept>::value)
    >::template apply<bound_concept>::type type;
};

#define BOOST_PP_FILENAME_1 <boost/type_erasure/detail/instantiate.hpp>
#define BOOST_PP_ITERATION_LIMITS (0, BOOST_TYPE_ERASURE_MAX_FUNCTIONS)
#include BOOST_PP_ITERATE()

}
}
}

#endif

#else

#define N BOOST_PP_ITERATION()

#define BOOST_TYPE_ERASURE_INSTANTIATE(z, n, data)\
    (void)&BOOST_PP_CAT(T, n)::apply;

#define BOOST_TYPE_ERASURE_AT(z, n, data)       \
    typename ::boost::mpl::at_c<data, n>::type

#if N

template<BOOST_PP_ENUM_PARAMS(N, class T)>
struct BOOST_PP_CAT(instantiate_concept, N) {
    static void apply() {
        BOOST_PP_REPEAT(N, BOOST_TYPE_ERASURE_INSTANTIATE, ~)
    }
};

#else

template<class T = void>
struct instantiate_concept0 {
    static void apply() {}
};

#endif

template<>
struct make_instantiate_concept_impl<N>
{
    template<class Seq>
    struct apply
    {
        typedef ::boost::type_erasure::detail::BOOST_PP_CAT(instantiate_concept, N)<
            BOOST_PP_ENUM(N, BOOST_TYPE_ERASURE_AT, Seq)
        > type;
    };
};

#undef BOOST_TYPE_ERASURE_AT
#undef BOOST_TYPE_ERASURE_INSTANTIATE

#undef N

#endif
