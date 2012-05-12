// Boost.TypeErasure library
//
// Copyright 2011 Steven Watanabe
//
// Distributed under the Boost Software License Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// $Id$

#ifndef BOOST_TYPE_ERASURE_DETAIL_NORMALIZE_HPP_INCLUDED
#define BOOST_TYPE_ERASURE_DETAIL_NORMALIZE_HPP_INCLUDED

#include <boost/mpl/eval_if.hpp>
#include <boost/mpl/fold.hpp>
#include <boost/mpl/is_sequence.hpp>
#include <boost/mpl/set.hpp>
#include <boost/mpl/insert.hpp>
#include <boost/mpl/vector.hpp>
#include <boost/mpl/back_inserter.hpp>
#include <boost/mpl/copy.hpp>
#include <boost/type_erasure/detail/get_placeholders.hpp>
#include <boost/type_erasure/relaxed_match.hpp>
#include <boost/type_erasure/builtin.hpp>

namespace boost {
namespace type_erasure {
namespace detail {

template<class Concept, class Out = ::boost::mpl::set0<> >
struct normalize_concept_impl
{
    typedef typename ::boost::mpl::eval_if< ::boost::mpl::is_sequence<Concept>,
        ::boost::mpl::fold<Concept, Out, normalize_concept_impl< ::boost::mpl::_2, ::boost::mpl::_1> >,
        ::boost::mpl::insert<Out, Concept>
    >::type type;
};

struct append_typeinfo
{
    template<class Set, class T>
    struct apply
    {
        typedef typename ::boost::mpl::insert<
            Set,
            ::boost::type_erasure::typeid_<T>
        >::type type;
    };
};

template<class Seq>
struct add_typeinfo
{
    typedef typename ::boost::mpl::fold<
        Seq,
        ::boost::mpl::set0<>,
        ::boost::type_erasure::detail::get_placeholders<
            ::boost::mpl::_2,
            ::boost::mpl::_1
        >
    >::type placeholders;
    typedef typename ::boost::mpl::fold<
        placeholders,
        Seq,
        ::boost::type_erasure::detail::append_typeinfo
    >::type type;
};

template<class Concept>
struct normalize_concept
{
    typedef typename normalize_concept_impl<Concept>::type basic;
    typedef typename ::boost::mpl::copy<
        typename ::boost::mpl::eval_if<
            ::boost::type_erasure::is_relaxed<Concept>,
            ::boost::type_erasure::detail::add_typeinfo<basic>,
            ::boost::mpl::identity<basic>
        >::type,
        ::boost::mpl::back_inserter< ::boost::mpl::vector0<> >
    >::type type;
};

template<class Concept, class Out = ::boost::mpl::set0<> >
struct collect_concepts
{
    typedef typename ::boost::mpl::insert<
        Out,
        Concept
    >::type type1;
    typedef typename ::boost::mpl::eval_if< ::boost::mpl::is_sequence<Concept>,
        ::boost::mpl::fold<Concept, type1, collect_concepts< ::boost::mpl::_2, ::boost::mpl::_1> >,
        ::boost::mpl::identity<type1>
    >::type type;
};

}
}
}

#endif
