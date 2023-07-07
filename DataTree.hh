#ifndef _DATA_TREE_HH
#define _DATA_TREE_HH

#include <map>
#include "Transaction.hh"

class DataNode {
public:
  virtual DataNode* findOrCreate(const std::string& name, Transaction tx) = 0;
  virtual DataNode* setString(const std::string& value, Transaction tx) = 0;
  virtual const std::string& getString() const = 0;
private:
};

class InternalNode {
public:
  virtual DataNode* getDataNode(Transaction tx) const = 0;
private:
};

class InteriorData : public DataNode {
public:
private:
};

class InteriorType : public InternalNode {
public:
  virtual DataNode* findOrCreate(Transaction tx) const = 0;
private:
  std::map<uint32_t, InteriorData*> _txDataMap;
};

class ContainerData : public InteriorData {
private:
  DataNode* findOrCreate(const std::string& name, Transaction tx) = 0;
  DataNode* setString(const std::string& value, Transaction tx) = 0;
  const std::string& getString() const = 0;
public:
  std::map<std::string, InternalNode*> _children;
};

class Value : public DataNode, InternalNode {
public:
  DataNode* getDataNode(Transaction tx) const;
private:
};

class LeafValue : public Value {
private:
  DataNode* findOrCreate(const std::string& name, Transaction tx) = 0;
  DataNode* setString(const std::string& value, Transaction tx) = 0;
  const std::string& getString() const = 0;
public:
  std::string _v;
};

#endif
