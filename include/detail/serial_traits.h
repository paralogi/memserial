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

using nulltype = std::nullptr_t;

using string = std::string;
using wstring = std::wstring;
using u16string = std::u16string;
using u32string = std::u32string;

using system_clock = std::chrono::system_clock;
using steady_clock = std::chrono::steady_clock;
using high_resolution_clock = std::chrono::high_resolution_clock;

using nanoseconds = std::chrono::nanoseconds;
using microseconds = std::chrono::microseconds;
using milliseconds = std::chrono::milliseconds;
using seconds = std::chrono::seconds;
using minutes = std::chrono::minutes;
using hours = std::chrono::hours;

template< typename ... Args >
using tuple = std::tuple< Args... >;

template< typename ... Args >
using basic_string = std::basic_string< Args... >;

template< typename ... Args >
using vector = std::vector< Args... >;

template< typename Arg, std::size_t Dim >
using array = std::array< Arg, Dim >;

template< std::size_t Bits >
using bitset = std::bitset< Bits >;

template< typename ... Args >
using time_point = std::chrono::time_point< Args... >;

template< typename ... Args >
using duration = std::chrono::duration< Args... >;

/**
 *
 */
template< std::size_t Index >
struct size_t_ : std::integral_constant< std::size_t, Index > {};

/**
 *
 */
template< typename T >
struct primitive_traits : public std::true_type {
    static constexpr std::size_t InternalIdent = 0;
};

template<>
struct primitive_traits< bool > : public std::false_type {
    static constexpr std::size_t InternalIdent = 1;
};

template<>
struct primitive_traits< char > : public std::false_type {
    static constexpr std::size_t InternalIdent = 2;
};

template<>
struct primitive_traits< wchar_t > : public std::false_type {
    static constexpr std::size_t InternalIdent = 3;
};

template<>
struct primitive_traits< char16_t > : public std::false_type {
    static constexpr std::size_t InternalIdent = 4;
};

template<>
struct primitive_traits< char32_t > : public std::false_type {
    static constexpr std::size_t InternalIdent = 5;
};

/**
 *
 */
template< typename T >
using is_primitive = std::integral_constant< bool,
        std::is_arithmetic< T >::value ||
        std::is_enum< T >::value >;

template< typename T >
using is_integer_signed = std::integral_constant< bool,
        primitive_traits< T >::value &&
        std::is_integral< T >::value &&
        std::is_signed< T >::value >;

template< typename T >
using is_integer_unsigned = std::integral_constant< bool,
        primitive_traits< T >::value &&
        std::is_integral< T >::value &&
        std::is_unsigned< T >::value >;

/**
 *
 */
template< typename T, typename = std::true_type >
struct rebind_primitive {
    using InternalType = T;
    static constexpr uint64_t InternalIdent = primitive_traits< T >::InternalIdent;
};

template< typename T >
struct rebind_primitive< T, is_integer_signed< T > > {
    using InternalType = T;
    static constexpr uint64_t InternalIdent = 1 << 8;
};

template< typename T >
struct rebind_primitive< T, is_integer_unsigned< T > > {
    using InternalType = T;
    static constexpr uint64_t InternalIdent = 2 << 8;
};

template< typename T >
struct rebind_primitive< T, typename std::is_floating_point< T >::type > {
    using InternalType = T;
    static constexpr uint64_t InternalIdent = 3 << 8;
};

template< typename T >
struct rebind_primitive< T, typename std::is_enum< T >::type > {
    using InternalType = typename std::underlying_type< T >::type;
    static constexpr uint64_t InternalIdent = rebind_primitive< InternalType >::InternalIdent << 16;
};

/**
 *
 */
template< typename T >
struct aggregate_traits : public std::true_type {
    static constexpr uint64_t InternalIdent = 0;
};

template< typename Arg, std::size_t Dim >
struct aggregate_traits< array< Arg, Dim > > : public std::false_type {
    static constexpr uint64_t InternalIdent = 1;
};

template< std::size_t Bits >
struct aggregate_traits< bitset< Bits > > : public std::false_type {
    static constexpr uint64_t InternalIdent = 2;
};

template< typename ... Args >
struct aggregate_traits< basic_string< Args... > > : public std::false_type {
    static constexpr uint64_t InternalIdent = 3;
};

template< typename ... Args >
struct aggregate_traits< vector< Args... > > : public std::false_type {
    static constexpr uint64_t InternalIdent = 4;
};

template< typename ... Args >
struct aggregate_traits< time_point< Args... > > : public std::false_type {
    static constexpr uint64_t InternalIdent = 5;
};

template< typename ... Args >
struct aggregate_traits< duration< Args... > > : public std::false_type {
    static constexpr uint64_t InternalIdent = 6;
};

template<>
struct aggregate_traits< system_clock > : public std::false_type {
    static constexpr uint64_t InternalIdent = 7;
};

template<>
struct aggregate_traits< steady_clock > : public std::false_type {
    static constexpr uint64_t InternalIdent = 8;
};

/**
 *
 */
template< typename T >
using is_aggregate = std::integral_constant< bool,
        aggregate_traits< T >::value &&
        std::is_class< T >::value >;

/**
 *
 */
template< typename T, typename = std::true_type >
struct rebind_aggregate {
    using ValueType = T;
    static constexpr std::size_t TupleNesting = 1;
    static constexpr std::size_t TupleSize = boost::pfr::tuple_size< ValueType >::value;

    template< std::size_t Index >
    struct TupleField {
        using FieldType = typename boost::pfr::tuple_element< Index, ValueType >::type;
        static constexpr FieldType& get( ValueType& value ) {
            return boost::pfr::get< Index >( value );
        }
    };
};

template< typename ... Args >
struct rebind_aggregate< tuple< Args... >, std::true_type > {
    using ValueType = tuple< Args... >;
    static constexpr std::size_t TupleNesting = 0;
    static constexpr std::size_t TupleSize = std::tuple_size< ValueType >::value;

    template< std::size_t Index >
    struct TupleField {
        using FieldType = typename std::tuple_element< Index, ValueType >::type;
        static constexpr FieldType& get( ValueType& value ) {
            return std::get< Index >( value );
        }
    };
};

/**
 *
 */
template< typename T >
using is_serial = std::integral_constant< bool,
        aggregate_traits< T >::value ||
        is_primitive< T >::value ||
        is_aggregate< T >::value >;

}} // --- namespace
