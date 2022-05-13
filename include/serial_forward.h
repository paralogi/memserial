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

#include <cassert>
#include <cstdint>
#include <climits>
#include <cstring>
#include <string>
#include <vector>
#include <tuple>
#include <array>
#include <bitset>
#include <chrono>
#include <ratio>
#include <complex>
#include <type_traits>
#include <algorithm>
#include <iostream>

#define PACK( ... ) __VA_ARGS__
#define UNPACK( ... ) #__VA_ARGS__

/**
 * \internal
 * Maximum nesting level for serializable types, default is 16.
 * Prevents looping in case of recursive structures.
 */
#if !defined( SERIAL_NESTING_LIMIT )
#define SERIAL_NESTING_LIMIT 16
#endif

/**
 * \internal
 * The default value of the hash function.
 * Used as the initial value for the calculation.
 */
#if !defined( SERIAL_HASH_SALT )
#define SERIAL_HASH_SALT 0xffffffff
#endif

#include "serial_view.h"
#include "serial_endian.h"
#include "serial_exception.h"
#include "detail/serial_intrinsic.h"
#include "detail/serial_traits.h"
#include "detail/serial_iterator.h"
#include "detail/serial_metatype.h"
#include "detail/serial_nulltype.h"
#include "detail/serial_primitive.h"
#include "detail/serial_aggregate.h"
#include "detail/serial_string.h"
#include "detail/serial_vector.h"
#include "detail/serial_tuple.h"
#include "detail/serial_array.h"
#include "detail/serial_bitset.h"
#include "detail/serial_timepoint.h"
#include "detail/serial_duration.h"
#include "detail/serial_complex.h"
#include "detail/serial_ident.h"

/**
 * \internal
 * Declaring serialization methods for library compilation.
 */
#if !defined( DISABLE_SERIALIZATION )
#include "serialization.h"
#include "detail/serialization_impl.h"
#endif

/**
 * \internal
 * Declaring trace methods for library compilation.
 */
#if !defined( DISABLE_SERIAL_TRACE )
#include "serial_trace.h"
#include "detail/serial_trace_impl.h"
#endif

/**
 * \internal
 * Declaring version methods for library compilation.
 */
#if defined( ENABLE_SERIAL_VERSION )
#include "serial_version.h"
#include "detail/serial_version_impl.h"
#define SERIAL_VERSION_HEADER "detail/serial_hash.h"
#endif

/**
 * \internal
 * Declaring library bindings to provide compile time information about serializable types.
 * Include SERIAL_REBIND_HEADER before defining serializable types.
 */
#if !defined( SERIAL_REBIND_HEADER )
#include "boost/pfr/core.hpp"
#include "detail/serial_rebind.h"
#define SERIAL_REBIND_HEADER "detail/serial_dummy.h"
#endif

/**
 * \internal
 * Declaring library version to check relevance of serialization methods and serializable types.
 * Include SERIAL_VERSION_HEADER after defining serializable types.
 */
#if !defined( SERIAL_VERSION_HEADER )
#define SERIAL_VERSION_HEADER "detail/serial_dummy.h"
#endif

/**
 * \internal
 * Macro declares information about serializable types and serialization methods.
 */
#define SERIAL_TYPE_INFO( Type ) \
SERIAL_TYPE( PACK( Type ) ) \
SERIAL_INFO( PACK( Type ) )

#if defined( QT_CORE_LIB )
#include <QByteArray>
#include <QDebug>

#define SERIAL_INFO( Type ) \
namespace memserial { \
SERIALIALIZE( PACK( Type ), std::string ) \
SERIALIALIZE( PACK( Type ), SerialView ) \
SERIALIALIZE( PACK( Type ), QByteArray ) \
SERIAL_PARSE( PACK( Type ), std::string ) \
SERIAL_PARSE( PACK( Type ), SerialView ) \
SERIAL_PARSE( PACK( Type ), QByteArray ) \
SERIAL_PRINT( PACK( Type ), std::ostream ) \
SERIAL_PRINT( PACK( Type ), QDebug ) \
SERIAL_SIZE( PACK( Type ) ) \
SERIAL_IDENT( PACK( Type ) ) \
SERIAL_ALIAS( PACK( Type ) ) \
SERIAL_VERSION( PACK( Type ) ) \
}

#define SERIAL_INFO_COMMON \
namespace memserial { \
SERIAL_HASH( std::string ) \
SERIAL_HASH( SerialView ) \
SERIAL_HASH( QByteArray ) \
SERIAL_TRACE( std::string, std::ostream ) \
SERIAL_TRACE( SerialView, std::ostream ) \
SERIAL_TRACE( QByteArray, QDebug ) \
QDebug operator<<( QDebug dbg, long double value ) { \
    dbg << double( value ); \
    return dbg.maybeSpace(); \
} \
}

#else

#define SERIAL_INFO( Type ) \
namespace memserial { \
SERIALIALIZE( PACK( Type ), std::string ) \
SERIALIALIZE( PACK( Type ), SerialView ) \
SERIAL_PARSE( PACK( Type ), std::string ) \
SERIAL_PARSE( PACK( Type ), SerialView ) \
SERIAL_PRINT( PACK( Type ), std::ostream ) \
SERIAL_SIZE( PACK( Type ) ) \
SERIAL_IDENT( PACK( Type ) ) \
SERIAL_ALIAS( PACK( Type ) ) \
SERIAL_VERSION( PACK( Type ) ) \
}

#define SERIAL_INFO_COMMON \
namespace memserial { \
SERIAL_HASH( std::string ) \
SERIAL_HASH( SerialView ) \
SERIAL_TRACE( std::string, std::ostream ) \
SERIAL_TRACE( SerialView, std::ostream ) \
}

#endif

#if !defined( SERIALIALIZE )
#define SERIALIALIZE( Type, ByteArray )
#endif

#if !defined( SERIAL_PARSE )
#define SERIAL_PARSE( Type, ByteArray )
#endif

#if !defined( SERIAL_SIZE )
#define SERIAL_SIZE( Type )
#endif

#if !defined( SERIAL_HASH )
#define SERIAL_HASH( ByteArray )
#endif

#if !defined( SERIAL_IDENT )
#define SERIAL_IDENT( Type )
#endif

#if !defined( SERIAL_ALIAS )
#define SERIAL_ALIAS( Type )
#endif

#if !defined( SERIAL_VERSION )
#define SERIAL_VERSION( Type )
#endif

#if !defined( SERIAL_PRINT )
#define SERIAL_PRINT( Type, Stream )
#endif

#if !defined( SERIAL_TRACE )
#define SERIAL_TRACE( ByteArray, Stream )
#endif

using memserial::detail::nulltype;
SERIAL_INFO( nulltype )
SERIAL_INFO_COMMON
