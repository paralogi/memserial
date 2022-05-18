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
template< typename Clock, typename Duration >
struct SerialType< time_point< Clock, Duration >, std::true_type > {
    using ValueType = time_point< Clock, Duration >;
    using ClockType = typename ValueType::clock;
    using DataType = typename ValueType::duration;

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
        hash_combine( value, serial_traits< ClockType >::internal_ident );
        SerialType< DataType >::hash( value, nesting );
    }

    /**
     *
     */
    static constexpr std::size_t size() {

        return SerialType< DataType >::size();
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

        SerialType< DataType >::bout( value.time_since_epoch(), begin );
    }

    /**
     *
     */
    template< typename Iterator >
    static void bin( ValueType& value, Iterator& begin ) {

        DataType data;
        SerialType< DataType >::bin( data, begin );
        value = ValueType( data );
    }

    /**
     *
     */
    template< typename Stream >
    static void debug( const ValueType& value, Stream& stream, uint8_t level ) {

        stream << SerialMetatype < ValueType > ::alias().data <<
                "< " << SerialMetatype< ClockType >::alias().data << " >: ";

        SerialType< DataType >::debug( value.time_since_epoch(), stream, level );
    }
};

}} // --- namespace
