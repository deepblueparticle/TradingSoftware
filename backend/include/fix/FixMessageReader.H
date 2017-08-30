/*
 * =====================================================================================
 *
 *       Filename:  FixMessageReader.H
 *
 *    Description:  
 *
 *        Created:  09/19/2016 06:36:29 AM
 *       Compiler:  g++
 *
 *         Author:  Soumya Prasad Ukil
 *        Company:  AlgoEngine
 *
 * =====================================================================================
 */

#ifndef FixMessageReader_H
#define FixMessageReader_H

#include <string>
#include <unordered_map>

namespace Algo
{
  class FixMessageReader
  {
    public :
      /**
       * @brief Display a fix string in human readable format.
       * This is just a debug message and should not be used in production code.
       * @param Header you wish to add.. example Inbound from OMS,
       *        outbound to PLUS
       * @param The fixMessage in string format
       * @return
       */
      static void deserialize(const std::string &direction,const std::string &fixMessage);
    private :
      /**
       * All of the FIX 4.4 tags we use including some custom tags.
       * The map saves the Key ( Tag Number ) to its description ( Tag Text ).
       */
      static std::unordered_map< std::string , std::string> _fixTags;

      /**
       * All the Fix Values that we are aware of.
       * The key is created using the tag number and tag value separated by '-'.
       * The value is the description of this tag value.
       */
      static std::unordered_map< std::string , std::string> _fixTagsDesc;
  };
}

#endif
