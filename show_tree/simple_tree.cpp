#include <string>
#include <vector>
#include <iostream>
#include <iomanip>
#include <fstream>

#include <nlohmann/json.hpp>

#include "simple_tree.hpp"

static int serialize_node_to_json(nlohmann::json &j, simple_tree_node &node) {
	int i;
	int type = node.type();

	j["type"] = type;
    if(type == NT_STRING) {
        j["value"] = node.str_value();
    } else if(type == NT_INT) {
        j["value"] = node.int_value();
    } else if(type == NT_FLOAT) {
		j["value"] = node.float_value();
    } 

	if(node.get_children_count() > 0) {
		std::vector<nlohmann::json>	children;
		for(i = 0; i<node.get_children_count(); i++) {
			nlohmann::json  j_child_node;
			simple_tree_node child_node = node.get_child(i);

			serialize_node_to_json(j_child_node, child_node);
			children.push_back(j_child_node);
		}
		j["children"] = children;
	}

	return 0;
}

static int serialize_to_json_file(std::string file_name, simple_tree_node &root_node) {
	int res;
    nlohmann::json  j;

    res = serialize_node_to_json(j, root_node);

	if(res == 0) {
    	std::ofstream f(file_name);

    	if(!f) {
        	std::cerr << "Unable to open output file\n" << std::endl;
        	return -1;
    	}
		f << std::setw(2) << j << std::endl;
	}
	return res;
}

static int serialize_node_to_binary(std::ofstream &f, simple_tree_node &node) {
	int tmp = node.type();

	f.write((char *)&tmp, 1);

    if(tmp == NT_STRING) {
		std::string value = node.str_value();
		tmp = value.length();	
	
		f.write((char *)&tmp, 4);
		f.write(value.c_str(), tmp);
    } else if(tmp == NT_INT) {
        tmp = node.int_value();
        f.write((char *)&tmp, 4);
    } else if(tmp == NT_FLOAT) {
        float value = node.float_value();
        f.write((char *)&value, 4);
    } 

	tmp = node.get_children_count();
	f.write((char *)&tmp, 4);
    if(tmp > 0) {
		int i;

        for(i = 0; i<node.get_children_count(); i++) {
			simple_tree_node child_node = node.get_child(i);
            serialize_node_to_binary(f, child_node);
        }
    }

	return 0;
}

static int  serialize_to_binary_file(std::string file_name, simple_tree_node &root_node) {
    std::ofstream f(file_name, std::ofstream::binary | std::ofstream::trunc);

    if(!f) {
        std::cerr << "Unable to open output file\n" << std::endl;
        return -1;
    }

    return serialize_node_to_binary(f, root_node);
}



int simple_tree::serialize(std::string file_name, file_fmt_t format) {
	int res;

    if(format == F_TEXT) {                                                                                      
		res = serialize_to_json_file(file_name, _root_node);                                                           
	} else {                                                                                                    
        res = serialize_to_binary_file(file_name, _root_node);                                                         
    }

	return res;
}

static int unserialize_node_from_json(nlohmann::json &j, simple_tree_node &node) {
    int type =  j["type"].get<int>();

    if(type == NT_STRING) {
        node.value(j["value"].get<std::string>());
    } else if(type == NT_INT) {
        node.value(j["value"].get<int>());
    } else if(type == NT_FLOAT) {
        node.value(j["value"].get<float>());
    } else {
        return -1;
    }

    if (j.find("children") != j.end()) {
        nlohmann::json a = j["children"].get<nlohmann::json>();

        for (nlohmann::json::iterator it = a.begin(); it != a.end(); ++it) {
            simple_tree_node child;
            unserialize_node_from_json(*it, child);
            node.add_child(child);
        }
    }

    return 0;
}

static int unserialize_json_file(std::string file_name, simple_tree_node &root_node) {
    nlohmann::json  j;
    std::ifstream f(file_name);

    if(!f) {
        std::cerr << "Unable to open input file\n" << std::endl;
        return -1;
    }

    f >> j;

    return unserialize_node_from_json(j, root_node);
}

static int unserialize_node_from_binary(std::ifstream &f, simple_tree_node &node) {
	int i;
	int type = 0;
	int num_child_nodes = 0;

	f.read((char *)&type, 1);

    if(type == NT_STRING) {
		int len;
		char *buf;

		f.read((char *)&len, 4);
		buf = new char[len + 1];
		if(buf)	{
			std::memset(buf, 0, len+1);
			f.read(buf, len);
			node.value(std::string(buf));
			delete[] buf;
		}		
    } else if(type == NT_INT) {
		int value;
		f.read((char *)&value, 4);
		node.value(value);
    } else if(type == NT_FLOAT) {
		float value;
		f.read((char *)&value, 4);
		node.value(value);
    } else {
        return -1;
    }

	f.read((char *)&num_child_nodes, 4);

	if(num_child_nodes < 0) {
		return -1;
	}

	for(i=0; i<num_child_nodes; i++) {
    	simple_tree_node child;
        if(unserialize_node_from_binary(f, child) < 0) {
			return -1;
		}
        node.add_child(child);
		
	}

	return 0;
}

static int  unserialize_binary_file(std::string file_name, simple_tree_node &root_node) {

    std::ifstream f(file_name, std::ifstream::binary);

    if(!f) {
        std::cerr << "Unable to open input file\n" << std::endl;
        return -1;
    }

    return unserialize_node_from_binary(f, root_node);
}


int simple_tree::unserialize(std::string file_name, file_fmt_t format) {
	int res;

    if(format == F_TEXT) {                                                                                      
		res = unserialize_json_file(file_name, _root_node);                                                           
	} else {                                                                                                    
        res = unserialize_binary_file(file_name, _root_node);                                                         
    }

	return res;
}

void simple_tree::dump() {
    _root_node.dump(0);
}
