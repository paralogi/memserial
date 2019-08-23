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
struct SerialHelpers< bitset< Bits > > {
    using ValueType = bitset< Bits >;

    /**
     *
     */
    static uint64_t typeHash( uint32_t nesting ) {

        auto seed = SerialMetatype< ValueType >::ident();
        hashCombine( seed, uint64_t( Bits ) );
        return seed;
    }

    /**
     *
     */
    static std::size_t byteSize( const ValueType& value ) {

        constexpr auto bytes_count = value.size() / __CHAR_BIT__ +
                ( value.size() % __CHAR_BIT__ == 0 ? 0 : 1 );
        return bytes_count;
    }

    /**
     *
     */
    template< typename Iterator >
    static void toBytes( const ValueType& value, Iterator&& begin, Iterator&& end ) {

        assert( std::ptrdiff_t( byteSize( value ) ) <= std::distance( begin, end ) );

        for ( std::size_t index = 0; index < value.size(); ++index )
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

        for ( std::size_t index = 0; index < value.size(); ++index )
            value[ index ] = ( ( begin[ index / __CHAR_BIT__ ] >> ( index % __CHAR_BIT__ ) ) & 1 );

        begin += byteSize( value );
    }

    /**
     *
     */
    template< typename Stream >
    static void toDebug( const ValueType& value, Stream&& stream, uint8_t level ) {

        stream << SerialMetatype< ValueType >::alias().data <<
                "< " << value.size() << " >: " <<
                '"' << value.to_string().c_str() << '"';
    }
};

}} // --- namespace
