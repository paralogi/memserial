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
struct IdentFunctor {
    uint64_t& m_id;
    const char* m_name;
    std::size_t m_size;

    IdentFunctor( uint64_t& id, const char* name, std::size_t size ) :
            m_id( id ),
            m_name( name ),
            m_size( size ) {
    }

    template< typename T >
    bool operator()( T& value ) {
        if ( !SerialMetatype< T >::alias().equal( m_name, m_size ) )
            return false;
        m_id = SerialHelpers< T >::typeHash( SERIAL_NESTING_MAX );
        return true;
    }
};

/**
 *
 */
struct AliasFunctor {
    std::string& m_name;
    uint64_t m_ident;

    AliasFunctor( std::string& name, uint64_t ident ) :
            m_name( name ),
            m_ident( ident ) {
    }

    template< typename T >
    bool operator()( T& value ) {
        if ( SerialHelpers< T >::typeHash( SERIAL_NESTING_MAX ) != m_ident )
            return false;
        m_name = SerialMetatype< T >::alias().string();
        return true;
    }
};

/**
 *
 */
template< typename Stream >
struct PrintFunctor {
    Stream& m_stream;
    const std::string& m_bytes;
    uint64_t m_ident;

    PrintFunctor( Stream& stream, const std::string& bytes, uint64_t ident ) :
            m_stream( stream ),
            m_bytes( bytes ),
            m_ident( ident ) {
    }

    template< typename T >
    bool operator()( T& value ) {
        if ( SerialHelpers< T >::typeHash( SERIAL_NESTING_MAX ) != m_ident )
            return false;
        print( m_stream, parse< T >( m_bytes ) );
        return true;
    }
};

} // --- namespace

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
    IdentFunctor functor( id, name, size );
    searchSerial( std::move( functor ) );
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
    AliasFunctor functor( name, ident );
    searchSerial( std::move( functor ) );
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

    try {
        using namespace detail;
        using StreamType = typename std::remove_reference< Stream >::type;
        PrintFunctor< StreamType > functor( stream, bytes, ident );
        return searchSerial( std::move( functor ) );
    }
    catch ( const SerialException& ) {
        return false;
    }
}

/**
 *
 */
template< typename Stream >
bool print( Stream&& stream, const std::string& bytes ) {

    using namespace detail;
    using HashType = uint64_t;

    auto begin = bytes.data();
    auto end = begin + sizeof( HashType );
    HashType ident;

    SerialHelpers< HashType >::fromBytes( ident, begin, end );
    return print( stream, bytes, ident );
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
