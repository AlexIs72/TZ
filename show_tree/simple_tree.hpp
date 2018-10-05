#ifndef __SIMPLE_TREE_HPP__
#define __SIMPLE_TREE_HPP__

#include "simple_tree_node.hpp"


typedef enum _file_fmt {
    F_TEXT = 0,
    F_BINARY
} file_fmt_t;


class simple_tree {
	private:
		simple_tree_node _root_node;

	public:
		int serialize(std::string file_name, file_fmt_t format);
		int unserialize(std::string file_name, file_fmt_t format);

		void dump();
};


#endif