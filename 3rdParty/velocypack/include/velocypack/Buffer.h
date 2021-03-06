////////////////////////////////////////////////////////////////////////////////
/// @brief Library to build up VPack documents.
///
/// DISCLAIMER
///
/// Copyright 2015 ArangoDB GmbH, Cologne, Germany
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
/// @author Max Neunhoeffer
/// @author Jan Steemann
/// @author Copyright 2015, ArangoDB GmbH, Cologne, Germany
////////////////////////////////////////////////////////////////////////////////

#ifndef VELOCYPACK_BUFFER_H
#define VELOCYPACK_BUFFER_H 1

#include <cstring>
#include <string>

#include "velocypack/velocypack-common.h"
#include "velocypack/Exception.h"

namespace avocadodb {
namespace velocypack {

template <typename T>
class Buffer {
 public:
  Buffer() : _buffer(_local), _alloc(sizeof(_local)), _pos(0) {
#ifdef VELOCYPACK_DEBUG
    // poison memory
    memset(_buffer, 0xa5, _alloc);
#endif
    initWithNone();
  }

  explicit Buffer(ValueLength expectedLength) : Buffer() {
    reserve(expectedLength);
    initWithNone();
  }

  Buffer(Buffer const& that) : Buffer() {
    if (that._pos > 0) {
      if (that._pos > sizeof(_local)) {
        _buffer = new T[checkOverflow(that._pos)];
        _alloc = that._pos;
      }
      else {
        _alloc = sizeof(_local);
      }
      memcpy(_buffer, that._buffer, checkOverflow(that._pos));
      _pos = that._pos;
    }
  }

  Buffer& operator=(Buffer const& that) {
    if (this != &that) {
      if (that._pos <= _alloc) { 
        // our own buffer is big enough to hold the data
        initWithNone();
        memcpy(_buffer, that._buffer, checkOverflow(that._pos));
      }
      else {
        // our own buffer is not big enough to hold the data
        auto buffer = new T[checkOverflow(that._pos)];
        initWithNone();
        memcpy(buffer, that._buffer, checkOverflow(that._pos));

        if (_buffer != _local) {
          delete[] _buffer;
        }
        _buffer = buffer;
        _alloc = that._pos;
      }

      _pos = that._pos;
    }
    return *this;
  }

  Buffer(Buffer&& that) : Buffer() {
    if (that._buffer == that._local) {
      memcpy(_buffer, that._buffer, checkOverflow(that._pos));
    } else {
      _buffer = that._buffer;
      _alloc = that._alloc;
      that._buffer = that._local;
      that._alloc = sizeof(that._local);
    }
    _pos = that._pos;
    that._pos = 0;
  }

  Buffer& operator=(Buffer&& that) {
    if (this != &that) {
      if (that._buffer == that._local) {
        memcpy(_buffer, that._buffer, checkOverflow(that._pos));
      } else {
        if (_buffer != _local) {
          delete[] _buffer;
        }
        _buffer = that._buffer;
        _alloc = that._alloc;
        that._buffer = that._local;
        that._alloc = sizeof(that._local);
      }
      _pos = that._pos;
      that._pos = 0;
    }
    return *this;
  }

  ~Buffer() { clear(); }

  inline T* data() { return _buffer; }
  inline T const* data() const { return _buffer; }

  inline bool empty() const { return _pos == 0; }
  inline ValueLength size() const { return _pos; }
  inline ValueLength length() const { return _pos; }
  inline ValueLength byteSize() const { return _pos; }
  
  inline ValueLength capacity() const noexcept { return _alloc; }

  std::string toString() const {
    return std::string(reinterpret_cast<char const*>(_buffer), _pos);
  }

  void reset() noexcept { 
    _pos = 0;
    initWithNone();
  }

  void resetTo(ValueLength position) {
    if (position > _alloc) { 
      throw Exception(Exception::IndexOutOfBounds);
    }
    _pos = position;
  }

  void clear() {
    reset();
    if (_buffer != _local) {
      delete[] _buffer;
      _buffer = _local;
      _alloc = sizeof(_local);
#ifdef VELOCYPACK_DEBUG
      // poison memory
      memset(_buffer, 0xa5, _alloc);
#endif
      initWithNone();
    }
  }
  
  inline T& operator[](size_t position) noexcept {
    return _buffer[position];
  }

  inline T const& operator[](size_t position) const noexcept {
    return _buffer[position];
  }
  
  inline T& at(size_t position) {
    if (position >= _pos) {
      throw Exception(Exception::IndexOutOfBounds);
    }
    return operator[](position);
  }
  
  inline T const& at(size_t position) const {
    if (position >= _pos) {
      throw Exception(Exception::IndexOutOfBounds);
    }
    return operator[](position);
  }

  inline void push_back(char c) {
    reserve(1);
    _buffer[_pos++] = c;
  }
  
  void append(uint8_t const* p, ValueLength len) {
    reserve(len);
    memcpy(_buffer + _pos, p, checkOverflow(len));
    _pos += len;
  }

  void append(char const* p, ValueLength len) {
    reserve(len);
    memcpy(_buffer + _pos, p, checkOverflow(len));
    _pos += len;
  }
  
  void append(std::string const& value) {
    return append(value.c_str(), value.size());
  }
  
  void append(Buffer<T> const& value) {
    return append(value.data(), value.size());
  }

  void reserve(ValueLength len) {
    if (_pos + len < _alloc) {
      return;
    }

    VELOCYPACK_ASSERT(_pos + len >= sizeof(_local));

    // need reallocation
    ValueLength newLen = _pos + len;
    static double const GrowthFactor = 1.25;
    if (_pos > 0 && newLen < GrowthFactor * _pos) {
      // ensure the buffer grows sensibly and not by 1 byte only
      newLen = static_cast<ValueLength>(GrowthFactor * _pos);
    }
    VELOCYPACK_ASSERT(newLen > _pos);

    T* p = new T[checkOverflow(newLen)];
#ifdef VELOCYPACK_DEBUG
    // poison memory
    memset(p, 0xa5, newLen);
#endif
    // copy old data
    memcpy(p, _buffer, checkOverflow(_pos));
    if (_buffer != _local) {
      delete[] _buffer;
    }
    _buffer = p;
    _alloc = newLen;
  }

  // reserve and zero fill
  void prealloc(ValueLength len) {
    reserve(len);
#ifdef VELOCYPACK_DEBUG
    // poison memory
    memset(_buffer + _pos, 0xa5, len);
#endif
    _pos += len;
  }

 private:
  // initialize Buffer with a None value
  inline void initWithNone() noexcept { _buffer[0] = '\x00'; }

  T* _buffer;
  ValueLength _alloc;
  ValueLength _pos;

  // an already initialized space for small values
  T _local[192];
};

typedef Buffer<char> CharBuffer;

template<typename T>
struct BufferNonDeleter {
  void operator()(Buffer<T>*) {
  }
};

}  // namespace avocadodb::velocypack
}  // namespace avocadodb

#endif
