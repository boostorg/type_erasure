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

#include <boost/mpl/assert.hpp>
#include <boost/mpl/eval_if.hpp>
#include <boost/mpl/identity.hpp>
#include <boost/mpl/is_sequence.hpp>
#include <boost/mpl/set.hpp>
#include <boost/mpl/map.hpp>
#include <boost/mpl/has_key.hpp>
#include <boost/mpl/insert.hpp>
#include <boost/mpl/vector.hpp>
#include <boost/mpl/back_inserter.hpp>
#include <boost/mpl/inserter.hpp>
#include <boost/mpl/fold.hpp>
#include <boost/mpl/transform.hpp>
#include <boost/mpl/copy.hpp>
#include <boost/mpl/at.hpp>
#include <boost/type_traits/is_same.hpp>
#include <boost/type_erasure/detail/get_placeholders.hpp>
#include <boost/type_erasure/detail/rebind_placeholders.hpp>
#include <boost/type_erasure/detail/normalize_deduced.hpp>
#include <boost/type_erasure/relaxed_match.hpp>
#include <boost/type_erasure/builtin.hpp>

namespace boost {
namespace type_erasure {

template<class F>
struct deduced;

template<class T, class U>
struct same_type;

namespace detail {

struct substitution_map_tag {};

template<class M>
struct substitution_map
{
    typedef substitution_map_tag tag;
    typedef M map_type;
};

}
}

namespace mpl {

template<>
struct at_impl< ::boost::type_erasure::detail::substitution_map_tag>
{
    template<class Seq, class Key>
    struct apply
    {
        typedef typename ::boost::mpl::eval_if<
            ::boost::mpl::has_key<typename Seq::map_type, Key>,
            ::boost::mpl::at<typename Seq::map_type, Key>,
            ::boost::mpl::identity<Key>
        >::type type;
    };
};

template<>
struct has_key_impl< ::boost::type_erasure::detail::substitution_map_tag>
{
    template<class Seq, class Key>
    struct apply : boost::mpl::true_
    {};
};

}

namespace type_erasure {
namespace detail {

template<class Out, class T>
struct insert_concept
{
    typedef ::boost::mpl::pair<
        typename ::boost::mpl::insert<typename Out::first, T>::type,
        typename Out::second
    > type;
};

template<class M, class T>
struct resolve_same_type
{
    typedef typename ::boost::mpl::eval_if< ::boost::mpl::has_key<M, T>,
        ::boost::type_erasure::detail::resolve_same_type<
            M,
            typename ::boost::mpl::at<M, T>::type
        >,
        ::boost::mpl::identity<T>
    >::type type;
};

template<class T, class U>
struct select_pair
{
    BOOST_MPL_ASSERT((::boost::is_same<T, U>));
    typedef void type;
};

template<class T, class U>
struct select_pair<T, ::boost::type_erasure::deduced<U> >
{
    typedef ::boost::mpl::pair< ::boost::type_erasure::deduced<U>, T> type;
};

template<class T, class U>
struct select_pair< ::boost::type_erasure::deduced<T>, U>
{
    typedef ::boost::mpl::pair< ::boost::type_erasure::deduced<T>, U> type;
};

template<class T, class U>
struct select_pair<
    ::boost::type_erasure::deduced<T>,
    ::boost::type_erasure::deduced<U>
>
{
    typedef ::boost::mpl::pair<
        ::boost::type_erasure::deduced<T>,
        ::boost::type_erasure::deduced<U>
    > type;
};

template<class M, class T>
struct normalize_placeholder
{
    typedef typename ::boost::mpl::eval_if< ::boost::mpl::has_key<M, T>,
        ::boost::type_erasure::detail::normalize_placeholder<
            M,
            typename ::boost::mpl::at<M, T>::type
        >,
        ::boost::mpl::identity<T>
    >::type type;
};

template<class M, class T>
struct normalize_placeholder<M, ::boost::type_erasure::deduced<T> >
{
    typedef typename ::boost::mpl::eval_if< ::boost::mpl::has_key<M, T>,
        ::boost::type_erasure::detail::normalize_placeholder<
            M,
            typename ::boost::mpl::at<M, T>::type
        >,
        ::boost::type_erasure::detail::normalize_deduced<
            M,
            T
        >
    >::type type;
};

template<class M>
struct create_placeholder_map
{
    typedef typename ::boost::mpl::fold<
        M,
        ::boost::mpl::map0<>,
        ::boost::mpl::insert<
            ::boost::mpl::_1,
            ::boost::mpl::pair<
                ::boost::mpl::first< ::boost::mpl::_2>,
                ::boost::type_erasure::detail::normalize_placeholder<M, ::boost::mpl::second< ::boost::mpl::_2> >
            >
        >
    >::type type;
};

template<class Bindings, class P, class Out, class Sub>
struct convert_deduced
{
    typedef typename ::boost::type_erasure::detail::rebind_placeholders_in_argument<
        typename P::first,
        Bindings
    >::type result1;
    typedef typename ::boost::mpl::at<Sub, result1>::type result;
    typedef typename ::boost::mpl::eval_if<
        ::boost::mpl::has_key<Out, typename P::second>,
        ::boost::mpl::identity<Out>,
        ::boost::mpl::insert<Out, ::boost::mpl::pair<typename P::second, result> >
    >::type type;
    BOOST_MPL_ASSERT((boost::is_same<typename ::boost::mpl::at<type, typename P::second>::type, result>));
};

template<class Bindings, class M, class Sub>
struct convert_deductions
{
    typedef typename ::boost::mpl::fold<
        M,
        Bindings,
        ::boost::type_erasure::detail::convert_deduced<
            Bindings, ::boost::mpl::_2, ::boost::mpl::_1, Sub
        >
    >::type type;
};

template<class Bindings, class P, class Out>
struct add_deduced
{
    typedef typename ::boost::type_erasure::detail::rebind_placeholders_in_argument<
        typename P::first,
        Bindings
    >::type result;
    typedef typename ::boost::mpl::eval_if<
        ::boost::mpl::has_key<Out, typename P::second>,
        ::boost::mpl::identity<Out>,
        ::boost::mpl::insert<Out, ::boost::mpl::pair<typename P::second, result> >
    >::type type;
    BOOST_MPL_ASSERT((boost::is_same<typename ::boost::mpl::at<type, typename P::second>::type, result>));
};

template<class Bindings, class M>
struct add_deductions
{
    typedef typename ::boost::mpl::fold<
        M,
        Bindings,
        ::boost::type_erasure::detail::add_deduced<
            Bindings, ::boost::mpl::_2, ::boost::mpl::_1
        >
    >::type type;
};

template<class Out, class T, class U>
struct insert_concept<Out, ::boost::type_erasure::same_type<T, U> >
{
    typedef typename ::boost::type_erasure::detail::resolve_same_type<
        typename Out::second,
        T
    >::type t1;
    typedef typename ::boost::type_erasure::detail::resolve_same_type<
        typename Out::second,
        U
    >::type t2;
    typedef ::boost::mpl::pair<
        typename Out::first,
        typename ::boost::mpl::eval_if<
            ::boost::is_same<t1, t2>,
            ::boost::mpl::identity<typename Out::second>,
            ::boost::mpl::insert<
                typename Out::second,
                typename ::boost::type_erasure::detail::select_pair<
                    t1,
                    t2
                >::type
            >
        >::type
    > type;
};

template<class Concept, class Out = ::boost::mpl::pair< ::boost::mpl::set0<>, ::boost::mpl::map0<> > >
struct normalize_concept_impl
{
    typedef typename ::boost::mpl::eval_if< ::boost::mpl::is_sequence<Concept>,
        ::boost::mpl::fold<Concept, Out, normalize_concept_impl< ::boost::mpl::_2, ::boost::mpl::_1> >,
        ::boost::type_erasure::detail::insert_concept<Out, Concept>
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
struct get_placeholder_normalization_map
{
    typedef typename ::boost::type_erasure::detail::create_placeholder_map<
        typename normalize_concept_impl<Concept>::type::second
    >::type type;
};

template<class Concept>
struct normalize_concept
{
    typedef typename normalize_concept_impl<Concept>::type impl;
    typedef typename ::boost::type_erasure::detail::create_placeholder_map<
        typename impl::second
    >::type substitutions;
    typedef typename ::boost::mpl::fold<
        typename impl::first,
        ::boost::mpl::set0<>,
        ::boost::mpl::insert<
            ::boost::mpl::_1,
            ::boost::type_erasure::detail::rebind_placeholders<
                ::boost::mpl::_2,
                ::boost::type_erasure::detail::substitution_map<substitutions>
            >
        >
    >::type basic;
    typedef typename ::boost::mpl::copy<
        typename ::boost::mpl::eval_if<
            ::boost::type_erasure::is_relaxed<Concept>,
            ::boost::type_erasure::detail::add_typeinfo<basic>,
            ::boost::mpl::identity<basic>
        >::type,
        ::boost::mpl::back_inserter< ::boost::mpl::vector0<> >
    >::type type;
};

template<
    class Concept,
    class Map = typename ::boost::type_erasure::detail::create_placeholder_map<
            typename ::boost::type_erasure::detail::normalize_concept_impl<
                Concept
            >::type::second
        >::type,
    class Out = ::boost::mpl::set0<>
>
struct collect_concepts
{
    typedef typename ::boost::type_erasure::detail::rebind_placeholders<
        Concept,
        ::boost::type_erasure::detail::substitution_map<Map>
    >::type transformed;
    typedef typename ::boost::mpl::eval_if<
        ::boost::is_same<transformed, void>,
        ::boost::mpl::identity<Out>,
        ::boost::mpl::insert<
            Out,
            transformed
        >
    >::type type1;
    typedef typename ::boost::mpl::eval_if< ::boost::mpl::is_sequence<Concept>,
        ::boost::mpl::fold<Concept, type1, collect_concepts< ::boost::mpl::_2, Map, ::boost::mpl::_1> >,
        ::boost::mpl::identity<type1>
    >::type type;
};

}
}
}

#endif
