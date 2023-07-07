#ifndef _YANG_DB_HH_
#define _YANG_DB_HH_

#include <map>
#include <list>
#include <memory>

#include "DataTree.hh"

class Db {
public:
  Transaction startTx(Transaction::Type type);
  bool stopTx(Transaction tx);
  const DataNode* read(Transaction tx) const;
  DataNode* modify(Transaction tx);
private:
  uint32_t commitedTx;
  std::map<uint32_t, uint32_t> ongoingReadTxs;
  std::list<uint32_t> ongoingWriteTxs;
  std::unique_ptr<InteriorType> _root;
};

#endif
