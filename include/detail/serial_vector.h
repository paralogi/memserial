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
struct SerialHelpers< vector< Args... >, std::true_type > {
    using ValueType = vector< Args... >;
    using SizeType = uint32_t;
    using DataType = typename ValueType::value_type;

    /**
     *
     */
    static uint64_t typeHash( uint32_t nesting ) {

        auto seed = SerialMetatype< ValueType >::ident();
        hashCombine( seed, hash< SizeType >() );
        hashCombine( seed, SerialHelpers< DataType >::typeHash( nesting ) );
        return seed;
    }

    /**
     *
     */
    static std::size_t byteSize( const ValueType& value ) {

        assert( value.size() < std::size_t( std::numeric_limits< SizeType >::max() ) );

        std::size_t byte_size = sizeof( SizeType );
        for ( const auto& data : value )
            byte_size += SerialHelpers< DataType >::byteSize( data );

        return byte_size;
    }

    /**
     *
     */
    template< typename Iterator >
    static void toBytes( const ValueType& value, Iterator&& begin, Iterator&& end ) {

        assert( std::ptrdiff_t( byteSize( value ) ) <= std::distance( begin, end ) );

        auto size_size = sizeof( SizeType );
        auto data_size = SizeType( value.size() );
        auto raw_size = reinterpret_cast< const char* >( &data_size );
        begin = std::copy_n( raw_size, size_size, begin );

        for ( const auto& data : value )
            SerialHelpers< DataType >::toBytes( data, begin, end );
    }

    /**
     *
     */
    template< typename Iterator >
    static void fromBytes( ValueType& value, Iterator&& begin, Iterator&& end ) {

        auto size_size = sizeof( SizeType );
        if ( std::ptrdiff_t( size_size ) > std::distance( begin, end ) )
            throw SerialException( SerialException::ExcOutOfRange );

        SizeType data_size;
        auto raw_size = reinterpret_cast< char* >( &data_size );
        std::copy_n( begin, size_size, raw_size );
        begin += size_size;

        if ( data_size > 0 ) {
            value.resize( data_size );
            for ( auto& data : value )
                SerialHelpers< DataType >::fromBytes( data, begin, end );
        }
    }

    /**
     *
     */
    template< typename Stream >
    static void toDebug( const ValueType& value, Stream&& stream, uint8_t level ) {

        stream << SerialMetatype< ValueType >::alias().data <<
                "< " << SerialMetatype< DataType >::alias().data <<
                " >[" << value.size() << "]: ";

        if ( value.empty() ) {
            stream << "empty";
            return;
        }

        if ( is_primitive< DataType >::value ) {
            std::string separator = std::string( ", " );

            for ( std::size_t index = 0; index < value.size(); ++index ) {
                if ( index > 0 )
                    stream << separator.c_str();
                SerialHelpers< DataType >::toDebug( value[ index ], stream, level + 1 );
            }
        }

        else {
            std::string separator( 3 * ( level + 1 ) + 1, ' ' );
            separator[ 0 ] = '\n';

            for ( std::size_t index = 0; index < value.size(); ++index ) {
                stream << separator.c_str() << index << ": ";
                SerialHelpers< DataType >::toDebug( value[ index ], stream, level + 1 );
            }
        }
    }
};

}} // --- namespace
