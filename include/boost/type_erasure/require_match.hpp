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

#ifndef BOOST_TYPE_ERASURE_REQUIRE_MATCH_HPP_INCLUDED
#define BOOST_TYPE_ERASURE_REQUIRE_MATCH_HPP_INCLUDED

#include <stdexcept>
#include <boost/throw_exception.hpp>
#include <boost/mpl/bool.hpp>
#include <boost/mpl/and.hpp>
#include <boost/type_traits/is_same.hpp>
#include <boost/preprocessor/cat.hpp>
#include <boost/preprocessor/facilities/intercept.hpp>
#include <boost/preprocessor/iteration/iterate.hpp>
#include <boost/preprocessor/repetition/repeat.hpp>
#include <boost/preprocessor/repetition/enum_params.hpp>
#include <boost/preprocessor/repetition/enum_trailing_params.hpp>
#include <boost/preprocessor/repetition/enum_trailing_binary_params.hpp>
#include <boost/type_erasure/detail/extract_concept.hpp>
#include <boost/type_erasure/relaxed_match.hpp>
#include <boost/type_erasure/check_match.hpp>

namespace boost {
namespace type_erasure {

template<class Concept>
class binding;

/**
 * Exception thrown when the arguments to a primitive concept
 * are incorrect.
 */
struct bad_function_call : ::std::invalid_argument
{
    bad_function_call() : ::std::invalid_argument("bad_function_call") {}
};

#ifdef BOOST_TYPE_ERASURE_DOXYGEN

/**
 * Checks that the actual types stored in all the @ref any
 * arguments match the types specified by @c binding.  If
 * they do not match then,
 * - If @ref relaxed_match is in @c Concept, throws @ref bad_function_call.
 * - Otherwise the behavior is undefined.
 *
 * If @c binding is not specified, it will be deduced from
 * the arguments.
 *
 * \post @c call(binding, concept, args...) is valid.
 */
template<class Concept, class Op, class... U>
void require_match(const binding<Concept>& binding_arg, const Op& f, U&&... args);

/**
 * \overload
 */
template<class Op, class... U>
void require_match(const Op& f, U&&... args);

#else

#if !defined(BOOST_NO_VARIADIC_TEMPLATES)

namespace detail {

template<class Concept, class Op, class... U>
void require_match_impl(
    ::boost::mpl::true_,
    const ::boost::type_erasure::binding<Concept>& table,
    const Op& op,
    U&... arg)
{
    if(!::boost::type_erasure::check_match(table, op, arg...)) {
        BOOST_THROW_EXCEPTION(::boost::type_erasure::bad_function_call());
    }
}

template<class Concept, class Op, class... U>
void require_match_impl(
    ::boost::mpl::false_,
    const ::boost::type_erasure::binding<Concept>&,
    const Op&,
    U&...)
{}

template<class Op, class... U>
void require_match_impl(
    ::boost::mpl::true_,
    const Op& op,
    U&... arg)
{
    if(!::boost::type_erasure::check_match(op, arg...)) {
        BOOST_THROW_EXCEPTION(::boost::type_erasure::bad_function_call());
    }
}

template<class Op, class... U>
void require_match_impl(
    ::boost::mpl::false_,
    const Op&,
    U&...)
{}

}

template<class Concept, class Op, class... U>
void require_match(
    const ::boost::type_erasure::binding<Concept>& table,
    const Op& op,
    U&... arg)
{
    ::boost::type_erasure::is_relaxed<Concept> cond;
    ::boost::type_erasure::detail::require_match_impl(cond, table, op, arg...);
}

template<class Op, class... U>
void require_match(
    const Op& op,
    U&... arg)
{
    ::boost::type_erasure::is_relaxed<
        typename ::boost::type_erasure::detail::extract_concept<
            typename ::boost::type_erasure::detail::get_signature<Op>::type,
            U...>::type
    > cond;
    ::boost::type_erasure::detail::require_match_impl(cond, op, arg...);
}

#else

#define BOOST_PP_FILENAME_1 <boost/type_erasure/require_match.hpp>
#define BOOST_PP_ITERATION_LIMITS (0, BOOST_TYPE_ERASURE_MAX_ARITY)
#include BOOST_PP_ITERATE()

#endif

#endif

}
}

#endif

#else

#define N BOOST_PP_ITERATION()

namespace detail {

template<
    class Concept,
    class Op
    BOOST_PP_ENUM_TRAILING_PARAMS(N, class U)
>
void require_match_impl(
    ::boost::mpl::true_,
    const ::boost::type_erasure::binding<Concept>& table,
    const Op& op
    BOOST_PP_ENUM_TRAILING_BINARY_PARAMS(N, U, & arg))
{
    if(!::boost::type_erasure::check_match
        (table, op BOOST_PP_ENUM_TRAILING_PARAMS(N, arg))) {
        BOOST_THROW_EXCEPTION(::boost::type_erasure::bad_function_call());
    }
}

template<
    class Concept,
    class Op
    BOOST_PP_ENUM_TRAILING_PARAMS(N, class U)
>
void require_match_impl(
    ::boost::mpl::false_,
    const ::boost::type_erasure::binding<Concept>&,
    const Op&
    BOOST_PP_ENUM_TRAILING_BINARY_PARAMS(N, U, & BOOST_PP_INTERCEPT))
{}

#if N != 0

template<
    class Op
    BOOST_PP_ENUM_TRAILING_PARAMS(N, class U)
>
void require_match_impl(
    ::boost::mpl::true_,
    const Op& op
    BOOST_PP_ENUM_TRAILING_BINARY_PARAMS(N, U, & arg))
{
    if(!::boost::type_erasure::check_match
        (op BOOST_PP_ENUM_TRAILING_PARAMS(N, arg))) {
        BOOST_THROW_EXCEPTION(::boost::type_erasure::bad_function_call());
    }
}

template<
    class Op
    BOOST_PP_ENUM_TRAILING_PARAMS(N, class U)
>
void require_match_impl(
    ::boost::mpl::false_,
    const Op&
    BOOST_PP_ENUM_TRAILING_BINARY_PARAMS(N, U, & BOOST_PP_INTERCEPT))
{}

#endif

}

template<
    class Concept,
    class Op
    BOOST_PP_ENUM_TRAILING_PARAMS(N, class U)
>
void require_match(
    const ::boost::type_erasure::binding<Concept>& table,
    const Op& op
    BOOST_PP_ENUM_TRAILING_BINARY_PARAMS(N, U, & arg))
{
    ::boost::type_erasure::is_relaxed<Concept> cond;
    ::boost::type_erasure::detail::require_match_impl
        (cond, table, op BOOST_PP_ENUM_TRAILING_PARAMS(N, arg));
}

#if N != 0

template<
    class Op
    BOOST_PP_ENUM_TRAILING_PARAMS(N, class U)
>
void require_match(
    const Op& op
    BOOST_PP_ENUM_TRAILING_BINARY_PARAMS(N, U, & arg))
{
    ::boost::type_erasure::is_relaxed<
        typename ::boost::type_erasure::detail::BOOST_PP_CAT(do_extract_concept, N)<
        typename ::boost::type_erasure::detail::get_signature<Op>::type,
        BOOST_PP_ENUM_PARAMS(N, U)>::type
    > cond;
    ::boost::type_erasure::detail::require_match_impl
        (cond, op BOOST_PP_ENUM_TRAILING_PARAMS(N, arg));
}

#endif

#undef N

#endif
