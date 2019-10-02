#include <thread>
#include <deque>

#include "utils/port/config.h"
#include "utils/MpscPollableQueue.h"

#include "Client.h"
#include "ClientImpl.h"

namespace pl
{

Client::Client() : impl_(std::make_unique<Impl>())
{
}

void Client::Send(Request &&request)
{
	impl_->send(std::move(request));
}

Client::Indicator Client::Receive(std::uint32_t timeout)
{
	return impl_->receive(timeout);
}










#if defined(ANDROID_WRAP)
void Client::SetCallbacks(Client::Callbacks *callbacks)
{

    if (callbacks)
    {
        impl_->str_stream = std::bind(&Callbacks::SendString, callbacks, std::placeholders::_1);
        impl_->bin_stream = std::bind(&Callbacks::SendBinary, callbacks, std::placeholders::_1, std::placeholders::_2);
		impl_->sock_command = std::bind(&Callbacks::SockCommand, callbacks, std::placeholders::_1);
		impl_->log_stream = std::bind(&Callbacks::SendLog, callbacks, std::placeholders::_1);
	}
}
#else
void Client::SetCallbackSockCommand(CallbackSockCommand callback)
{
	impl_->sock_command = callback;
}

void Client::SetCallbackStrStream(CallbackStrStream callback)
{
	impl_->str_stream = callback;
}

void Client::SetCallbackBinStream(CallbackBinStream callback)
{
	impl_->bin_stream = callback;
}

void Client::SetCallbackLog(CallbackLog callback)
{
	impl_->log_stream = callback;
}

#endif

void Client::OnConnect(std::string addr, std::uint16_t port, unsigned ver)
{
	impl_->OnConnect(addr, port, ver);
}

void Client::OnDisconnect(std::string addr, std::uint16_t port, unsigned ver)
{
	impl_->OnDisconnect(addr, port, ver);
}

#if defined(ANDROID_WRAP)
bool Client::StrStream(std::string str)
{
    return impl_->StrStream(str.c_str(), str.length());
}

bool Client::BinStream(const void* data, std::uint32_t len)
{
    return impl_->BinStream(static_cast<const unsigned char *> (data), len);
}
#else
bool Client::StrStream(const char* str, std::uint32_t len)
{
	return impl_->StrStream(str, len);
}

bool Client::BinStream(const unsigned char* data, std::uint32_t len)
{
	return impl_->BinStream(data, len);
}
#endif

Client::~Client() = default;
Client::Client(Client &&other) = default;
Client &Client::operator=(Client &&other) = default;

}


