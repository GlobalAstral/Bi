#pragma once

#include <List.hpp>
#include <Errors.hpp>
#include <sstream>

namespace Dict {
  template <typename K, typename V>
  class Dict {
    public:
      Dict(std::function<bool(K, K)> keys_criteria = [](K i, K a){return i == a;}, std::function<bool(V, V)> values_criteria = [](V i, V a){return i == a;}) {
        this->keys = Lists::List<K>{keys_criteria};
        this->values = Lists::List<V>{values_criteria};
      }

      V get(K key) {
        if (!this->keys.contains(key)) throw std::out_of_range("Key not present");
        return this->values.at(this->keys.index(key));
      }

      void set(K key, V val) {
        if (this->keys.contains(key)) {
          int i = this->keys.index(key);
          this->values.pop(i);
          this->values.insert(val, i);
          return;
        }
        this->keys.push(key);
        this->values.push(val);
      }

      bool contains(K key) {
        return this->keys.contains(key);
      }
      int size() {
        return this->keys.size();
      }
      std::string toString() {
        std::stringstream ss;
        for (int i = 0; i < this->size(); i++) {
          ss << this->keys.at(i) << " : " << this->values.at(i) << std::endl;
        }
        return ss.str();
      }
    private:
      Lists::List<K> keys;
      Lists::List<V> values;
  };
}
