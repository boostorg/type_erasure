// Boost.TypeErasure library
//
// Copyright 2011 Steven Watanabe
//
// Distributed under the Boost Software License Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// $Id$

#include <boost/type_erasure/builtin.hpp>
#include <boost/type_erasure/operators.hpp>
#include <boost/type_erasure/iterator.hpp>
#include <boost/type_erasure/callable.hpp>
#include <boost/type_erasure/any.hpp>
#include <boost/type_erasure/relaxed_match.hpp>
#include <boost/mpl/vector.hpp>
#include <iostream>
#include <vector>

namespace mpl = boost::mpl;
using namespace boost::type_erasure;

//[intro
/*`
    Here is a simple example using the library to implement
    a type safe printf.
 */
typedef any<
    mpl::vector<
        copy_constructible<>,
        ostreamable<>
    >
> any_printable;

typedef std::vector<any_printable> print_storage;

void print_impl(const char * format, const print_storage& args) {
    int idx = 0;
    while(char ch = *format++) {
        if (ch == '%') {
            switch(*format++) {
            case '%': std::cout << '%'; break;
            case 'd': std::cout << std::dec << args.at(idx++); break;
            case 'x': std::cout << std::hex << args.at(idx++); break;
            case '\0': return;
            }
        } else {
            std::cout << ch;
        }
    }
}

template<class... T>
void print(const char * format, const T&... t)
{
    print_storage args = { any_printable(t)... };
    print_impl(format, args);
}
/*`
    The top level function just stores all the arguments
    in a vector and forwards to the real implementation.
    `any_printable<>`, as its name suggests, is like
    Boost.Any, but also supports streaming to `std::cout`.
 */
/*`
    Boost.TypeErasure generalizes a technique used by
    several other Boost libraries.
 */
typedef any<
    mpl::vector<
        copy_constructible<>,
        typeid_<>,
        relaxed_match
    >
> boost_any; // equivalent to boost::any

typedef any<
    mpl::vector<
        copy_constructible<>,
        callable<void(int)>,
        typeid_<>,
        relaxed_match
    >
> boost_function; // equivalent to boost::function<void(int)>

typedef any<
    mpl::vector<
        forward_iterator<>,
        same_type<forward_iterator<>::value_type, int>,
        relaxed_match
    >
> any_iterator;
// equivalent to
// boost::any_range<int, boost::forward_traversal_tag,
//      int&, std::ptrdiff_t>::iterator
//]
