#include "../server.hpp"
using std::cout;
using std::endl;
using std::string;
using std::vector;

int  deleteDirectory(const char* path) {
	DIR* dir = opendir(path);
	if (dir == nullptr) {
		std::cerr << "Error opening directory: " << path << std::endl;
		return 1;
	}

	struct dirent* entry;
	while ((entry = readdir(dir)) != nullptr) {
		if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
			std::string filePath = std::string(path) + "/" + entry->d_name;

			if (entry->d_type == DT_DIR) {
				// Recursively delete subdirectories
				deleteDirectory(filePath.c_str());
				// Remove the empty directory
				if (rmdir(filePath.c_str()) != 0) {
					std::cerr << "Error deleting directory: " << filePath << std::endl;
					return 1;
				}
			} else {
				// Delete files
				if (unlink(filePath.c_str()) != 0) {
					std::cerr << "Error deleting file: " << filePath << std::endl;
					return 1;
				}
			}
		}
	}

	closedir(dir);
	return 0;
}

void	client::_delete_(request& requestObj){
	requestObj.setStatusCode("204 No Content");
	string path = requestObj.loc.getRoot() + requestObj.getrequestURI();
	if (checkExistance(requestObj))
			return ;
	if (isDirectory(path.c_str())){//directory
		if (!endsWithSlash(path)){//no slash
			codeNpath(requestObj,"409 Conflict",
				errorPageTamplate("409, Conflict.").c_str(), errorpages);
			return ;
		}
		else {//with slash
			if (deleteDirectory(path.c_str()))
			{
				if (access(path.c_str(), W_OK) == 0)
				{
					codeNpath(requestObj,"403 Forbidden",
						errorPageTamplate("403, Forbidden.").c_str(), errorpages);
						return ;
				}
				else
				{
					codeNpath(requestObj,"503 Internal Server Error",
						errorPageTamplate("503 , Internal Server Error.").c_str(), errorpages);
						return ;
				}
			}
		}
	}
	else {//is file
		if (unlink(requestObj.getFilePath().c_str()) != 0) {
			codeNpath(requestObj,"403 Forbidden",
			errorPageTamplate("403, Forbidden.").c_str(), errorpages);
			return ;
		}
	}
}
