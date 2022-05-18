// Copyright 2019 Ilya Kiselev.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once

namespace memserial {
namespace detail {

/**
 *
 */
template< typename ... Args >
struct SerialType< tuple< Args... >, std::true_type > {
    using ValueType = tuple< Args... >;
    static constexpr std::size_t tuple_size = std::tuple_size< ValueType >::value;

    template< std::size_t Index >
    using FieldType = typename std::tuple_element< Index, ValueType >::type;

    /**
     *
     */
    static constexpr bool match( uint32_t value ) {

        MatchFunctor functor{ value, SERIAL_HASH_SALT, SERIAL_NESTING_LIMIT };
        return search_sequence( functor, size_t_< 0 >{}, size_t_< tuple_size >{} );
    }

    /**
     *
     */
    static constexpr uint32_t hash() {

        uint32_t real_hash = SERIAL_HASH_SALT;
        hash( real_hash );
        return real_hash;
    }

    static constexpr void hash( uint32_t& value, std::size_t nesting = SERIAL_NESTING_LIMIT ) {

        HashFunctor functor{ value, nesting };
        foreach_sequence( functor, size_t_< 0 >{}, size_t_< tuple_size >{} );
    }

    /**
     *
     */
    static constexpr std::size_t size() {

        ConstFunctor functor{ 0 };
        foreach_sequence( functor, size_t_< 0 >{}, size_t_< tuple_size >{} );
        return functor.size;
    }

    /**
     *
     */
    static std::size_t size( const ValueType& value ) {

        SizeFunctor functor{ const_cast< ValueType& >( value ), 0 };
        foreach_sequence( functor, size_t_< 0 >{}, size_t_< tuple_size >{} );
        return functor.size;
    }

    /**
     *
     */
    template< typename Iterator >
    static void init( ValueType& value, Iterator& begin, Iterator& end ) {

        Iterator least_end = end - size();
        InitFunctor< Iterator > functor{ value, begin, least_end };
        foreach_sequence( functor, size_t_< 0 >{}, size_t_< tuple_size >{} );
    }

    /**
     *
     */
    template< typename Iterator >
    static void bout( const ValueType& value, Iterator& begin ) {

        BoutFunctor< Iterator > functor{ const_cast< ValueType& >( value ), begin };
        foreach_sequence( functor, size_t_< 0 >{}, size_t_< tuple_size >{} );
    }

    /**
     *
     */
    template< typename Iterator >
    static void bin( ValueType& value, Iterator& begin ) {

        BinFunctor< Iterator > functor{ value, begin };
        foreach_sequence( functor, size_t_< 0 >{}, size_t_< tuple_size >{} );
    }

    /**
     *
     */
    template< typename Stream >
    static void debug( const ValueType& value, Stream& stream, uint8_t level ) {

        stream << SerialMetatype< ValueType >::alias().data << ": ";

        std::string separator( 3 * ( level + 1 ) + 1, ' ' );
        separator[ 0 ] = '\n';

        DebugFunctor< Stream > functor{ const_cast< ValueType& >( value ), stream, std::move( separator ), level };
        foreach_sequence( functor, size_t_< 0 >{}, size_t_< tuple_size >{} );
    }

    /**
     *
     */
    struct MatchFunctor {
        uint32_t value;
        uint32_t real_hash;
        std::size_t nesting;

        template< std::size_t Index >
        constexpr bool operator()( size_t_< Index > ) {
            SerialType< FieldType< Index > >::hash( real_hash, nesting );
            return real_hash == value;
        }
    };

    /**
     *
     */
    struct HashFunctor {
        uint32_t& value;
        std::size_t nesting;

        template< std::size_t Index >
        constexpr void operator()( size_t_< Index > ) {
            SerialType< FieldType< Index > >::hash( value, nesting );
        }
    };

    /**
     *
     */
    struct ConstFunctor {
        std::size_t size;

        template< std::size_t Index >
        constexpr void operator()( size_t_< Index > ) {
            size += SerialType< FieldType< Index > >::size();
        }
    };

    /**
     *
     */
    struct SizeFunctor {
        ValueType& value;
        std::size_t size;

        template< std::size_t Index >
        constexpr void operator()( size_t_< Index > ) {
            size += SerialType< FieldType< Index > >::size( std::get< Index >( value ) );
        }
    };

    /**
     *
     */
    template< typename Iterator >
    struct InitFunctor {
        ValueType& value;
        Iterator& begin;
        Iterator& end;

        template< std::size_t Index >
        constexpr void operator()( size_t_< Index > ) {
            end += SerialType< FieldType< Index > >::size();
            SerialType< FieldType< Index > >::init( std::get< Index >( value ), begin, end );
        }
    };

    /**
     *
     */
    template< typename Iterator >
    struct BoutFunctor {
        ValueType& value;
        Iterator& begin;

        template< std::size_t Index >
        constexpr void operator()( size_t_< Index > ) {
            SerialType< FieldType< Index > >::bout( std::get< Index >( value ), begin );
        }
    };

    /**
     *
     */
    template< typename Iterator >
    struct BinFunctor {
        ValueType& value;
        Iterator& begin;

        template< std::size_t Index >
        constexpr void operator()( size_t_< Index > ) {
            SerialType< FieldType< Index > >::bin( std::get< Index >( value ), begin );
        }
    };

    /**
     *
     */
    template< typename Stream >
    struct DebugFunctor {
        ValueType& value;
        Stream& stream;
        std::string separator;
        uint8_t level;

        template< std::size_t Index >
        constexpr void operator()( size_t_< Index > ) {
            stream << separator.c_str();
            if ( is_primitive< FieldType< Index > >::value )
                stream << SerialMetatype< FieldType< Index > >::alias().data << ": ";
            SerialType< FieldType< Index > >::debug( std::get< Index >( value ), stream, level + 1 );
        }
    };
};

}} // --- namespace
