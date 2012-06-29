// Boost.TypeErasure library
//
// Copyright 2012 Steven Watanabe
//
// Distributed under the Boost Software License Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// $Id$

#ifndef BOOST_TYPE_ERASURE_MEMBER_HPP_INCLUDED
#define BOOST_TYPE_ERASURE_MEMBER_HPP_INCLUDED

#include <boost/preprocessor/cat.hpp>
#include <boost/preprocessor/dec.hpp>
#include <boost/preprocessor/comma_if.hpp>
#include <boost/preprocessor/if.hpp>
#include <boost/preprocessor/comparison/not_equal.hpp>
#include <boost/preprocessor/repetition/enum.hpp>
#include <boost/preprocessor/repetition/enum_params.hpp>
#include <boost/preprocessor/repetition/enum_trailing_params.hpp>
#include <boost/preprocessor/repetition/enum_trailing_binary_params.hpp>
#include <boost/preprocessor/seq/for_each.hpp>
#include <boost/preprocessor/seq/pop_back.hpp>
#include <boost/preprocessor/seq/size.hpp>
#include <boost/preprocessor/seq/elem.hpp>
#include <boost/preprocessor/tuple/eat.hpp>

/** INTERNAL ONLY */
#define BOOST_TYPE_ERASURE_OPEN_NAMESPACE_F(z, data, x) \
    namespace x {

/** INTERNAL ONLY */
#define BOOST_TYPE_ERASURE_OPEN_NAMEPACE_I(seq)\
    BOOST_PP_SEQ_FOR_EACH(BOOST_TYPE_ERASURE_OPEN_NAMESPACE_F, ~, BOOST_PP_SEQ_POP_BACK(seq))

/** INTERNAL ONLY */
#define BOOST_TYPE_ERASURE_OPEN_NAMESPACE(seq)\
    BOOST_PP_IF(BOOST_PP_NOT_EQUAL(BOOST_PP_SEQ_SIZE(seq), 1), BOOST_TYPE_ERASURE_OPEN_NAMEPACE_I, BOOST_PP_TUPLE_EAT(1))(seq)

/** INTERNAL ONLY */
#define BOOST_TYPE_ERASURE_CLOSE_NAMESPACE(seq) \
    BOOST_PP_REPEAT(BOOST_PP_DEC(BOOST_PP_SEQ_SIZE(seq)), } BOOST_PP_TUPLE_EAT(3), ~)

/** INTERNAL ONLY */
#define BOOST_TYPE_ERASURE_MEMBER_ARG(z, n, data)  \
    typename ::boost::type_erasure::rebind_any<Base, BOOST_PP_CAT(A, n)>::type BOOST_PP_CAT(a, n)

/** INTERNAL ONLY */
#define BOOST_TYPE_ERASURE_QUALIFIED_NAME_F(z, data, x)\
    ::x

/** INTERNAL ONLY */
#define BOOST_TYPE_ERASURE_QUALIFIED_NAME(seq) \
    BOOST_PP_SEQ_FOR_EACH(BOOST_TYPE_ERASURE_QUALIFIED_NAME_F, ~, seq)

/** INTERNAL ONLY */
#define BOOST_TYPE_ERASURE_QUALIFIED_ID(seq, N) \
    BOOST_TYPE_ERASURE_QUALIFIED_NAME(seq)<R(BOOST_PP_ENUM_PARAMS(N, A)), T>

/** INTERNAL ONLY */
#define BOOST_TYPE_ERASURE_QUALIFIED_ID_C(seq, N) \
    BOOST_TYPE_ERASURE_QUALIFIED_NAME(seq)<R(BOOST_PP_ENUM_PARAMS(N, A)), const T>

/** INTERNAL ONLY */
#define BOOST_TYPE_ERASURE_MEMBER_II(qual_name, concept_name, member, N)                        \
    BOOST_TYPE_ERASURE_OPEN_NAMESPACE(qual_name)                                                \
    template<class Sig, class T = ::boost::type_erasure::_self>                                 \
    struct concept_name;                                                                        \
    template<class R BOOST_PP_ENUM_TRAILING_PARAMS(N, class A), class T>                        \
    struct concept_name<R(BOOST_PP_ENUM_PARAMS(N, A)), T> {                                     \
        static R apply(T& t BOOST_PP_ENUM_TRAILING_BINARY_PARAMS(N, A, a))                      \
        { return t.member(BOOST_PP_ENUM_PARAMS(N, a)); }                                        \
    };                                                                                          \
    template<BOOST_PP_ENUM_PARAMS(N, class A) BOOST_PP_COMMA_IF(N) class T>                     \
    struct concept_name<void(BOOST_PP_ENUM_PARAMS(N, A)), T> {                                  \
        static void apply(T& t BOOST_PP_ENUM_TRAILING_BINARY_PARAMS(N, A, a))                   \
        { t.member(BOOST_PP_ENUM_PARAMS(N, a)); }                                               \
    };                                                                                          \
    BOOST_TYPE_ERASURE_CLOSE_NAMESPACE(qual_name)                                               \
    namespace boost {                                                                           \
    namespace type_erasure {                                                                    \
    template<                                                                                   \
        class R BOOST_PP_ENUM_TRAILING_PARAMS(N, class A),                                      \
        class T, class Base, class Enable>                                                      \
    struct concept_interface<                                                                   \
        BOOST_TYPE_ERASURE_QUALIFIED_ID(qual_name, N),                                          \
        Base, T,  Enable> : Base                                                                \
    {                                                                                           \
        typedef void BOOST_PP_CAT(_boost_type_erasure_has_member, member);                      \
        typename rebind_any<Base, R>::type member(                                              \
            BOOST_PP_ENUM(N, BOOST_TYPE_ERASURE_MEMBER_ARG, ~))                                 \
        {                                                                                       \
            return ::boost::type_erasure::call(                                                 \
                BOOST_TYPE_ERASURE_QUALIFIED_ID(qual_name, N)(),                                \
                *this BOOST_PP_ENUM_TRAILING_PARAMS(N, a));                                     \
        }                                                                                       \
    };                                                                                          \
    template<                                                                                   \
        class R BOOST_PP_ENUM_TRAILING_PARAMS(N, class A),                                      \
        class T, class Base, class Enable>                                                      \
    struct concept_interface<                                                                   \
        BOOST_TYPE_ERASURE_QUALIFIED_ID_C(qual_name, N),                                        \
        Base, T, Enable> : Base                                                                 \
    {                                                                                           \
        typedef void BOOST_PP_CAT(_boost_type_erasure_has_member, member);                      \
        typename rebind_any<Base, R>::type member(                                              \
            BOOST_PP_ENUM(N, BOOST_TYPE_ERASURE_MEMBER_ARG, ~)) const                           \
        {                                                                                       \
            return ::boost::type_erasure::call(                                                 \
                BOOST_TYPE_ERASURE_QUALIFIED_ID_C(qual_name, N)(),                              \
                *this BOOST_PP_ENUM_TRAILING_PARAMS(N, a));                                     \
        }                                                                                       \
    };                                                                                          \
    template<class R BOOST_PP_ENUM_TRAILING_PARAMS(N, class A), class T, class Base>            \
    struct concept_interface<                                                                   \
        BOOST_TYPE_ERASURE_QUALIFIED_ID(qual_name, N),                                          \
        Base, T, typename Base::BOOST_PP_CAT(_boost_type_erasure_has_member, member)> : Base    \
    {                                                                                           \
        using Base::member;                                                                     \
        typename rebind_any<Base, R>::type member(                                              \
            BOOST_PP_ENUM(N, BOOST_TYPE_ERASURE_MEMBER_ARG, ~))                                 \
        {                                                                                       \
            return ::boost::type_erasure::call(                                                 \
                BOOST_TYPE_ERASURE_QUALIFIED_ID(qual_name, N)(),                                \
                *this BOOST_PP_ENUM_TRAILING_PARAMS(N, a));                                     \
        }                                                                                       \
    };                                                                                          \
    template<class R BOOST_PP_ENUM_TRAILING_PARAMS(N, class A), class T, class Base>            \
    struct concept_interface<                                                                   \
        BOOST_TYPE_ERASURE_QUALIFIED_ID_C(qual_name, N),                                        \
        Base, T,                                                                                \
        typename Base::BOOST_PP_CAT(_boost_type_erasure_has_member, member)> : Base             \
    {                                                                                           \
        using Base::member;                                                                     \
        typename rebind_any<Base, R>::type member(                                              \
            BOOST_PP_ENUM(N, BOOST_TYPE_ERASURE_MEMBER_ARG, ~)) const                           \
        {                                                                                       \
            return ::boost::type_erasure::call(                                                 \
                BOOST_TYPE_ERASURE_QUALIFIED_ID_C(qual_name, N)(),                              \
                *this BOOST_PP_ENUM_TRAILING_PARAMS(N, a));                                     \
        }                                                                                       \
    };                                                                                          \
    }}

/** INTERNAL ONLY */
#define BOOST_TYPE_ERASURE_MEMBER_I(namespace_name, concept_name, member, N)\
    BOOST_TYPE_ERASURE_MEMBER_II(namespace_name, concept_name, member, N)

/**
 * \brief Defines a primitive concept for a member function.
 *
 * \param qualified_name should be a preprocessor sequence
 * of the form (namespace1)(namespace2)...(concept_name).
 * \param member is the name of the member function.
 * \param N is the number of arguments of the function.
 *
 * The declaration of the concept is
 * \code
 * template<class Sig, class T = _self>
 * struct ::namespace1::namespace2::...::concept_name;
 * \endcode
 * where Sig is a function type giving the
 * signature of the member function, and T is the
 * object type.  T may be const-qualified for
 * const member functions.
 *
 * This macro can only be used in the global namespace.
 *
 * Example:
 *
 * \code
 * BOOST_TYPE_ERASURE_MEMBER((boost)(has_push_back), push_back, 1)
 * \endcode
 */
#define BOOST_TYPE_ERASURE_MEMBER(qualified_name, member, N)                                \
    BOOST_TYPE_ERASURE_MEMBER_I(                                                            \
        qualified_name,                                                                     \
        BOOST_PP_SEQ_ELEM(BOOST_PP_DEC(BOOST_PP_SEQ_SIZE(qualified_name)), qualified_name), \
        member,                                                                             \
        N)

#endif
