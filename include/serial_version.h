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

#include <cstdint>

namespace memserial {

/**
 * \~english
 * \brief Returns the current version of serialization methods and serializable types.
 * \return Version number.
 * \~russian
 * \brief Возвращает актуальную версию методов сериализации и сериализуемых типов.
 * \return Номер версии.
 */
uint64_t serialVersion();

/**
 * \~english
 * \brief Checks the version relevance of serialization methods and serializable types for current build.
 * \return Sign of relevance.
 * \~russian
 * \brief Проверяет актуальность версии методов сериализации и сериализуемых типов для текущей сборки.
 * \return Признак актуальности.
 */
bool checkVersion();

/**
 * \~english
 * \brief Checks the version relevance of serializable type for current build.
 * \return Sign of relevance.
 * \~russian
 * \brief Проверяет актуальность версии сериализуемого типа для текущей сборки.
 * \return Признак актуальности.
 */
template< typename T >
bool checkVersion();

} // --- namespace
