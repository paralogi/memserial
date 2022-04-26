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

#include <iostream>

namespace memserial {

/**
 * \~english
 * \brief Prints structured data in human-readable format.
 * \param stream Class object for streaming output.
 * \param value Original data structure.
 * \~russian
 * \brief Печатает структуру данных в человекочитаемом формате.
 * \param stream Объект класса для потокового вывода.
 * \param value Исходная структура данных.
 */
template< typename T, typename Stream >
void print( const T& value, Stream&& stream = std::cout );

/**
 * \~english
 * \brief Prints serialized data in human-readable format.
 * \param stream Class object for streaming output.
 * \param bytes Byte sequence.
 * \~russian
 * \brief Печатает сериализованные данные в человекочитаемом формате.
 * \param stream Объект класса для потокового вывода.
 * \param bytes Последовательность байт.
 */
template< typename ByteArray, typename Stream >
void trace( const ByteArray& bytes, Stream&& stream = std::cout );

} // --- namespace
