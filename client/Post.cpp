#include "../server.hpp"
using std::cout;
using std::endl;
using std::string;
using std::vector;

void	client::_post_(request& requestObj, server& _server){
	if (requestObj.loc.getUpload()) {
		requestObj.setStatusCode("201 Created");
		requestObj.parseRequest(requestObj.theBody, _server);
		requestObj.setredirectURL(requestObj.getUploadPath());
	}
	else if (!requestObj.loc.getUpload()){
		if (checkExistance(requestObj))
			return ;
		if (isDirectory(requestObj.getFilePath().c_str()))
		{
			if (!endsWithSlash(requestObj.getFilePath()))
			{
				requestObj.setredirectURL(requestObj.getrequestURI() + "/");
				return ;
			}
			else {
				if (!_server.getIndex().empty()) {
					codeNpath(requestObj, "403 Forbidden", errorPageTamplate("403, Forbidden."), errorpages);
					return ;
				}
				else {
					isCGI(requestObj, requestObj.getFilePath(), _server);
					if (requestObj.is_CGI == 0){
						codeNpath(requestObj, "403 Forbidden", errorPageTamplate("403, Forbidden."), errorpages);
						return ;
					}
				}
			}
		}
		else {
			isCGI(requestObj, requestObj.getFilePath(), _server);
			if (requestObj.is_CGI == 0) {
				codeNpath(requestObj, "403 Forbidden", errorPageTamplate("403, Forbidden."), errorpages);
				return ;
			}
		}
	}
}
