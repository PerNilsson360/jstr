#ifndef _XPATH_DATA_HH_
#define _XPATH_DATA_HH_

#include <vector>
#include <string>
#include "Node.hh"

class XpathData {
public:
	enum Type {
		Number,
		Bool,
		String,
		NodeSet
	};
	XpathData();
	XpathData(const XpathData& xd);
	XpathData(double d);
	XpathData(bool b);
	XpathData(const std::string& s);
	XpathData(const std::string& name, const nlohmann::json& json);
	XpathData(const Node& node);
	XpathData(const std::vector<Node>& ns);
	XpathData& operator=(const XpathData& xd);
	~XpathData();
	/**
	 * @return true if this object represents a literal or a node with a value.
	 */
	bool isValue() const;
	double getNumber() const;
	bool getBool() const;
	/**
	 * Returns the XML "string value" of the data.
	 * Primitive values are interpreted as XML text nodes.
	 * Objects and arrays are mapped to elements.
	 * "The string-value of an element node is the concatenation of the string-values of
	 *  all text node descendants of the element node in document order."
	 * @return the value as a string
	 */ 
	std::string getString() const;
	const std::vector<Node>& getNodeSet() const;
	XpathData getNodeSetSize() const;
	XpathData getLocalName() const;
	XpathData getRoot() const;
	bool operator==(const XpathData& xd) const;
	bool operator!=(const XpathData& xd) const;
	bool operator<(const XpathData& xd) const;
	bool operator<=(const XpathData& xd) const;
	bool operator>(const XpathData& xd) const;
	bool operator>=(const XpathData& xd) const;
private:
	void checkOrderingRelationArgs(const XpathData& xd) const;
	void assign(const XpathData& xd);
	void clear();
	Type _type;
	union Data {
		double n;
		bool b;
		std::string* s;
		std::vector<Node>* ns;
	} _d;
};

inline
bool
operator==(const XpathData& xd, double d) {
	const std::vector<Node>& ns = xd.getNodeSet();
	for (const Node& l : xd.getNodeSet()) {
		if (l.getNumber() == d) {
			return true;
		}
	}
	return false;
}

inline
bool
operator==(const XpathData& xd, const std::string& s) {
	const std::vector<Node>& ns = xd.getNodeSet();
	for (const Node& l : xd.getNodeSet()) {
		const std::string& ls = l.getString();
		if (ls == s) {
			return true;
		}
	}
	return false;
}

inline
bool
operator==(const XpathData& xd, bool b) {
	for (const Node& l : xd.getNodeSet()) {
		if (l.getBool() == b) {
			return true;
		}
	}
	return false;
}

inline
bool
operator!=(const XpathData& xd, double d) {
	const std::vector<Node>& ns = xd.getNodeSet();
	for (const Node& l : xd.getNodeSet()) {
		if (l.getNumber() != d) {
			return true;
		}
	}
	return false;
}

inline
bool
operator!=(const XpathData& xd, const std::string& s) {
	const std::vector<Node>& ns = xd.getNodeSet();
	for (const Node& l : xd.getNodeSet()) {
		const std::string& ls = l.getString();
		if (ls != s) {
			return true;
		}
	}
	return false;
}

inline
bool
operator!=(const XpathData& xd, bool b) {
	for (const Node& l : xd.getNodeSet()) {
		if (l.getBool() != b) {
			return true;
		}
	}
	return false;
}

#endif
