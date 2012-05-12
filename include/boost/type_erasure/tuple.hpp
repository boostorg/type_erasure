// Boost.TypeErasure library
//
// Copyright 2011 Steven Watanabe
//
// Distributed under the Boost Software License Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// $Id$

#if !defined(BOOST_PP_IS_ITERATING)

#ifndef BOOST_TYPE_ERASURE_TUPLE_HPP_INCLUDED
#define BOOST_TYPE_ERASURE_TUPLE_HPP_INCLUDED

#include <boost/mpl/int.hpp>
#include <boost/mpl/minus.hpp>
#include <boost/mpl/equal_to.hpp>
#include <boost/mpl/map.hpp>
#include <boost/fusion/include/category_of.hpp>
#include <boost/fusion/include/iterator_facade.hpp>
#include <boost/fusion/include/sequence_facade.hpp>
#include <boost/preprocessor/cat.hpp>
#include <boost/preprocessor/iteration/iterate.hpp>
#include <boost/preprocessor/repetition/repeat.hpp>
#include <boost/preprocessor/repetition/enum.hpp>
#include <boost/preprocessor/repetition/enum_params.hpp>
#include <boost/preprocessor/repetition/enum_trailing_params.hpp>
#include <boost/preprocessor/repetition/enum_binary_params.hpp>
#include <boost/preprocessor/repetition/enum_trailing_binary_params.hpp>
#include <boost/preprocessor/repetition/enum_params_with_a_default.hpp>
#include <boost/type_erasure/any.hpp>
#include <boost/type_erasure/static_binding.hpp>
#include <boost/type_erasure/config.hpp>

namespace boost {
namespace type_erasure {

/** INTERNAL ONLY */
struct na {};

namespace detail {

template<int N, class Tuple>
struct get_impl;

template<class Concept,
    BOOST_PP_ENUM_PARAMS_WITH_A_DEFAULT(
        BOOST_TYPE_ERASURE_MAX_TUPLE_SIZE, class T, ::boost::type_erasure::na)>
struct tuple_storage;

}

/** INTERNAL ONLY */
template<class Tuple, int N>
class tuple_iterator :
    public ::boost::fusion::iterator_facade<
        tuple_iterator<Tuple, N>,
        ::boost::fusion::random_access_traversal_tag
    >
{
public:
    typedef ::boost::mpl::int_<N> index;
    explicit tuple_iterator(Tuple& t_arg) : t(&t_arg) {}
    template<class It>
    struct value_of
    {
        typedef typename ::boost::type_erasure::detail::get_impl<
            It::index::value,
            Tuple
        >::value_type type;
    };
    template<class It>
    struct deref :
        ::boost::type_erasure::detail::get_impl<It::index::value, Tuple>
    {
        typedef typename ::boost::type_erasure::detail::get_impl<
            It::index::value,
            Tuple
        >::type type;
        static type call(It it)
        {
            return ::boost::type_erasure::detail::get_impl<
                It::index::value,
                Tuple
            >::call(*it.t);
        }
    };
    template<class It, class M>
    struct advance
    {
        typedef tuple_iterator<Tuple, (It::index::value+M::value)> type;
        static type call(It it) { return type(*it.t); }
    };
    template<class It>
    struct next : advance<It, ::boost::mpl::int_<1> > {};
    template<class It>
    struct prior : advance<It, ::boost::mpl::int_<-1> > {};
    template<class It1, class It2>
    struct distance
    {
        typedef typename ::boost::mpl::minus<
            typename It2::index,
            typename It1::index
        >::type type;
        static type call(It1, It2) { return type(); }
    };
private:
    Tuple* t;
};

/** INTERNAL ONLY */
template<class Derived>
struct tuple_base :
    ::boost::fusion::sequence_facade<
        Derived,
        ::boost::fusion::random_access_traversal_tag
    >
{
    template<class Seq>
    struct begin
    {
        typedef ::boost::type_erasure::tuple_iterator<Seq, 0> type;
        static type call(Seq& seq) { return type(seq); }
    };
    template<class Seq>
    struct end
    {
        typedef ::boost::type_erasure::tuple_iterator<
            Seq,
            Seq::tuple_size::value
        > type;
        static type call(Seq& seq) { return type(seq); }
    };
    template<class Seq>
    struct size
    {
        typedef typename Seq::tuple_size type;
        static type call(Seq& seq) { return type(); }
    };
    template<class Seq>
    struct empty
    {
        typedef typename boost::mpl::equal_to<
            typename Seq::tuple_size,
            boost::mpl::int_<0>
        >::type type;
        static type call(Seq& seq) { return type(); }
    };
    template<class Seq, class N>
    struct at : ::boost::type_erasure::detail::get_impl<N::value, Seq> {};
    template<class Seq, class N>
    struct value_at
    {
        typedef typename ::boost::type_erasure::detail::get_impl<
            N::value,
            Seq
        >::value_type type;
    };
};

#ifdef BOOST_TYPE_ERASURE_DOXYGEN

/**
 * @ref tuple is a Boost.Fusion Random Access Sequence containing
 * @ref any "anys". Concept is interpreted in the same way as for
 * @ref any.  The remaining arguments must be (possibly const
 * and/or reference qualified) placeholders.
 */
template<class Concept, class... T>
class tuple
{
public:
    /**
     * Constructs a tuple.  Each element of @c args will
     * be used to initialize the corresponding member.
     */
    template<class... U>
    explicit tuple(U&&... args);
};

template<int N, class Concept, class... T>
any<Concept, TN>& get(tuple<Concept, T...>& arg);
template<int N, class Concept, class... T>
const any<Concept, TN>& get(const tuple<Concept, T...>& arg);

#else

template<class Concept,
    BOOST_PP_ENUM_PARAMS_WITH_A_DEFAULT(
        BOOST_TYPE_ERASURE_MAX_TUPLE_SIZE, class T, ::boost::type_erasure::na)>
class tuple;

template<
    int N,
    class Concept
    BOOST_PP_ENUM_TRAILING_PARAMS(BOOST_TYPE_ERASURE_MAX_TUPLE_SIZE, class T)
>
typename detail::get_impl<
    N,
    tuple<
        Concept
        BOOST_PP_ENUM_TRAILING_PARAMS(BOOST_TYPE_ERASURE_MAX_TUPLE_SIZE, T)
    >
>::type get(
    tuple<
        Concept
        BOOST_PP_ENUM_TRAILING_PARAMS(BOOST_TYPE_ERASURE_MAX_TUPLE_SIZE, T)
    >& arg)
{
    return detail::get_impl<
        N,
        tuple<
            Concept
            BOOST_PP_ENUM_TRAILING_PARAMS(BOOST_TYPE_ERASURE_MAX_TUPLE_SIZE, T)
        >
    >::call(arg);
}

template<
    int N,
    class Concept
    BOOST_PP_ENUM_TRAILING_PARAMS(BOOST_TYPE_ERASURE_MAX_TUPLE_SIZE, class T)
>
typename detail::get_impl<
    N,
    const tuple<
        Concept
        BOOST_PP_ENUM_TRAILING_PARAMS(BOOST_TYPE_ERASURE_MAX_TUPLE_SIZE, T)
    >
>::type get(
    const tuple<
        Concept
        BOOST_PP_ENUM_TRAILING_PARAMS(BOOST_TYPE_ERASURE_MAX_TUPLE_SIZE, T)
    >& arg)
{
    return detail::get_impl<
        N,
        const tuple<
            Concept
            BOOST_PP_ENUM_TRAILING_PARAMS(BOOST_TYPE_ERASURE_MAX_TUPLE_SIZE, T)
        >
    >::call(arg);
}

#endif
    
/** INTERNAL ONLY */
#define BOOST_PP_FILENAME_1 <boost/type_erasure/tuple.hpp>
/** INTERNAL ONLY */
#define BOOST_PP_ITERATION_LIMITS (0, BOOST_TYPE_ERASURE_MAX_TUPLE_SIZE)
#include BOOST_PP_ITERATE()

}
}

#endif

#else

#define N BOOST_PP_ITERATION()

#define BOOST_TYPE_ERASURE_TAG_TYPEDEF(z, n, data)                          \
    typedef BOOST_PP_CAT(T, n) BOOST_PP_CAT(tag_type, n);                   \
    typedef typename ::boost::remove_reference<BOOST_PP_CAT(T, n)>::type    \
        BOOST_PP_CAT(tag, n);

#define BOOST_TYPE_ERASURE_PAIR(z, n, data) \
    ::boost::mpl::pair<BOOST_PP_CAT(tag, n), BOOST_PP_CAT(U, n)>

#define BOOST_TYPE_ERASURE_CONSTRUCT(z, n, data)\
    BOOST_PP_CAT(t, n)(BOOST_PP_CAT(u, n), table)

#define BOOST_TYPE_ERASURE_MEMBER(z, n, data)\
    ::boost::type_erasure::any<Concept, BOOST_PP_CAT(T, n)> BOOST_PP_CAT(t, n);

#if N == 1
#define BOOST_TYPE_ERASURE_EXPLICIT explicit
#else
#define BOOST_TYPE_ERASURE_EXPLICIT
#endif

namespace detail {
    
template<class Concept BOOST_PP_ENUM_TRAILING_PARAMS(N, class T)>
struct tuple_storage
#if N != BOOST_TYPE_ERASURE_MAX_TUPLE_SIZE
    <Concept BOOST_PP_ENUM_TRAILING_PARAMS(N, T)>
#endif
{
#if N
    template<class Table BOOST_PP_ENUM_TRAILING_PARAMS(N, class U)>
    tuple_storage(
        const Table& table BOOST_PP_ENUM_TRAILING_BINARY_PARAMS(N, U, &u))
        :
        BOOST_PP_ENUM(N, BOOST_TYPE_ERASURE_CONSTRUCT, ~) {}
#else
    template<class Table>
    explicit tuple_storage(const Table&) {}
#endif
    BOOST_PP_REPEAT(N, BOOST_TYPE_ERASURE_MEMBER, `)
};

#if N != BOOST_TYPE_ERASURE_MAX_TUPLE_SIZE

template<class Tuple>
struct get_impl<N, Tuple>
{
    typedef any<
        typename Tuple::concept_type,
        typename Tuple::BOOST_PP_CAT(tag_type, N)
    > value_type;
    typedef value_type& type;
    static type call(Tuple& arg)
    { return arg.impl.BOOST_PP_CAT(t, N); }
};

template<class Tuple>
struct get_impl<N, const Tuple>
{
    typedef any<
        typename Tuple::concept_type,
        typename Tuple::BOOST_PP_CAT(tag_type, N)
    > value_type;
    typedef const value_type& type;
    static type call(const Tuple& arg)
    { return arg.impl.BOOST_PP_CAT(t, N); }
};

#endif

}

template<class Concept BOOST_PP_ENUM_TRAILING_PARAMS(N, class T)>
class tuple 
#if N != BOOST_TYPE_ERASURE_MAX_TUPLE_SIZE
    <Concept BOOST_PP_ENUM_TRAILING_PARAMS(N, T)>
#endif
    : public tuple_base<tuple<Concept BOOST_PP_ENUM_TRAILING_PARAMS(N, T)> >
{
    typedef Concept concept_type;
    BOOST_PP_REPEAT(N, BOOST_TYPE_ERASURE_TAG_TYPEDEF, ~)
public:
    typedef ::boost::mpl::int_<N> tuple_size;
#if N
    template<BOOST_PP_ENUM_PARAMS(N, class U)>
#endif
    BOOST_TYPE_ERASURE_EXPLICIT
    tuple(BOOST_PP_ENUM_BINARY_PARAMS(N, U, &u)) :
        impl(
            ::boost::type_erasure::make_binding<
                ::boost::mpl::map<
                    BOOST_PP_ENUM(N, BOOST_TYPE_ERASURE_PAIR, ~)
                >
            >()
            BOOST_PP_ENUM_TRAILING_PARAMS(N, u)
        )
    {}
#if N
    template<BOOST_PP_ENUM_PARAMS(N, class U)>
    BOOST_TYPE_ERASURE_EXPLICIT 
    tuple(BOOST_PP_ENUM_BINARY_PARAMS(N, const U, &u)) :
        impl(
            ::boost::type_erasure::make_binding<
                ::boost::mpl::map<
                    BOOST_PP_ENUM(N, BOOST_TYPE_ERASURE_PAIR, ~)
                >
            >()
            BOOST_PP_ENUM_TRAILING_PARAMS(N, u)
        )
    {}
#endif
private:
    template<int M, class Tuple>
    friend struct ::boost::type_erasure::detail::get_impl;
    ::boost::type_erasure::detail::tuple_storage<
        Concept
        BOOST_PP_ENUM_TRAILING_PARAMS(N, T)
    > impl;
};

#undef BOOST_TYPE_ERASURE_EXPLICIT
#undef BOOST_TYPE_ERASURE_MEMBER
#undef BOOST_TYPE_ERASURE_CONSTRUCT
#undef BOOST_TYPE_ERASURE_PAIR
#undef BOOST_TYPE_ERASURE_TAG_TYPEDEF
#undef N

#endif
