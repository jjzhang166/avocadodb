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
/// @author Jan Christoph Uhde
////////////////////////////////////////////////////////////////////////////////

#ifndef AVOCADODB_REST_VST_RESPONSE_H
#define AVOCADODB_REST_VST_RESPONSE_H 1

#include "Basics/StringBuffer.h"
#include "Rest/GeneralResponse.h"
#include "Rest/VstMessage.h"

namespace avocadodb {
class RestBatchHandler;

namespace rest {
class VstCommTask;
class GeneralCommTask;
}

using rest::VPackMessageNoOwnBuffer;

class VstResponse : public GeneralResponse {
  friend class rest::GeneralCommTask;
  friend class rest::VstCommTask;
  friend class RestBatchHandler;  // TODO must be removed

  VstResponse(ResponseCode code, uint64_t id);

 public:
  static bool HIDE_PRODUCT_HEADER;

  // required by base
  virtual uint64_t messageId() const override { return _messageId; }
  void reset(ResponseCode code) final;
  virtual avocadodb::Endpoint::TransportType transportType() override {
    return avocadodb::Endpoint::TransportType::VST;
  };

  VPackMessageNoOwnBuffer prepareForNetwork();

 private:
  //_responseCode   - from Base
  //_headers        - from Base
  std::shared_ptr<VPackBuffer<uint8_t>>
      _header;  // generated form _headers when prepared for network
  uint64_t _messageId;
};
}

#endif
