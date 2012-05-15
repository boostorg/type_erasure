// Boost.TypeErasure library
//
// Copyright 2011 Steven Watanabe
//
// Distributed under the Boost Software License Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// $Id$

#ifndef BOOST_TYPE_ERASURE_ANY_HPP_INCLUDED
#define BOOST_TYPE_ERASURE_ANY_HPP_INCLUDED

#include <algorithm>
#include <boost/config.hpp>
#include <boost/utility/enable_if.hpp>
#include <boost/utility/addressof.hpp>
#include <boost/utility/declval.hpp>
#include <boost/mpl/bool.hpp>
#include <boost/mpl/or.hpp>
#include <boost/mpl/pair.hpp>
#include <boost/mpl/map.hpp>
#include <boost/mpl/fold.hpp>
#include <boost/type_traits/remove_reference.hpp>
#include <boost/type_traits/remove_const.hpp>
#include <boost/type_traits/is_same.hpp>
#include <boost/type_traits/is_const.hpp>
#include <boost/preprocessor/cat.hpp>
#include <boost/preprocessor/iteration/iterate.hpp>
#include <boost/preprocessor/repetition/enum_params.hpp>
#include <boost/preprocessor/repetition/enum_binary_params.hpp>
#include <boost/preprocessor/repetition/enum_trailing_params.hpp>
#include <boost/preprocessor/repetition/enum_trailing_binary_params.hpp>
#include <boost/type_erasure/detail/access.hpp>
#include <boost/type_erasure/detail/any_base.hpp>
#include <boost/type_erasure/detail/normalize.hpp>
#include <boost/type_erasure/detail/storage.hpp>
#include <boost/type_erasure/config.hpp>
#include <boost/type_erasure/binding.hpp>
#include <boost/type_erasure/static_binding.hpp>
#include <boost/type_erasure/concept_interface.hpp>
#include <boost/type_erasure/call.hpp>
#include <boost/type_erasure/relaxed_match.hpp>

namespace boost {
namespace type_erasure {

template<class Sig>
struct constructible;

template<class T>
struct destructible;

template<class T, class U>
struct assignable;

namespace detail {

template<class Derived, class Concept, class T>
struct compute_bases
{
    typedef typename ::boost::mpl::fold<
        typename ::boost::type_erasure::detail::collect_concepts<
            Concept
        >::type,
        ::boost::type_erasure::any_base<Derived>,
        ::boost::type_erasure::concept_interface<
            ::boost::mpl::_2,
            ::boost::mpl::_1,
            T
        >
    >::type type;
};

template<class T>
T make(T*) { return T(); }

// This dance is necessary to avoid errors calling
// an ellipsis function with a non-trivially-copyable
// argument.

typedef char no;
struct yes { no dummy[2]; };

template<class Op>
yes check_overload(const Op*);
no check_overload(const void*);

struct fallback {};

template<class T>
fallback make_fallback(const T&, boost::mpl::false_)
{
    return fallback();
}

template<class T>
const T& make_fallback(const T& arg, boost::mpl::true_)
{
    return arg;
}

template<class T>
struct is_any : ::boost::mpl::false_ {};

template<class Concept, class T>
struct is_any<any<Concept, T> > : ::boost::mpl::true_ {};

}

#ifdef BOOST_MSVC
#pragma warning(push)
#pragma warning(disable:4355)
#pragma warning(disable:4521)
#endif

/**
 * The class template @ref any provides runtime polymorphism.
 *
 * \tparam Concept The concept that the type should model.
 * \tparam T A placeholder specifying which type this is.
 *
 * @c Concept is defined as follows:
 * - The built-in concepts provided by the library are valid concepts.
 * - Any user-defined primitive concept is a valid concept.
 * - An MPL Forward Sequence of concepts is a valid concept.
 *
 * Note that this definition is recursive and allows concepts to
 * composed in a natural way.
 */
template<class Concept, class T = _self>
class any :
    public ::boost::type_erasure::detail::compute_bases<
        ::boost::type_erasure::any<Concept, T>,
        Concept,
        T
    >::type
{
    typedef ::boost::type_erasure::binding<Concept> table_type;
public:
    /** INTERNAL ONLY */
    typedef Concept _boost_type_erasure_concept_type;
    /** INTERNAL ONLY */
    any(const ::boost::type_erasure::detail::storage& data_arg, const table_type& table_arg)
      : table(table_arg),
        data(data_arg)
    {}
    /**
     * Constructs an @ref any to hold a copy of @c data.
     *
     * \param data The object to construct the @ref any from.
     *
     * \pre @c U is a model of @c Concept.
     * \pre @c U must be \CopyConstructible.
     * \pre @c Concept must not refer to any placeholder besides @c T.
     *
     * \throws std::bad_alloc or whatever that the copy
     *         constructor of @c U throws.
     */
    template<class U>
    explicit any(const U& data_arg)
      : table(
            ::boost::type_erasure::make_binding<
                ::boost::mpl::map< ::boost::mpl::pair<T, U> >
            >()
        ),
        data(data_arg)
    {}
    /**
     * Constructs an @ref any to hold a copy of @c data.
     *
     * \param data The object to construct the @ref any from.
     * \param binding Specifies the actual types that
     *        all the placeholders should bind to.
     *
     * \pre @c U is a model of @c Concept.
     * \pre @c U must be \CopyConstructible.
     * \pre @c Map is an MPL map with an entry for every
     *         placeholder referred to by @c Concept.
     *
     * \throws std::bad_alloc or whatever that the copy
     *         constructor of @c U throws.
     */
    template<class U, class Map>
    any(const U& data_arg, const static_binding<Map>& binding_arg)
      : table(binding_arg),
        data(data_arg)
    {}
    /**
     * Copies an @ref any.
     *
     * \param other The object to make a copy of.
     *
     * \pre @c Concept must contain @ref constructible<T(const T&)>.
     *
     * \throws std::bad_alloc or whatever that the copy
     *         constructor of the contained type throws.
     */
    any(const any& other)
      : table(other.table),
        data(::boost::type_erasure::call(constructible<T(const T&)>(), other))
    {}
    /**
     * Upcasts from an @ref any with stricter requirements to
     * an @ref any with weaker requirements.
     *
     * \param other The object to make a copy of.
     *
     * \pre @c Concept must contain @ref constructible<T(const T&)>.
     * \pre @c Concept must not refer to any placeholder besides @c T.
     * \pre After substituting @c T for @c Tag2, the requirements of
     *      @c Concept2 must be a superset of of the requirements of
     *      @c Concept.
     *
     * \throws std::bad_alloc or whatever that the copy
     *         constructor of the contained type throws.
     */
    template<class Concept2, class Tag2>
    any(const any<Concept2, Tag2>& other)
      : table(
            ::boost::type_erasure::detail::access::table(other),
            ::boost::mpl::map<
                ::boost::mpl::pair<
                    T,
                    typename ::boost::remove_const<
                        typename ::boost::remove_reference<Tag2>::type
                    >::type
                >
            >()
        ),
        data(::boost::type_erasure::call(
            constructible<
                typename ::boost::remove_const<
                    typename boost::remove_reference<Tag2>::type
                >::type(const typename boost::remove_reference<Tag2>::type&)
            >(), other)
        )
    {}
    /**
     * Constructs an @ref any from another @ref any.
     *
     * \param other The object to make a copy of.
     * \param binding Specifies the mapping between the two concepts.
     *
     * \pre @c Concept must contain @ref constructible<T(const T&)>.
     * \pre @c Map must be an MPL map with keys for all the placeholders
     *      used by @c Concept and values for the corresponding
     *      placeholders in @c Concept2.
     * \pre After substituting placeholders according to @c Map, the
     *      requirements of @c Concept2 must be a superset of of the
     *      requirements of @c Concept.
     *
     * \throws std::bad_alloc or whatever that the copy
     *         constructor of the contained type throws.
     */
    template<class Concept2, class Tag2, class Map>
    any(const any<Concept2, Tag2>& other, const static_binding<Map>& binding_arg)
      : table(::boost::type_erasure::detail::access::table(other), binding_arg),
        data(::boost::type_erasure::call(
            constructible<
                typename ::boost::remove_const<
                    typename boost::remove_reference<Tag2>::type
                >::type(const typename boost::remove_reference<Tag2>::type&)
            >(), other)
        )
    {}
    /**
     * Constructs an @ref any from another @ref any.
     *
     * \param other The object to make a copy of.
     * \param binding Specifies the bindings of placeholders to actual types.
     *
     * \pre @c Concept must contain @ref constructible<T(const T&)>.
     * \pre The type stored in @c other must match the type expected by
     *      @c binding.
     *
     * \post binding_of(*this) == @c binding
     *
     * \throws std::bad_alloc or whatever that the copy
     *         constructor of the contained type throws.
     */
    template<class Concept2, class Tag2>
    any(const any<Concept2, Tag2>& other, const binding<Concept>& binding_arg)
      : table(binding_arg),
        data(::boost::type_erasure::call(
            constructible<
                typename ::boost::remove_const<
                    typename boost::remove_reference<Tag2>::type
                >::type(const typename boost::remove_reference<Tag2>::type&)
            >(), other)
        )
    {}

#ifdef BOOST_TYPE_ERASURE_DOXYGEN

    /**
     * Calls a constructor of the contained type.  The bindings
     * will be deduced from the arguments.
     *
     * \param arg The arguments to be passed to the underlying constructor.
     *
     * \pre @c Concept must contain a matching instance of @ref constructible.
     * \pre At least one of the arguments must by an @ref any with the
     *      same @c Concept as this.
     * \pre The bindings of all the arguments that are @ref any's, must
     *      be the same.
     *
     * \throws std::bad_alloc or whatever that the
     *         constructor of the contained type throws.
     */
    template<class... U>
    explicit any(U&&... arg);

    /**
     * Calls a constructor of the contained type.
     *
     * \param binding Specifies the bindings of placeholders to actual types.
     * \param arg The arguments to be passed to the underlying constructor.
     *
     * \pre @c Concept must contain a matching instance of @ref constructible.
     * \pre The contained type of every argument that is an @ref any, must
     *      be the same as that specified by @c binding.
     *
     * \post binding_of(*this) == @c binding
     *
     * \throws std::bad_alloc or whatever that the
     *         constructor of the contained type throws.
     */
    template<class... U>
    explicit any(const binding<Concept>& binding_arg, U&&... arg)
      : table(binding_arg),
        data(
            ::boost::type_erasure::detail::make(
                false? this->_boost_type_erasure_deduce_constructor(arg...) : 0
            )(arg...)
        )
    {}

#else
    // construction from a reference
    any(const any<Concept, T&>& other)
      : table(::boost::type_erasure::detail::access::table(other)),
        data(::boost::type_erasure::call(
            ::boost::type_erasure::detail::make(
                false? this->_boost_type_erasure_deduce_constructor(other) : 0
            ), other)
        )
    {}
    any(any<Concept, T&>& other)
      : table(::boost::type_erasure::detail::access::table(other)),
        data(::boost::type_erasure::call(
            ::boost::type_erasure::detail::make(
                false? this->_boost_type_erasure_deduce_constructor(other) : 0
            ), other)
        )
    {}
    any(const any<Concept, const T&>& other)
      : table(::boost::type_erasure::detail::access::table(other)),
        data(::boost::type_erasure::call(
            ::boost::type_erasure::detail::make(
                false? this->_boost_type_erasure_deduce_constructor(other) : 0
            ), other)
        )
    {}
    any(any<Concept, const T&>& other)
      : table(::boost::type_erasure::detail::access::table(other)),
        data(::boost::type_erasure::call(
            ::boost::type_erasure::detail::make(
                false? this->_boost_type_erasure_deduce_constructor(other) : 0
            ), other)
        )
    {}

    // disambiguating overloads
    template<class U, class Map>
    any(U& data_arg, static_binding<Map>& binding_arg)
      : table(binding_arg),
        data(data_arg)
    {}
    template<class U, class Map>
    any(const U& data_arg, static_binding<Map>& binding_arg)
      : table(binding_arg),
        data(data_arg)
    {}
    template<class U, class Map>
    any(U& data_arg, const static_binding<Map>& binding_arg)
      : table(binding_arg),
        data(data_arg)
    {}
    template<class Concept2, class Tag2, class Map>
    any(any<Concept2, Tag2>& other, static_binding<Map>& binding_arg)
      : table(::boost::type_erasure::detail::access::table(other), binding_arg),
        data(::boost::type_erasure::call(
            constructible<
                typename ::boost::remove_const<
                    typename boost::remove_reference<Tag2>::type
                >::type(const typename boost::remove_reference<Tag2>::type&)
            >(), other)
        )
    {}
    template<class Concept2, class Tag2, class Map>
    any(any<Concept2, Tag2>& other, const static_binding<Map>& binding_arg)
      : table(::boost::type_erasure::detail::access::table(other), binding_arg),
        data(::boost::type_erasure::call(
            constructible<
                typename ::boost::remove_const<
                    typename boost::remove_reference<Tag2>::type
                >::type(const typename boost::remove_reference<Tag2>::type&)
            >(), other)
        )
    {}
    template<class Concept2, class Tag2, class Map>
    any(const any<Concept2, Tag2>& other, static_binding<Map>& binding_arg)
      : table(::boost::type_erasure::detail::access::table(other), binding_arg),
        data(::boost::type_erasure::call(
            constructible<
                typename ::boost::remove_const<
                    typename boost::remove_reference<Tag2>::type
                >::type(const typename boost::remove_reference<Tag2>::type&)
            >(), other)
        )
    {}
    template<class Concept2, class Tag2>
    any(any<Concept2, Tag2>& other, binding<Concept>& binding_arg)
      : table(binding_arg),
        data(::boost::type_erasure::call(
            constructible<
                typename ::boost::remove_const<
                    typename boost::remove_reference<Tag2>::type
                >::type(const typename boost::remove_reference<Tag2>::type&)
            >(), other)
        )
    {}
    template<class Concept2, class Tag2>
    any(any<Concept2, Tag2>& other, const binding<Concept>& binding_arg)
      : table(binding_arg),
        data(::boost::type_erasure::call(
            constructible<
                typename ::boost::remove_const<
                    typename boost::remove_reference<Tag2>::type
                >::type(const typename boost::remove_reference<Tag2>::type&)
            >(), other)
        )
    {}
    template<class Concept2, class Tag2>
    any(const any<Concept2, Tag2>& other, binding<Concept>& binding_arg)
      : table(binding_arg),
        data(::boost::type_erasure::call(
            constructible<
                typename ::boost::remove_const<
                    typename boost::remove_reference<Tag2>::type
                >::type(const typename boost::remove_reference<Tag2>::type&)
            >(), other)
        )
    {}

    // One argument is a special case.  The argument must be an any
    // and the constructor must be explicit.
    template<class Tag2>
    explicit any(const any<Concept, Tag2>& other)
      : table(::boost::type_erasure::detail::access::table(other)),
        data(::boost::type_erasure::call(
            ::boost::type_erasure::detail::make(
                false? this->_boost_type_erasure_deduce_constructor(other) : 0
            ), other)
        )
    {}
    template<class Tag2>
    explicit any(any<Concept, Tag2>& other)
      : table(::boost::type_erasure::detail::access::table(other)),
        data(::boost::type_erasure::call(
            ::boost::type_erasure::detail::make(
                false? this->_boost_type_erasure_deduce_constructor(other) : 0
            ), other)
        )
    {}

    explicit any(const binding<Concept>& binding_arg)
      : table(binding_arg),
        data(
            ::boost::type_erasure::call(
                binding_arg,
                ::boost::type_erasure::constructible<T()>()
            )
        )
    {}

#if !defined(BOOST_NO_VARIADIC_TEMPLATES)

    template<class R, class... A, class... U>
    const table_type& _boost_type_erasure_extract_table(
        ::boost::type_erasure::constructible<R(A...)>*,
        U&... u)
    {
        return *::boost::type_erasure::detail::extract_table(static_cast<void(*)(A...)>(0), u...);
    }

    template<class U0, class U1, class... U>
    any(const U0& u0, const U1& u1, const U&... u)
      : table(
            _boost_type_erasure_extract_table(
                false? this->_boost_type_erasure_deduce_constructor(u0, u1, u...) : 0,
                u0, u1, u...
            )
        ),
        data(
            ::boost::type_erasure::detail::make(
                false? this->_boost_type_erasure_deduce_constructor(u0, u1, u...) : 0
            )(u0, u1, u...)
        )
    {}

    template<class U0, class U1, class... U>
    any(U0& u0, U1& u1, U&... u)
      : table(
            _boost_type_erasure_extract_table(
                false? this->_boost_type_erasure_deduce_constructor(u0, u1, u...) : 0,
                u0, u1, u...
            )
        ),
        data(
            ::boost::type_erasure::detail::make(
                false? this->_boost_type_erasure_deduce_constructor(u0, u1, u...) : 0
            )(u0, u1, u...)
        )
    {}

    template<class U0, class... U>
    any(const binding<Concept>& binding_arg, const U0& u0, const U&... u)
      : table(binding_arg),
        data(
            ::boost::type_erasure::call(
                binding_arg,
                ::boost::type_erasure::detail::make(
                    false? this->_boost_type_erasure_deduce_constructor(u0, u...) : 0
                ),
                u0, u...
            )
        )
    {}

    template<class U0, class... U>
    any(const binding<Concept>& binding_arg, U0& u0, U&... u)
      : table(binding_arg),
        data(
            ::boost::type_erasure::call(
                binding_arg,
                ::boost::type_erasure::detail::make(
                    false? this->_boost_type_erasure_deduce_constructor(u0, u...) : 0
                ),
                u0, u...
            )
        )
    {}
    
    // disambiguate
    template<class U0, class... U>
    any(binding<Concept>& binding_arg, U0& u0, U&... u)
      : table(binding_arg),
        data(
            ::boost::type_erasure::call(
                binding_arg,
                ::boost::type_erasure::detail::make(
                    false? this->_boost_type_erasure_deduce_constructor(u0, u...) : 0
                ),
                u0, u...
            )
        )
    {}

#else

#define BOOST_PP_FILENAME_1 <boost/type_erasure/detail/construct.hpp>
#define BOOST_PP_ITERATION_LIMITS (1, BOOST_TYPE_ERASURE_MAX_ARITY)
#include BOOST_PP_ITERATE()

#endif

#endif

    /**
     * Assigns to an @ref any.
     *
     * If an appropriate overload of @ref assignable is not available
     * and @ref relaxed_match is in @c Concept, falls back on
     * constructing from @c other.
     *
     * \throws Whatever the assignment operator of the contained
     *         type throws.  When falling back on construction,
     *         throws @c std::bad_alloc or whatever the copy
     *         constructor of the contained type throws.  In
     *         this case assignment provides the strong exception
     *         guarantee.  When calling the assignment operator
     *         of the contained type, the exception guarantee is
     *         whatever the contained type provides.
     */
    any& operator=(const any& other)
    {
        _boost_type_erasure_resolve_assign(other);
        return *this;
    }
    /**
     * Assigns to an @ref any.
     *
     * If an appropriate overload of @ref assignable is not available
     * and @ref relaxed_match is in @c Concept, falls back on
     * constructing from @c other.
     *
     * \throws Whatever the assignment operator of the contained
     *         type throws.  When falling back on construction,
     *         throws @c std::bad_alloc or whatever the copy
     *         constructor of the contained type throws.  In
     *         this case assignment provides the strong exception
     *         guarantee.  When calling an assignment operator
     *         of the contained type, the exception guarantee is
     *         whatever the contained type provides.
     */
    template<class U>
    any& operator=(const U& other)
    {
        _boost_type_erasure_resolve_assign(other);
        return *this;
    }
    /**
     * \pre @c Concept includes @c destructible<T>.
     */
    ~any()
    {
        table.template find<destructible<T> >()(data);
    }
private:
    /** INTERNAL ONLY */
    void _boost_type_erasure_swap(any& other)
    {
        ::std::swap(data, other.data);
        ::std::swap(table, other.table);
    }
    /** INTERNAL ONLY */
    template<class Other>
    void _boost_type_erasure_resolve_assign(const Other& other)
    {
        _boost_type_erasure_assign_impl(
            other,
            false? this->_boost_type_erasure_deduce_assign(
                ::boost::type_erasure::detail::make_fallback(
                    other,
                    ::boost::mpl::bool_<
                        sizeof(
                            ::boost::type_erasure::detail::check_overload(
                                ::boost::declval<any&>().
                                    _boost_type_erasure_deduce_assign(other)
                            )
                        ) == sizeof(::boost::type_erasure::detail::yes)
                    >()
                )
            ) : 0,
            ::boost::type_erasure::is_relaxed<Concept>()
        );
    }
    /** INTERNAL ONLY */
    template<class Other, class U>
    void _boost_type_erasure_assign_impl(
        const Other& other,
        const assignable<T, U>*,
        ::boost::mpl::false_)
    {
        ::boost::type_erasure::call(assignable<T, U>(), *this, other);
    }
    /** INTERNAL ONLY */
    template<class Other, class U>
    void _boost_type_erasure_assign_impl(
        const Other& other,
        const assignable<T, U>*,
        ::boost::mpl::true_)
    {
        ::boost::type_erasure::call(assignable<T, U>(), *this, other);
    }
    /** INTERNAL ONLY */
    template<class Other>
    void _boost_type_erasure_assign_impl(
        const Other& other,
        const void*,
        ::boost::mpl::true_)
    {
        any temp(other);
        _boost_type_erasure_swap(temp);
    }
    /** INTERNAL ONLY */
    template<class Concept2, class Tag2>
    void _boost_type_erasure_resolve_assign(const any<Concept2, Tag2>& other)
    {
        _boost_type_erasure_assign_impl(
            other,
            false? this->_boost_type_erasure_deduce_assign(
                ::boost::type_erasure::detail::make_fallback(
                    other,
                    ::boost::mpl::bool_<
                        sizeof(
                            ::boost::type_erasure::detail::check_overload(
                                ::boost::declval<any&>().
                                    _boost_type_erasure_deduce_assign(other)
                            )
                        ) == sizeof(::boost::type_erasure::detail::yes)
                    >()
                )
            ) : 0,
            false? this->_boost_type_erasure_deduce_constructor(
                ::boost::type_erasure::detail::make_fallback(
                    other,
                    ::boost::mpl::bool_<
                        sizeof(
                            ::boost::type_erasure::detail::check_overload(
                                ::boost::declval<any&>().
                                    _boost_type_erasure_deduce_constructor(other)
                            )
                        ) == sizeof(::boost::type_erasure::detail::yes)
                    >()
                )
            ) : 0,
            ::boost::type_erasure::is_relaxed<Concept>()
        );
    }
    /** INTERNAL ONLY */
    template<class Other, class U>
    void _boost_type_erasure_assign_impl(
        const Other& other,
        const assignable<T, U>*,
        const void*,
        ::boost::mpl::false_)
    {
        ::boost::type_erasure::call(assignable<T, U>(), *this, other);
    }
    /** INTERNAL ONLY */
    template<class Other, class U>
    void _boost_type_erasure_assign_impl(
        const Other& other,
        const assignable<T, U>*,
        const void*,
        ::boost::mpl::true_)
    {
        ::boost::type_erasure::call(assignable<T, U>(), *this, other);
    }
    /** INTERNAL ONLY */
    template<class Other, class Sig>
    void _boost_type_erasure_assign_impl(
        const Other& other,
        const void*,
        const constructible<Sig>*,
        ::boost::mpl::true_)
    {
        any temp(other);
        _boost_type_erasure_swap(temp);
    }
    /** INTERNAL ONLY */
    template<class Other, class U, class Sig>
    void _boost_type_erasure_assign_impl(
        const Other& other,
        const assignable<T, U>*,
        const constructible<Sig>*,
        ::boost::mpl::true_)
    {
        if(::boost::type_erasure::check_match(assignable<T, U>(), *this, other))
        {
            ::boost::type_erasure::unchecked_call(assignable<T, U>(), *this, other);
        }
        else
        {
            any temp(other);
            _boost_type_erasure_swap(temp);
        }
    }
    friend struct ::boost::type_erasure::detail::access;
    // The table has to be initialized first for exception
    // safety in some constructors.
    table_type table;
    ::boost::type_erasure::detail::storage data;
};

template<class Concept, class T>
class any<Concept, T&> :
    public ::boost::type_erasure::detail::compute_bases<
        ::boost::type_erasure::any<Concept, T&>,
        Concept,
        T
    >::type
{
    typedef ::boost::type_erasure::binding<Concept> table_type;
public:
    /** INTERNAL ONLY */
    typedef Concept _boost_type_erasure_concept_type;
    /** INTERNAL ONLY */
    any(const ::boost::type_erasure::detail::storage& data_arg,
        const table_type& table_arg)
      : data(data_arg),
        table(table_arg)
    {}
    /**
     * Constructs an @ref any from a reference.
     *
     * \param arg The object to bind the reference to.
     *
     * \pre @c U is a model of @c Concept.
     * \pre @c Concept must not refer to any placeholder besides @c T.
     *
     * \throws Nothing.
     */
    template<class U>
    explicit any(U& arg
#ifndef BOOST_TYPE_ERASURE_DOXYGEN
        ,  typename ::boost::disable_if<
            ::boost::mpl::or_<
                ::boost::is_const<U>,
                ::boost::type_erasure::detail::is_any<U>
            >
        >::type* = 0
#endif
        )
      : table(
            ::boost::type_erasure::make_binding<
                ::boost::mpl::map< ::boost::mpl::pair<T, U> >
            >()
        )
    {
        data.data = ::boost::addressof(arg);
    }
    /**
     * Constructs an @ref any from a reference.
     *
     * \param arg The object to bind the reference to.
     * \param binding Specifies the actual types that
     *        all the placeholders should bind to.
     *
     * \pre @c U is a model of @c Concept.
     * \pre @c Map is an MPL map with an entry for every
     *         placeholder referred to by @c Concept.
     *
     * \throws Nothing.
     */
    template<class U, class Map>
    any(U& arg, const static_binding<Map>& binding_arg)
      : table(binding_arg)
    {
        data.data = ::boost::addressof(arg);
    }
    /**
     * Constructs an @ref any from another reference.
     *
     * \param other The reference to copy.
     *
     * \throws Nothing.
     */
    any(const any& other)
      : data(other.data),
        table(other.table)
    {}
#ifndef BOOST_TYPE_ERASURE_DOXYGEN
    any(any& other)
      : data(other.data),
        table(other.table)
    {}
#endif
    /**
     * Constructs an @ref any from another @ref any.
     *
     * \param other The object to bind the reference to.
     *
     * \throws Nothing.
     */
    any(any<Concept, T>& other)
      : data(::boost::type_erasure::detail::access::data(other)),
        table(::boost::type_erasure::detail::access::table(other))
    {}
    /**
     * Constructs an @ref any from another reference.
     *
     * \param other The reference to copy.
     *
     * \pre @c Concept must not refer to any placeholder besides @c T.
     * \pre After substituting @c T for @c Tag2, the requirements of
     *      @c Concept2 must be a superset of of the requirements of
     *      @c Concept.
     *
     * \throws std::bad_alloc
     */
    template<class Concept2, class Tag2>
    any(const any<Concept2, Tag2&>& other
#ifndef BOOST_TYPE_ERASURE_DOXYGEN
        , typename ::boost::disable_if<
            ::boost::mpl::or_<
                ::boost::is_same<Concept, Concept2>,
                ::boost::is_const<Tag2>
            >
        >::type* = 0
#endif
        )
      : data(::boost::type_erasure::detail::access::data(other)),
        table(
            ::boost::type_erasure::detail::access::table(other),
            ::boost::mpl::map<
                ::boost::mpl::pair<
                    T,
                    Tag2
                >
            >())
    {}
    /**
     * Constructs an @ref any from another @ref any.
     *
     * \param other The object to bind the reference to.
     *
     * \pre @c Concept must not refer to any placeholder besides @c T.
     * \pre After substituting @c T for @c Tag2, the requirements of
     *      @c Concept2 must be a superset of of the requirements of
     *      @c Concept.
     *
     * \throws std::bad_alloc
     */
    template<class Concept2, class Tag2>
    any(any<Concept2, Tag2>& other
#ifndef BOOST_TYPE_ERASURE_DOXYGEN
        , typename ::boost::disable_if<
            ::boost::mpl::or_<
                ::boost::is_same<Concept, Concept2>,
                ::boost::is_const<typename ::boost::remove_reference<Tag2>::type>
            >
        >::type* = 0
#endif
        )
      : data(::boost::type_erasure::detail::access::data(other)),
        table(
            ::boost::type_erasure::detail::access::table(other),
            ::boost::mpl::map<
                ::boost::mpl::pair<
                    T,
                    typename ::boost::remove_reference<Tag2>::type
                >
            >())
    {}
    /**
     * Constructs an @ref any from another reference.
     *
     * \param other The reference to copy.
     * \param binding Specifies the mapping between the two concepts.
     *
     * \pre @c Map must be an MPL map with keys for all the placeholders
     *      used by @c Concept and values for the corresponding
     *      placeholders in @c Concept2.
     * \pre After substituting placeholders according to @c Map, the
     *      requirements of @c Concept2 must be a superset of of the
     *      requirements of @c Concept.
     *
     * \throws std::bad_alloc
     */
    template<class Concept2, class Tag2, class Map>
    any(const any<Concept2, Tag2&>& other, const static_binding<Map>& binding_arg
#ifndef BOOST_TYPE_ERASURE_DOXYGEN
        , typename ::boost::disable_if< ::boost::is_const<Tag2> >::type* = 0
#endif
        )
      : data(::boost::type_erasure::detail::access::data(other)),
        table(::boost::type_erasure::detail::access::table(other), binding_arg)
    {}
    /**
     * Constructs an @ref any from another @ref any.
     *
     * \param other The object to bind the reference to.
     * \param binding Specifies the mapping between the two concepts.
     *
     * \pre @c Map must be an MPL map with keys for all the placeholders
     *      used by @c Concept and values for the corresponding
     *      placeholders in @c Concept2.
     * \pre After substituting placeholders according to @c Map, the
     *      requirements of @c Concept2 must be a superset of of the
     *      requirements of @c Concept.
     *
     * \throws std::bad_alloc
     */
    template<class Concept2, class Tag2, class Map>
    any(any<Concept2, Tag2>& other, const static_binding<Map>& binding_arg
#ifndef BOOST_TYPE_ERASURE_DOXYGEN
        , typename ::boost::disable_if<
            ::boost::is_const<typename ::boost::remove_reference<Tag2>::type>
        >::type* = 0
#endif
        )
      : data(::boost::type_erasure::detail::access::data(other)),
        table(::boost::type_erasure::detail::access::table(other), binding_arg)
    {}
    /**
     * Constructs an @ref any from another reference.
     *
     * \param other The reference to copy.
     * \param binding Specifies the bindings of placeholders to actual types.
     *
     * \pre The type stored in @c other must match the type expected by
     *      @c binding.
     *
     * \post binding_of(*this) == @c binding
     *
     * \throws Nothing.
     */
    template<class Concept2, class Tag2>
    any(const any<Concept2, Tag2&>& other, const binding<Concept>& binding_arg
#ifndef BOOST_TYPE_ERASURE_DOXYGEN
        , typename ::boost::disable_if<
            ::boost::is_const<Tag2>
        >::type* = 0
#endif
        )
      : data(::boost::type_erasure::detail::access::data(other)),
        table(binding_arg)
    {}
    /**
     * Constructs an @ref any from another @ref any.
     *
     * \param other The object to bind the reference to.
     * \param binding Specifies the bindings of placeholders to actual types.
     *
     * \pre The type stored in @c other must match the type expected by
     *      @c binding.
     *
     * \post binding_of(*this) == @c binding
     *
     * \throws Nothing.
     */
    template<class Concept2, class Tag2>
    any(any<Concept2, Tag2>& other, const binding<Concept>& binding_arg
#ifndef BOOST_TYPE_ERASURE_DOXYGEN
        , typename ::boost::disable_if<
            ::boost::is_const<typename ::boost::remove_reference<Tag2>::type>
        >::type* = 0
#endif
        )
      : data(::boost::type_erasure::detail::access::data(other)),
        table(binding_arg)
    {}
    
    /**
     * Assigns to an @ref any.
     *
     * If an appropriate overload of @ref assignable is not available
     * and @ref relaxed_match is in @c Concept, falls back on
     * constructing from @c other.
     *
     * \throws Whatever the assignment operator of the contained
     *         type throws.  When falling back on construction,
     *         throws @c std::bad_alloc.  In this case assignment
     *         provides the strong exception guarantee.  When
     *         calling the assignment operator of the contained type,
     *         the exception guarantee is whatever the contained type provides.
     */
    any& operator=(const any& other)
    {
        _boost_type_erasure_resolve_assign(other);
        return *this;
    }
    
    /**
     * Assigns to an @ref any.
     *
     * If an appropriate overload of @ref assignable is not available
     * and @ref relaxed_match is in @c Concept, falls back on
     * constructing from @c other.
     *
     * \throws Whatever the assignment operator of the contained
     *         type throws.  When falling back on construction,
     *         throws @c std::bad_alloc.  In this case assignment
     *         provides the strong exception guarantee.  When
     *         calling the assignment operator of the contained type,
     *         the exception guarantee is whatever the contained type provides.
     */
    template<class U>
    any& operator=(U& other)
    {
        _boost_type_erasure_resolve_assign(other);
        return *this;
    }
    
    /**
     * Assigns to an @ref any.
     *
     * If an appropriate overload of @ref assignable is not available
     * and @ref relaxed_match is in @c Concept, falls back on
     * constructing from @c other.
     *
     * \throws Whatever the assignment operator of the contained
     *         type throws.  When falling back on construction,
     *         throws @c std::bad_alloc.  In this case assignment
     *         provides the strong exception guarantee.  When
     *         calling the assignment operator of the contained type,
     *         the exception guarantee is whatever the contained type provides.
     */
    template<class U>
    any& operator=(const U& other)
    {
        _boost_type_erasure_resolve_assign(other);
        return *this;
    }

private:

    /** INTERNAL ONLY */
    void _boost_type_erasure_swap(any& other)
    {
        ::std::swap(data, other.data);
        ::std::swap(table, other.table);
    }
    /** INTERNAL ONLY */
    template<class Other>
    void _boost_type_erasure_resolve_assign(Other& other)
    {
        _boost_type_erasure_assign_impl(
            other,
            false? this->_boost_type_erasure_deduce_assign(
                ::boost::type_erasure::detail::make_fallback(
                    other,
                    ::boost::mpl::bool_<
                        sizeof(
                            ::boost::type_erasure::detail::check_overload(
                                ::boost::declval<any&>().
                                    _boost_type_erasure_deduce_assign(other)
                            )
                        ) == sizeof(::boost::type_erasure::detail::yes)
                    >()
                )
            ) : 0,
            ::boost::type_erasure::is_relaxed<Concept>()
        );
    }
    /** INTERNAL ONLY */
    template<class Other, class U>
    void _boost_type_erasure_assign_impl(
        Other& other,
        const assignable<T, U>*,
        ::boost::mpl::false_)
    {
        ::boost::type_erasure::call(assignable<T, U>(), *this, other);
    }
    /** INTERNAL ONLY */
    template<class Other, class U>
    void _boost_type_erasure_assign_impl(
        Other& other,
        const assignable<T, U>*,
        ::boost::mpl::true_)
    {
        if(::boost::type_erasure::check_match(assignable<T, U>(), *this, other)) {
            ::boost::type_erasure::unchecked_call(assignable<T, U>(), *this, other);
        } else {
            any temp(other);
            _boost_type_erasure_swap(temp);
        }
    }
    /** INTERNAL ONLY */
    template<class Other>
    void _boost_type_erasure_assign_impl(
        Other& other,
        const void*,
        ::boost::mpl::true_)
    {
        any temp(other);
        _boost_type_erasure_swap(temp);
    }

    friend struct ::boost::type_erasure::detail::access;
    ::boost::type_erasure::detail::storage data;
    table_type table;
};

#ifdef BOOST_MSVC
#pragma warning(pop)
#endif

template<class Concept, class T>
class any<Concept, const T&> :
    public ::boost::type_erasure::detail::compute_bases<
        ::boost::type_erasure::any<Concept, const T&>,
        Concept,
        T
    >::type
{
    typedef ::boost::type_erasure::binding<Concept> table_type;
public:
    /** INTERNAL ONLY */
    typedef Concept _boost_type_erasure_concept_type;
    /** INTERNAL ONLY */
    any(const ::boost::type_erasure::detail::storage& data_arg,
        const table_type& table_arg)
      : data(data_arg),
        table(table_arg)
    {}
    /**
     * Constructs an @ref any from a reference.
     *
     * \param arg The object to bind the reference to.
     *
     * \pre @c U is a model of @c Concept.
     * \pre @c Concept must not refer to any placeholder besides @c T.
     *
     * \throws Nothing.
     */
    template<class U>
    explicit any(const U& arg)
      : table(
            ::boost::type_erasure::make_binding<
                ::boost::mpl::map< ::boost::mpl::pair<T, U> >
            >()
        )
    {
        data.data = const_cast<void*>(static_cast<const void*>(::boost::addressof(arg)));
    }
    /**
     * Constructs an @ref any from a reference.
     *
     * \param arg The object to bind the reference to.
     * \param binding Specifies the actual types that
     *        all the placeholders should bind to.
     *
     * \pre @c U is a model of @c Concept.
     * \pre @c Map is an MPL map with an entry for every
     *         placeholder referred to by @c Concept.
     *
     * \throws Nothing.
     */
    template<class U, class Map>
    any(const U& arg, const static_binding<Map>& binding_arg)
      : table(binding_arg)
    {
        data.data = const_cast<void*>(static_cast<const void*>(::boost::addressof(arg)));
    }
    /**
     * Constructs an @ref any from another @ref any.
     *
     * \param other The reference to copy.
     *
     * \throws Nothing.
     */
    any(const any& other)
      : data(other.data),
        table(other.table)
    {}
    /**
     * Constructs an @ref any from another @ref any.
     *
     * \param other The reference to copy.
     *
     * \throws Nothing.
     */
    any(const any<Concept, T&>& other)
      : data(::boost::type_erasure::detail::access::data(other)),
        table(::boost::type_erasure::detail::access::table(other))
    {}
    /**
     * Constructs an @ref any from another @ref any.
     *
     * \param other The object to bind the reference to.
     *
     * \throws Nothing.
     */
    any(const any<Concept, T>& other)
      : data(::boost::type_erasure::detail::access::data(other)),
        table(::boost::type_erasure::detail::access::table(other))
    {}
    /**
     * Constructs an @ref any from another @ref any.
     *
     * \param other The object to bind the reference to.
     *
     * \pre @c Concept must not refer to any placeholder besides @c T.
     * \pre After substituting @c T for @c Tag2, the requirements of
     *      @c Concept2 must be a superset of of the requirements of
     *      @c Concept.
     *
     * \throws std::bad_alloc
     */
    template<class Concept2, class Tag2>
    any(const any<Concept2, Tag2>& other
#ifndef BOOST_TYPE_ERASURE_DOXYGEN
        , typename ::boost::disable_if< ::boost::is_same<Concept, Concept2> >::type* = 0
#endif
        )
      : data(::boost::type_erasure::detail::access::data(other)),
        table(
            ::boost::type_erasure::detail::access::table(other),
            ::boost::mpl::map<
                ::boost::mpl::pair<
                    T,
                    typename ::boost::remove_const<
                        typename ::boost::remove_reference<Tag2>::type
                    >::type
                >
            >())
    {}
    /**
     * Constructs an @ref any from another @ref any.
     *
     * \param other The object to bind the reference to.
     * \param binding Specifies the mapping between the two concepts.
     *
     * \pre @c Map must be an MPL map with keys for all the placeholders
     *      used by @c Concept and values for the corresponding
     *      placeholders in @c Concept2.
     * \pre After substituting placeholders according to @c Map, the
     *      requirements of @c Concept2 must be a superset of of the
     *      requirements of @c Concept.
     *
     * \throws std::bad_alloc
     */
    template<class Concept2, class Tag2, class Map>
    any(const any<Concept2, Tag2>& other, const static_binding<Map>& binding_arg)
      : data(::boost::type_erasure::detail::access::data(other)),
        table(::boost::type_erasure::detail::access::table(other), binding_arg)
    {}
    /**
     * Constructs an @ref any from another @ref any.
     *
     * \param other The object to bind the reference to.
     * \param binding Specifies the bindings of placeholders to actual types.
     *
     * \pre The type stored in @c other must match the type expected by
     *      @c binding.
     *
     * \post binding_of(*this) == @c binding
     *
     * \throws Nothing.
     */
    template<class Concept2, class Tag2>
    any(const any<Concept2, Tag2>& other, const binding<Concept>& binding_arg)
      : data(::boost::type_erasure::detail::access::data(other)),
        table(binding_arg)
    {}
    
    
    /**
     * Assigns to an @ref any.
     *
     * \pre @ref relaxed_match is in @c Concept.
     *
     * \throws Nothing.
     */
    any& operator=(const any& other)
    {
        BOOST_MPL_ASSERT((::boost::type_erasure::is_relaxed<Concept>));
        any temp(other);
        _boost_type_erasure_swap(temp);
        return *this;
    }
    /**
     * Assigns to an @ref any.
     *
     * \pre @ref relaxed_match is in @c Concept.
     *
     * \throws std::bad_alloc.  Provides the strong exception guarantee.
     */
    template<class U>
    any& operator=(const U& other)
    {
        BOOST_MPL_ASSERT((::boost::type_erasure::is_relaxed<Concept>));
        any temp(other);
        _boost_type_erasure_swap(temp);
        return *this;
    }

private:
    /** INTERNAL ONLY */
    void _boost_type_erasure_swap(any& other)
    {
        ::std::swap(data, other.data);
        ::std::swap(table, other.table);
    }
    friend struct ::boost::type_erasure::detail::access;
    ::boost::type_erasure::detail::storage data;
    table_type table;
};

}
}

#endif
