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

using atto = std::atto;
using femto = std::femto;
using pico = std::pico;
using nano = std::nano;
using micro = std::micro;
using milli = std::milli;
using centi = std::centi;
using deci = std::deci;
using deca = std::deca;
using hecto = std::hecto;
using kilo = std::kilo;
using mega = std::mega;
using giga = std::giga;
using tera = std::tera;
using peta = std::peta;
using exa = std::exa;

template< typename ... Args >
using basic_string = std::basic_string< Args... >;

template< typename ... Args >
using vector = std::vector< Args... >;

template< typename ... Args >
using tuple = std::tuple< Args... >;

template< typename Arg, std::size_t Dim >
using array = std::array< Arg, Dim >;

template< std::size_t Bits >
using bitset = std::bitset< Bits >;

template< typename Clock, typename Duration >
using time_point = std::chrono::time_point< Clock, Duration >;

template< typename Rep, typename Period >
using duration = std::chrono::duration< Rep, Period >;

template< std::intmax_t Num, std::intmax_t Den >
using ratio = std::ratio< Num, Den >;

template< typename Arg >
using complex = std::complex< Arg >;

template< std::size_t Index >
struct size_t_ : std::integral_constant< std::size_t, Index > {};

/**
 *
 */
template< typename T >
struct endian_traits : std::true_type {
    static constexpr SerialEndian internal_endian = DefaultEndian;
};

template< typename ByteArray, SerialEndian endian >
struct endian_traits< SerialStorage< ByteArray, endian > >: std::false_type {
    static constexpr SerialEndian internal_endian = endian;
};

template< typename ByteArray, SerialEndian endian >
struct endian_traits< SerialWrapper< ByteArray, endian > >: std::false_type {
    static constexpr SerialEndian internal_endian = endian;
};

/**
 *
 */
template< SerialEndian endian, std::size_t size, typename = std::true_type >
struct rebind_endian {
    static constexpr SerialEndian internal_endian = endian;
};

template< SerialEndian endian >
struct rebind_endian< endian, 1, std::true_type > {
    static constexpr SerialEndian internal_endian = NativeEndian;
};

template< std::size_t size >
struct rebind_endian< SERIAL_NATIVE_ENDIAN, size, std::true_type > {
    static constexpr SerialEndian internal_endian = NativeEndian;
};

template<>
struct rebind_endian< SERIAL_NATIVE_ENDIAN, 1, std::true_type > {
    static constexpr SerialEndian internal_endian = NativeEndian;
};

/**
 *
 */
template< typename T >
struct primitive_traits : std::true_type {
    static constexpr std::size_t internal_ident = 0;
};

template<>
struct primitive_traits< bool > : std::false_type {
    static constexpr std::size_t internal_ident = 1;
};

template<>
struct primitive_traits< char > : std::false_type {
    static constexpr std::size_t internal_ident = 2;
};

template<>
struct primitive_traits< wchar_t > : std::false_type {
    static constexpr std::size_t internal_ident = 3;
};

template<>
struct primitive_traits< char16_t > : std::false_type {
    static constexpr std::size_t internal_ident = 4;
};

template<>
struct primitive_traits< char32_t > : std::false_type {
    static constexpr std::size_t internal_ident = 5;
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
    static constexpr uint32_t internal_ident = primitive_traits< T >::internal_ident;
};

template< typename T >
struct rebind_primitive< T, is_integer_signed< T > > {
    using InternalType = T;
    static constexpr uint32_t internal_ident = 1 << 8;
};

template< typename T >
struct rebind_primitive< T, is_integer_unsigned< T > > {
    using InternalType = T;
    static constexpr uint32_t internal_ident = 2 << 8;
};

template< typename T >
struct rebind_primitive< T, typename std::is_floating_point< T >::type > {
    using InternalType = T;
    static constexpr uint32_t internal_ident = 3 << 8;
};

template< typename T >
struct rebind_primitive< T, typename std::is_enum< T >::type > {
    using InternalType = typename std::underlying_type< T >::type;
    static constexpr uint32_t internal_ident = rebind_primitive< InternalType >::internal_ident << 16;
};

/**
 *
 */
template< typename T >
struct serial_traits : std::true_type {
    static constexpr uint32_t internal_ident = 0;
};

template< typename Arg, std::size_t Dim >
struct serial_traits< array< Arg, Dim > > : std::false_type {
    static constexpr uint32_t internal_ident = 1;
};

template< std::size_t Bits >
struct serial_traits< bitset< Bits > > : std::false_type {
    static constexpr uint32_t internal_ident = 2;
};

template< typename ... Args >
struct serial_traits< basic_string< Args... > > : std::false_type {
    static constexpr uint32_t internal_ident = 3;
};

template< typename ... Args >
struct serial_traits< vector< Args... > > : std::false_type {
    static constexpr uint32_t internal_ident = 4;
};

template< typename Clock, typename Duration >
struct serial_traits< time_point< Clock, Duration > > : std::false_type {
    static constexpr uint32_t internal_ident = 5;
};

template< typename Rep, typename Period >
struct serial_traits< duration< Rep, Period > > : std::false_type {
    static constexpr uint32_t internal_ident = 6;
};

template<>
struct serial_traits< system_clock > : std::false_type {
    static constexpr uint32_t internal_ident = 7;
};

template<>
struct serial_traits< steady_clock > : std::false_type {
    static constexpr uint32_t internal_ident = 8;
};

template< typename Arg >
struct serial_traits< complex< Arg > > : std::false_type {
    static constexpr uint32_t internal_ident = 9;
};

template< typename ... Args >
struct serial_traits< tuple< Args... > > : std::false_type {
    static constexpr uint32_t internal_ident = 10;
};

template< std::intmax_t Num, std::intmax_t Den >
struct serial_traits< ratio< Num, Den > > : std::false_type {
    static constexpr uint32_t internal_ident = 11;
};

/**
 *
 */
template< typename T >
using is_class = std::integral_constant< bool,
        std::is_class< T >::value >;

template< typename T >
using is_serial = std::integral_constant< bool,
        serial_traits< T >::value &&
        is_class< T >::value >;

/**
 *
 */
template< typename T, typename I = std::true_type >
struct rebind_serial;

}} // --- namespace
