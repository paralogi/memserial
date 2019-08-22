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
 * Метод сериализации структурированных данных.
 * Возвращает последовательность байт, представленных исходной структурой.
 */
template< typename T >
std::string serialize( const T& value );

/**
 * Метод разбора сериализованных данных.
 * Возвращает исходную структуру данных, восстановленную из набора байт.
 * Выбрасывает исключение "SerialException" в случае некорректных данных.
 */
template< typename T >
T parse( const std::string& bytes );

/**
 * Переопределенный метод для разбора указателя на массив байт.
 * Выбрасывает исключение "SerialException" в случае некорректных данных.
 */
template< typename T >
T parse( const char* bytes, std::size_t size );

/**
 * Возвращает идентификатор для сериализуемого типа.
 */
template< typename T >
uint64_t ident();

/**
 * Переопределенный метод для получения идентификатора по имени типа.
 * В случае, если имя не найдено, вернет size_t( -1 ).
 */
uint64_t ident( const std::string& name );

/**
 * Переопределенный метод возвращает идентификатор по строковому имени.
 * В случае, если имя не найдено, вернет size_t( -1 ).
 */
uint64_t ident( const char* name, std::size_t size );

/**
 * Возвращает читаемое имя для сериализуемого типа.
 */
template< typename T >
std::string alias();

/**
 * Переопределенный метод для получения имени по идентификатору типа.
 * В случае, если идентификатор не найден, вернет пустую строку.
 */
std::string alias( uint64_t ident );

/**
 * Метод для вывода структурированных данных в человекочитаемом формате.
 */
template< typename Stream, typename T >
void print( Stream&& stream, const T& value );

/**
 * Переопределенный метод для вывода данных по идентификатору типа.
 */
template< typename Stream >
bool print( Stream&& stream, const std::string& bytes, uint64_t ident );

/**
 * Переопределенный метод для вывода данных без информации о типе.
 */
template< typename Stream >
bool print( Stream&& stream, const std::string& bytes );

} // --- namespace
