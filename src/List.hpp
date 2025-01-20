#pragma once

#include <iostream>
#include <functional>

namespace Lists {
  template <typename T>
  class List {
    public:
      List(std::function<bool(T, T)> criteria = [](T i, T a){return i == a;}) {
        this->buf = (T*)malloc(List::BASE_SIZE);
        this->_size = 0;
        this->equals_criteria = criteria;
      }

      void push(T item){
        this->_size++;
        this->buf = (T*)realloc(this->buf, (this->_size)*sizeof(item));
        memcpy(&(this->buf[(this->_size)-1]), &item, sizeof(item));
      }

      void insert(T item, int index) {
        if (index < 0)
          throw std::out_of_range("Negative Index");
        this->_size++;
        this->buf = (T*)realloc(this->buf, (this->_size)*sizeof(item));
        memcpy(&(this->buf[index+1]), &(this->buf[index]), (this->_size-index)*sizeof(item));
        memcpy(&(this->buf[index]), &item, sizeof(item));
      }

      void pop(int index) {
        if (index >= this->_size)
          throw std::out_of_range("Array index out of range");
        if (index < 0)
          throw std::out_of_range("Negative Index");
        T* temp = (T*)malloc(sizeof(T)*(this->_size - 1));
        if (index > 0)
          memcpy(temp, this->buf, index*sizeof(T));
        memcpy(temp, &this->buf[index+1], (this->_size-index-1)*sizeof(T));
        this->buf = temp;
        this->_size--;
      }

      int size() {
        return this->_size;
      }

      T at(int index) {
        if (index >= this->_size)
          throw std::out_of_range("Array index out of range");
        if (index < 0)
          throw std::out_of_range("Negative Index");
        return this->buf[index];
      }

      bool contains(T item) {
        for (int i = 0; i < this->size(); i++) {
          T a = this->at(i);
          if (this->equals_criteria(a, item))
            return true;
        }
        return false;
      }

      int index(T item) {
        for (int i = 0; i < this->size(); i++) {
          if (this->equals_criteria(this->at(i), item))
            return i;
        }
        return -1;
      }

      void reset() {
        this->_size = 0;
        this->buf = (T*)realloc(this->buf, (List::BASE_SIZE)*sizeof(T));
      }

      List<T> copy() {
        Lists::List<T> cpy{};
        for (int i = 0; i < this->size(); i++)
          cpy.push(this->at(i));
        return cpy;
      }
      
      int last() {
        return this->_size-1;
      }

    private:
      static const size_t BASE_SIZE = 1;
      std::function<bool(T, T)> equals_criteria;
      T* buf;
      int _size;
  };
}
