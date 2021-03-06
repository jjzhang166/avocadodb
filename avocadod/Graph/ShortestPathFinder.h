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
/// @author Michael Hackstein
////////////////////////////////////////////////////////////////////////////////

#ifndef AVOCADODB_GRAPH_SHORTEST_PATH_FINDER_H
#define AVOCADODB_GRAPH_SHORTEST_PATH_FINDER_H 1

#include "Basics/Common.h"

#include <velocypack/Slice.h>

namespace avocadodb {
namespace graph {
class ShortestPathResult;
}

namespace graph {

class ShortestPathFinder {
 protected:
  ShortestPathFinder() {}

 public:
  virtual ~ShortestPathFinder() {}

  virtual bool shortestPath(avocadodb::velocypack::Slice const& start,
                            avocadodb::velocypack::Slice const& target,
                            avocadodb::graph::ShortestPathResult& result,
                            std::function<void()> const& callback) = 0;

};

}  // namespace graph
}  // namespace avocadodb

#endif
