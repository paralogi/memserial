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
template< typename Arg >
struct SerialType< complex< Arg >, is_primitive< typename complex< Arg >::value_type > > {
    using ValueType = complex< Arg >;
    using DataType = typename ValueType::value_type;

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
        SerialType< DataType >::hash( value, nesting );
    }

    /**
     *
     */
    static std::size_t size( const ValueType& value ) {

        return sizeof( DataType ) * 2;
    }

    /**
     *
     */
    template< typename Iterator >
    static void bout( const ValueType& value, Iterator& begin, Iterator& end ) {

        assert( std::ptrdiff_t( size( value ) ) <= std::distance( begin, end ) );

        DataType real_part = value.real();
        begin.bout( real_part );

        DataType imag_part = value.imag();
        begin.bout( imag_part );
    }

    /**
     *
     */
    template< typename Iterator >
    static void bin( ValueType& value, Iterator& begin, Iterator& end ) {

        if ( std::ptrdiff_t( size( value ) ) > std::distance( begin, end ) )
            throw SerialException( SerialException::ExcOutOfRange );

        DataType real_part;
        begin.bin( real_part );

        DataType imag_part;
        begin.bin( imag_part );

        value.real( real_part );
        value.imag( imag_part );
    }

    /**
     *
     */
    template< typename Stream >
    static void debug( const ValueType& value, Stream& stream, uint8_t level ) {

        stream << SerialMetatype< ValueType >::alias().data <<
                "< " << SerialMetatype< DataType >::alias().data <<
                " >: " << value.real() << "+" << value.imag() << "i";
    }
};

}} // --- namespace
