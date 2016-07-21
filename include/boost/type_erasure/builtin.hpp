// Boost.TypeErasure library
//
// Copyright 2011 Steven Watanabe
//
// Distributed under the Boost Software License Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// $Id$

#ifndef BOOST_TYPE_ERASURE_BUILTIN_HPP_INCLUDED
#define BOOST_TYPE_ERASURE_BUILTIN_HPP_INCLUDED

#include <boost/mpl/vector.hpp>
#include <boost/type_erasure/detail/storage.hpp>
#include <boost/type_erasure/placeholder.hpp>
#include <boost/type_erasure/constructible.hpp>
#include <boost/type_erasure/rebind_any.hpp>
#ifndef BOOST_NO_CXX11_RVALUE_REFERENCES
#   include <boost/static_assert.hpp>
#   include <boost/type_traits/is_const.hpp>
#   include <boost/type_traits/is_reference.hpp>
#   include <utility>  // std::move
#endif
#include <typeinfo>

namespace boost {
namespace type_erasure {

/**
 * The @ref destructible concept enables forwarding to
 * the destructor of the contained type.  This is required
 * whenever an @ref any is created by value.
 *
 * \note The @ref destructible concept rarely needs to
 * be specified explicitly, because it is included in
 * the @ref copy_constructible concept.
 *
 * \note @ref destructible may not be specialized and
 * may not be passed to \call as it depends on the
 * implementation details of @ref any.
 */
template<class T = _self>
struct destructible
{
    /** INTERNAL ONLY */
    typedef void (*type)(detail::storage&);
    /** INTERNAL ONLY */
    static void value(detail::storage& arg)
    {
        delete static_cast<T*>(arg.data);
    }
    /** INTERNAL ONLY */
    static void apply(detail::storage& arg)
    { 
        delete static_cast<T*>(arg.data);
    }
};

/**
 * The @ref copy_constructible concept allows objects to
 * be copied and destroyed.
 *
 * \note This concept is defined to match C++ 2003,
 * [lib.copyconstructible].  It is not equivalent to
 * the concept of the same name in C++11.
 */
template<class T = _self>
struct copy_constructible :
    ::boost::mpl::vector<constructible<T(const T&)>, destructible<T> >
{};

/**
 * Enables assignment of @ref any types.
 */
template<class T = _self, class U = T>
struct assignable
{
    static void apply(T& dst, const U& src) { dst = src; }
};

/** INTERNAL ONLY */
template<class T, class U, class Base>
struct concept_interface<assignable<T, U>, Base, T> : Base
{
    using Base::_boost_type_erasure_deduce_assign;
    assignable<T, U>* _boost_type_erasure_deduce_assign(
        typename ::boost::type_erasure::rebind_any<Base, const U&>::type)
    {
        return 0;
    }
};

#ifndef BOOST_NO_CXX11_RVALUE_REFERENCES

/**
 * Enables move-assignment of @ref any types.
 *
 * \note Template parameter U must be an unqualified
 * type, neither a reference nor a const type.
 */
template<class T = _self, class U = T>
struct move_assignable
{
    BOOST_STATIC_ASSERT_MSG(
            !::boost::is_reference<U>::value && !::boost::is_const<U>::value,
            "Template parameter U must be unqualified. "
            "(It may neither be a const nor a reference type.)");
    static void apply(T& dst, U&& src) { dst = std::move(src); }
};

/** INTERNAL ONLY */
template<class T, class U, class Base>
struct concept_interface<move_assignable<T, U>, Base, T> : Base
{
    using Base::_boost_type_erasure_deduce_move_assign;
    move_assignable<T, U>* _boost_type_erasure_deduce_move_assign(
        typename ::boost::type_erasure::rebind_any<Base, U&&>::type)
    {
        return 0;
    }
};

#endif

/**
 * Enables runtime type information.  This is required
 * if you want to use \any_cast or \typeid_of.
 *
 * \note @ref typeid_ cannot be specialized because several
 * library components including \any_cast would not work
 * correctly if its behavior changed.  There is no need
 * to specialize it anyway, since it works for all types.
 * @ref typeid_ also cannot be passed to \call.  To access it,
 * use \typeid_of.
 */
template<class T = _self>
struct typeid_
{
    /** INTERNAL ONLY */
    typedef const std::type_info& (*type)();
    /** INTERNAL ONLY */
    static const std::type_info& value()
    {
        return typeid(T);
    }
    /** INTERNAL ONLY */
    static const std::type_info& apply()
    {
        return typeid(T);
    }
};

namespace detail {

template<class C>
struct get_null_vtable_entry;

template<class T>
struct get_null_vtable_entry< ::boost::type_erasure::typeid_<T> >
{
    typedef typeid_<void> type;
};

struct null_destroy {
    static void value(::boost::type_erasure::detail::storage&) {}
};

template<class T>
struct get_null_vtable_entry< ::boost::type_erasure::destructible<T> >
{
    typedef ::boost::type_erasure::detail::null_destroy type;
};

}

}
}

#endif
