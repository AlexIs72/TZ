#include <sys/stat.h>
#include <stdio.h>
#include <unistd.h>

#include <iostream>
#include <string>

#include "simple_tree.hpp"

int usage(const char *prg_name, int exit_code) {

	std::cerr << "Usage: " 	<< prg_name <<
		" -i <input_file> " << 
		" -o <output_file> " << 
		" [-f <format: binary|text(default)>]" << 
		std::endl;

	return exit_code;
}

int main(int argc, char *argv[]) {
    int opt, res;
	file_fmt_t  format = F_TEXT;
    simple_tree tree;
	std::string input_file;
	std::string out_file;
	std::string tmp;

	if(argc < 3) {
		return usage(argv[0], -1);
	}
	
	while ((opt = getopt(argc, argv, "i:o:f:")) != -1) {
    	switch (opt) {
        	case 'i':
				input_file = optarg;
            	break;
            case 'o':
				out_file = optarg;
                break;
            case 'f':
				tmp = optarg;
				if(tmp.compare("binary") == 0) {
					format = F_BINARY;	
				} else if(tmp.compare("text") == 0) {
					format = F_TEXT;
				} else {
					std::cerr << "Unknwon format value : " << tmp << std::endl;
					return usage(argv[0], -1);
				}
                break;
            default: /* '?' */
				std::cerr << "Unknwon parameter : " << opt << std::endl;
				return usage(argv[0], -1);
		}
	}

	if(input_file.empty()) {
		std::cerr << "No input file specified!" << std::endl;
		return usage(argv[0], -1);
	}


	if(out_file.empty()) {
		std::cerr << "No out file specified!" << std::endl;
		return usage(argv[0], -1);
	}
	
    if(tree.unserialize(input_file, format) < 0) {
        std::cerr << "Unserialize error!" << std::endl;
        return -1;
    }

    tree.dump();
   
    if(tree.serialize(out_file, format) < 0) {
        std::cerr << "Serialize error!" << std::endl;
        return -1;
    }

	return 0;
}
