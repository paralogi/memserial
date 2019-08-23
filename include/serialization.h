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

namespace memserial {

/**
 * The serialization method of structured data.
 * Returns a sequence of bytes represented by the original structure.
 *
 * Метод сериализации структурированных данных.
 * Возвращает последовательность байт, представленных исходной структурой.
 */
template< typename T >
std::string serialize( const T& value );

/**
 * Method for parsing serialized data.
 * Returns the original data structure recovered from a set of bytes.
 * Throws a SerialException in case of invalid data.
 *
 * Метод разбора сериализованных данных.
 * Возвращает исходную структуру данных, восстановленную из набора байт.
 * Выбрасывает исключение "SerialException" в случае некорректных данных.
 */
template< typename T >
T parse( const std::string& bytes );

/**
 * Overridden method for parsing a pointer to an array of bytes.
 * Throws a SerialException in case of invalid data.
 *
 * Переопределенный метод для разбора указателя на массив байт.
 * Выбрасывает исключение "SerialException" в случае некорректных данных.
 */
template< typename T >
T parse( const char* bytes, std::size_t size );

/**
 * Returns identifier of the serializable type.
 *
 * Возвращает идентификатор сериализуемого типа.
 */
template< typename T >
uint64_t ident();

/**
 * Overridden method returns identifier by type alias.
 * In case the name is not found, will return size_t( -1 ).
 *
 * Переопределенный метод возвращает идентификатор по имени типа.
 * В случае, если имя не найдено, вернет size_t( -1 ).
 */
uint64_t ident( const std::string& name );

/**
 * Overridden method returns identifier by string name.
 * In case the name is not found, will return size_t( -1 ).
 *
 * Переопределенный метод возвращает идентификатор по строковому имени.
 * В случае, если имя не найдено, вернет size_t( -1 ).
 */
uint64_t ident( const char* name, std::size_t size );

/**
 * Returns the readable name for the serializable type.
 *
 * Возвращает читаемое имя для сериализуемого типа.
 */
template< typename T >
std::string alias();

/**
 * Overridden method returns name by type identifier.
 * In case the identifier is not found, will return an empty string.
 *
 * Переопределенный метод возвращает имя по идентификатору типа.
 * В случае, если идентификатор не найден, вернет пустую строку.
 */
std::string alias( uint64_t ident );

/**
 * Prints structured data in a human-readable format.
 *
 * Выводит структурированные данные в человекочитаемом формате.
 */
template< typename Stream, typename T >
void print( Stream&& stream, const T& value );

/**
 * Overridden method prints data by type identifier.
 *
 * Переопределенный метод выводит данные по идентификатору типа.
 */
template< typename Stream >
bool print( Stream&& stream, const std::string& bytes, uint64_t ident );

/**
 * The overridden method prints data without type information.
 *
 * Переопределенный метод выводит данные без информации о типе.
 */
template< typename Stream >
bool print( Stream&& stream, const std::string& bytes );

} // --- namespace
