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
template< typename Arg, std::size_t Dim >
struct SerialType< array< Arg, Dim >, is_primitive< typename array< Arg, Dim >::value_type > > {
    using ValueType = array< Arg, Dim >;
    using SizeType = uint32_t;
    using DataType = typename ValueType::value_type;

    static_assert( Dim <= std::size_t( std::numeric_limits< SizeType >::max() ) );

    /**
     *
     */
    static constexpr bool match( uint32_t value ) {

        uint32_t real_hash = SERIAL_HASH_SALT;

        for ( std::size_t index = 0; index < Dim && real_hash != value; ++index )
            SerialType< DataType >::hash( real_hash );

        return real_hash == value;
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

        for ( std::size_t index = 0; index < Dim; ++index )
            SerialType< DataType >::hash( value, nesting );
    }

    /**
     *
     */
    static constexpr std::size_t size() {

        return sizeof( DataType ) * Dim;
    }

    /**
     *
     */
    static std::size_t size( const ValueType& value ) {

        return size();
    }

    /**
     *
     */
    template< typename Iterator >
    static void init( ValueType& value, Iterator& begin, Iterator& end ) {

        begin += size();
    }

    /**
     *
     */
    template< typename Iterator >
    static void bout( const ValueType& value, Iterator& begin ) {

        begin.bout( &value[ 0 ], Dim );
    }

    /**
     *
     */
    template< typename Iterator >
    static void bin( ValueType& value, Iterator& begin ) {

        begin.bin( &value[ 0 ], Dim );
    }

    /**
     *
     */
    template< typename Stream >
    static void debug( const ValueType& value, Stream& stream, uint8_t level ) {

        stream << SerialMetatype< ValueType >::alias().data <<
                "< " << SerialMetatype< DataType >::alias().data <<
                ", " << Dim << " >: ";

        if ( Dim == 0 ) {
            stream << "empty";
            return;
        }

        std::string separator( ", " );

        for ( std::size_t index = 0; index < Dim; ++index ) {
            if ( index > 0 )
                stream << separator.c_str();
            SerialType< DataType >::debug( value[ index ], stream, level + 1 );
        }
    }
};

/**
 *
 */
template< typename Arg, std::size_t Dim >
struct SerialType< array< Arg, Dim >, is_class< typename array< Arg, Dim >::value_type > > {
    using ValueType = array< Arg, Dim >;
    using SizeType = uint32_t;
    using DataType = typename ValueType::value_type;

    static_assert( Dim <= std::size_t( std::numeric_limits< SizeType >::max() ) );

    /**
     *
     */
    static constexpr bool match( uint32_t value ) {

        uint32_t real_hash = SERIAL_HASH_SALT;

        for ( std::size_t index = 0; index < Dim && real_hash != value; ++index )
            SerialType< DataType >::hash( real_hash );

        return real_hash == value;
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

        for ( std::size_t index = 0; index < Dim; ++index )
            SerialType< DataType >::hash( value, nesting );
    }

    /**
     *
     */
    static constexpr std::size_t size() {

        return SerialType< DataType >::size() * Dim;
    }

    /**
     *
     */
    static std::size_t size( const ValueType& value ) {

        std::size_t byte_size = 0;

        for ( const auto& data : value )
            byte_size += SerialType< DataType >::size( data );

        return byte_size;
    }

    /**
     *
     */
    template< typename Iterator >
    static void init( ValueType& value, Iterator& begin, Iterator& end ) {

        Iterator least_end = end - size();

        for ( auto& data : value ) {
            least_end += SerialType< DataType >::size();
            SerialType< DataType >::init( data, begin, least_end );
        }
    }

    /**
     *
     */
    template< typename Iterator >
    static void bout( const ValueType& value, Iterator& begin ) {

        for ( const auto& data : value )
            SerialType< DataType >::bout( data, begin );
    }

    /**
     *
     */
    template< typename Iterator >
    static void bin( ValueType& value, Iterator& begin ) {

        for ( auto& data : value )
            SerialType< DataType >::bin( data, begin );
    }

    /**
     *
     */
    template< typename Stream >
    static void debug( const ValueType& value, Stream& stream, uint8_t level ) {

        stream << SerialMetatype< ValueType >::alias().data <<
                "< " << SerialMetatype< DataType >::alias().data <<
                ", " << Dim << " >: ";

        if ( Dim == 0 ) {
            stream << "empty";
            return;
        }

        std::string separator( 3 * ( level + 1 ) + 1, ' ' );
        separator[ 0 ] = '\n';

        for ( std::size_t index = 0; index < Dim; ++index ) {
            stream << separator.c_str() << index << ": ";
            SerialType< DataType >::debug( value[ index ], stream, level + 1 );
        }
    }
};

}} // --- namespace
