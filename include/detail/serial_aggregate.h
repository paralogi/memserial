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
struct SerialHelpers< aggregate_t< T > > {
    using ValueType = aggregate_t< T >;

    /**
     *
     */
    static uint64_t typeHash( uint32_t nesting ) {

        auto seed = SerialMetatype< ValueType >::ident();
        if ( nesting-- <= 0 )
            return seed;

        boost::pfr::for_each_field( ValueType{},
                [ &seed, nesting ]( auto&& field ) {
                    using FieldTypeCv = typename std::remove_reference< decltype( field ) >::type;
                    using FieldType = typename std::remove_cv< FieldTypeCv >::type;
                    hashCombine( seed, SerialHelpers< FieldType >::typeHash( nesting ) );
                } );

        return seed;
    }

    /**
     *
     */
    static std::size_t byteSize( const ValueType& value ) {

        std::size_t byte_size = 0;

        boost::pfr::for_each_field( const_cast< ValueType& >( value ),
                [ &byte_size ]( auto&& field ) {
                    using FieldTypeCv = typename std::remove_reference< decltype( field ) >::type;
                    using FieldType = typename std::remove_cv< FieldTypeCv >::type;
                    byte_size += SerialHelpers< FieldType >::byteSize( field );
                } );

        return byte_size;
    }

    /**
     *
     */
    template< typename Iterator >
    static void toBytes( const ValueType& value, Iterator&& begin, Iterator&& end ) {

        boost::pfr::for_each_field( const_cast< ValueType& >( value ),
                [ &begin, &end ]( auto&& field ) {
                    using FieldTypeCv = typename std::remove_reference< decltype( field ) >::type;
                    using FieldType = typename std::remove_cv< FieldTypeCv >::type;
                    SerialHelpers< FieldType >::toBytes( field, begin, end );
                } );
    }

    /**
     *
     */
    template< typename Iterator >
    static void fromBytes( ValueType& value, Iterator&& begin, Iterator&& end ) {

        boost::pfr::for_each_field( value,
                [ &begin, &end ]( auto&& field ) {
                    using FieldTypeCv = typename std::remove_reference< decltype( field ) >::type;
                    using FieldType = typename std::remove_cv< FieldTypeCv >::type;
                    SerialHelpers< FieldType >::fromBytes( field, begin, end );
                } );
    }

    /**
     *
     */
    template< typename Stream >
    static void toDebug( const ValueType& value, Stream&& stream, uint8_t level ) {

        stream << SerialMetatype< ValueType >::alias().data << ": ";

        if ( boost::pfr::tuple_size_v< ValueType > == 0 ) {
            stream << "empty";
            return;
        }

        std::string separator( 3 * ( level + 1 ) + 1, ' ' );
        separator[ 0 ] = '\n';

        boost::pfr::for_each_field( const_cast< ValueType& >( value ),
                [ &stream, &separator, level ]( auto&& field, std::size_t index ) {
                    using FieldTypeCv = typename std::remove_reference< decltype( field ) >::type;
                    using FieldType = typename std::remove_cv< FieldTypeCv >::type;
                    stream << separator.c_str();
                    if ( is_primitive< FieldType >::value )
                        stream << SerialMetatype< FieldType >::alias().data << ": ";
                    SerialHelpers< FieldType >::toDebug( field, stream, level + 1 );
                } );
    }
};

}} // --- namespace
