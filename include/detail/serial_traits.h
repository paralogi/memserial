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
struct is_supported : public std::false_type {};

template< typename ... Args >
struct is_supported< basic_string< Args... > > : public std::true_type {};

template< typename ... Args >
struct is_supported< vector< Args... > > : public std::true_type {};

template< typename Arg, std::size_t Dim >
struct is_supported< array< Arg, Dim > > : public std::true_type {};

template< std::size_t Bits >
struct is_supported< bitset< Bits > > : public std::true_type {};

template< typename ... Args >
struct is_supported< time_point< Args... > > : public std::true_type {};

template< typename ... Args >
struct is_supported< duration< Args... > > : public std::true_type {};

/**
 *
 */
template< typename T >
using is_primitive = std::integral_constant< bool,
        std::is_arithmetic< T >::value ||
        std::is_enum< T >::value >;

/**
 *
 */
template< typename T >
using is_aggregate = std::integral_constant< bool,
        std::is_class< T >::value &&
        !is_supported< T >::value >;

/**
 *
 */
template< typename T >
using is_serial = std::integral_constant< bool,
        is_supported< T >::value ||
        is_primitive< T >::value ||
        is_aggregate< T >::value >;

}} // --- namespace
