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
struct SerialHelpers< nulltype, std::true_type > {
    using ValueType = nulltype;

    /**
     *
     */
    static constexpr bool matchHash( uint64_t hash ) {

        return SERIAL_HASH_MAX == hash;
    }

    /**
     *
     */
    static constexpr uint64_t typeHash() {

        return SERIAL_HASH_MAX;
    }

    static constexpr void typeHash( uint64_t& hash, std::size_t nesting = SERIAL_NESTING_MAX ) {

    }

    /**
     *
     */
    static std::size_t byteSize( const ValueType& ) {

        return 0;
    }

    /**
     *
     */
    template< typename Iterator >
    static void toBytes( const ValueType& value, Iterator&& begin, Iterator&& end ) {

    }

    /**
     *
     */
    template< typename Iterator >
    static void fromBytes( ValueType& value, Iterator&& begin, Iterator&& end ) {

    }

    /**
     *
     */
    template< typename Stream >
    static void toDebug( const ValueType& value, Stream&& stream, uint8_t level ) {

        stream << SerialMetatype< ValueType >::alias().data;
    }
};

}} // --- namespace
