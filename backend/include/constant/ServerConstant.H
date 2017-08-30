/*
 * =====================================================================================
 *
 *       Filename:  ServerConstant.H
 *
 *    Description:  This holds all backend service constants
 *
 *        Created:  09/21/2016 06:40:00 AM
 *       Compiler:  g++
 *
 *         Author:  Soumya Prasad Ukil
 *        Company:  AlgoEngine
 *
 * =====================================================================================
 */


#ifndef ServerConstant_H
#define ServerConstant_H

namespace Algo
{
  namespace ServerConstant 
  {
    const int maxOrdersPerSecond = 10000;
    const int digitsForSeconds = 100000;
    const std::string ConfigFile = "./config/AlgoEngine.conf";
    const int MAXDATABUFFER = 32768;
    const int MAX_BUF_SIZE = 4096;
  };
}

#endif
