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

#include <tuple>
#include <string>
#include <vector>
#include <array>
#include <bitset>
#include <chrono>
#include <type_traits>
#include <algorithm>
#include <iostream>
#include <cassert>

#if defined( QT_CORE_LIB )
#include <QByteArray>
#include <QDebug>
#endif

#include "boost/pfr/precise/core.hpp"

/**
 * \internal
 * \~english
 * Maximum nesting level for serializable types, default is 16.
 * Prevents looping in case of recursive structures.
 * \~russian
 * Максимальное значение вложенности для сериализуемых типов, по умолчанию 16.
 * Предотвращает зацикливание в случае рекурсивных структур.
 */
#if !defined( SERIAL_NESTING_MAX )
#define SERIAL_NESTING_MAX 16
#endif

/**
 * \internal
 * \~english
 * The maximum value of the hash function.
 * Used as the initial value for the calculation.
 * \~russian
 * Максимальное значение хэш-функции.
 * Используется как начальное значение для расчета.
 */
#if !defined( SERIAL_HASH_MAX )
#define SERIAL_HASH_MAX 0xffffffff
#endif

#include "serial_exception.h"
#include "detail/serial_traits.h"
#include "detail/serial_metatype.h"
#include "detail/serial_nulltype.h"
#include "detail/serial_primitive.h"
#include "detail/serial_aggregate.h"
#include "detail/serial_string.h"
#include "detail/serial_vector.h"
#include "detail/serial_array.h"
#include "detail/serial_bitset.h"
#include "detail/serial_timepoint.h"
#include "detail/serial_duration.h"
#include "detail/serial_hash.h"

#if !defined( DISABLE_SERIALIZATION )
#include "serialization.h"
#include "detail/serial_default.h"
#endif

#define PACK( ... ) __VA_ARGS__
#define UNPACK( ... ) #__VA_ARGS__

/**
 * \internal
 * \~english
 * Macro declares compile time counter.
 * \~russian
 * Макросы объявляет счетчик времени компиляции.
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
                0 )))))))))))

#define COUNTER_INC( Tag ) \
constexpr size_t_< COUNTER_READ( Tag ) + 1 > \
counterReminder( Tag&&, \
        size_t_< ( COUNTER_READ( Tag ) + 1 ) & ~COUNTER_READ( Tag ) >, \
        size_t_< ( COUNTER_READ( Tag ) + 1 ) & COUNTER_READ( Tag ) > ) \
        { return {}; }

/**
 * \internal
 * \~english
 * Macro declares information about serializable type.
 * \~russian
 * Макрос объявляет информацию о сериализуемом типе.
 */
#define ENABLE_SERIAL_TYPE( Type ) \
namespace memserial { \
namespace detail { \
COUNTER_INC( SerialCounter ) \
template<> \
struct SerialIdentity< COUNTER_READ( SerialCounter ) > { \
    using SerialTag = std::true_type; \
    using ValueType = Type; \
}; \
template<> \
struct SerialMetatype< Type > { \
    static constexpr SerialAlias alias() { return UNPACK( Type ); } \
    static constexpr SerialHash hash() { return { alias().hash(), SerialHelpers< Type >::typeHash() }; } \
}; \
}}

#define ENABLE_SERIAL_TYPE_SIMPLE( Type ) \
namespace memserial { \
namespace detail { \
template<> \
struct SerialMetatype< Type > { \
    static constexpr SerialAlias alias() { return UNPACK( Type ); } \
}; \
}}

#define ENABLE_SERIAL_TYPE_TEMPLATE( Type ) \
namespace memserial { \
namespace detail { \
template< typename... Args > \
struct SerialMetatype< Type< Args... > > { \
    static constexpr SerialAlias alias() { return UNPACK( Type ); } \
}; \
}}

#define ENABLE_SERIAL_TYPE_TEMPLATE_ARG1( Type, ArgType0 ) \
namespace memserial { \
namespace detail { \
template< ArgType0 Arg > \
struct SerialMetatype< Type< Arg > > { \
    static constexpr SerialAlias alias() { return UNPACK( Type ); } \
}; \
}}

#define ENABLE_SERIAL_TYPE_TEMPLATE_ARG2( Type, ArgType0, ArgType1 ) \
namespace memserial { \
namespace detail { \
template< ArgType0 Arg0, ArgType1 Arg1 > \
struct SerialMetatype< Type< Arg0, Arg1 > > { \
    static constexpr SerialAlias alias() { return UNPACK( Type ); } \
}; \
}}

#if defined( DISABLE_SERIALIZATION )

/**
 * \internal
 * \~english
 * Macro declares information about serializable types.
 * \~russian
 * Макрос объявляет информацию о сериализуемых типах.
 */
#define ENABLE_SERIAL_TYPE_INFO( Type ) \
ENABLE_SERIAL_TYPE( PACK( Type ) )

#elif defined( QT_CORE_LIB )

/**
 * \internal
 * \~english
 * Macro declares information about serializable types and serialization methods using Qt.
 * \~russian
 * Макрос объявляет информацию о сериализуемых типах и методах сериализации, использующих Qt.
 */
#define ENABLE_SERIAL_TYPE_INFO( Type ) \
ENABLE_SERIAL_TYPE( PACK( Type ) ) \
namespace memserial { \
template std::string serialize< std::string, Type >( const Type& ); \
template QByteArray serialize< QByteArray, Type >( const Type& ); \
template Type parse< Type, std::string >( const std::string& ); \
template Type parse< Type, QByteArray >( const QByteArray& ); \
template uint64_t ident< Type >(); \
template std::string alias< Type, std::string >(); \
template QByteArray alias< Type, QByteArray >(); \
template void print< std::ostream&, Type >( std::ostream&, const Type& ); \
template void print< std::ostream, Type >( std::ostream&&, const Type& ); \
template void print< QDebug&, Type >( QDebug&, const Type& ); \
template void print< QDebug, Type >( QDebug&&, const Type& ); \
}

#else

/**
 * \internal
 * \~english
 * Macro declares information about serializable types and serialization methods.
 * \~russian
 * Макрос объявляет информацию о сериализуемых типах и методах сериализации.
 */
#define ENABLE_SERIAL_TYPE_INFO( Type ) \
ENABLE_SERIAL_TYPE( PACK( Type ) ) \
namespace memserial { \
template std::string serialize< std::string, Type >( const Type& ); \
template Type parse< Type, std::string >( const std::string& ); \
template uint64_t ident< Type >(); \
template std::string alias< Type, std::string >(); \
template void print< std::ostream&, Type >( std::ostream&, const Type& ); \
template void print< std::ostream, Type >( std::ostream&&, const Type& ); \
}

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
ENABLE_SERIAL_TYPE_TEMPLATE( tuple )
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
