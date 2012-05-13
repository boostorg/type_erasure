// Boost.TypeErasure library
//
// Copyright 2011 Steven Watanabe
//
// Distributed under the Boost Software License Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// $Id$

#ifndef BOOST_TYPE_ERASURE_SAME_TYPE_HPP_INCLUDED
#define BOOST_TYPE_ERASURE_SAME_TYPE_HPP_INCLUDED

namespace boost {
namespace type_erasure {

/**
 * A built in concept that indicates that two
 * types are the same.  Either T or U or both
 * can be placeholders.
 */
template<class T, class U>
struct same_type {};

}
}

#endif
