#include <cstddef>
#include <iomanip>

#include "simple_tree_node.hpp"


simple_tree_node::simple_tree_node() : _parent(nullptr), _type(NT_UNKNOWN) {
}

simple_tree_node::simple_tree_node(const simple_tree_node &obj) {
	_parent = obj._parent;
	_type = obj._type;
	_value = obj._value;
	_child_nodes = obj._child_nodes;
}

void simple_tree_node::add_child(simple_tree_node &node) {
	node.parent(this);
	_child_nodes.push_back(node);
}

void simple_tree_node::dump(int level) {
    if(_type != NT_UNKNOWN) {
        if(level != 0) {
			std::cout << std::setw(level*4) << " ";
        }
        std::cout << str_value() << std::endl;

		for (std::vector<simple_tree_node>::iterator it = _child_nodes.begin() ; it != _child_nodes.end(); ++it) {
			it->dump(level+1);
//			std::cout << it->str_value() << std::endl;
		}
    }
}

