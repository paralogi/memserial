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

#ifdef QT_CORE_LIB
#include <QDebug>
#endif

#include "boost/pfr/precise/core.hpp"

/**
 * Maximum nesting level for serializable types, default is 16.
 * Prevents looping in case of recursive structures.
 *
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
 * Macro for reading compile time counter values.
 *
 * Макросы для чтения значения счетчика времени компиляции.
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
COUNTER_READ_BASE( Tag, 2048, \
COUNTER_READ_BASE( Tag, 4096, \
COUNTER_READ_BASE( Tag, 8192, \
COUNTER_READ_BASE( Tag, 16384, \
COUNTER_READ_BASE( Tag, 32768, \
COUNTER_READ_BASE( Tag, 65536, \
                0 )))))))))))))))))

#define COUNTER_INC( Tag ) \
constexpr size_t_< COUNTER_READ( Tag ) + 1 > \
counterReminder( Tag&&, \
        size_t_< ( COUNTER_READ( Tag ) + 1 ) & ~COUNTER_READ( Tag ) >, \
        size_t_< ( COUNTER_READ( Tag ) + 1 ) & COUNTER_READ( Tag ) > ) \
        { return {}; }

/**
 * Macro declares information about serializable type
 *
 * Макрос объявляет информацию о сериализуемом типе.
 */
#define ENABLE_SERIAL_TYPE_SIMPLE( Type, Internal ) \
namespace memserial { \
namespace detail { \
COUNTER_INC( SerialCounter ) \
template<> \
struct SerialIdentity< COUNTER_READ( SerialCounter ) > { \
    using SerialTag = std::true_type; \
    using InternalTag = std::integral_constant< bool, Internal >; \
    static Type value() { return {}; } \
}; \
template<> \
struct SerialMetatype< Type > { \
    static constexpr uint64_t ident() { return COUNTER_READ( SerialCounter ); } \
    static constexpr string_view alias() { return #Type; } \
}; \
}}

#define ENABLE_SERIAL_TYPE_TEMPLATE( Type, Internal ) \
namespace memserial { \
namespace detail { \
COUNTER_INC( SerialCounter ) \
template<> \
struct SerialIdentity< COUNTER_READ( SerialCounter ) > { \
    using SerialTag = std::true_type; \
    using InternalTag = std::integral_constant< bool, Internal >; \
    template< typename... Args > \
    static Type< Args... > value() { return {}; } \
}; \
template< typename... Args > \
struct SerialMetatype< Type< Args... > > { \
    static constexpr uint64_t ident() { return COUNTER_READ( SerialCounter ); } \
    static constexpr string_view alias() { return #Type; } \
}; \
}}

#define ENABLE_SERIAL_TYPE_TEMPLATE_ARG1( Type, Internal, ArgType0 ) \
namespace memserial { \
namespace detail { \
COUNTER_INC( SerialCounter ) \
template<> \
struct SerialIdentity< COUNTER_READ( SerialCounter ) > { \
    using SerialTag = std::true_type; \
    using InternalTag = std::integral_constant< bool, Internal >; \
    template< ArgType0 Arg > \
    static Type< Arg > value() { return {}; } \
}; \
template< ArgType0 Arg > \
struct SerialMetatype< Type< Arg > > { \
    static constexpr uint64_t ident() { return COUNTER_READ( SerialCounter ); } \
    static constexpr string_view alias() { return #Type; } \
}; \
}}

#define ENABLE_SERIAL_TYPE_TEMPLATE_ARG2( Type, Internal, ArgType0, ArgType1 ) \
namespace memserial { \
namespace detail { \
COUNTER_INC( SerialCounter ) \
template<> \
struct SerialIdentity< COUNTER_READ( SerialCounter ) > { \
    using SerialTag = std::true_type; \
    using InternalTag = std::integral_constant< bool, Internal >; \
    template< ArgType0 Arg0, ArgType1 Arg1 > \
    static Type< Arg0, Arg1 > value() { return {}; } \
}; \
template< ArgType0 Arg0, ArgType1 Arg1 > \
struct SerialMetatype< Type< Arg0, Arg1 > > { \
    static constexpr uint64_t ident() { return COUNTER_READ( SerialCounter ); } \
    static constexpr string_view alias() { return #Type; } \
}; \
}}

/**
 * Macro enables support for serialization methods.
 *
 * Макрос включает поддержку методов сериализации.
 */
#ifdef QT_CORE_LIB
#define ENABLE_SERIAL_TYPE_INFO( Type ) \
ENABLE_SERIAL_TYPE_SIMPLE( Type, false ) \
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
}
#else
#define ENABLE_SERIAL_TYPE_INFO( Type ) \
ENABLE_SERIAL_TYPE_SIMPLE( Type, false ) \
namespace memserial { \
template std::string serialize< Type >( const Type& ); \
template Type parse< Type >( const std::string& ); \
template Type parse< Type >( const char*, std::size_t ); \
template uint64_t ident< Type >(); \
template std::string alias< Type >(); \
template void print< std::ostream&, Type >( std::ostream&, const Type& ); \
template void print< std::ostream, Type >( std::ostream&&, const Type& ); \
}
#endif

ENABLE_SERIAL_TYPE_SIMPLE( bool, true )
ENABLE_SERIAL_TYPE_SIMPLE( unsigned char, true )
ENABLE_SERIAL_TYPE_SIMPLE( unsigned short, true )
ENABLE_SERIAL_TYPE_SIMPLE( unsigned int, true )
ENABLE_SERIAL_TYPE_SIMPLE( unsigned long, true )
ENABLE_SERIAL_TYPE_SIMPLE( unsigned long long, true )
ENABLE_SERIAL_TYPE_SIMPLE( signed char, true )
ENABLE_SERIAL_TYPE_SIMPLE( short, true )
ENABLE_SERIAL_TYPE_SIMPLE( int, true )
ENABLE_SERIAL_TYPE_SIMPLE( long, true )
ENABLE_SERIAL_TYPE_SIMPLE( long long, true )
ENABLE_SERIAL_TYPE_SIMPLE( char, true )
ENABLE_SERIAL_TYPE_SIMPLE( wchar_t, true )
ENABLE_SERIAL_TYPE_SIMPLE( char16_t, true )
ENABLE_SERIAL_TYPE_SIMPLE( char32_t, true )
ENABLE_SERIAL_TYPE_SIMPLE( float, true )
ENABLE_SERIAL_TYPE_SIMPLE( double, true )
ENABLE_SERIAL_TYPE_SIMPLE( long double, true )

ENABLE_SERIAL_TYPE_SIMPLE( string, true )
ENABLE_SERIAL_TYPE_SIMPLE( wstring, true )
ENABLE_SERIAL_TYPE_SIMPLE( u16string, true )
ENABLE_SERIAL_TYPE_SIMPLE( u32string, true )
ENABLE_SERIAL_TYPE_TEMPLATE( basic_string, true )

ENABLE_SERIAL_TYPE_TEMPLATE( vector, true )
ENABLE_SERIAL_TYPE_TEMPLATE_ARG2( array, true, typename, std::size_t )
ENABLE_SERIAL_TYPE_TEMPLATE_ARG1( bitset, true, std::size_t )

ENABLE_SERIAL_TYPE_SIMPLE( system_clock, true )
ENABLE_SERIAL_TYPE_SIMPLE( steady_clock, true )
ENABLE_SERIAL_TYPE_SIMPLE( nanoseconds, true )
ENABLE_SERIAL_TYPE_SIMPLE( microseconds, true )
ENABLE_SERIAL_TYPE_SIMPLE( milliseconds, true )
ENABLE_SERIAL_TYPE_SIMPLE( seconds, true )
ENABLE_SERIAL_TYPE_SIMPLE( minutes, true )
ENABLE_SERIAL_TYPE_SIMPLE( hours, true )
ENABLE_SERIAL_TYPE_TEMPLATE( time_point, true )
ENABLE_SERIAL_TYPE_TEMPLATE( duration, true )
