////////////////////////////////////////////////////////////////////////////////
/// DISCLAIMER
///
/// Copyright 2014-2017 AvocadoDB GmbH, Cologne, Germany
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
/// Copyright holder is AvocadoDB GmbH, Cologne, Germany
///
/// @author Jan Christoph Uhde
////////////////////////////////////////////////////////////////////////////////

#include "RocksDBTypes.h"

using namespace avocadodb;

namespace {

static RocksDBEntryType database = avocadodb::RocksDBEntryType::Database;
static rocksdb::Slice Database(
    reinterpret_cast<std::underlying_type<RocksDBEntryType>::type*>(&database),
    1);

static RocksDBEntryType collection = RocksDBEntryType::Collection;
static rocksdb::Slice Collection(
    reinterpret_cast<std::underlying_type<RocksDBEntryType>::type*>(
        &collection),
    1);

static RocksDBEntryType counterVal = RocksDBEntryType::CounterValue;
static rocksdb::Slice CounterValue(
    reinterpret_cast<std::underlying_type<RocksDBEntryType>::type*>(
        &counterVal),
    1);

static RocksDBEntryType document = RocksDBEntryType::Document;
static rocksdb::Slice Document(
    reinterpret_cast<std::underlying_type<RocksDBEntryType>::type*>(&document),
    1);

static RocksDBEntryType primaryIndexValue = RocksDBEntryType::PrimaryIndexValue;
static rocksdb::Slice PrimaryIndexValue(
    reinterpret_cast<std::underlying_type<RocksDBEntryType>::type*>(
        &primaryIndexValue),
    1);

static RocksDBEntryType edgeIndexValue = RocksDBEntryType::EdgeIndexValue;
static rocksdb::Slice EdgeIndexValue(
    reinterpret_cast<std::underlying_type<RocksDBEntryType>::type*>(
        &edgeIndexValue),
    1);

static RocksDBEntryType vpackIndexValue = RocksDBEntryType::VPackIndexValue;
static rocksdb::Slice VPackIndexValue(
    reinterpret_cast<std::underlying_type<RocksDBEntryType>::type*>(
        &vpackIndexValue),
    1);

static RocksDBEntryType uniqueVPIndex = RocksDBEntryType::UniqueVPackIndexValue;
static rocksdb::Slice UniqueVPackIndexValue(
    reinterpret_cast<std::underlying_type<RocksDBEntryType>::type*>(
        &uniqueVPIndex),
    1);

static RocksDBEntryType fulltextIndexValue =
    RocksDBEntryType::FulltextIndexValue;
static rocksdb::Slice FulltextIndexValue(
    reinterpret_cast<std::underlying_type<RocksDBEntryType>::type*>(
        &fulltextIndexValue),
    1);

static RocksDBEntryType geoIndexValue = RocksDBEntryType::GeoIndexValue;
static rocksdb::Slice GeoIndexValue(
    reinterpret_cast<std::underlying_type<RocksDBEntryType>::type*>(
        &geoIndexValue),
    1);

static RocksDBEntryType view = RocksDBEntryType::View;
static rocksdb::Slice View(
    reinterpret_cast<std::underlying_type<RocksDBEntryType>::type*>(&view), 1);

static RocksDBEntryType settingsValue = RocksDBEntryType::SettingsValue;
static rocksdb::Slice SettingsValue(
    reinterpret_cast<std::underlying_type<RocksDBEntryType>::type*>(
        &settingsValue),
    1);

static RocksDBEntryType replicationApplierConfig =
    RocksDBEntryType::ReplicationApplierConfig;
static rocksdb::Slice ReplicationApplierConfig(
    reinterpret_cast<std::underlying_type<RocksDBEntryType>::type*>(
        &replicationApplierConfig),
    1);

static RocksDBEntryType indexEstimateValue =
    RocksDBEntryType::IndexEstimateValue;
static rocksdb::Slice IndexEstimateValue(
    reinterpret_cast<std::underlying_type<RocksDBEntryType>::type*>(
        &indexEstimateValue),
    1);

static RocksDBEntryType keyGeneratorValue = RocksDBEntryType::KeyGeneratorValue;
static rocksdb::Slice KeyGeneratorValue(
    reinterpret_cast<std::underlying_type<RocksDBEntryType>::type*>(
        &keyGeneratorValue),
    1);
}

char const* avocadodb::rocksDBEntryTypeName(avocadodb::RocksDBEntryType type) {
  switch (type) {
    case avocadodb::RocksDBEntryType::Database:
      return "Database";
    case avocadodb::RocksDBEntryType::Collection:
      return "Collection";
    case avocadodb::RocksDBEntryType::CounterValue:
      return "CounterValue";
    case avocadodb::RocksDBEntryType::Document:
      return "Document";
    case avocadodb::RocksDBEntryType::PrimaryIndexValue:
      return "PrimaryIndexValue";
    case avocadodb::RocksDBEntryType::EdgeIndexValue:
      return "EdgeIndexValue";
    case avocadodb::RocksDBEntryType::VPackIndexValue:
      return "VPackIndexValue";
    case avocadodb::RocksDBEntryType::UniqueVPackIndexValue:
      return "UniqueVPackIndexValue";
    case avocadodb::RocksDBEntryType::View:
      return "View";
    case avocadodb::RocksDBEntryType::SettingsValue:
      return "SettingsValue";
    case avocadodb::RocksDBEntryType::ReplicationApplierConfig:
      return "ReplicationApplierConfig";
    case avocadodb::RocksDBEntryType::FulltextIndexValue:
      return "FulltextIndexValue";
    case avocadodb::RocksDBEntryType::GeoIndexValue:
      return "GeoIndexValue";
    case avocadodb::RocksDBEntryType::IndexEstimateValue:
      return "IndexEstimateValue";
    case avocadodb::RocksDBEntryType::KeyGeneratorValue:
      return "KeyGeneratorValue";
  }
  return "Invalid";
}

char const* avocadodb::rocksDBLogTypeName(avocadodb::RocksDBLogType type) {
  switch (type) {
    case avocadodb::RocksDBLogType::DatabaseCreate:
      return "DatabaseCreate";
    case avocadodb::RocksDBLogType::DatabaseDrop:
      return "DatabaseDrop";
    case avocadodb::RocksDBLogType::CollectionCreate:
      return "CollectionCreate";
    case avocadodb::RocksDBLogType::CollectionDrop:
      return "CollectionDrop";
    case avocadodb::RocksDBLogType::CollectionRename:
      return "CollectionRename";
    case avocadodb::RocksDBLogType::CollectionChange:
      return "CollectionChange";
    case avocadodb::RocksDBLogType::IndexCreate:
      return "IndexCreate";
    case avocadodb::RocksDBLogType::IndexDrop:
      return "IndexDrop";
    case avocadodb::RocksDBLogType::ViewCreate:
      return "ViewCreate";
    case avocadodb::RocksDBLogType::ViewDrop:
      return "ViewDrop";
    case avocadodb::RocksDBLogType::ViewChange:
      return "ViewChange";
    case avocadodb::RocksDBLogType::BeginTransaction:
      return "BeginTransaction";
    case avocadodb::RocksDBLogType::DocumentOperationsPrologue:
      return "DocumentOperationsPrologue";
    case avocadodb::RocksDBLogType::DocumentRemove:
      return "DocumentRemove";
    case avocadodb::RocksDBLogType::SinglePut:
      return "SinglePut";
    case avocadodb::RocksDBLogType::SingleRemove:
      return "SingleRemove";
    case avocadodb::RocksDBLogType::Invalid:
      return "Invalid";
  }
  return "Invalid";
}

rocksdb::Slice const& avocadodb::rocksDBSlice(RocksDBEntryType const& type) {
  switch (type) {
    case RocksDBEntryType::Database:
      return Database;
    case RocksDBEntryType::Collection:
      return Collection;
    case RocksDBEntryType::CounterValue:
      return CounterValue;
    case RocksDBEntryType::Document:
      return Document;
    case RocksDBEntryType::PrimaryIndexValue:
      return PrimaryIndexValue;
    case RocksDBEntryType::EdgeIndexValue:
      return EdgeIndexValue;
    case RocksDBEntryType::VPackIndexValue:
      return VPackIndexValue;
    case RocksDBEntryType::UniqueVPackIndexValue:
      return UniqueVPackIndexValue;
    case RocksDBEntryType::FulltextIndexValue:
      return FulltextIndexValue;
    case RocksDBEntryType::GeoIndexValue:
      return GeoIndexValue;
    case RocksDBEntryType::View:
      return View;
    case RocksDBEntryType::SettingsValue:
      return SettingsValue;
    case RocksDBEntryType::ReplicationApplierConfig:
      return ReplicationApplierConfig;
    case RocksDBEntryType::IndexEstimateValue:
      return IndexEstimateValue;
    case RocksDBEntryType::KeyGeneratorValue:
      return KeyGeneratorValue;
  }

  return Document;  // avoids warning - errorslice instead ?!
}


char avocadodb::rocksDBFormatVersion() {
  return '0';
}