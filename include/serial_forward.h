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

#include <string>
#include <vector>
#include <array>
#include <bitset>
#include <chrono>
#include <type_traits>
#include <algorithm>
#include <iostream>
#include <cassert>

#ifdef USE_QT
#include <QDebug>
#endif

#include "boost/pfr/precise/core.hpp"

/**
 * Максимальное значение вложенности для сериализуемых типов, по умолчанию 16.
 * Предотвращает зацикливание в случае рекурсивных структур.
 */
#define SERIAL_NESTING_MAX 16

#include "serialization.h"
#include "serial_exception.h"
#include "detail/serial_traits.h"
#include "detail/serial_metatype.h"
#include "detail/serial_helpers.h"
#include "detail/serial_nulltype.h"
#include "detail/serial_primitive.h"
#include "detail/serial_aggregate.h"
#include "detail/serial_string.h"
#include "detail/serial_vector.h"
#include "detail/serial_array.h"
#include "detail/serial_bitset.h"
#include "detail/serial_timepoint.h"
#include "detail/serial_duration.h"
#include "detail/serial_default.h"

/**
 * Макросы для чтение значения счетчика времени компиляции.
 */
#define COUNTER_READ_BASE( Tag, Base, Tail ) \
counterReminder( Tag{}, size_t_< Base >(), size_t_< Tail >() )

#define COUNTER_READ( Tag ) \
COUNTER_READ_BASE( Tag, 1, \
COUNTER_READ_BASE( Tag, 2, \
COUNTER_READ_BASE( Tag, 4, \
COUNTER_READ_BASE( Tag, 8, \
COUNTER_READ_BASE( Tag, 16, \
COUNTER_READ_BASE( Tag, 32, \
COUNTER_READ_BASE( Tag, 64, \
COUNTER_READ_BASE( Tag, 128, \
COUNTER_READ_BASE( Tag, 256, \
COUNTER_READ_BASE( Tag, 512, \
COUNTER_READ_BASE( Tag, 1024, \
        0 ) ) ) ) ) ) ) ) ) ) )

#define COUNTER_INC( Tag ) \
constexpr size_t_< COUNTER_READ( Tag ) + 1 > \
counterReminder( Tag&&, \
        size_t_< ( COUNTER_READ( Tag ) + 1 ) & ~COUNTER_READ( Tag ) >, \
        size_t_< ( COUNTER_READ( Tag ) + 1 ) & COUNTER_READ( Tag ) > ) \
        { return {}; }

/**
 * Макрос объявляет информацию о типе для сериализации.
 */
#define ENABLE_SERIAL_TYPE_SIMPLE( Type ) \
namespace memserial { \
namespace detail { \
COUNTER_INC( SerialCounter ) \
template<> \
struct SerialIdentity< COUNTER_READ( SerialCounter ) > { \
    static Type value() { return {}; } \
    static constexpr string_view alias() { return #Type; } \
}; \
template<> \
struct SerialMetatype< Type > { \
    static constexpr uint64_t ident() { return COUNTER_READ( SerialCounter ); } \
    static constexpr string_view alias() { return #Type; } \
}; \
}}

#define ENABLE_SERIAL_TYPE_TEMPLATE( Type ) \
namespace memserial { \
namespace detail { \
COUNTER_INC( SerialCounter ) \
template<> \
struct SerialIdentity< COUNTER_READ( SerialCounter ) > { \
    template< typename... Args > \
    static Type< Args... > value() { return {}; } \
    static constexpr string_view alias() { return #Type; } \
}; \
template< typename... Args > \
struct SerialMetatype< Type< Args... > > { \
    static constexpr uint64_t ident() { return COUNTER_READ( SerialCounter ); } \
    static constexpr string_view alias() { return #Type; } \
}; \
}}

#define ENABLE_SERIAL_TYPE_TEMPLATE_ARG1( Type, ArgType0 ) \
namespace memserial { \
namespace detail { \
COUNTER_INC( SerialCounter ) \
template<> \
struct SerialIdentity< COUNTER_READ( SerialCounter ) > { \
    template< ArgType0 Arg > \
    static Type< Arg > value() { return {}; } \
    static constexpr string_view alias() { return #Type; } \
}; \
template< ArgType0 Arg > \
struct SerialMetatype< Type< Arg > > { \
    static constexpr uint64_t ident() { return COUNTER_READ( SerialCounter ); } \
    static constexpr string_view alias() { return #Type; } \
}; \
}}

#define ENABLE_SERIAL_TYPE_TEMPLATE_ARG2( Type, ArgType0, ArgType1 ) \
namespace memserial { \
namespace detail { \
COUNTER_INC( SerialCounter ) \
template<> \
struct SerialIdentity< COUNTER_READ( SerialCounter ) > { \
    template< ArgType0 Arg0, ArgType1 Arg1 > \
    static Type< Arg0, Arg1 > value() { return {}; } \
    static constexpr string_view alias() { return #Type; } \
}; \
template< ArgType0 Arg0, ArgType1 Arg1 > \
struct SerialMetatype< Type< Arg0, Arg1 > > { \
    static constexpr uint64_t ident() { return COUNTER_READ( SerialCounter ); } \
    static constexpr string_view alias() { return #Type; } \
}; \
}}

/**
 * Макрос включает поддержку сериализации сообщений по идентификатору.
 */
#ifdef USE_QT
#define ENABLE_SERIAL_TYPE_INFO( Type ) \
namespace memserial { \
template std::string serialize< Type >( const Type& ); \
template Type parse< Type >( const std::string& ); \
template Type parse< Type >( const char*, std::size_t ); \
template uint64_t ident< Type >(); \
template std::string alias< Type >(); \
template void print< std::ostream&, Type >( std::ostream&, const Type& ); \
template void print< std::ostream, Type >( std::ostream&&, const Type& ); \
template void print< QDebug&, Type >( QDebug&, const Type& ); \
template void print< QDebug, Type >( QDebug&&, const Type& ); \
namespace detail { \
COUNTER_INC( SerialCounter ) \
template<> \
struct SerialIdentity< COUNTER_READ( SerialCounter ) > { \
    static Type value() { return {}; } \
    static constexpr string_view alias() { return #Type; } \
}; \
template<> \
struct SerialMetatype< Type > { \
    static constexpr uint64_t ident() { return COUNTER_READ( SerialCounter ); } \
    static constexpr string_view alias() { return #Type; } \
}; \
}}
#else
#define ENABLE_SERIAL_TYPE_INFO( Type ) \
namespace memserial { \
template std::string serialize< Type >( const Type& ); \
template Type parse< Type >( const std::string& ); \
template Type parse< Type >( const char*, std::size_t ); \
template uint64_t ident< Type >(); \
template std::string alias< Type >(); \
template void print< std::ostream&, Type >( std::ostream&, const Type& ); \
template void print< std::ostream, Type >( std::ostream&&, const Type& ); \
namespace detail { \
COUNTER_INC( SerialCounter ) \
template<> \
struct SerialIdentity< COUNTER_READ( SerialCounter ) > { \
    static Type value() { return {}; } \
    static constexpr string_view alias() { return #Type; } \
}; \
template<> \
struct SerialMetatype< Type > { \
    static constexpr uint64_t ident() { return COUNTER_READ( SerialCounter ); } \
    static constexpr string_view alias() { return #Type; } \
}; \
}}
#endif

ENABLE_SERIAL_TYPE_SIMPLE( bool )
ENABLE_SERIAL_TYPE_SIMPLE( unsigned char )
ENABLE_SERIAL_TYPE_SIMPLE( unsigned short )
ENABLE_SERIAL_TYPE_SIMPLE( unsigned int )
ENABLE_SERIAL_TYPE_SIMPLE( unsigned long )
ENABLE_SERIAL_TYPE_SIMPLE( unsigned long long )
ENABLE_SERIAL_TYPE_SIMPLE( signed char )
ENABLE_SERIAL_TYPE_SIMPLE( short )
ENABLE_SERIAL_TYPE_SIMPLE( int )
ENABLE_SERIAL_TYPE_SIMPLE( long )
ENABLE_SERIAL_TYPE_SIMPLE( long long )
ENABLE_SERIAL_TYPE_SIMPLE( char )
ENABLE_SERIAL_TYPE_SIMPLE( wchar_t )
ENABLE_SERIAL_TYPE_SIMPLE( char16_t )
ENABLE_SERIAL_TYPE_SIMPLE( char32_t )
ENABLE_SERIAL_TYPE_SIMPLE( float )
ENABLE_SERIAL_TYPE_SIMPLE( double )
ENABLE_SERIAL_TYPE_SIMPLE( long double )

ENABLE_SERIAL_TYPE_SIMPLE( string )
ENABLE_SERIAL_TYPE_SIMPLE( wstring )
ENABLE_SERIAL_TYPE_SIMPLE( u16string )
ENABLE_SERIAL_TYPE_SIMPLE( u32string )
ENABLE_SERIAL_TYPE_TEMPLATE( basic_string )

ENABLE_SERIAL_TYPE_TEMPLATE( vector )
ENABLE_SERIAL_TYPE_TEMPLATE_ARG2( array, typename, std::size_t )
ENABLE_SERIAL_TYPE_TEMPLATE_ARG1( bitset, std::size_t )

ENABLE_SERIAL_TYPE_SIMPLE( system_clock )
ENABLE_SERIAL_TYPE_SIMPLE( steady_clock )
ENABLE_SERIAL_TYPE_SIMPLE( nanoseconds )
ENABLE_SERIAL_TYPE_SIMPLE( microseconds )
ENABLE_SERIAL_TYPE_SIMPLE( milliseconds )
ENABLE_SERIAL_TYPE_SIMPLE( seconds )
ENABLE_SERIAL_TYPE_SIMPLE( minutes )
ENABLE_SERIAL_TYPE_SIMPLE( hours )
ENABLE_SERIAL_TYPE_TEMPLATE( time_point )
ENABLE_SERIAL_TYPE_TEMPLATE( duration )
