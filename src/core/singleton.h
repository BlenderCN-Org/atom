#pragma once

#include <cassert>

namespace atom {
namespace utils {

/**
 * Sablona pre singleton, stara sa o pristup k singletonu.
 * Inicializacia je prostrednictvom metody set_instance.
 * Pristup k inicializacnej metode si riadi odvodena trieda.
 * Tato trieda asi nesmie byt pouzita pre logovanie (pretoze pouziva funkcie
 * info, warning, error).
 */
template<class T>
class Singleton {
public:
  static T& instance()
  {
    return *our_instance;
  }

private:
  // zakazane kopirovanie
  Singleton(const Singleton<T>&);

  // zakazane kopirovanie
  Singleton<T>& operator=(const Singleton<T>&);

protected:
  Singleton()
  {
//    info("Singleton create %s", demangle(typeid(T).name()).c_str());
  }

  ~Singleton()
  {
//    info("Singleton destroy %s ", demangle(typeid(T).name()).c_str());
  }

  static void set_instance(T *instance)
  {
    assert(our_instance == nullptr);
    our_instance = instance;
  }

private:
  static T* our_instance;
};

template<class T> T* Singleton<T>::our_instance = nullptr;

}
}
