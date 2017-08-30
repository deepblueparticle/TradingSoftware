#ifndef CLIENT_UTILS_H
#define CLIENT_UTILS_H

#include <unordered_map>
#include <memory>

class QString;

namespace Algo {
struct algomodel1;
bool verifyTime(const QString &time);
}

using AlgosMap = std::unordered_map<long, std::shared_ptr<Algo::algomodel1>>;

#endif // CLIENT_UTILS_H
