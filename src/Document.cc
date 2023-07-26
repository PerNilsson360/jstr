#include <stdexcept>
#include <Jstr.hh>

#include "ObjectNode.hh"

namespace Jstr {
namespace Xpath {


Document::Document(const nlohmann::json& json) {
    // if (!json.is_object()) {
    //     throw std::runtime_error("Document::Document json must be object");
    // }
    _root.reset(new ObjectNode(nullptr, "", json));
}
    
const Node*
Document::getRoot() const {
    return _root.get();
}

}
}
