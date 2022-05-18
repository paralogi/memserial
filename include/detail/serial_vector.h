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
template< typename ... Args >
struct SerialType< vector< Args... >, is_primitive< typename vector< Args... >::value_type > > {
    using ValueType = vector< Args... >;
    using SizeType = uint32_t;
    using DataType = typename ValueType::value_type;

    /**
     *
     */
    static constexpr bool match( uint32_t value ) {

        return hash() == value;
    }

    /**
     *
     */
    static constexpr uint32_t hash() {

        uint32_t real_hash = SERIAL_HASH_SALT;
        hash( real_hash );
        return real_hash;
    }

    static constexpr void hash( uint32_t& value, std::size_t nesting = SERIAL_NESTING_LIMIT ) {

        hash_combine( value, serial_traits< ValueType >::internal_ident );
        SerialType< SizeType >::hash( value, nesting );
        SerialType< DataType >::hash( value, nesting );
    }

    /**
     *
     */
    static constexpr std::size_t size() {

        return sizeof( SizeType );
    }

    /**
     *
     */
    static std::size_t size( const ValueType& value ) {

        if ( value.size() > std::size_t( std::numeric_limits< SizeType >::max() ) )
            throw SerialException( SerialException::ExcArrayOverflow );

        return sizeof( SizeType ) + sizeof( DataType ) * value.size();
    }

    /**
     *
     */
    template< typename Iterator >
    static void init( ValueType& value, Iterator& begin, Iterator& end ) {

        SizeType data_size;
        begin.bin( data_size );

        if ( std::ptrdiff_t( sizeof( DataType ) * data_size ) > std::distance( begin, end ) )
            throw SerialException( SerialException::ExcBufferOverflow );

        value.resize( data_size );
        begin += sizeof( DataType ) * data_size;
    }

    /**
     *
     */
    template< typename Iterator >
    static void bout( const ValueType& value, Iterator& begin ) {

        begin.bout( SizeType( value.size() ) );
        begin.bout( &value[ 0 ], value.size() );
    }

    /**
     *
     */
    template< typename Iterator >
    static void bin( ValueType& value, Iterator& begin ) {

        begin += sizeof( SizeType );
        begin.bin( &value[ 0 ], value.size() );
    }

    /**
     *
     */
    template< typename Stream >
    static void debug( const ValueType& value, Stream& stream, uint8_t level ) {

        stream << SerialMetatype< ValueType >::alias().data <<
                "< " << SerialMetatype< DataType >::alias().data <<
                " >[" << value.size() << "]: ";

        if ( value.empty() ) {
            stream << "empty";
            return;
        }

        std::string separator = std::string( ", " );

        for ( std::size_t index = 0; index < value.size(); ++index ) {
            if ( index > 0 )
                stream << separator.c_str();
            SerialType< DataType >::debug( value[ index ], stream, level + 1 );
        }
    }
};

/**
 *
 */
template< typename ... Args >
struct SerialType< vector< Args... >, is_class< typename vector< Args... >::value_type > > {
    using ValueType = vector< Args... >;
    using SizeType = uint32_t;
    using DataType = typename ValueType::value_type;

    /**
     *
     */
    static constexpr bool match( uint32_t value ) {

        return hash() == value;
    }

    /**
     *
     */
    static constexpr uint32_t hash() {

        uint32_t real_hash = SERIAL_HASH_SALT;
        hash( real_hash );
        return real_hash;
    }

    static constexpr void hash( uint32_t& value, std::size_t nesting = SERIAL_NESTING_LIMIT ) {

        hash_combine( value, serial_traits< ValueType >::internal_ident );
        SerialType< SizeType >::hash( value, nesting );
        SerialType< DataType >::hash( value, nesting );
    }

    /**
     *
     */
    static constexpr std::size_t size() {

        return sizeof( SizeType );
    }

    /**
     *
     */
    static std::size_t size( const ValueType& value ) {

        if ( value.size() > std::size_t( std::numeric_limits< SizeType >::max() ) )
            throw SerialException( SerialException::ExcArrayOverflow );

        std::size_t byte_size = sizeof( SizeType );

        for ( const auto& data : value )
            byte_size += SerialType< DataType >::size( data );

        return byte_size;
    }

    /**
     *
     */
    template< typename Iterator >
    static void init( ValueType& value, Iterator& begin, Iterator& end ) {

        SizeType data_size;
        begin.bin( data_size );

        if ( std::ptrdiff_t( SerialType< DataType >::size() * data_size ) > std::distance( begin, end ) )
            throw SerialException( SerialException::ExcBufferOverflow );

        Iterator least_end = end - SerialType< DataType >::size() * data_size;
        value.resize( data_size );

        for ( auto& data : value ) {
            least_end += SerialType< DataType >::size();
            SerialType< DataType >::init( data, begin, least_end );
        }
    }

    /**
     *
     */
    template< typename Iterator >
    static void bout( const ValueType& value, Iterator& begin ) {

        begin.bout( SizeType( value.size() ) );

        for ( const auto& data : value )
            SerialType< DataType >::bout( data, begin );
    }

    /**
     *
     */
    template< typename Iterator >
    static void bin( ValueType& value, Iterator& begin ) {

        begin += sizeof( SizeType );

        for ( auto& data : value )
            SerialType< DataType >::bin( data, begin );
    }

    /**
     *
     */
    template< typename Stream >
    static void debug( const ValueType& value, Stream& stream, uint8_t level ) {

        stream << SerialMetatype< ValueType >::alias().data <<
                "< " << SerialMetatype< DataType >::alias().data <<
                " >[" << value.size() << "]: ";

        if ( value.empty() ) {
            stream << "empty";
            return;
        }

        std::string separator( 3 * ( level + 1 ) + 1, ' ' );
        separator[ 0 ] = '\n';

        for ( std::size_t index = 0; index < value.size(); ++index ) {
            stream << separator.c_str() << index << ": ";
            SerialType< DataType >::debug( value[ index ], stream, level + 1 );
        }
    }
};

}} // --- namespace
