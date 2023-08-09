#include <Message.hpp>

Message::NumericReply::NumericReply(std::string code, std::string param) throw()
{
	_code = code;
	_param = param;
}

Message::NumericReply::~NumericReply(void) throw()
{
	return ;
}

const char *Message::NumericReply::what() const throw()
{
	return (_code.c_str());
}


const char *Message::NumericReply::param() const throw()
{
	return (_param.c_str());
}
