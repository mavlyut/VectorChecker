#pragma once

#include "gtest/gtest.h"


template <typename T>
struct element {
  element() {
    add_instance();
  }

  element(T const& val) : val(val) {
    add_instance();
  }

  element(element const& rhs) : val(rhs.val) {
    copy();
    add_instance();
  }

  element& operator=(element const& rhs) {
    assert_exists();
    rhs.assert_exists();
    copy();
    val = rhs.val;
    return *this;
  }

  ~element() {
    delete_instance();
  }

  static std::unordered_set<element const*>& instances() {
    static std::unordered_set<element const*> instances;
    return instances;
  }

  static void expect_no_instances() {
    if (!instances().empty()) {
      FAIL() << "not all instances are destroyed";
      instances().clear();
    }
  }

  static void set_throw_countdown(size_t val) {
    throw_countdown = val;
  }

  friend bool operator==(element const& a, element const& b) {
    return a.val == b.val;
  }

  friend bool operator!=(element const& a, element const& b) {
    return a.val != b.val;
  }

private:
  void add_instance() {
    auto p = instances().insert(this);
    if (!p.second) {
      FAIL() << "a new object is created at the address "
             << static_cast<void*>(this)
             << " while the previous object at this address was not destroyed";
    }
  }

  void delete_instance() {
    size_t erased = instances().erase(this);
    if (erased != 1) {
      FAIL() << "attempt of destroying non-existing object at address "
             << static_cast<void*>(this);
    }
  }

  void assert_exists() const {
    std::unordered_set<element const*> const& inst = instances();
    bool exists = inst.find(this) != inst.end();
    if (!exists) {
      FAIL() << "accessing an non-existsing object at address "
             << static_cast<void const*>(this);
    }
  }

  void copy() {
    if (throw_countdown != 0) {
      --throw_countdown;
      if (throw_countdown == 0)
        throw std::runtime_error("copy failed");
    }
  }

private:
  T val;
  static inline size_t throw_countdown = 0;
};