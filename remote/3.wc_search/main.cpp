#include <iostream>
#include <string>
#include <boost/tokenizer.hpp>

using namespace std;
using namespace boost;

int main(int argc, char* argv[]) {
	std::string text = "token, test   string";

	(void)sizeof(argc);
	(void)sizeof(*argv);

    boost::char_separator<char> sep(", ");
    boost::tokenizer<char_separator<char>> tokens(text, sep);
    for (const auto& t : tokens) {
    	std::cout << t << "." << std::endl;
    }
}




