// Boost.TypeErasure library
//
// Copyright 2011-2012 Steven Watanabe
//
// Distributed under the Boost Software License Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// $Id$

#ifndef BOOST_TYPE_ERASURE_EXCEPTION_HPP_INCLUDED
#define BOOST_TYPE_ERASURE_EXCEPTION_HPP_INCLUDED

#include <stdexcept>

namespace boost {
namespace type_erasure {

/**
 * Exception thrown when the arguments to a primitive concept
 * are incorrect.
 */
struct bad_function_call : ::std::invalid_argument
{
    bad_function_call() : ::std::invalid_argument("bad_function_call") {}
};

}
}

#endif
