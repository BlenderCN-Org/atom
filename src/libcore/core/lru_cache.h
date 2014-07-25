#pragma once

#include <cassert>
#include <map>

namespace atom {

template<typename Key, typename T>
class LRUCache {
public:
  struct DictionaryValue;
  typedef std::map<Key, DictionaryValue> Dictionary;

  /**
   * LRUCache iterator.
   *
   * Obal na iterator interneho slovnika.
   */
  class IteratorWrapper {
  public:
    explicit IteratorWrapper(typename Dictionary::iterator base_iterator)
      : my_base_iterator(base_iterator)
    {}

    IteratorWrapper(const IteratorWrapper &wrapper)
      : my_base_iterator(wrapper.my_base_iterator)
    {}

    T& operator*()
    { return my_base_iterator->second.value; }

    const T& operator*() const
    { return my_base_iterator->second.value; }

    T* operator->()
    { return &(operator*()); }

    const T* operator->() const
    { return &(operator*()); }

    bool operator!=(const IteratorWrapper &iter) const
    { return my_base_iterator != iter.my_base_iterator; }

    bool operator==(const IteratorWrapper &iter)
    { return my_base_iterator == iter.my_base_iterator; }

    IteratorWrapper operator++()
    { ++my_base_iterator; return *this; }

  private:
    typename Dictionary::iterator my_base_iterator;
  };

  typedef IteratorWrapper iterator;
  typedef const IteratorWrapper const_iterator;

  /**
   * Struktura ktora sa uklada ako hodnota do slovnika.
   */
  struct DictionaryValue {
    T value;
    typename Dictionary::iterator prev;
    typename Dictionary::iterator next;
  };

  /**
   * Construct lru cache with required max_size.
   */
  LRUCache(int max_size)
    : my_head(std::end(my_dict))
    , my_tail(std::end(my_dict))
    , my_max_size(max_size)
  {
    assert(max_size > 0);
  }

  ~LRUCache()
  {
    my_dict.clear();
  }

  /**
   * Vlozenie prvku.
   */
  iterator insert(const Key &key, const T &value)
  {
    pre_insert();
    // vracia pair<iterator, bool>
    auto ret = my_dict.insert(std::make_pair(
          key, DictionaryValue{value, std::end(my_dict), my_head}));
    post_insert(ret.first);
    return IteratorWrapper(ret.first);
  }

  /**
   * Vlozenie prvku s move semantikou.
   */
  iterator insert(const Key &key, T &&value)
  {
    pre_insert();
    // vracia pair<iterator, bool>
    auto ret = my_dict.insert(std::make_pair(
          key, DictionaryValue{std::move(value), std::end(my_dict), my_head}));
    post_insert(ret.first);
    return IteratorWrapper(ret.first);
  }

  void clear()
  {
    my_dict.clear();
    my_head = std::end(my_dict);
    my_tail  = std::end(my_dict);
  }

  typename Dictionary::size_type size() const
  { return my_dict.size(); }

  iterator find(const Key &key)
  { return IteratorWrapper(my_dict.find(key)); }

  iterator begin()
  { return IteratorWrapper(my_dict.begin()); }

  iterator end()
  { return IteratorWrapper(my_dict.end()); }

private:  // private structs
  /**
   * Priprava pred vlozenim noveho prvku (zmazanie jedneho prvku ak je cache plna).
   */
  void pre_insert()
  {
    if (my_dict.size() >= my_max_size) {
      typename Dictionary::iterator current_tail = my_tail;
      typename Dictionary::iterator tail_prev = current_tail->second.prev;

      if (tail_prev != my_dict.end())
        tail_prev->second.next = my_dict.end();
      else
        my_head = my_dict.end();

      my_tail = tail_prev;
      my_dict.erase(current_tail);
    }
  }

  /**
   * Zakoncenie operacie vlozenie noveho prvku.
   *
   * @param new_item iterator na novo vlozeny prvok
   */
  void post_insert(typename Dictionary::iterator &new_item)
  {
    typename Dictionary::iterator current_head = my_head;
    new_item->second.next = current_head;

    if (current_head != std::end(my_dict))
      current_head->second.prev = new_item;
    else
      my_tail = new_item;

    my_head = new_item;
  }

private:
  Dictionary my_dict;
  typename Dictionary::iterator my_head;
  typename Dictionary::iterator my_tail;
  unsigned my_max_size;
};

}
