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
template<>
struct SerialType< nulltype, std::true_type > {
    using ValueType = nulltype;

    /**
     *
     */
    static constexpr bool match( uint32_t hash ) {

        return SERIAL_HASH_SALT == hash;
    }

    /**
     *
     */
    static constexpr uint32_t hash() {

        return SERIAL_HASH_SALT;
    }

    static constexpr void hash( uint32_t& hash, std::size_t nesting = SERIAL_NESTING_LIMIT ) {

    }

    /**
     *
     */
    static constexpr std::size_t size() {

        return 0;
    }

    /**
     *
     */
    static std::size_t size( const ValueType& value ) {

        return 0;
    }

    /**
     *
     */
    template< typename Iterator >
    static void init( ValueType& value, Iterator& begin, Iterator& end ) {

    }

    /**
     *
     */
    template< typename Iterator >
    static void bout( const ValueType& value, Iterator& begin ) {

    }

    /**
     *
     */
    template< typename Iterator >
    static void bin( ValueType& value, Iterator& begin ) {

    }

    /**
     *
     */
    template< typename Stream >
    static void debug( const ValueType& value, Stream& stream, uint8_t level ) {

    }
};

}} // --- namespace
