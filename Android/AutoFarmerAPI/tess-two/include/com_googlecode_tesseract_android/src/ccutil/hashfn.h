/**********************************************************************
 * File:        hashfn.h  (Formerly hash.h)
 * Description: Portability hacks for hash_map, hash_set and unique_ptr.
 * Author:      Ray Smith
 * Created:     Wed Jan 08 14:08:25 PST 2014
 *
 * (C) Copyright 2014, Google Inc.
 ** Licensed under the Apache License, Version 2.0 (the "License");
 ** you may not use this file except in compliance with the License.
 ** You may obtain a copy of the License at
 ** http://www.apache.org/licenses/LICENSE-2.0
 ** Unless required by applicable law or agreed to in writing, software
 ** distributed under the License is distributed on an "AS IS" BASIS,
 ** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 ** See the License for the specific language governing permissions and
 ** limitations under the License.
 *
 **********************************************************************/

#ifndef           HASHFN_H
#define           HASHFN_H

#if (__cplusplus >= 201103L) || defined(_MSC_VER)  // Visual Studio
#include <unordered_map>
#include <unordered_set>
#if defined(_MSC_VER) && (_MSC_VER >= 1500 && _MSC_VER < 1600)  // VS 2008
#define TessHashMap std::tr1::unordered_map
#define TessHashSet std::tr1::unordered_set
#else  // _MSC_VER
#define TessHashMap std::unordered_map
#define TessHashSet std::unordered_set
#include <memory>
#define SmartPtr std::unique_ptr
#define HAVE_UNIQUE_PTR
#endif  // _MSC_VER
#elif (defined(__GNUC__) && (((__GNUC__ == 3) && (__GNUC_MINOR__ > 0)) || \
  __GNUC__ >= 4))  // gcc
// hash_set is deprecated in gcc
#include <ext/hash_map>
#include <ext/hash_set>
using __gnu_cxx::hash_map;
using __gnu_cxx::hash_set;
#define TessHashMap __gnu_cxx::hash_map
#define TessHashSet __gnu_cxx::hash_set
#else
#include <hash_map>
#include <hash_set>
#define TessHashMap hash_map
#define TessHashSet :hash_set
#endif  // gcc

#ifndef HAVE_UNIQUE_PTR
// Trivial smart ptr. Expand to add features of std::unique_ptr as required.
template<class T> class SmartPtr {
 public:
  SmartPtr() : ptr_(NULL) {}
  explicit SmartPtr(T* ptr) : ptr_(ptr) {}
  ~SmartPtr() {
    delete ptr_;
  }

  T* get() const {
    return ptr_;
  }
  void reset(T* ptr) {
    delete ptr_;
    ptr_ = ptr;
  }
  bool operator==(const T* ptr) const {
    return ptr_ == ptr;
  }
  T* operator->() const {
    return ptr_;
  }
 private:
  T* ptr_;
};
#endif  // HAVE_UNIQUE_PTR

#endif  // HASHFN_H
