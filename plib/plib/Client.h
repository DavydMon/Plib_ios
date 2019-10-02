#pragma once

#include "pl_api.h"

#include <cstdint>

#if !ANDROID_WRAP
    #include <memory>
    #include <functional>
#endif

namespace pl
{
	typedef std::uint32_t PAPI_ID;

	class Client final
	{
		public:

			enum class SockCommand
			{
				connect,
				disconnect
			};
 
			struct Request
			{
				PAPI_ID id;
				pl_api::object_ptr<pl_api::Function> function;

#if defined(ANDROID_WRAP)
                Request(PAPI_ID request_id, pl::pl_api::Function * func)
                    :id(request_id), function(func)
                {}
                Request() = default;
#endif
			};

			struct Indicator
			{
				bool event;
				PAPI_ID id;
				pl_api::object_ptr<pl_api::Object> object;
#if defined(ANDROID_WRAP)
                Indicator(Indicator && ) = default;
                Indicator() = default;
#endif
			};

            void Send(Request &&request);
            Indicator Receive(std::uint32_t timeout);


            

#if defined(ANDROID_WRAP)
            class Callbacks
            {
				public:
					virtual bool SendString(const std::string & str) = 0;
					virtual bool SendBinary(const void *buf, std::uint32_t len) = 0;
					virtual bool SockCommand(std::uint32_t command) = 0;
					virtual bool SendLog(const std::string & str) = 0;

					virtual ~Callbacks() = default;
            };

            void SetCallbacks(Callbacks *callbacks);
#else
			typedef std::function<bool(std::uint32_t)> CallbackSockCommand;
			typedef std::function<bool(const char*,std::uint32_t)> CallbackStrStream;
			typedef std::function<bool(const unsigned char*, std::uint32_t)> CallbackBinStream;
			typedef std::function<bool(const char*, std::uint32_t)> CallbackLog;




			void SetCallbackSockCommand(CallbackSockCommand callback);
			void SetCallbackStrStream(CallbackStrStream callback);
			void SetCallbackBinStream(CallbackBinStream callback);
			void SetCallbackLog(CallbackLog callback);
#endif
			void OnConnect(std::string addr, std::uint16_t port, unsigned ver);
			void OnDisconnect(std::string addr, std::uint16_t port, unsigned ver);
#if defined(ANDROID_WRAP)
            bool StrStream(std::string str);
            bool BinStream(const void* data, std::uint32_t len);
#else
			bool StrStream(const char* str, std::uint32_t len);
			bool BinStream(const unsigned char* data, std::uint32_t len);
#endif


			Client();
			Client(Client &&other);
			Client &operator=(Client &&other);

			~Client();



		private:

			class Impl;
			std::unique_ptr<Impl> impl_;

	};

}
