////////////////////////////////////////////////////////////////////////////////
/// DISCLAIMER
///
/// Copyright 2014-2016 ArangoDB GmbH, Cologne, Germany
/// Copyright 2004-2014 triAGENS GmbH, Cologne, Germany
///
/// Licensed under the Apache License, Version 2.0 (the "License");
/// you may not use this file except in compliance with the License.
/// You may obtain a copy of the License at
///
///     http://www.apache.org/licenses/LICENSE-2.0
///
/// Unless required by applicable law or agreed to in writing, software
/// distributed under the License is distributed on an "AS IS" BASIS,
/// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
/// See the License for the specific language governing permissions and
/// limitations under the License.
///
/// Copyright holder is ArangoDB GmbH, Cologne, Germany
///
/// @author Jan Steemann
////////////////////////////////////////////////////////////////////////////////

#ifndef ARANGOD_MMFILES_MMFILES_FULLTEXT_COMMON_H
#define ARANGOD_MMFILES_MMFILES_FULLTEXT_COMMON_H 1

#include "Basics/Common.h"

/// @brief enable debugging
#undef TRI_FULLTEXT_DEBUG

/// @brief typedef for the fulltext index
/// this is just a void* for users, and the actual index definitions is only
/// used internally in file fulltext-index.c
/// the reason is the index does some binary stuff its users should not bother
/// with
typedef void TRI_fts_index_t;

#endif
