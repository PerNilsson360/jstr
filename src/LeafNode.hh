#ifndef _LEAF_NODE_HH_
#define _LEAF_NODE_HH_

#include <Jstr.hh>

namespace Jstr {
namespace Xpath {

class LeafNode : public Node {
public:
    LeafNode() = delete;
    LeafNode(const Node* parent, const std::string& name, const nlohmann::json& json);
    LeafNode(const LeafNode& node) = delete;
    LeafNode& operator=(const LeafNode& node) = delete;
    bool isValue() const override;
    void getChild(const std::string& name, std::vector<const Node*>& result) const override;
    void getChildren(std::vector<const Node*>& result) const override;
    void getSubTreeNodes(std::vector<const Node*>& result) const override;
    void search(const std::string& name, std::vector<const Node*>& result) const override;
};
    
}
}

#endif
