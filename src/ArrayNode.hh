#ifndef _ARRAY_NODE_HH_
#define _ARRAY_NODE_HH_

#include "ObjectNode.hh"

namespace Jstr {
namespace Xpath {

class ArrayNode : public ObjectNode {
public:
    ArrayNode() = delete;
    ArrayNode(const Node* parent, const std::string& name, const nlohmann::json& json, int64_t i);
    ArrayNode(const ArrayNode& node) = delete;
    ArrayNode& operator=(const ArrayNode& node) = delete;
    bool isValue() const override;
    const nlohmann::json& getJson() const override;
    bool isArrayChild() const override;
    //void getSubTreeNodes(std::vector<const Node*>& result) const override;
private:
    void instantiateChildren() const;
    int64_t _i;
};

}
}

#endif
