/*
 * =====================================================================================
 *
 *       Filename:  ThreadSafeVector.H
 *
 *    Description:
 *
 *        Created:  12/13/2016 03:18:01 AM
 *       Compiler:  g++
 *
 *         Author:  Soumya Prasad Ukil
 *        Company:  AlgoEngine
 *
 * =====================================================================================
 */

#ifndef ThreadSafeVector_H
#define ThreadSafeVector_H

#include <boost/serialization/access.hpp>
#include <boost/serialization/vector.hpp>

#include <vector>
#include <mutex>
#include <string>
#include <algorithm>

namespace Algo
{
  template<typename T>
  class ThreadSafeVector : public std::vector<T>
  {
    friend class boost::serialization::access;
    public:
      bool find(const T &data_)
      {
        _lock.lock();
        bool found = (std::find(this->begin(), this->end(), data_) != this->end());
       _lock.unlock();
        return found;
      }

      void addData(const T &data_)
      {
        _lock.lock();
        this->push_back(data_);
        _lock.unlock();
      }

      void deleteData(const T &data_)
      {
        _lock.lock();
        typename std::vector<T>::iterator position = std::find(this->begin(), this->end(), data_);
        if (position != this->end())
          this->erase(position);
        _lock.unlock();
      }

      T last()
      {
        _lock.lock();
        T data = this->back();
        _lock.unlock();
        return data;
      }
    private:
      template<class Archive>
      void serialize(Archive &ar, const unsigned int version)
      {
          ar & boost::serialization::base_object<std::vector<T>>(*this);
      }

      std::mutex _lock;
  };
}

#endif
