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

#ifndef BOOST_TYPE_ERASURE_CALL_HPP_INCLUDED
#define BOOST_TYPE_ERASURE_CALL_HPP_INCLUDED

#include <boost/assert.hpp>
#include <boost/mpl/bool.hpp>
#include <boost/mpl/eval_if.hpp>
#include <boost/mpl/identity.hpp>
#include <boost/type_traits/remove_cv.hpp>
#include <boost/type_traits/remove_reference.hpp>
#include <boost/preprocessor/cat.hpp>
#include <boost/preprocessor/inc.hpp>
#include <boost/preprocessor/iteration/iterate.hpp>
#include <boost/preprocessor/repetition/repeat.hpp>
#include <boost/preprocessor/repetition/enum.hpp>
#include <boost/preprocessor/repetition/enum_params.hpp>
#include <boost/preprocessor/repetition/enum_binary_params.hpp>
#include <boost/preprocessor/repetition/enum_trailing_params.hpp>
#include <boost/preprocessor/repetition/enum_trailing_binary_params.hpp>
#include <boost/type_erasure/detail/access.hpp>
#include <boost/type_erasure/detail/adapt_to_vtable.hpp>
#include <boost/type_erasure/detail/extract_concept.hpp>
#include <boost/type_erasure/detail/get_signature.hpp>
#include <boost/type_erasure/is_placeholder.hpp>
#include <boost/type_erasure/concept_of.hpp>
#include <boost/type_erasure/config.hpp>
#include <boost/type_erasure/require_match.hpp>

namespace boost {
namespace type_erasure {

template<class Concept, class Placeholder>
class any;

template<class Concept>
class binding;

namespace detail {

template<class T>
struct is_placeholder_arg :
    ::boost::type_erasure::is_placeholder<
        typename ::boost::remove_cv<
            typename ::boost::remove_reference<T>::type
        >::type
    >
{};

template<class T, class Table>
int maybe_get_table(const T& arg, const Table*& table, boost::mpl::true_)
{
    if(table == 0) {
        table = &::boost::type_erasure::detail::access::table(arg);
    }
    return 0;
}

template<class T, class Table>
int maybe_get_table(const T&, const Table*&, boost::mpl::false_) { return 0; }

template<class T>
::boost::type_erasure::detail::storage& convert_arg(any_base<T>& arg, boost::mpl::true_)
{
    return ::boost::type_erasure::detail::access::data(arg);
}

template<class Concept, class T>
const ::boost::type_erasure::detail::storage&
convert_arg(any_base<any<Concept, const T&> >& arg, boost::mpl::true_)
{
    return ::boost::type_erasure::detail::access::data(arg);
}

template<class T>
const ::boost::type_erasure::detail::storage&
convert_arg(const any_base<T>& arg, boost::mpl::true_)
{
    return ::boost::type_erasure::detail::access::data(arg);
}

template<class T>
T& convert_arg(T& arg, boost::mpl::false_) { return arg; }

}

#ifdef BOOST_TYPE_ERASURE_DOXYGEN

/**
 * Dispatches a type erased function.
 *
 * If @c binding is not specified, it will be deduced from
 * the arguments.  Naturally this requires at least one
 * argument to be an @ref any.
 *
 * @c Op must be a primitive concept which is present in
 * @c Concept.  Its signature determines how the arguments of
 * call are handled.  All the arguments that are placeholders
 * in the signature are unwrapped.  The types that they hold
 * must match the types specified by @c binding.  The other
 * arguments will be passed through unchanged.
 *
 * \return The result of the operation.  If the
 *         result type is a placeholder, the result will be
 *         converted to the appropriate @ref any type.
 *
 * Example:
 *
 * @code
 * typedef addable<_a, int, _b> concept;
 * any<concept, _a> a = ...;
 * any<concept, _b> b(call(concept(), a, 10));
 * @endcode
 *
 * The signature of @ref addable is <code>_b(const _a&, const int&)</code>
 * FIXME: This won't compile for lack of copy_constructible.
 */
template<class Concept, class Op, class... U>
typename ::boost::type_erasure::detail::call_impl<Sig, U..., Concept>::type
call(const binding<Concept>& binding_arg, const Op&, U&&... args);

/**
 * \overload
 */
template<class Op, class... U>
typename ::boost::type_erasure::detail::call_impl<Sig, U...>::type
call(const Op&, U&&... args);

#else

namespace detail {
    
template<class Sig, class Args, class Concept = void>
struct call_impl;

template<class Op, class Args, class Concept = void>
struct call_result :
    call_impl<
        typename ::boost::type_erasure::detail::get_signature<Op>::type,
        Args,
        Concept>
{};

template<class C1, class Args, class Concept>
struct call_result<
    ::boost::type_erasure::binding<C1>,
    Args,
    Concept
>
{};

}

#if !defined(BOOST_NO_VARIADIC_TEMPLATES)

namespace detail {

template<class... T>
void ignore(const T&...) {}

template<class R, class... T, class... U>
const ::boost::type_erasure::binding<
    typename ::boost::type_erasure::detail::extract_concept<void(T...), U...>::type>*
extract_table(R(*)(T...), const U&... arg)
{
    const ::boost::type_erasure::binding<
        typename ::boost::type_erasure::detail::extract_concept<
            void(T...), U...>::type>* result = 0;

    // the order that we run maybe_get_table in doesn't matter
    ignore(::boost::type_erasure::detail::maybe_get_table(
        arg,
        result,
        ::boost::type_erasure::detail::is_placeholder_arg<T>())...);

    BOOST_ASSERT(result != 0);
    return result;
}

template<class Sig, class Args, class Concept, bool ReturnsAny>
struct call_impl_dispatch;

template<class R, class... T, class... U, class Concept>
struct call_impl_dispatch<R(T...), void(U...), Concept, false>
{
    typedef R type;
    template<class F>
    static R apply(const ::boost::type_erasure::binding<Concept>* table, U... arg)
    {
        return table->template find<F>()(
            ::boost::type_erasure::detail::convert_arg(
                arg,
                ::boost::type_erasure::detail::is_placeholder_arg<T>())...);
    }
};

template<class R, class... T, class... U, class Concept>
struct call_impl_dispatch<R(T...), void(U...), Concept, true>
{
    typedef ::boost::type_erasure::any<Concept, R> type;
    template<class F>
    static type apply(const ::boost::type_erasure::binding<Concept>* table, U... arg)
    {
        return type(table->template find<F>()(
            ::boost::type_erasure::detail::convert_arg(
                arg,
                ::boost::type_erasure::detail::is_placeholder_arg<T>())...), *table);
    }
};

template<class R, class... T, class... U, class Concept>
struct call_impl<R(T...), void(U...), Concept> :
    ::boost::type_erasure::detail::call_impl_dispatch<
        R(T...),
        void(U...),
        Concept,
        ::boost::type_erasure::detail::is_placeholder_arg<R>::value
    >
{
};

template<class R, class... T, class... U>
struct call_impl<R(T...), void(U&...), void> :
    ::boost::type_erasure::detail::call_impl_dispatch<
        R(T...),
        void(U&...),
        typename ::boost::type_erasure::detail::extract_concept<
            void(T...),
            U...
        >::type,
        ::boost::type_erasure::detail::is_placeholder_arg<R>::value
    >
{
};

}

template<
    class Concept,
    class Op,
    class... U
>
typename ::boost::type_erasure::detail::call_result<
    Op,
    void(U&...),
    Concept
>::type
unchecked_call(
    const ::boost::type_erasure::binding<Concept>& table,
    const Op&,
    U&... arg)
{
    return ::boost::type_erasure::detail::call_impl<
        typename ::boost::type_erasure::detail::get_signature<Op>::type,
        void(U&...),
        Concept
    >::template apply<
        typename ::boost::type_erasure::detail::adapt_to_vtable<Op>::type
    >(&table, arg...);
}

template<class Concept, class Op, class... U>
typename ::boost::type_erasure::detail::call_result<
    Op,
    void(U&...),
    Concept
>::type
call(
    const ::boost::type_erasure::binding<Concept>& table,
    const Op& f,
    U&... arg)
{
    ::boost::type_erasure::require_match(table, f, arg...);
    return ::boost::type_erasure::unchecked_call(table, f, arg...);
}

template<class Op, class... U>
typename ::boost::type_erasure::detail::call_result<
    Op,
    void(U&...)
>::type
unchecked_call(
    const Op&,
    U&... arg)
{
    return ::boost::type_erasure::detail::call_impl<
        typename ::boost::type_erasure::detail::get_signature<Op>::type,
        void(U&...)
    >::template apply<
        typename ::boost::type_erasure::detail::adapt_to_vtable<Op>::type
    >(::boost::type_erasure::detail::extract_table(
        static_cast<typename ::boost::type_erasure::detail::get_signature<Op>::type*>(0), arg...),
        arg...);
}

template<class Op, class... U>
typename ::boost::type_erasure::detail::call_result<
    Op,
    void(U&...)
>::type
call(
    const Op& f,
    U&... arg)
{
    ::boost::type_erasure::require_match(f, arg...);
    return ::boost::type_erasure::unchecked_call(f, arg...);
}


#else

#define BOOST_PP_FILENAME_1 <boost/type_erasure/call.hpp>
#define BOOST_PP_ITERATION_LIMITS (0, BOOST_TYPE_ERASURE_MAX_ARITY)
#include BOOST_PP_ITERATE()

#endif

#endif

}
}

#endif

#else

#define N BOOST_PP_ITERATION()

#define BOOST_TYPE_ERASURE_CONVERT_ARG(z, n, data)                      \
    ::boost::type_erasure::detail::convert_arg(                         \
        BOOST_PP_CAT(arg, n),                                           \
        ::boost::type_erasure::detail::is_placeholder_arg<BOOST_PP_CAT(T, n)>())

#define BOOST_TYPE_ERASURE_GET_TABLE(z, n, data)                        \
    ::boost::type_erasure::detail::maybe_get_table(                     \
        BOOST_PP_CAT(arg, n),                                           \
        result,                                                         \
        ::boost::type_erasure::detail::is_placeholder_arg<BOOST_PP_CAT(T, n)>());

namespace detail {

#if N != 0

template<
    class R,
    BOOST_PP_ENUM_PARAMS(N, class T),
    BOOST_PP_ENUM_PARAMS(N, class U)>
const ::boost::type_erasure::binding<
    typename ::boost::type_erasure::detail::BOOST_PP_CAT(extract_concept, N)<
        BOOST_PP_ENUM_PARAMS(N, T),
        BOOST_PP_ENUM_PARAMS(N, U)>::type>*
BOOST_PP_CAT(extract_table, N)(R(*)(BOOST_PP_ENUM_PARAMS(N, T)), BOOST_PP_ENUM_BINARY_PARAMS(N, const U, &arg))
{
    const ::boost::type_erasure::binding<
        typename ::boost::type_erasure::detail::BOOST_PP_CAT(extract_concept, N)<
            BOOST_PP_ENUM_PARAMS(N, T),
            BOOST_PP_ENUM_PARAMS(N, U)>::type>* result = 0;
    BOOST_PP_REPEAT(N, BOOST_TYPE_ERASURE_GET_TABLE, ~)
    BOOST_ASSERT(result != 0);
    return result;
}

#endif

template<
    class R
    BOOST_PP_ENUM_TRAILING_PARAMS(N, class T)
    BOOST_PP_ENUM_TRAILING_PARAMS(N, class U),
    class Concept
#if N != 0
        = typename ::boost::type_erasure::detail::BOOST_PP_CAT(extract_concept, N)<
            BOOST_PP_ENUM_PARAMS(N, T),
            BOOST_PP_ENUM_PARAMS(N, U)
        >::type
#endif
    ,
    bool ReturnsAny = ::boost::type_erasure::detail::is_placeholder_arg<R>::value>
struct BOOST_PP_CAT(call_impl, N);

template<
    class R
    BOOST_PP_ENUM_TRAILING_PARAMS(N, class T)
    BOOST_PP_ENUM_TRAILING_PARAMS(N, class U),
    class Concept
>
struct BOOST_PP_CAT(call_impl, N)<
    R
    BOOST_PP_ENUM_TRAILING_PARAMS(N, T)
    BOOST_PP_ENUM_TRAILING_PARAMS(N, U),
    Concept,
    false
>
{
    typedef R type;
    template<class F>
    static R apply(const ::boost::type_erasure::binding<Concept>* table
        BOOST_PP_ENUM_TRAILING_BINARY_PARAMS(N, U, & arg))
    {
        return table->template find<F>()(
            BOOST_PP_ENUM(N, BOOST_TYPE_ERASURE_CONVERT_ARG, ~));
    }
};

template<
    class R
    BOOST_PP_ENUM_TRAILING_PARAMS(N, class T)
    BOOST_PP_ENUM_TRAILING_PARAMS(N, class U),
    class Concept
>
struct BOOST_PP_CAT(call_impl, N)<
    R
    BOOST_PP_ENUM_TRAILING_PARAMS(N, T)
    BOOST_PP_ENUM_TRAILING_PARAMS(N, U),
    Concept,
    true
>
{
    typedef ::boost::type_erasure::any<Concept, R> type;
    template<class F>
    static type apply(const ::boost::type_erasure::binding<Concept>* table
        BOOST_PP_ENUM_TRAILING_BINARY_PARAMS(N, U, & arg))
    {
        return type(table->template find<F>()(
            BOOST_PP_ENUM(N, BOOST_TYPE_ERASURE_CONVERT_ARG, ~)), *table);
    }
};

template<
    class R
    BOOST_PP_ENUM_TRAILING_PARAMS(N, class T)
    BOOST_PP_ENUM_TRAILING_PARAMS(N, class U),
    class Concept
>
struct call_impl<R(BOOST_PP_ENUM_PARAMS(N, T)), void(BOOST_PP_ENUM_BINARY_PARAMS(N, U, &u)), Concept>
  : BOOST_PP_CAT(call_impl, N)<R BOOST_PP_ENUM_TRAILING_PARAMS(N, T) BOOST_PP_ENUM_TRAILING_PARAMS(N, U), Concept>
{};

#if N != 0

template<
    class R
    BOOST_PP_ENUM_TRAILING_PARAMS(N, class T)
    BOOST_PP_ENUM_TRAILING_PARAMS(N, class U)
>
struct call_impl<R(BOOST_PP_ENUM_PARAMS(N, T)), void(BOOST_PP_ENUM_BINARY_PARAMS(N, U, &u))>
  : BOOST_PP_CAT(call_impl, N)<R BOOST_PP_ENUM_TRAILING_PARAMS(N, T) BOOST_PP_ENUM_TRAILING_PARAMS(N, U)>
{};

#endif

}

template<
    class Concept,
    class Op
    BOOST_PP_ENUM_TRAILING_PARAMS(N, class U)
>
typename ::boost::type_erasure::detail::call_result<
    Op,
    void(BOOST_PP_ENUM_BINARY_PARAMS(N, U, &u)),
    Concept
>::type
unchecked_call(
    const ::boost::type_erasure::binding<Concept>& table,
    const Op&
    BOOST_PP_ENUM_TRAILING_BINARY_PARAMS(N, U, & arg))
{
    return ::boost::type_erasure::detail::call_impl<
        typename ::boost::type_erasure::detail::get_signature<Op>::type,
        void(BOOST_PP_ENUM_BINARY_PARAMS(N, U, &u)),
        Concept
    >::template apply<
        typename ::boost::type_erasure::detail::adapt_to_vtable<Op>::type
    >(&table BOOST_PP_ENUM_TRAILING_PARAMS(N, arg));
}

template<
    class Concept,
    class Op
    BOOST_PP_ENUM_TRAILING_PARAMS(N, class U)
>
typename ::boost::type_erasure::detail::call_result<
    Op,
    void(BOOST_PP_ENUM_BINARY_PARAMS(N, U, &u)),
    Concept
>::type
call(
    const ::boost::type_erasure::binding<Concept>& table,
    const Op& f
    BOOST_PP_ENUM_TRAILING_BINARY_PARAMS(N, U, & arg))
{
    ::boost::type_erasure::require_match(table, f BOOST_PP_ENUM_TRAILING_PARAMS(N, arg));
    return ::boost::type_erasure::unchecked_call(table, f BOOST_PP_ENUM_TRAILING_PARAMS(N, arg));
}

#if N != 0

template<
    class Op
    BOOST_PP_ENUM_TRAILING_PARAMS(N, class U)
>
typename ::boost::type_erasure::detail::call_result<
    Op,
    void(BOOST_PP_ENUM_BINARY_PARAMS(N, U, &u))
>::type
unchecked_call(
    const Op&
    BOOST_PP_ENUM_TRAILING_BINARY_PARAMS(N, U, & arg))
{
    return ::boost::type_erasure::detail::call_impl<
        typename ::boost::type_erasure::detail::get_signature<Op>::type,
        void(BOOST_PP_ENUM_BINARY_PARAMS(N, U, &u))
    >::template apply<
        typename ::boost::type_erasure::detail::adapt_to_vtable<Op>::type
    >(
        ::boost::type_erasure::detail::BOOST_PP_CAT(extract_table, N)(
            (typename ::boost::type_erasure::detail::get_signature<Op>::type*)0,
            BOOST_PP_ENUM_PARAMS(N, arg))
        BOOST_PP_ENUM_TRAILING_PARAMS(N, arg)
    );
}

template<
    class Op
    BOOST_PP_ENUM_TRAILING_PARAMS(N, class U)
>
typename ::boost::type_erasure::detail::call_result<
    Op,
    void(BOOST_PP_ENUM_BINARY_PARAMS(N, U, &u))
>::type
call(
    const Op& f
    BOOST_PP_ENUM_TRAILING_BINARY_PARAMS(N, U, & arg))
{
    ::boost::type_erasure::require_match(f BOOST_PP_ENUM_TRAILING_PARAMS(N, arg));
    return ::boost::type_erasure::unchecked_call(f BOOST_PP_ENUM_TRAILING_PARAMS(N, arg));
}

#endif

#undef BOOST_TYPE_ERASURE_GET_TABLE
#undef BOOST_TYPE_ERASURE_CONVERT_ARG
#undef N

#endif
