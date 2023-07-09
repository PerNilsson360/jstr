#ifndef _NODE_HH_
#define _NODE_HH_

#include <memory>
#include <nlohmann/json.hpp>

class Node {
public:
	Node();
	Node(const std::string& name, const nlohmann::json& json, int64_t i = -1);
	Node(const Node* parent, const std::string& name, const nlohmann::json& json, int64_t i = -1);
	Node(const Node& node);
	Node& operator=(const Node& node);
	Node getRoot() const;
	const Node* getParent() const;
	const nlohmann::json& getJson() const;
	bool isValue() const;
	double getNumber() const;
	bool getBool() const;
	std::string getString() const;
	const std::string& getLocalName() const;
	bool isArrayChild() const;
	void getChild(const std::string& name , std::vector<Node>& result) const;
	void getChildren(std::vector<Node>& result) const;
	void getSubTreeNodes(std::vector<Node>& result) const;
	void search(const std::string& name, std::vector<Node>& result) const;
	bool operator==(const Node& r) const;
private:
	void assign(const Node& node);
	std::unique_ptr<const Node> _parent;
	std::string _name;
	const nlohmann::json* _json;
	int64_t _i;
};

#endif
