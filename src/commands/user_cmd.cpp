#include "Server.hpp"

bool	forbiddenUsername(std::string param) 
{
    if (param.empty())
		return true;
	else if (param.find('\0') != std::string::npos)
		return true;
	else if (param.find('\n') != std::string::npos)
		return true;
	else if (param.find('\r') != std::string::npos)
		return true;
	else if (param.find('@') != std::string::npos)
		return true;
	return false;
}

bool isNumber(std::string num) {
	for (unsigned int i = 0; i < num.size(); i++) {
		if (isdigit(num[i]) == 0)
			return false;
	}
	return true;
}

bool areValidParams(const std::vector<std::string> &params) 
{
	if (forbiddenUsername(params[0]) || params[0].find(' ') != std::string::npos)
		return false;
	else if (isNumber(params[1]) && std::atol(params[1].c_str()) > 256)
		return false;
	else if (isNumber(params[1]) == false && (params[1] != params[0]))
		return false;
	else if (params[3].size() < 1 || forbiddenUsername(params[3]))
		return false;
	return true;
}

void user_cmd(User &user, Command &cmd)
{

}