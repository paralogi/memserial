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
 * Специализация строковых типов.
 */
template< typename ... Args >
struct SerialHelpers< basic_string< Args... > > {
    using ValueType = basic_string< Args... >;
    using SizeType = uint32_t;
    using DataType = typename ValueType::value_type;

    /**
     *
     */
    static uint64_t typeHash( uint32_t nesting ) {

        auto seed = SerialMetatype< ValueType >::ident();
        hashCombine( seed, hash< SizeType >() );
        hashCombine( seed, hash< DataType >() );
        return seed;
    }

    /**
     *
     */
    static std::size_t byteSize( const ValueType& value ) {

        assert( value.size() < std::size_t( std::numeric_limits< SizeType >::max() ) );

        std::size_t byte_size = sizeof( SizeType );
        byte_size += sizeof( DataType ) * value.size();
        return byte_size;
    }

    /**
     *
     */
    template< typename Iterator >
    static void toBytes( const ValueType& value, Iterator&& begin, Iterator&& end ) {

        assert( std::ptrdiff_t( byteSize( value ) ) <= std::distance( begin, end ) );

        auto size_size = sizeof( SizeType );
        auto char_size = SizeType( value.size() );
        auto raw_size = reinterpret_cast< const char* >( &char_size );
        begin = std::copy_n( raw_size, size_size, begin );

        auto data_size = sizeof( DataType ) * char_size;
        auto raw_data = reinterpret_cast< const char* >( &value[ 0 ] );
        begin = std::copy_n( raw_data, data_size, begin );
    }

    /**
     *
     */
    template< typename Iterator >
    static void fromBytes( ValueType& value, Iterator&& begin, Iterator&& end ) {

        auto size_size = sizeof( SizeType );
        if ( std::ptrdiff_t( size_size ) > std::distance( begin, end ) )
            throw SerialException( SerialException::ExcOutOfRange );

        SizeType char_size;
        auto raw_size = reinterpret_cast< char* >( &char_size );
        std::copy_n( begin, size_size, raw_size );
        begin += size_size;

        auto data_size = sizeof( DataType ) * char_size;
        if ( std::ptrdiff_t( data_size ) > std::distance( begin, end ) )
            throw SerialException( SerialException::ExcOutOfRange );

        if ( data_size > 0 ) {
            value.resize( char_size );
            auto raw_data = reinterpret_cast< char* >( &value[ 0 ] );
            std::copy_n( begin, data_size, raw_data );
            begin += data_size;
        }
    }

    /**
     *
     */
    template< typename Stream >
    static void toDebug( const ValueType& value, Stream&& stream, uint8_t level ) {

        stream << SerialMetatype< ValueType >::alias().data <<
                "[" << value.size() << "]: " <<
                '"' << value.c_str() << '"';
    }
};

}} // --- namespace
