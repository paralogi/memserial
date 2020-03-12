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
struct SerialHelpers< bitset< Bits >, std::true_type > {
    using ValueType = bitset< Bits >;

    /**
     *
     */
    static constexpr bool matchHash( uint32_t hash ) {

        return typeHash() == hash;
    }

    /**
     *
     */
    static constexpr uint32_t typeHash() {

        uint32_t type_hash = SERIAL_HASH_MAX;
        typeHash( type_hash );
        return type_hash;
    }

    static constexpr void typeHash( uint32_t& hash, std::size_t nesting = SERIAL_NESTING_MAX ) {

        hashCombine( hash, aggregate_traits< ValueType >::InternalIdent );
        hashCombine( hash, uint32_t( Bits ) );
    }

    /**
     *
     */
    static std::size_t byteSize( const ValueType& value ) {

        constexpr auto bytes_count = Bits / __CHAR_BIT__ +
                ( Bits % __CHAR_BIT__ == 0 ? 0 : 1 );
        return bytes_count;
    }

    /**
     *
     */
    template< typename Iterator >
    static void toBytes( const ValueType& value, Iterator&& begin, Iterator&& end ) {

        assert( std::ptrdiff_t( byteSize( value ) ) <= std::distance( begin, end ) );

        for ( std::size_t index = 0; index < Bits; ++index )
            begin[ index / __CHAR_BIT__ ] |= ( value[ index ] << ( index % __CHAR_BIT__ ) );

        begin += byteSize( value );
    }

    /**
     *
     */
    template< typename Iterator >
    static void fromBytes( ValueType& value, Iterator&& begin, Iterator&& end ) {

        if ( std::ptrdiff_t( byteSize( value ) ) > std::distance( begin, end ) )
            throw SerialException( SerialException::ExcOutOfRange );

        for ( std::size_t index = 0; index < Bits; ++index )
            value[ index ] = ( ( begin[ index / __CHAR_BIT__ ] >> ( index % __CHAR_BIT__ ) ) & 1 );

        begin += byteSize( value );
    }

    /**
     *
     */
    template< typename Stream >
    static void toDebug( const ValueType& value, Stream&& stream, uint8_t level ) {

        stream << SerialMetatype< ValueType >::alias().data <<
                "< " << Bits << " >: " <<
                '"' << value.to_string().c_str() << '"';
    }
};

}} // --- namespace
