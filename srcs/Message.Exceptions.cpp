#include <Message.hpp>

Message::NumericReply::NumericReply(std::string code, std::string what) throw()
{
	_code = code;
	_what = what;
}

Message::NumericReply::~NumericReply(void) throw()
{
	return ;
}

const char *Message::NumericReply::code() const throw()
{
	return (_code.c_str());
}

const char *Message::NumericReply::what() const throw()
{
	return (_what.c_str());
}
