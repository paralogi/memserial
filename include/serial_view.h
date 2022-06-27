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
#include <cassert>

namespace memserial {

/**
 * \~english
 * \brief The byte array class used in serialization methods.
 * \~russian
 * \brief Класс байтового массива, используемый в методах сериализации.
 */
class SerialView {
public:
    using reference = std::string::reference;
    using const_reference = std::string::const_reference;
    using pointer = std::string::pointer;
    using const_pointer = std::string::const_pointer;
    using iterator = std::string::iterator;
    using const_iterator = std::string::const_iterator;

    SerialView() :
            m_alloc( nullptr ),
            m_data( nullptr ),
            m_size( 0 ) {
    }

    template< std::size_t N >
    SerialView( char ( &data )[ N ] ) :
            m_alloc( nullptr ),
            m_data( data ),
            m_size( N ) {
    }

    SerialView( char* data, std::size_t size ) :
            m_alloc( nullptr ),
            m_data( data ),
            m_size( size ) {
    }

    SerialView( std::string& data ) :
            m_alloc( nullptr ),
            m_data( &data[ 0 ] ),
            m_size( data.size() ) {
    }

    SerialView( std::size_t size ) :
            m_alloc( static_cast< char* >( operator new( size ) ) ),
            m_data( m_alloc ),
            m_size( size ) {
    }

    SerialView( const SerialView& data ) :
            m_alloc( static_cast< char* >( operator new( data.m_size ) ) ),
            m_data( m_alloc ),
            m_size( data.m_size ) {
        std::memcpy( m_alloc, data.m_alloc, m_size );
    }

    SerialView( SerialView&& data ) :
            m_alloc( data.m_alloc ),
            m_data( data.m_data ),
            m_size( data.m_size ) {
        data.m_alloc = nullptr;
        data.m_data = nullptr;
        data.m_size = 0;
    }

    ~SerialView() {
        if ( m_alloc )
            operator delete( m_alloc );
    }

    iterator begin() {
        return iterator( m_data );
    }

    const_iterator begin() const {
        return const_iterator( m_data );
    }

    iterator end() {
        return iterator( m_data + m_size );
    }

    const_iterator end() const {
        return const_iterator( m_data + m_size );
    }

    std::size_t size() const {
        return m_size;
    }

    void resize( std::size_t size ) {
        assert( !m_alloc );
        m_alloc = static_cast< char* >( operator new( size ) );
        m_data = m_alloc;
        m_size = size;
    }

    void clear() {
        assert( m_alloc );
        operator delete( m_alloc );
        m_alloc = nullptr;
        m_data = nullptr;
        m_size = 0;
    }

    bool empty() const {
        return m_size == 0;
    }

    const_reference operator[]( std::size_t pos ) const {
        assert( pos < m_size );
        return m_data[ pos ];
    }

    reference operator[]( std::size_t pos ) {
        assert( pos < m_size );
        return m_data[ pos ];
    }

    const_pointer data() const {
        return m_data;
    }

    pointer data() {
        return m_data;
    }

private:
    char* m_alloc;
    char* m_data;
    std::size_t m_size;
};

} // --- namespace
