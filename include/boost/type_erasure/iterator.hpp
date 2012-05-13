// Boost.TypeErasure library
//
// Copyright 2011 Steven Watanabe
//
// Distributed under the Boost Software License Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// $Id$

#ifndef BOOST_TYPE_ERASURE_ITERATOR_HPP_INCLUDED
#define BOOST_TYPE_ERASURE_ITERATOR_HPP_INCLUDED

#include <iterator>
#include <boost/mpl/vector.hpp>
#include <boost/mpl/if.hpp>
#include <boost/type_traits/remove_const.hpp>
#include <boost/type_traits/remove_reference.hpp>
#include <boost/type_erasure/operators.hpp>
#include <boost/type_erasure/builtin.hpp>
#include <boost/type_erasure/deduced.hpp>
#include <boost/type_erasure/is_placeholder.hpp>

namespace boost {
namespace type_erasure {

namespace detail {

template<class T>
struct iterator_value_type_impl
{
    typedef typename ::std::iterator_traits<T>::value_type type;
};

}

template<class T>
struct iterator_value_type
{
    typedef typename ::boost::mpl::eval_if<
        ::boost::type_erasure::is_placeholder<T>,
        ::boost::mpl::identity<void>,
        ::boost::type_erasure::detail::iterator_value_type_impl<T>
    >::type type;
};

template<
    class T = _self,
    class ValueType = typename ::boost::type_erasure::deduced<iterator_value_type<T> >::type,
    class DifferenceType = ::std::ptrdiff_t,
    class Reference = ValueType&
>
struct forward_iterator :
    boost::mpl::vector<
        copy_constructible<T>,
        constructible<T()>,
        equality_comparable<T>,
        dereferenceable<Reference, T>,
        incrementable<T>,
        assignable<T>
    >
{
    typedef typename ::boost::remove_const<ValueType>::type value_type;
    typedef DifferenceType difference_type;
    typedef Reference reference;
};

/// \cond show_operators

template<class T, class ValueType, class DifferenceType, class Reference, class Base>
struct concept_interface<forward_iterator<T, ValueType, DifferenceType, Reference>, Base, T>
    : Base
{
    typedef typename rebind_any<Base, ValueType>::type value_type;
    typedef typename rebind_any<Base, Reference>::type reference;
    typedef DifferenceType difference_type;
    typedef typename ::boost::mpl::if_< ::boost::is_reference<reference>,
        typename ::boost::remove_reference<reference>::type*,
        value_type*
    >::type pointer;
    typedef std::forward_iterator_tag iterator_category;
};

/// \endcond

#if 0

template<class T, class ValueType, class DifferenceType = std::ptrdiff_t, class Reference = ValueType&>
struct bidirectional_iterator :
    boost::mpl::vector<
        forward_iterator<T, ValueType, DifferenceType, Pointer, Reference>,
        decrementable<T>
    >
{};

/// \cond show_operators

template<class T, class ValueType, class DifferenceType, class Pointer, class Reference, class Base>
struct concept_interface<bidirectional_iterator<T, ValueType, DifferenceType, Pointer, Reference>, Base, T>
    : Base
{
    typedef std::bidirectional_iterator_tag iterator_category;
};

/// \endcond

template<class T, class ValueType, class DifferenceType = std::ptrdiff_t, class Pointer = ValueType*, class Reference = ValueType&>
struct random_access_iterator :
    boost::mpl::vector<
        bidirectional_iterator<T, ValueType, DifferenceType, Pointer, Reference>,
        addable<T, DifferenceType, T>,
        addable<DifferenceType, T, T>,
        subtractable<T, DifferenceType, T>,
        subtractable<T, T, DifferenceType>,
        subscriptable<Reference, T, DifferenceType>
    >
{};

/// \cond show_operators

template<class T, class ValueType, class DifferenceType, class Pointer, class Reference, class Base>
struct concept_interface<random_access_iterator<T, ValueType, DifferenceType, Pointer, Reference>, Base, T>
    : Base
{
    typedef std::random_access_iterator_tag iterator_category;
};

/// \endcond

#endif

}
}

#endif
