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
/// @author Kaveh Vahedipour
////////////////////////////////////////////////////////////////////////////////

#ifndef AVOCADOD_CONSENSUS_INCEPTION_H
#define AVOCADOD_CONSENSUS_INCEPTION_H 1

#include <memory>

#include "Agency/AgencyCommon.h"
#include "Basics/Common.h"
#include "Basics/ConditionVariable.h"
#include "Basics/Mutex.h"
#include "Basics/Thread.h"

#include <velocypack/Builder.h>
#include <velocypack/velocypack-aliases.h>

namespace avocadodb {
namespace consensus {

class Agent;

/// @brief This class organises the startup of the agency until the point
///        where the RAFT implementation can commence function
class Inception : public Thread {

public:

  /// @brief Default ctor
  Inception();

  /// @brief Construct with agent
  explicit Inception(Agent*);

  /// @brief Defualt dtor
  virtual ~Inception();

  /// @brief Report in from other agents measurements
  void reportIn(query_t const&);

  /// @brief Report acknowledged version for peer id
  void reportVersionForEp(std::string const&, size_t);

  void beginShutdown() override;
  void run() override;

 private:

  /// @brief We are a restarting active RAFT agent
  bool restartingActiveAgent();
  
  /// @brief Gossip your way into the agency
  void gossip();

  Agent* _agent;                           //< @brief The agent
  avocadodb::basics::ConditionVariable _cv; //< @brief For proper shutdown
  std::vector<double> _pings;              //< @brief pings
  std::map<std::string,size_t> _acked;     //< @brief acknowledged config version
  mutable avocadodb::Mutex _vLock;          //< @brieg Guard _acked
  mutable avocadodb::Mutex _pLock;          //< @brief Guard _pings
  std::vector<std::vector<double>> _measurements; //< @brief measurements
  mutable avocadodb::Mutex _mLock;          //< @brief Guard _measurements
  
};

}}

#endif
