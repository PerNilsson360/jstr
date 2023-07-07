#include "Transaction.hh"
#include "YangDb.hh"

Transaction
Db::startTx(Transaction::Type txType) {
  return Transaction(txType, 0);
}

bool
Db::stopTx(Transaction tx) {
  return true;
}

const DataNode*
Db::read(Transaction tx) const {
}

DataNode*
Db::modify(Transaction tx) {
}
