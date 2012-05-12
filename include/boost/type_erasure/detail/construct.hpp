// Boost.TypeErasure library
//
// Copyright 2011 Steven Watanabe
//
// Distributed under the Boost Software License Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// $Id$

// NO #include GUARD.

#define N BOOST_PP_ITERATION()

#if N > 1

    template<
        class R
        BOOST_PP_ENUM_TRAILING_PARAMS(N, class A)
        BOOST_PP_ENUM_TRAILING_PARAMS(N, class U)
    >
    const table_type& _boost_type_erasure_extract_table(
        ::boost::type_erasure::constructible<R(BOOST_PP_ENUM_PARAMS(N, A))>*
        BOOST_PP_ENUM_TRAILING_BINARY_PARAMS(N, U, &u))
    {
        return *::boost::type_erasure::detail::BOOST_PP_CAT(extract_table, N)(
            (R(*)(BOOST_PP_ENUM_PARAMS(N, A)))0,
            BOOST_PP_ENUM_PARAMS(N, u));
    }

    template<BOOST_PP_ENUM_PARAMS(N, class U)>
    any(BOOST_PP_ENUM_BINARY_PARAMS(N, const U, &u))
      : table(
            _boost_type_erasure_extract_table(
                false? this->_boost_type_erasure_deduce_constructor(BOOST_PP_ENUM_PARAMS(N, u)) : 0
                BOOST_PP_ENUM_TRAILING_PARAMS(N, u)
            )
        ),
        data(::boost::type_erasure::call(
            ::boost::type_erasure::detail::make(
                false? this->_boost_type_erasure_deduce_constructor(BOOST_PP_ENUM_PARAMS(N, u)) : 0
            ), BOOST_PP_ENUM_PARAMS(N, u))
        )
    {}

    template<BOOST_PP_ENUM_PARAMS(N, class U)>
    any(BOOST_PP_ENUM_BINARY_PARAMS(N, U, &u))
      : table(
            _boost_type_erasure_extract_table(
                false? this->_boost_type_erasure_deduce_constructor(BOOST_PP_ENUM_PARAMS(N, u)) : 0
                BOOST_PP_ENUM_TRAILING_PARAMS(N, u)
            )
        ),
        data(::boost::type_erasure::call(
            ::boost::type_erasure::detail::make(
                false? this->_boost_type_erasure_deduce_constructor(BOOST_PP_ENUM_PARAMS(N, u)) : 0
            ), BOOST_PP_ENUM_PARAMS(N, u))
        )
    {}

#endif

    template<BOOST_PP_ENUM_PARAMS(N, class U)>
    any(const binding<Concept>& binding_arg BOOST_PP_ENUM_TRAILING_BINARY_PARAMS(N, const U, &u))
      : table(binding_arg),
        data(
            ::boost::type_erasure::call(
                binding_arg,
                ::boost::type_erasure::detail::make(
                    false? this->_boost_type_erasure_deduce_constructor(BOOST_PP_ENUM_PARAMS(N, u)) : 0
                )
                BOOST_PP_ENUM_TRAILING_PARAMS(N, u)
            )
        )
    {}

    template<BOOST_PP_ENUM_PARAMS(N, class U)>
    any(const binding<Concept>& binding_arg BOOST_PP_ENUM_TRAILING_BINARY_PARAMS(N, U, &u))
      : table(binding_arg),
        data(
            ::boost::type_erasure::call(
                binding_arg,
                ::boost::type_erasure::detail::make(
                    false? this->_boost_type_erasure_deduce_constructor(BOOST_PP_ENUM_PARAMS(N, u)) : 0
                )
                BOOST_PP_ENUM_TRAILING_PARAMS(N, u)
            )
        )
    {}

    // disambiguate
    template<BOOST_PP_ENUM_PARAMS(N, class U)>
    any(binding<Concept>& binding_arg BOOST_PP_ENUM_TRAILING_BINARY_PARAMS(N, U, &u))
      : table(binding_arg),
        data(
            ::boost::type_erasure::call(
                binding_arg,
                ::boost::type_erasure::detail::make(
                    false? this->_boost_type_erasure_deduce_constructor(BOOST_PP_ENUM_PARAMS(N, u)) : 0
                )
                BOOST_PP_ENUM_TRAILING_PARAMS(N, u)
            )
        )
    {}

#undef N
