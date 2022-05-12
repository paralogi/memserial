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
 * \~english
 * \brief Returns structured data in serialized form.
 * \param value Data structure.
 * \return Byte array represented by the original structure.
 * \~russian
 * \brief Возвращает структуру данных в сериализованном виде.
 * \param value Структура данных.
 * \return Массив байт, представленная исходной структурой.
 */
template< typename ByteArray = std::string, typename T >
ByteArray serialize( const T& value );

/**
 * \~english
 * \brief Returns structured data in serialized form.
 * \param bytes Reference to byte array.
 * \param value Data structure.
 * \~russian
 * \brief Возвращает структуру данных в сериализованном виде.
 * \param bytes Ссылка на массив байт.
 * \param value Структура данных.
 */
template< typename ByteArray = std::string, typename T >
void serialize( ByteArray& bytes, const T& value );

/**
 * \~english
 * \brief Returns serialized data in structured form.
 * \param bytes Byte array.
 * \return Original data structure recovered from a set of bytes.
 * \throw SerialException In case of invalid data.
 * \~russian
 * \brief Возвращает сериализованные данные в структурированном виде.
 * \param bytes Массив байт.
 * \return Исходная структура данных, восстановленная из набора байт.
 * \throw SerialException В случае некорректных данных.
 */
template< typename T, typename ByteArray = std::string >
T parse( const ByteArray& bytes );

/**
 * \~english
 * \brief Returns serialized data in structured form.
 * \param value Reference to data structure.
 * \param bytes Byte array.
 * \throw SerialException In case of invalid data.
 * \~russian
 * \brief Возвращает сериализованные данные в структурированном виде.
 * \param value Ссылка на структуру данных.
 * \param bytes Массив байт.
 * \throw SerialException В случае некорректных данных.
 */
template< typename T, typename ByteArray = std::string >
void parse( T& value, const ByteArray& bytes );

/**
 * \~english
 * \brief Returns real size of the serialized data.
 * \param value Data structure.
 * \return Bytes count.
 * \~russian
 * \brief Возвращает итоговый размер сериализованных данных.
 * \param value Cтруктура данных.
 * \return Количество байт.
 */
template< typename T >
uint64_t size( const T& value );

/**
 * \~english
 * \brief Returns identifier from the serialized data.
 * \param bytes Byte array.
 * \return Type identifier.
 * \~russian
 * \brief Возвращает идентификатор из сериализованных данных.
 * \param value Массив байт.
 * \return Идентификатор типа.
 */
template< typename ByteArray = std::string >
uint64_t hash( const ByteArray& bytes );

/**
 * \~english
 * \brief Returns identifier of the serializable type.
 * \return Type identifier.
 * \~russian
 * \brief Возвращает идентификатор сериализуемого типа.
 * \return Идентификатор типа.
 */
template< typename T >
uint64_t ident();

/**
 * \~english
 * \brief Returns identifier of the type by its string name.
 * \param alias Type string name.
 * \return Type identifier or -1 if the name is not found.
 * \~russian
 * \brief Возвращает идентификатор типа по его строковому имени.
 * \param alias Строковое имя типа.
 * \return Идентификатор типа или -1, если имя не найдено.
 */
uint64_t ident( const std::string& alias );

/**
 * \~english
 * \brief Returns name of the serializable type.
 * \return Type string name.
 * \~russian
 * \brief Возвращает имя сериализуемого типа.
 * \return Строковое имя типа.
 */
template< typename T >
std::string alias();

/**
 * \~english
 * \brief Returns name of the type by its identifier.
 * \param ident Type identifier.
 * \return Type string name or empty string if identifier is not found.
 * \~russian
 * \brief Возвращает имя типа по его идентификатору.
 * \param ident Идентификатор типа.
 * \return Строковое имя типа или пустая строка, если идентификатор не найден.
 */
std::string alias( uint64_t ident );

} // --- namespace
