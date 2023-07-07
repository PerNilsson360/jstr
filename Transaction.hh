#ifndef _TRANSACTION_HH_
#define _TRANSACTION_HH_

#include <cstdint>

class Transaction {
public:
  enum Type {
    Read,
    Write
  };
  Type getType() const { return _type; }
  uint32_t getTx() const { return _tx; }
  uint32_t getCommitedTx() const { return _commitedTx; }
private:
  Transaction(Type type, uint32_t tx) : _type(type), _tx(tx) {}
  Type _type;
  uint32_t _tx;
  uint32_t _commitedTx;
  friend class Db;
};

#endif

