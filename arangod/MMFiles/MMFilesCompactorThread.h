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
/// @author Dr. Frank Celler
////////////////////////////////////////////////////////////////////////////////

#ifndef ARANGOD_MMFILES_MMFILES_COMPACTOR_THREAD_H
#define ARANGOD_MMFILES_MMFILES_COMPACTOR_THREAD_H 1

#include "Basics/Common.h"
#include "Basics/ConditionVariable.h"
#include "Basics/Thread.h"
#include "VocBase/voc-types.h"

struct MMFilesDatafile;
struct MMFilesMarker;
struct TRI_vocbase_t;

namespace arangodb {
struct CompactionContext;
class LogicalCollection;
namespace transaction {
class Methods;
}

class MMFilesCompactorThread final : public Thread {
 private:
  /// @brief compaction instruction for a single datafile
  struct CompactionInfo {
    MMFilesDatafile* _datafile;
    bool _keepDeletions;
  };

  /// @brief auxiliary struct used when initializing compaction
  struct CompactionInitialContext {
    transaction::Methods* _trx;
    LogicalCollection* _collection;
    int64_t _targetSize;
    TRI_voc_fid_t _fid;
    bool _keepDeletions;
    bool _failed;

    CompactionInitialContext(transaction::Methods* trx, LogicalCollection* collection)
        : _trx(trx), _collection(collection), _targetSize(0), _fid(0), _keepDeletions(false), _failed(false) {}
  };

 public:
  explicit MMFilesCompactorThread(TRI_vocbase_t* vocbase);
  ~MMFilesCompactorThread();

  void signal();

  /// @brief callback to drop a datafile
  static void DropDatafileCallback(MMFilesDatafile* datafile, LogicalCollection* collection);
  /// @brief callback to rename a datafile
  static void RenameDatafileCallback(MMFilesDatafile* datafile, MMFilesDatafile* compactor, LogicalCollection* collection);

 protected:
  void run() override;

 private:
  /// @brief calculate the target size for the compactor to be created
  CompactionInitialContext getCompactionContext(
    transaction::Methods* trx, LogicalCollection* collection,
    std::vector<CompactionInfo> const& toCompact);

  /// @brief compact the specified datafiles
  void compactDatafiles(LogicalCollection* collection, std::vector<CompactionInfo> const&);

  /// @brief checks all datafiles of a collection
  bool compactCollection(LogicalCollection* collection, bool& wasBlocked);

  int removeCompactor(LogicalCollection* collection, MMFilesDatafile* datafile);

  /// @brief remove an empty datafile
  int removeDatafile(LogicalCollection* collection, MMFilesDatafile* datafile);

  /// @brief determine the number of documents in the collection
  uint64_t getNumberOfDocuments(LogicalCollection* collection);

  /// @brief write a copy of the marker into the datafile
  int copyMarker(MMFilesDatafile* compactor, MMFilesMarker const* marker,
                 MMFilesMarker** result);

  /// @brief wait time between compaction runs when idle
  static constexpr unsigned compactionSleepTime() { return 1000 * 1000; }

  /// @brief compaction interval in seconds
  static constexpr double compactionCollectionInterval() { return 10.0; }
  
  /// @brief maximum number of files to compact and concat
  static constexpr unsigned maxFiles() { return 3; }

  /// @brief maximum multiple of journal filesize of a compacted file
  /// a value of 3 means that the maximum filesize of the compacted file is
  /// 3 x (collection->journalSize)
  static constexpr unsigned maxSizeFactor() { return 3; }

  static constexpr unsigned smallDatafileSize() { return 128 * 1024; }
  
  /// @brief maximum filesize of resulting compacted file
  static constexpr uint64_t maxResultFilesize() { return 128 * 1024 * 1024; }

  /// @brief minimum number of deletion marker in file from which on we will
  /// compact it if nothing else qualifies file for compaction
  static constexpr int64_t deadNumberThreshold() { return 16384; }

  /// @brief minimum size of dead data (in bytes) in a datafile that will make
  /// the datafile eligible for compaction at all.
  /// Any datafile with less dead data than the threshold will not become a
  /// candidate for compaction.
  static constexpr int64_t deadSizeThreshold() { return 128 * 1024; }

  /// @brief percentage of dead documents in a datafile that will trigger the
  /// compaction
  /// for example, if the collection contains 800 bytes of alive and 400 bytes of
  /// dead documents, the share of the dead documents is 400 / (400 + 800) = 33 %.
  /// if this value if higher than the threshold, the datafile will be compacted
  static constexpr double deadShare() { return 0.1; }

 private:
  TRI_vocbase_t* _vocbase;

  arangodb::basics::ConditionVariable _condition;
};

}

#endif
