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
struct SerialHelpers< array< Arg, Dim >, std::true_type > {
    using ValueType = array< Arg, Dim >;
    using DataType = typename ValueType::value_type;

    /**
     *
     */
    static uint64_t typeHash( uint32_t nesting ) {

        auto seed = SerialMetatype< ValueType >::ident();
        hashCombine( seed, uint64_t( Dim ) );
        hashCombine( seed, SerialHelpers< DataType >::typeHash( nesting ) );
        return seed;
    }

    /**
     *
     */
    static std::size_t byteSize( const ValueType& value ) {

        std::size_t byte_size = 0;

        for ( const auto& data : value )
            byte_size += SerialHelpers< DataType >::byteSize( data );

        return byte_size;
    }

    /**
     *
     */
    template< typename Iterator >
    static void toBytes( const ValueType& value, Iterator&& begin, Iterator&& end ) {

        for ( const auto& data : value )
            SerialHelpers< DataType >::toBytes( data, begin, end );
    }

    /**
     *
     */
    template< typename Iterator >
    static void fromBytes( ValueType& value, Iterator&& begin, Iterator&& end ) {

        for ( auto& data : value )
            SerialHelpers< DataType >::fromBytes( data, begin, end );
    }

    /**
     *
     */
    template< typename Stream >
    static void toDebug( const ValueType& value, Stream&& stream, uint8_t level ) {

        stream << SerialMetatype< ValueType >::alias().data <<
                "< " << SerialMetatype< DataType >::alias().data <<
                ", " << value.size() << " >: ";

        if ( value.size() == 0 ) {
            stream << "empty";
            return;
        }

        if ( is_primitive< DataType >::value ) {
            std::string separator( ", " );

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
