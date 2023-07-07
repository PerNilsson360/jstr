#include "DataTree.hh"

// InteriorType
DataNode*
InteriorType::findOrCreate(Transaction tx) const {
  DataNode* result;
  switch (tx.getType()) {
  case Transaction::Read: { 
    auto i = _txDataMap.find(tx.getCommitedTx());
    if (i == _txDataMap.end()) {
      throw std::runtime_error("InteriorType::findOrCreate, can not find tx ");
    }
    result = i->second;
    break;
  }
  case Transaction::Write:
    break;
  default:
    break;
  }
}

// ContainerData
DataNode*
ContainerData::findOrCreate(const std::string& name, Transaction tx) {
  DataNode* result;
  auto i = _children.find(name);
  if (i == _children.end()) {
    throw std::runtime_error("findOrCreate, can not find: " + name);
  }
  result = i->second->getDataNode(tx);
  return result;
}


DataNode* LeafValue::setString(const std::string& value, Transaction tx) {
  return nullptr;
}

const std::string& LeafValue::getString() const  {
  return _v;
}
