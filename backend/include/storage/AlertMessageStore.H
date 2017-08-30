/*
 * =====================================================================================
 *
 *       Filename:  AlertMessageStore.H
 *
 *    Description:  
 *
 *        Created:  09/20/2016 01:16:26 AM
 *       Compiler:  g++
 *
 *         Author:  Soumya Prasad Ukil
 *        Company:  AlgoEngine
 *
 * =====================================================================================
 */

#ifndef AlertMessageStore_H
#define AlertMessageStore_H

#include <mutex>
#include <vector>
#include <shared/ialertsstore.h>
#include <shared/commands.h>

namespace Algo
{
  class AlertMessageStore : public IAlertsStore
  {
    public:
      static AlertMessageStore& getInstance();
      void addAlert(const Alerts &alert) override;
      std::mutex &getMutex();
      const std::vector<Alerts> &getSink();
      void sendAlert(Alerts alert, std::string &user, bool add = false);

    private:
      AlertMessageStore();
      AlertMessageStore(const AlertMessageStore&) = delete;

      std::mutex          _mutex;
      std::vector<Alerts> _alertSink;
  };
}

#endif
