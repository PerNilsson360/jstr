#ifndef _OBJECT_NODE_HH_
#define _OBJECT_NODE_HH_

#include <vector>
#include <Jstr.hh>

namespace Jstr {
namespace Xpath {
    
class ObjectNode : public Node {
public:
    ObjectNode() = delete;
    ObjectNode(const Node* parent, const std::string& name, const nlohmann::json& json);
    ObjectNode(const ObjectNode& node) = delete;
    ~ObjectNode();
    ObjectNode& operator=(const ObjectNode& node) = delete;
    void getChild(const std::string& name, std::vector<const Node*>& result) const override;
    void getChildren(std::vector<const Node*>& result) const override;
    void getSubTreeNodes(std::vector<const Node*>& result) const override;
    void search(const std::string& name, std::vector<const Node*>& result) const override;
protected:
    void addChildNode(const std::string& name, const nlohmann::json& child) const; 
    mutable std::vector<const Node*>* _children;
private:
    virtual void instantiateChildren() const;
};

}
}
#endif
