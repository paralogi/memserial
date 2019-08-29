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
struct SerialHelpers< time_point< Args... >, std::true_type > {
    using ValueType = time_point< Args... >;
    using ClockType = typename ValueType::clock;
    using DataType = typename ValueType::duration;

    /**
     *
     */
    static uint64_t typeHash( uint32_t nesting ) {

        auto seed = SerialMetatype < ValueType > ::ident();
        hashCombine( seed, SerialMetatype< ClockType >::ident() );
        hashCombine( seed, SerialHelpers< DataType >::typeHash( nesting ) );
        return seed;
    }

    /**
     *
     */
    static std::size_t byteSize( const ValueType& value ) {

        return SerialHelpers< DataType >::byteSize( value.time_since_epoch() );
    }

    /**
     *
     */
    template< typename Iterator >
    static void toBytes( const ValueType& value, Iterator&& begin, Iterator&& end ) {

        SerialHelpers< DataType >::toBytes( value.time_since_epoch(), begin, end );
    }

    /**
     *
     */
    template< typename Iterator >
    static void fromBytes( ValueType& value, Iterator&& begin, Iterator&& end ) {

        DataType data;
        SerialHelpers< DataType >::fromBytes( data, begin, end );
        value = ValueType( data );
    }

    /**
     *
     */
    template< typename Stream >
    static void toDebug( const ValueType& value, Stream&& stream, uint8_t level ) {

        stream << SerialMetatype < ValueType > ::alias().data <<
                "< " << SerialMetatype< ClockType >::alias().data << " >: ";

        SerialHelpers< DataType >::toDebug( value.time_since_epoch(), stream, level );
    }
};

}} // --- namespace
