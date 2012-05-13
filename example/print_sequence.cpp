// Boost.TypeErasure library
//
// Copyright 2011 Steven Watanabe
//
// Distributed under the Boost Software License Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// $Id$

//[print_sequence

/*`
    (For the source of this example see
    [@boost:/libs/type_erasure/example/print_sequence.cpp print_sequence.cpp])

    This example demonstrates using Boost.TypeErasure to implement a
    virtual "template" function.
*/

#include <boost/type_erasure/any.hpp>
#include <boost/type_erasure/iterator.hpp>
#include <boost/type_erasure/operators.hpp>
#include <boost/range/begin.hpp>
#include <boost/range/end.hpp>
#include <boost/range/iterator.hpp>
#include <boost/range/value_type.hpp>
#include <iostream>
#include <iomanip>
#include <vector>

using namespace boost::type_erasure;

struct _t : placeholder<_t> {};
struct _iter : placeholder<_iter> {};
struct _os : placeholder<_os> {};

template<class T, class U = _self>
struct base_and_derived
{
    static T& apply(U& arg) { return arg; }
};

namespace boost {
namespace type_erasure {

template<class T, class U, class Base>
struct concept_interface<base_and_derived<T, U>, Base, U> : Base
{
    operator typename rebind_any<Base, const T&>::type() const
    {
        return call(base_and_derived<T, U>(), const_cast<concept_interface&>(*this));
    }
    operator typename rebind_any<Base, T&>::type()
    {
        return call(base_and_derived<T, U>(), *this);
    }
};

}
}

class abstract_printer {
public:
    // Range must be a Forward Range whose elements can be printed to os.
    template<class CharT, class Traits, class Range>
    void print(std::basic_ostream<CharT, Traits>& os, const Range& r) const {
        // Capture the argument types
        typedef typename boost::range_iterator<const Range>::type iter;
        typedef typename boost::range_value<const Range>::type val;
        typedef boost::mpl::map<
            boost::mpl::pair<_os, std::basic_ostream<CharT, Traits> >,
            boost::mpl::pair<_iter, iter>,
            boost::mpl::pair<_t, val>
        > types;
        ostream_type any_os(os, make_binding<types>());
        iterator_type first(boost::begin(r), make_binding<types>());
        iterator_type last(boost::end(r), make_binding<types>());
        // and forward to the real implementation
        do_print(any_os, first, last);
    }
    virtual ~abstract_printer() {}
protected:
    typedef boost::mpl::vector<
        base_and_derived<std::ios_base, _os>,
        ostreamable<_os, _t>,
        ostreamable<_os, const char*>,
        forward_iterator<_iter, const _t>
    > requirements;
    typedef boost::type_erasure::any<requirements, _os&> ostream_type;
    typedef boost::type_erasure::any<requirements, _iter> iterator_type;
    virtual void do_print(
        ostream_type os, iterator_type first, iterator_type last) const = 0;
};

class separator_printer : public abstract_printer {
public:
    explicit separator_printer(const std::string& sep) : separator(sep) {}
protected:
    virtual void do_print(
        ostream_type os, iterator_type first, iterator_type last) const {
        if(first != last) {
            os << *first;
            ++first;
            for(; first != last; ++first) {
                os << separator.c_str() << *first;
            }
        }
    }
private:
    std::string separator;
};

class column_separator_printer : public abstract_printer {
public:
    column_separator_printer(const std::string& sep, std::size_t num_columns)
      : separator(sep),
        cols(num_columns)
    {}
protected:
    virtual void do_print(
        ostream_type os, iterator_type first, iterator_type last) const {
        std::size_t count = 0;
        for(; first != last; ++first) {
            os << *first;
            boost::type_erasure::any<requirements, _iter> temp = first;
            ++temp;
            if(temp != last) {
                os << separator.c_str();
            }
            if(++count % cols == 0) {
                os << "\n";
            }
        }
    }
private:
    std::string separator;
    std::size_t cols;
};

class aligned_column_printer : public abstract_printer {
public:
    aligned_column_printer(std::size_t column_width, std::size_t num_columns)
      : width(column_width),
        cols(num_columns)
    {}
protected:
    virtual void do_print(
        ostream_type os, iterator_type first, iterator_type last) const
    {
        if(first == last) return;
        std::vector<iterator_type> column_iterators;

        // find the tops of the columns
        std::size_t count = 0;
        for(iterator_type iter = first; iter != last; ++iter) {
            ++count;
        }
        std::size_t rows = (count + cols - 1) / cols;
        count = 0;
        for(iterator_type iter = first; iter != last; ++iter) {
            if(count % rows == 0) {
                column_iterators.push_back(iter);
            }
            ++count;
        }

        iterator_type last_col = column_iterators.back();

        // print the full rows
        while(column_iterators.back() != last) {
            for(std::vector<iterator_type>::iterator
                iter = column_iterators.begin(),
                end = column_iterators.end(); iter != end; ++iter)
            {
                static_cast<std::ios_base&>(os).width(width);
                os << **iter;
                ++*iter;
            }
            os << "\n";
        }

        // print the rows that are missing the last column
        column_iterators.pop_back();
        if(!column_iterators.empty()) {
            while(column_iterators.back() != last_col) {
                for(std::vector<iterator_type>::iterator
                    iter = column_iterators.begin(),
                    end = column_iterators.end(); iter != end; ++iter)
                {
                    static_cast<std::ios_base&>(os).width(width);
                    os << **iter;
                    ++*iter;
                }
                os << "\n";
            }
        }
    }
private:
    std::size_t width;
    std::size_t cols;
};

int main() {
    int test[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
    separator_printer p1(",");
    p1.print(std::cout, test);
    std::cout << std::endl;
    column_separator_printer p2(",", 4);
    p2.print(std::cout, test);
    std::cout << std::endl;
    aligned_column_printer p3(16, 4);
    p3.print(std::cout, test);
}

//]
