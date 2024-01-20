#include <iostream>

int main()
{
	std::string myString = "----------------------------985590923439367282545490--\r";
	size_t found = myString.find("----------------------------985590923439367282545490\r");

	if (found != std::string::npos) {
		std::cout << "Substring found at position " << found << std::endl;
	} else {
		std::cout << "Substring not found" << std::endl;
	}
}