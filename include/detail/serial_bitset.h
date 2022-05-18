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
template< std::size_t Bits >
struct SerialType< bitset< Bits >, std::true_type > {
    using ValueType = bitset< Bits >;

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

        hash_combine( value, serial_traits< ValueType >::internal_ident );
        hash_combine( value, uint32_t( Bits ) );
    }

    /**
     *
     */
    static constexpr std::size_t size() {

        constexpr auto bytes_count = Bits / CHAR_BIT + ( Bits % CHAR_BIT == 0 ? 0 : 1 );
        return bytes_count;
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

        for ( std::size_t index = 0; index < Bits; ++index )
            begin[ index / CHAR_BIT ] |= ( value[ index ] << ( index % CHAR_BIT ) );

        begin += size();
    }

    /**
     *
     */
    template< typename Iterator >
    static void bin( ValueType& value, Iterator& begin ) {

        for ( std::size_t index = 0; index < Bits; ++index )
            value[ index ] = ( ( begin[ index / CHAR_BIT ] >> ( index % CHAR_BIT ) ) & 1 );

        begin += size();
    }

    /**
     *
     */
    template< typename Stream >
    static void debug( const ValueType& value, Stream& stream, uint8_t level ) {

        stream << SerialMetatype< ValueType >::alias().data <<
                "< " << Bits << " >: " <<
                '"' << value.to_string().c_str() << '"';
    }
};

}} // --- namespace
