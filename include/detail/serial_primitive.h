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
template< typename T >
struct SerialType< T, is_primitive< T > > {
    using ValueType = T;

    /**
     *
     */
    static constexpr bool match( uint32_t value ) {

        return hash() == value;
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

        hash_combine( value, rebind_primitive< ValueType >::internal_ident );
        hash_combine( value, uint32_t( sizeof( ValueType ) ) );
    }

    /**
     *
     */
    static std::size_t size( const ValueType& ) {

        return sizeof( ValueType );
    }

    /**
     *
     */
    template< typename Iterator >
    static void bout( const ValueType& value, Iterator& begin, Iterator& end ) {

        assert( std::ptrdiff_t( size( value ) ) <= std::distance( begin, end ) );

        begin.bout( value );
    }

    /**
     *
     */
    template< typename Iterator >
    static void bin( ValueType& value, Iterator& begin, Iterator& end ) {

        if ( std::ptrdiff_t( size( value ) ) > std::distance( begin, end ) )
            throw SerialException( SerialException::ExcOutOfRange );

        begin.bin( value );
    }

    /**
     *
     */
    template< typename Stream >
    static void debug( const ValueType& value, Stream& stream, uint8_t level ) {

        stream << +value;
    }
};

}} // --- namespace
