// Boost.TypeErasure library
//
// Copyright 2018 Steven Watanabe
//
// Distributed under the Boost Software License Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// $Id$

#ifndef BOOST_TYPE_ERASURE_FUNCTION_HPP_INCLUDED
#define BOOST_TYPE_ERASURE_FUNCTION_HPP_INCLUDED

#include <boost/config.hpp>
#include <boost/preprocessor/cat.hpp>
#include <boost/type_traits/remove_reference.hpp>
#include <boost/type_traits/remove_cv.hpp>
#include <boost/mpl/if.hpp>
#include <boost/type_erasure/detail/const.hpp>
#include <boost/type_erasure/rebind_any.hpp>
#include <boost/type_erasure/param.hpp>
#include <boost/type_erasure/is_placeholder.hpp>
#include <boost/type_erasure/call.hpp>
#include <boost/type_erasure/free.hpp>

#if defined(BOOST_NO_CXX11_TEMPLATE_ALIASES) || \
    defined(BOOST_NO_CXX11_VARIADIC_TEMPLATES) || \
    defined(BOOST_NO_CXX11_RVALUE_REFERENCES) || \
    BOOST_WORKAROUND(BOOST_MSVC, == 1800)
#error BOOST_TYPE_ERASURE_FUNCTION is not supported with this compiler.
#endif

namespace boost {
namespace type_erasure {
namespace detail {

template<int N>
using make_index_list_t = typename ::boost::type_erasure::detail::make_index_list<N>::type;

template<class... T>
using first_placeholder_t =
    typename ::boost::type_erasure::detail::first_placeholder<
        ::boost::remove_cv_t< ::boost::remove_reference_t<T> >...
    >::type;

template<class P, template<class ...> class interface, class Sig, class Concept, class Base, class ID>
using choose_member_interface = typename ::boost::mpl::if_c< ::boost::is_reference<P>::value,
    typename ::boost::mpl::if_c< ::boost::type_erasure::detail::is_non_const_ref<P>::value,
        interface<Sig, Concept, Base, const ID>,
        Base
    >::type,
    interface<Sig, Concept, Base, ID>
>::type;

template<class Sig>
struct memfun_to_func
{
    typedef Sig type;
};

template<class R, class C, class... A>
struct memfun_to_func<R (C::*)(A...)>
{
    typedef R type(C&, A...);
};

template<class R, class C, class... A>
struct memfun_to_func<R (C::*)(A...) const>
{
    typedef R type(const C&, A...);
};

template<class Sig>
using memfun_to_func_t = typename ::boost::type_erasure::detail::memfun_to_func<Sig>::type;

}
}
}

/** INTERNAL ONLY */
#define BOOST_TYPE_ERASURE_FUNCTION_II(concept_name, function_name, member_concept_name)\
template<class Sig>                                                         \
struct concept_name;                                                        \
                                                                            \
namespace boost_type_erasure_impl {                                         \
                                                                            \
template<class Sig, class Concept, class Base, class ID, class Enable = void>\
struct concept_name ## _member_interface;                                   \
                                                                            \
template<class R, class... A, class Concept, class Base, class ID, class V> \
struct concept_name ## _member_interface<R(A...), Concept, Base, ID, V> : Base\
{                                                                           \
    typedef void _boost_type_erasure_has_member ## function_name;           \
    ::boost::type_erasure::rebind_any_t<Base, R> function_name(::boost::type_erasure::as_param_t<Base, A>... a)\
    { return ::boost::type_erasure::call(Concept(), *this, std::forward<A>(a)...); }\
};                                                                          \
                                                                            \
template<class R, class... A, class Concept, class Base, class ID>          \
struct concept_name ## _member_interface<R(A...), Concept, Base, ID,        \
    typename Base::_boost_type_erasure_has_member ## function_name> : Base  \
{                                                                           \
    typedef void _boost_type_erasure_has_member ## function_name;           \
    using Base::function_name;                                              \
    ::boost::type_erasure::rebind_any_t<Base, R> function_name(::boost::type_erasure::as_param_t<Base, A>... a)\
    { return ::boost::type_erasure::call(Concept(), *this, std::forward<A>(a)...); }\
};                                                                          \
                                                                            \
template<class R, class... A, class Concept, class Base, class ID, class V> \
struct concept_name ## _member_interface<R(A...), Concept, Base, const ID, V> : Base\
{                                                                           \
    typedef void _boost_type_erasure_has_member ## function_name;           \
    ::boost::type_erasure::rebind_any_t<Base, R> function_name(::boost::type_erasure::as_param_t<Base, A>... a) const\
    { return ::boost::type_erasure::call(Concept(), *this, std::forward<A>(a)...); }\
};                                                                          \
                                                                            \
template<class R, class... A, class Concept, class Base, class ID>          \
struct concept_name ## _member_interface<R(A...), Concept, Base, const ID,  \
    typename Base::_boost_type_erasure_has_member ## function_name> : Base  \
{                                                                           \
    typedef void _boost_type_erasure_has_member ## function_name;           \
    using Base::function_name;                                              \
    ::boost::type_erasure::rebind_any_t<Base, R> function_name(::boost::type_erasure::as_param_t<Base, A>... a) const\
    { return ::boost::type_erasure::call(Concept(), *this, std::forward<A>(a)...); }\
};                                                                          \
                                                                            \
template<class Sig, class Base, class Idx>                                  \
struct concept_name ## _free_interface;                                     \
template<class R, class... T, class Base, int... I>                         \
struct concept_name ## _free_interface<R(T...), Base, ::boost::type_erasure::index_list<I...> > : Base {\
    typedef typename ::boost::type_erasure::detail::first_placeholder_index<\
        ::boost::remove_cv_t< ::boost::remove_reference_t<T> >...           \
    >::type _boost_type_erasure_free_p_idx;                                 \
    friend ::boost::type_erasure::rebind_any_t<Base, R>                     \
    function_name(                                                          \
        typename ::boost::mpl::eval_if_c<(_boost_type_erasure_free_p_idx::value == I),\
            ::boost::type_erasure::detail::maybe_const_this_param<T, Base>, \
            ::boost::type_erasure::as_param<Base, T>                        \
        >::type... t)                                                       \
    {                                                                       \
        return ::boost::type_erasure::call(                                 \
            concept_name<R(T...)>(),                                        \
            std::forward<typename ::boost::mpl::eval_if_c<(_boost_type_erasure_free_p_idx::value == I),\
                ::boost::type_erasure::detail::maybe_const_this_param<T, Base>, \
                ::boost::type_erasure::as_param<Base, T>                    \
            >::type>(t)...);                                                \
    }                                                                       \
};                                                                          \
                                                                            \
template<class Sig>                                                         \
struct concept_name ## member;                                              \
                                                                            \
template<class R, class T0, class... T>                                     \
struct concept_name ## member<R(T0, T...)> {                                \
    static R apply(T0 t0, T... t)                                           \
    { return t0.function_name(std::forward<T>(t)...); }                     \
};                                                                          \
                                                                            \
template<class T0, class... T>                                              \
struct concept_name ## member<void(T0, T...)> {                             \
    static void apply(T0 t0, T... t)                                        \
    { t0.function_name(std::forward<T>(t)...); }                            \
};                                                                          \
                                                                            \
template<class Sig>                                                         \
struct concept_name ## free;                                                \
                                                                            \
template<class R, class... T>                                               \
struct concept_name ## free<R(T...)> {                                      \
    static R apply(T... t)                                                  \
    { return function_name(std::forward<T>(t)...); }                        \
};                                                                          \
                                                                            \
template<class... T>                                                        \
struct concept_name ## free<void(T...)> {                                   \
    static void apply(T... t)                                               \
    { function_name(std::forward<T>(t)...); }                               \
};                                                                          \
                                                                            \
template<class Sig, class ID>                                               \
struct concept_name ## interface_chooser {                                  \
    template<class Base>                                                    \
    using apply = Base;                                                     \
};                                                                          \
                                                                            \
template<class R, class T, class... A>                                      \
struct concept_name ## interface_chooser<concept_name<R (T::*)(A...)>, T>   \
{                                                                           \
    template<class Base>                                                    \
    using apply = ::boost::type_erasure::detail::choose_member_interface<   \
        ::boost::type_erasure::placeholder_of_t<Base>,                      \
        concept_name ## _member_interface,                                  \
        R(A...), concept_name<R (T::*)(A...)>, Base, T>;                    \
};                                                                          \
                                                                            \
template<class R, class T, class... A>                                      \
struct concept_name ## interface_chooser<concept_name<R (T::*)(A...) const>, T>\
{                                                                           \
    template<class Base>                                                    \
    using apply = concept_name ## _member_interface<R(A...), concept_name<R(T::*)(A...) const>, Base, const T>;\
};                                                                          \
                                                                            \
template<class R, class... A>                                               \
struct concept_name ## interface_chooser<                                   \
    concept_name<R(A...)>,                                                  \
    typename ::boost::type_erasure::detail::first_placeholder<              \
        ::boost::remove_cv_t< ::boost::remove_reference_t<A> >...>::type>   \
{                                                                           \
    template<class Base>                                                    \
    using apply = concept_name ## _free_interface<R(A...), Base,            \
        ::boost::type_erasure::detail::make_index_list_t<sizeof...(A)> >;   \
};                                                                          \
                                                                            \
struct concept_name ## choose_interface {                                   \
    template<class Concept, class Base, class ID>                           \
    using apply = typename concept_name ## interface_chooser<Concept, ID>::template apply<Base>;\
};                                                                          \
                                                                            \
}                                                                           \
                                                                            \
template<class Sig>                                                         \
struct concept_name :                                                       \
    ::boost::mpl::if_c< ::boost::is_member_pointer<Sig>::value,             \
        boost_type_erasure_impl::concept_name##member< ::boost::type_erasure::detail::memfun_to_func_t<Sig> >,\
        boost_type_erasure_impl::concept_name##free<Sig>                    \
    >::type                                                                 \
{};                                                                         \
                                                                            \
template<class Sig>                                                         \
boost_type_erasure_impl::concept_name ## choose_interface                   \
boost_type_erasure_find_interface(concept_name<Sig>);                       \
                                                                            \
template<class Sig, class T = ::boost::type_erasure::_self>                 \
using member_concept_name = concept_name<Sig T::*>;

/** INTERNAL ONLY */
#define BOOST_TYPE_ERASURE_FUNCTION_I(concept_name, name, member_name)      \
    BOOST_TYPE_ERASURE_FUNCTION_II(concept_name, name, member_name)

/**
 * \brief Defines a primitive concept for a function.
 *
 * \param name is the name of the function
 *
 * The concept will be declared as
 * \code
 * template<class Sig>
 * struct has_name;
 * template<class Sig, class T = _self>
 * using has_member_name = has_name<Sig T::*>;
 * \endcode
 *
 * @c Sig should be either a pointer to a member function or
 * a function type.
 *
 * This macro must be used at namespace scope.
 *
 * Examples:
 *
 * \code
 * BOOST_TYPE_ERASURE_FUNCTION(size)
 * std::vector<int> v;
 * any_ref<has_size<std::size_t (_self::*)() const> > x = v;
 * any_ref<has_member_size<std::size_t() const> > y = v;
 * BOOST_TYPE_ERASURE_FUNCTION(swap);
 * any_ref<has_swap<void(_self&, _self&)> > z = v;
 * \endcode
 *
 * The concept defined by this macro may be specialized to
 * provide a concept map.  For member functions, the class
 * object will be passed by reference as the first argument.
 *
 * This macro supersedes \BOOST_TYPE_ERASURE_MEMBER and
 * \BOOST_TYPE_ERASURE_FREE.
 *
 * \BOOST_TYPE_ERASURE_FUNCTION is not available in C++03.
 */
#define BOOST_TYPE_ERASURE_FUNCTION(name) \
    BOOST_TYPE_ERASURE_FUNCTION_I(BOOST_PP_CAT(has_, name), name, BOOST_PP_CAT(has_member_, name))

#endif
