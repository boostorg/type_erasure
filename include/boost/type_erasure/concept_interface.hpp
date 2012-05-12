// Boost.TypeErasure library
//
// Copyright 2011 Steven Watanabe
//
// Distributed under the Boost Software License Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// $Id$

#ifndef BOOST_TYPE_ERASURE_CONCEPT_INTERFACE_HPP_INCLUDED
#define BOOST_TYPE_ERASURE_CONCEPT_INTERFACE_HPP_INCLUDED

namespace boost {
namespace type_erasure {

/**
 * The @ref concept_interface class can be specialized to
 * add behavior to an @ref any.
 *
 * \tparam Concept The concept that we're specializing
 *         @ref concept_interface for.
 * \tparam Base The base of this class.  Specializations of @ref
 *         concept_interface must inherit publicly from this type.
 * \tparam ID The placeholder representing this type.
 * \tparam Enable A dummy parameter that can be used for SFINAE.
 */
template<class Concept, class Base, class ID, class Enable = void>
struct concept_interface : Base {};

}
}

#endif
