#include "../server.hpp"
using std::cout;
using std::endl;
using std::string;
using std::vector;

void	client::_get_(request& requestObj,server& _server){
	if (checkExistance(requestObj))
		return ;
	if (isDirectory(requestObj.getFilePath().c_str()))
	{
		if (!endsWithSlash(requestObj.getFilePath()))
		{
			if (requestObj.loc.getIndex().empty()) {
				if (!_server.getAutoindex()) {
					codeNpath(requestObj, "403 Forbidden", errorPageTamplate("403, Forbidden."), errorpages);
					return ;
				}
				else {
					requestObj.setredirectURL(requestObj.getrequestURI() + "/");
					return ;
				}
			}
			else
				requestObj.setredirectURL(requestObj.getrequestURI() + "/" + requestObj.loc.getIndex());
			return ;
		}
		if (_server.getIndex().empty()) {
			if (!_server.getAutoindex()) {
				codeNpath(requestObj, "403 Forbidden", errorPageTamplate("403, Forbidden."), errorpages);
				return ;
			}
			else {
				generateAutoIndex(requestObj.getFilePath(), "autoindex.html");//?need to do lmsa l file d index
				codeNpath(requestObj,"200 ok", "autoindex.html", errorpages);
				return ;
			}
		}
		else {
			isCGI(requestObj, requestObj.getFilePath(), _server);
		}
	}
	else {
		isCGI(requestObj, requestObj.getFilePath(), _server);
	}
}
