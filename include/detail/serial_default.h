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

/**
 *
 */
template< typename T >
std::string serialize( const T& value ) {

    using namespace detail;
    using HashType = uint64_t;

    HashType hash = SerialHelpers< T >::typeHash( SERIAL_NESTING_MAX );
    auto size = SerialHelpers< T >::byteSize( value ) + sizeof( HashType );

    std::string bytes;
    bytes.resize( size );
    auto begin = bytes.begin();
    auto end = bytes.end();

    SerialHelpers< HashType >::toBytes( hash, begin, end );
    SerialHelpers< T >::toBytes( value, begin, end );

    return bytes;
}

/**
 *
 */
template< typename T >
T parse( const std::string& bytes ) {

    return parse< T >( bytes.data(), bytes.size() );
}

/**
 *
 */
template< typename T >
T parse( const char* bytes, std::size_t size ) {

    using namespace detail;
    using HashType = uint64_t;

    T value;
    auto begin = bytes;
    auto end = bytes + size;
    HashType hash = SerialHelpers< T >::typeHash( SERIAL_NESTING_MAX );
    HashType test_hash;

    SerialHelpers< HashType >::fromBytes( test_hash, begin, end );
    if ( hash != test_hash )
        throw SerialException( SerialException::ExcTypeMissmatch );

    SerialHelpers< T >::fromBytes( value, begin, end );
    if ( std::distance( begin, end ) > 0 )
        throw SerialException( SerialException::ExcTypeMissmatch );

    return value;
}

/**
 *
 */
template< typename T >
uint64_t ident() {

    return detail::SerialHelpers< T >::typeHash( SERIAL_NESTING_MAX );
}

/**
 *
 */
uint64_t ident( const std::string& name ) {

    return ident( name.data(), name.size() );
}

/**
 *
 */
uint64_t ident( const char* name, std::size_t size ) {

    using namespace detail;
    uint64_t id = -1;

    searchSerial( [ &id, name, size ]( auto&& value ) -> bool {
        using ValueType = typename std::remove_reference< decltype( value ) >::type;
        if ( !SerialMetatype< ValueType >::alias().equal( name, size ) )
            return false;
        id = SerialHelpers< ValueType >::typeHash( SERIAL_NESTING_MAX );
        return true;
    } );

    return id;
}

/**
 *
 */
template< typename T >
std::string alias() {

    return detail::SerialMetatype< T >::alias().string();
}

/**
 *
 */
std::string alias( uint64_t ident ) {

    using namespace detail;
    std::string name;

    searchSerial(
            [ &name, ident ]( auto&& value ) -> bool {
                using ValueType = typename std::remove_reference< decltype( value ) >::type;
                if ( SerialHelpers< ValueType >::typeHash( SERIAL_NESTING_MAX ) != ident )
                    return false;
                name = SerialMetatype< ValueType >::alias().string();
                return true;
            } );

    return name;
}

/**
 *
 */
template< typename Stream, typename T >
void print( Stream&& stream, const T& value ) {

    detail::SerialHelpers< T >::toDebug( value, stream, 0 );
}

/**
 *
 */
template< typename Stream >
bool print( Stream&& stream, const std::string& bytes, uint64_t ident ) {

    using namespace detail;
    return searchSerial(
            [ &stream, &bytes, ident ]( auto&& value ) -> bool {
                using ValueType = typename std::remove_reference< decltype( value ) >::type;
                if ( SerialHelpers< ValueType >::typeHash( SERIAL_NESTING_MAX ) != ident )
                    return false;
                try {
                    print( stream, parse< ValueType >( bytes ) );
                }
                catch ( const SerialException& ) {
                    return false;
                }
                return true;
            } );
}

/**
 *
 */
template< typename Stream >
bool print( Stream&& stream, const std::string& bytes ) {

    return detail::searchSerial(
            [ &stream, &bytes ]( auto&& value ) -> bool {
                using ValueType = typename std::remove_reference< decltype( value ) >::type;
                try {
                    print( stream, parse< ValueType >( bytes ) );
                }
                catch ( const SerialException& ) {
                    return false;
                }
                return true;
            } );
}

using detail::nulltype;
template std::string serialize< nulltype >( const nulltype& );
template nulltype parse< nulltype >( const std::string& );
template nulltype parse< nulltype >( const char*, std::size_t );
template uint64_t ident< nulltype >();
template std::string alias< nulltype >();
template void print< std::ostream&, nulltype >( std::ostream&, const nulltype& );
template void print< std::ostream, nulltype >( std::ostream&&, const nulltype& );
template bool print< std::ostream& >( std::ostream&, const std::string&, uint64_t );
template bool print< std::ostream >( std::ostream&&, const std::string&, uint64_t );
template bool print< std::ostream& >( std::ostream&, const std::string& );
template bool print< std::ostream >( std::ostream&&, const std::string& );

#ifdef USE_QT
template void print< QDebug&, nulltype >( QDebug&, const nulltype& );
template void print< QDebug, nulltype >( QDebug&&, const nulltype& );
template bool print< QDebug& >( QDebug&, const std::string&, uint64_t );
template bool print< QDebug >( QDebug&&, const std::string&, uint64_t );
template bool print< QDebug& >( QDebug&, const std::string& );
template bool print< QDebug >( QDebug&&, const std::string& );

QDebug operator<<( QDebug dbg, long double value ) {
    dbg << double( value );
    return dbg.maybeSpace();
}

#endif

} // --- namespace
