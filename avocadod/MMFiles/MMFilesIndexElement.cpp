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

#include "MMFilesIndexElement.h"
#include "Basics/VelocyPackHelper.h"
#include "Indexes/IndexLookupContext.h"
#include "MMFiles/MMFilesToken.h"

using namespace avocadodb;

MMFilesHashIndexElement::MMFilesHashIndexElement(TRI_voc_rid_t revisionId, std::vector<std::pair<VPackSlice, uint32_t>> const& values) 
    : _revisionId(revisionId), _hash(static_cast<uint32_t>(hash(values))) {
   
  for (size_t i = 0; i < values.size(); ++i) {
    subObject(i)->fill(values[i].first, values[i].second);
  }
}

MMFilesHashIndexElement* MMFilesHashIndexElement::initialize(MMFilesHashIndexElement* element, 
                                               TRI_voc_rid_t revisionId, 
                                               std::vector<std::pair<avocadodb::velocypack::Slice, uint32_t>> const& values) {
  TRI_ASSERT(!values.empty());
  return new (element) MMFilesHashIndexElement(revisionId, values);
}

/// @brief velocypack sub-object (for indexes, as part of IndexElement, 
/// if offset is non-zero, then it is an offset into the VelocyPack data in
/// the datafile or WAL file. If offset is 0, then data contains the actual data
/// in place.
avocadodb::velocypack::Slice MMFilesHashIndexElement::slice(IndexLookupContext* context, size_t position) const {
  MMFilesIndexElementValue const* sub = subObject(position);
  
  if (sub->isInline()) {
    return avocadodb::velocypack::Slice(&sub->value.data[0]);
  }
  
  uint32_t offset = sub->value.offset;
  if (offset == 0) {
    return basics::VelocyPackHelper::NullValue();
  } 
  uint8_t const* vpack = context->lookup(MMFilesToken{_revisionId});
  if (vpack == nullptr) {
    THROW_AVOCADO_EXCEPTION(TRI_ERROR_AVOCADO_DOCUMENT_NOT_FOUND);
  } 
  return VPackSlice(vpack + sub->value.offset);
}

uint64_t MMFilesHashIndexElement::hash(avocadodb::velocypack::Slice const& values) {
  uint64_t hash = 0x0123456789abcdef;
  size_t const n = values.length();
 
  for (size_t i = 0; i < n; ++i) {
    // must use normalized hash here, to normalize different representations 
    // of arrays/objects/numbers
    hash = values.at(i).normalizedHash(hash);
  }
  
  return hash & 0x00000000FFFFFFFFULL;
}

uint64_t MMFilesHashIndexElement::hash(std::vector<avocadodb::velocypack::Slice> const& values) {
  uint64_t hash = 0x0123456789abcdef;
  size_t const n = values.size();
 
  for (size_t i = 0; i < n; ++i) {
    // must use normalized hash here, to normalize different representations 
    // of arrays/objects/numbers
    hash = values[i].normalizedHash(hash);
  }
  
  return hash & 0x00000000FFFFFFFFULL;
}

uint64_t MMFilesHashIndexElement::hash(std::vector<std::pair<avocadodb::velocypack::Slice, uint32_t>> const& values) {
  uint64_t hash = 0x0123456789abcdef;
  size_t const n = values.size();
 
  for (size_t i = 0; i < n; ++i) {
    // must use normalized hash here, to normalize different representations 
    // of arrays/objects/numbers
    hash = values[i].first.normalizedHash(hash);
  }
  
  return hash & 0x00000000FFFFFFFFULL;
}

MMFilesSkiplistIndexElement::MMFilesSkiplistIndexElement(TRI_voc_rid_t revisionId, std::vector<std::pair<VPackSlice, uint32_t>> const& values) 
    : _revisionId(revisionId) {
   
  for (size_t i = 0; i < values.size(); ++i) {
    subObject(i)->fill(values[i].first, values[i].second);
  }
}

MMFilesSkiplistIndexElement* MMFilesSkiplistIndexElement::initialize(MMFilesSkiplistIndexElement* element,
                                                       TRI_voc_rid_t revisionId, 
                                                       std::vector<std::pair<avocadodb::velocypack::Slice, uint32_t>> const& values) {
  TRI_ASSERT(!values.empty());
  return new (element) MMFilesSkiplistIndexElement(revisionId, values);
}

/// @brief velocypack sub-object (for indexes, as part of IndexElement, 
/// if offset is non-zero, then it is an offset into the VelocyPack data in
/// the datafile or WAL file. If offset is 0, then data contains the actual data
/// in place.
avocadodb::velocypack::Slice MMFilesSkiplistIndexElement::slice(IndexLookupContext* context, size_t position) const {
  MMFilesIndexElementValue const* sub = subObject(position);
  
  if (sub->isInline()) {
    return avocadodb::velocypack::Slice(&sub->value.data[0]);
  }
  
  uint32_t offset = sub->value.offset;
  if (offset == 0) {
    return basics::VelocyPackHelper::NullValue();
  } 
  uint8_t const* vpack = context->lookup(MMFilesToken{_revisionId});
  if (vpack == nullptr) {
    THROW_AVOCADO_EXCEPTION(TRI_ERROR_AVOCADO_DOCUMENT_NOT_FOUND);
  } 
  return VPackSlice(vpack + sub->value.offset);
}

MMFilesSimpleIndexElement::MMFilesSimpleIndexElement(TRI_voc_rid_t revisionId, avocadodb::velocypack::Slice const& value, uint32_t offset)
    : _revisionId(revisionId), _hashAndOffset(hash(value) | (static_cast<uint64_t>(offset) << 32)) {} 
  
uint64_t MMFilesSimpleIndexElement::hash(avocadodb::velocypack::Slice const& value) {
  TRI_ASSERT(value.isString());
  return value.hashString() & 0x00000000FFFFFFFFULL;
}

VPackSlice MMFilesSimpleIndexElement::slice(IndexLookupContext* context) const {
  uint8_t const* vpack = context->lookup(MMFilesToken{_revisionId});
  if (vpack == nullptr) {
    THROW_AVOCADO_EXCEPTION(TRI_ERROR_AVOCADO_DOCUMENT_NOT_FOUND);
  } 
  return VPackSlice(vpack + offset());
}

