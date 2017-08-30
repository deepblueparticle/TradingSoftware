#ifndef SINGLETONE_OBJECT_STORAGE_MAP
#define SINGLETONE_OBJECT_STORAGE_MAP

#include <map>

namespace Algo {

/*! \class    SingletonObjectStorageMap
 *  \brief    Storage class to stote and retrieve child classes based on a key
 */

template <typename Object>
class SingletonObjectStorageMap
{
  public:
    /// Get singleton instance
    static SingletonObjectStorageMap& getInstance() 
    {
      static SingletonObjectStorageMap<Object> instance;
      return instance;
    }

    /*!
     * \brief Method is used to register object in factory based on a key.
     *        Internally object will be stored in a map.
     *
     * \param key This is key of map on which object will be stored
     * \param object This is the object to be stored in factory
     */
    void put(const std::string &key, Object *object)
    {
      _objectsMap[key] = object;
    }

    /*!
     * \brief Method is used to retrieve object from factory based on a key.
     *        Objects are stored in a map having a key associated with it.
     *
     * \param key Key to find out associated object from the factory
     * \return object based on the given key if it is present or null pointer.
     */
    Object* get(const std::string &key)
    {
      if (_objectsMap.find(key) != _objectsMap.end()) {
        return _objectsMap[key];
      }
      return nullptr;
    }

  private:
    SingletonObjectStorageMap() {}
    SingletonObjectStorageMap(const SingletonObjectStorageMap&);

    std::map<std::string, Object*> _objectsMap;
};
}

#endif // SINGLETONE_OBJECT_STORAGE_MAP
