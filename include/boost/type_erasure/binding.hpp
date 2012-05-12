// Boost.TypeErasure library
//
// Copyright 2011 Steven Watanabe
//
// Distributed under the Boost Software License Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// $Id$

#ifndef BOOST_TYPE_ERASURE_BINDING_HPP_INCLUDED
#define BOOST_TYPE_ERASURE_BINDING_HPP_INCLUDED

#include <boost/config.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <boost/mpl/transform.hpp>
#include <boost/type_erasure/static_binding.hpp>
#include <boost/type_erasure/detail/adapt_to_vtable.hpp>
#include <boost/type_erasure/detail/rebind_placeholders.hpp>
#include <boost/type_erasure/detail/vtable.hpp>
#include <boost/type_erasure/detail/normalize.hpp>

namespace boost {
namespace type_erasure {

/**
 * Stores the binding of a @c Concept to a set of actual types.
 * @c Concept is interpreted in the same way as with @ref any.
 */
template<class Concept>
class binding
{
    typedef typename ::boost::type_erasure::detail::normalize_concept<
        Concept>::type normalized;
    typedef typename ::boost::mpl::transform<normalized,
        ::boost::type_erasure::detail::maybe_adapt_to_vtable< ::boost::mpl::_1>
    >::type actual_concept;
    typedef typename ::boost::type_erasure::detail::make_vtable<
        actual_concept>::type table_type;
public:
    
    /**
     * \pre @c Map must be an MPL map with an entry for each placeholder
     *      referred to by @c Concept.
     *
     * \throws Nothing.
     */
    template<class Map>
    explicit binding(const Map&)
    {
        set<Map>();
    }
    
    /**
     * \pre @c Map must be an MPL map with an entry for each placeholder
     *      referred to by @c Concept.
     *
     * \throws Nothing.
     */
    template<class Map>
    binding(const static_binding<Map>&)
    {
        set<Map>();
    }

    /**
     * Converts from another set of bindings.
     *
     * \pre Map must be an MPL map with an entry for each placeholder
     *      referred to by @c Concept.  The mapped type should be the
     *      corresponding placeholder in Concept2.
     *
     * \throws std::bad_alloc
     */
    template<class Concept2, class Map>
    binding(const binding<Concept2>& other, const Map&)
      : manager(new table_type)
    {
        manager->template convert_from<Map>(*other.table);
        table = manager.get();
    }

    /**
     * Converts from another set of bindings.
     *
     * \pre Map must be an MPL map with an entry for each placeholder
     *      referred to by @c Concept.  The mapped type should be the
     *      corresponding placeholder in Concept2.
     *
     * \throws std::bad_alloc
     */
    template<class Concept2, class Map>
    binding(const binding<Concept2>& other, const static_binding<Map>&)
      : manager(new table_type)
    {
        manager->template convert_from<Map>(*other.table);
        table = manager.get();
    }

#ifndef BOOST_TYPE_ERASURE_DOXYGEN

    /** Special case optimization. */
    template<class Map>
    binding(const binding& other, const Map&)
      : table(other.table)
    {
        table_type t;
        t.template convert_from<Map>(*other.table);
    }

    /** Special case optimization. */
    template<class Map>
    binding(const binding& other, const static_binding<Map>&)
      : table(other.table)
    {
        table_type t;
        t.template convert_from<Map>(*other.table);
    }

#endif

    /**
     * \return true iff the sets of types that the placeholders
     *         bind to are the same for both arguments.
     *
     * \throws Nothing.
     */
    friend bool operator==(const binding& lhs, const binding& rhs)
    { return *lhs.table == *rhs.table; }
    
    /**
     * \return true iff the arguments do not map to identical
     *         sets of types.
     *
     * \throws Nothing.
     */
    friend bool operator!=(const binding& lhs, const binding& rhs)
    { return !(lhs == rhs); }

    /** INTERNAL ONLY */
    template<class T>
    typename T::type find() const { return table->lookup((T*)0); }
private:
    template<class C2>
    friend class binding;
    /** INTERNAL ONLY */
    template<class Bindings>
    void set()
    {
        table = &::boost::type_erasure::detail::make_vtable_init<
            typename ::boost::mpl::transform<actual_concept,
                ::boost::type_erasure::detail::rebind_placeholders<
                    ::boost::mpl::_1, Bindings>
            >::type,
            table_type
        >::type::value;
    }
    const table_type* table;
    ::boost::shared_ptr<table_type> manager;
};

}
}

#endif
