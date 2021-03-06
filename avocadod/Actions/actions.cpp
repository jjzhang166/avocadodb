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

#include "actions.h"

#include "Basics/ReadLocker.h"
#include "Basics/ReadWriteLock.h"
#include "Basics/StringUtils.h"
#include "Basics/WriteLocker.h"
#include "Logger/Logger.h"
#include "Rest/GeneralRequest.h"

using namespace avocadodb::basics;

////////////////////////////////////////////////////////////////////////////////
/// @brief actions
////////////////////////////////////////////////////////////////////////////////

static std::unordered_map<std::string, TRI_action_t*> Actions;

////////////////////////////////////////////////////////////////////////////////
/// @brief prefix actions
////////////////////////////////////////////////////////////////////////////////

static std::unordered_map<std::string, TRI_action_t*> PrefixActions;

////////////////////////////////////////////////////////////////////////////////
/// @brief actions lock
////////////////////////////////////////////////////////////////////////////////

static ReadWriteLock ActionsLock;

////////////////////////////////////////////////////////////////////////////////
/// @brief defines an action
////////////////////////////////////////////////////////////////////////////////

TRI_action_t* TRI_DefineActionVocBase(std::string const& name,
                                      TRI_action_t* action) {
  std::string url = name;

  while (!url.empty() && url[0] == '/') {
    url = url.substr(1);
  }

  action->_url = url;
  action->_urlParts = StringUtils::split(url, "/").size();
  
  std::unordered_map<std::string, TRI_action_t*>* which;

  WRITE_LOCKER(writeLocker, ActionsLock);

  // create a new action and store the callback function
  if (action->_isPrefix) {
    which = &PrefixActions;
  } else {
    which = &Actions;
  }

  auto it = which->find(url);

  if (it == which->end()) {
    which->emplace(url, action);
  } else {
    TRI_action_t* oldAction = (*it).second;

    delete action;
    action = oldAction;
  }

  // some debug output
  LOG_TOPIC(DEBUG, avocadodb::Logger::FIXME) << "created JavaScript " << (action->_isPrefix ? "prefix " : "") << " action '" << url << "'";

  // return old or new action description
  return action;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief looks up an action
////////////////////////////////////////////////////////////////////////////////

TRI_action_t* TRI_LookupActionVocBase(avocadodb::GeneralRequest* request) {
  // check if we know a callback
  std::vector<std::string> suffixes = request->decodedSuffixes();

  // find a direct match
  std::string name = StringUtils::join(suffixes, '/');

  READ_LOCKER(readLocker, ActionsLock);
  auto it = Actions.find(name);

  if (it != Actions.end()) {
    return (*it).second;
  }

  // find longest prefix match
  while (true) {
    auto it = PrefixActions.find(name);

    if (it != PrefixActions.end()) {
      return (*it).second;
    }

    readLocker.unlock();

    if (suffixes.empty()) {
      break;
    }

    suffixes.pop_back();
    name = StringUtils::join(suffixes, '/');

    readLocker.lock();
  }

  return nullptr;
}

////////////////////////////////////////////////////////////////////////////////
/// @brief deletes all defined actions
////////////////////////////////////////////////////////////////////////////////

void TRI_CleanupActions() {
  WRITE_LOCKER(writeLocker, ActionsLock);

  for (auto& it : Actions) {
    delete it.second;
  }
  Actions.clear();

  for (auto& it : PrefixActions) {
    delete it.second;
  }
  PrefixActions.clear();
}

void TRI_VisitActions(std::function<void(TRI_action_t*)> const& visitor) {
  READ_LOCKER(writeLocker, ActionsLock);
  
  for (auto& it : Actions) {
    visitor(it.second);
  }
  
  for (auto& it : PrefixActions) {
    visitor(it.second);
  }
}
