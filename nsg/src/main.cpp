
#include <ns/generator.h>

#include <stdio.h>
#include <filesystem>

int main(int argc, char** argv) {
	if (argc != 3) {
		printf("Invalid number of arguments. Tool require a project path and output folder.\n");
		return 1;
	}
	ns::generateProject(std::filesystem::absolute(argv[1]).generic_string().c_str(), std::filesystem::absolute(argv[2]).generic_string().c_str());

	return 0;
}
