#ifndef __SIMPLE_TREE_NODE_HPP__
#define __SIMPLE_TREE_NODE_HPP__

#include <string>
#include <vector>
#include <iostream>

typedef enum {
	NT_UNKNOWN = 0,
	NT_STRING,
	NT_INT,
	NT_FLOAT
} node_type_t;

class simple_tree_node {
	private:
		simple_tree_node 		*_parent;
		std::vector<simple_tree_node> 	_child_nodes;
		node_type_t				_type;		
//		std::variant<int, float, std::string>	_value;
//		int						_int_value;
//		float					_flt_value;
		std::string				_value;

	public:
		simple_tree_node(); 
		simple_tree_node(const simple_tree_node &obj);
		virtual ~simple_tree_node() {};

		inline void parent(simple_tree_node *parent) { _parent = parent; }

		void add_child(simple_tree_node &parent);
		inline int get_children_count() const { return _child_nodes.size(); }
		inline simple_tree_node get_child(int index) { return _child_nodes.at(index); }

		inline node_type_t type() const { return _type; }

		inline void value(int value) { _value = std::to_string(value); _type = NT_INT; }
		inline void value(float value) { _value = std::to_string(value); _type = NT_FLOAT; }
		inline void value(std::string value) { _value = value; _type = NT_STRING; }

		inline int int_value() const { return std::stoi(_value); }
		inline float float_value() const { return std::stod(_value); }
		inline std::string str_value() const { return _value; }

        void dump(int level);

	private:
//		void _init();
};

#endif
