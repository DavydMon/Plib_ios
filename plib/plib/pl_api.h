
#pragma once

#include <vector>
#ifdef SHARP_WRAP
#include <array>
#endif

#include "pl_object.h"


#if defined(ANDROID_WRAP)
#define PLAPI_DEF_CTORS_FUNCTION(TYPENAME) \
        TYPENAME() = default; \
        TYPENAME(TYPENAME &&) = default; \
        TYPENAME(Function && o) \
            throw (std::bad_cast) \
            : TYPENAME( std::move(dynamic_cast<decltype(*this)> (o)) ) \
            {} \
        TYPENAME(const TYPENAME &) = delete;

#define PLAPI_DEF_CTORS_OBJECT(TYPENAME) \
        TYPENAME() = default; \
        TYPENAME(TYPENAME &&) = default; \
        TYPENAME(Object && o) \
            throw (std::bad_cast) \
            : TYPENAME( std::move(dynamic_cast<decltype(*this)> (o)) ) \
            {} \
        TYPENAME(const TYPENAME &) = delete;

#endif


namespace pl
{

	template <class Type>
	using tl_object_ptr = std::unique_ptr<Type>;

	enum class MessengerAPI_Req
	{
		SetSubcriberInfo_Req,
		Online_Req,
		Offline_Req,
		RegistrationRequired_Cnf,
		Logout_Req,
		LogoutAll_Req,
		PushToken_Req,
		GetSubsribersStatus_Req,
		SetUserStatus_Req,
		Ping_Req,
		Ping_Cnf,
		TestMsg_Req,
		TestMsg_Cnf,
		GetRouteDirection_Req,
		SetRouteDirection_Req,

		Dial_Req,
		Dial_Cnf,
		Cancel_Req,
		PushClearCalls_Req,
		CallsBadge_Req,

		Msg_Req,
		Msg_Cnf,
		Rep_Req,
		Rep_Cnf,
		DelMsg_Req,
		Typing_Req,

		CS_PutMe_Req,
		CS_GetMe_Req,
		CS_PutContacts_Req,
		CS_DelContacts_Req,
		CS_GetProfiles_Req,
		CS_GetContacts_Req,
		CS_GetContactsHistory_Req,

		Chat_CheckList_Req,
		Chat_GetList_Req,
		Chat_GetInfo_Req, 
		Chat_CreatePrivate_Req,
		Chat_CreateGroup_Req,
		Chat_Delete_Req,
		Chat_AddNum_Req,
		Chat_DelNum_Req,
		Chat_SetName_Req,
		Chat_SetAvatar_Req,
		Chat_GetMsgs_Req,
		Chat_GetMsgsDel_Req,
		Chat_GetMsgsRepFinal_Req,
		Chat_GetMsgsRepRead_Req,
		Chat_NotRead_Req,
		Chat_ReadAll_Req,

		BL_Add_Req,
		BL_Del_Req,
		BL_Get_Req,

		FS_PutAvatar_Req,
		FS_GetAvatar_Req,
		FS_PutChatAvatar_Req,
		FS_GetChatAvatar_Req,
		FS_PutMsgFile_Req,
		FS_GetMsgFile_Req,

		Billing_Balance_Req,

		Max
	};

	enum class MessengerAPI_Ind
	{

		ConnectStatus,

		SetSubcriberInfo_Rsp,
		Online_Rsp,
		Offline_Rsp,
		RegistrationRequired_Ind,
		Logout_Rsp,
		LogoutAll_Rsp,
		PushToken_Rsp,
		GetSubsribersStatus_Rsp,
		ContactStatus_Ind,
		ContactUserStatus_Ind,
		SetUserStatus_Rsp,
		Ping_Rsp,
		Ping_Ind,
		TestMsg_Rsp,
		TestMsg_Ind,
		GetRouteDirection_Rsp,
		SetRouteDirection_Rsp,

		Dial_Rsp,
		Dial_Ind,
		Cancel_Rsp,
		PushClearCalls_Rsp,
		CallsBadge_Rsp,

		Msg_Rsp,
		Msg_Ind,
		Rep_Rsp,
		Rep_Ind,
		DelMsg_Rsp,
		Typing_Ind,

		CS_PutMe_Rsp,
		CS_GetMe_Rsp,
		CS_PutContacts_Rsp,
		CS_DelContacts_Rsp,
		CS_GetProfiles_Rsp,
		CS_GetContacts_Rsp,
		CS_GetContactsHistory_Rsp,

		Chat_CheckList_Rsp,
		Chat_GetList_Rsp,
		Chat_GetInfo_Rsp,
		Chat_CreatePrivate_Rsp,
		Chat_CreateGroup_Rsp,
		Chat_Delete_Rsp,
		Chat_AddNum_Rsp,
		Chat_DelNum_Rsp,
		Chat_SetName_Rsp,
		Chat_SetAvatar_Rsp,
		Chat_GetMsgs_Rsp,
		Chat_GetMsgsDel_Rsp,
		Chat_GetMsgsRepFinal_Rsp,
		Chat_GetMsgsRepRead_Rsp,
		Chat_ChangeInfo_Ind,
		Chat_NotRead_Rsp,
		Chat_ReadAll_Rsp,

		BL_Add_Rsp,
		BL_Del_Rsp,
		BL_Get_Rsp,

		FS_PutAvatar_Rsp,
		FS_GetAvatar_Rsp,
		FS_PutChatAvatar_Rsp,
		FS_GetChatAvatar_Rsp,
		FS_PutMsgFile_Rsp,
		FS_GetMsgFile_Rsp,

		Billing_Balance_Rsp,

		Max
	};

	enum class MessengerAPI_ConnectStatus
	{
		offline,
		connecting,




		registrtarion,
		online,

		Max
	};

	
	enum class MessengerAPI_Chats_Reslt
	{
		ok,

		last, 
		not_last, 

		error_db,
		error_int, 
		error_param,
		no_chat, 
		no_subscriber, 
		no_msg, 
		index_overlapped,
		index_old,
		chat_exists,


		tid,

		Max
	};

	enum class MessengerAPI_Msg_Result
	{
		ok,
		unknown,
		bad_request,
		internal_server_error,
		service_timeout,
		service_unavailable,
		unauthorized,
		not_found,
		billing_error,
		not_acceptable
	};

	enum class MessengerAPI_Msg_Status
	{
		unknown,
		sended,
		delivered,
		not_delivered,
		read,
		not_sended,
		read_local
	};
	

	enum class MessengerAPI_Result
	{
		ok,

		unknown,
		bad_request,
		internal_server_error,
		service_timeout,
		service_unavailable,
		unauthorized,
		not_found,
		billing_error,
		not_acceptable,
		socket_buffer_full,
		not_initialized,

		Max
	};

	enum class MessengerAPI_FileType
	{
		no,

		bin,
		picture,
		audio,
		video,

		Max
	};


#define PAPI_TxFileFragmetMaxSize  30000
#define PAPI_RxFileFragmetMaxSize  60000
#define PAPI_TestFragmetMaxSize  60000

	struct Msg
	{
		std::uint64_t msg_index = 0xffffffffffffffff;
		std::uint64_t sm_id = 0xffffffffffffffff;

		std::string sour_num;
		std::string dest_num;

		std::string submit_date_time;
		std::string done_date_time;
		std::string view_date_time;

		std::uint8_t state = 0xff;
		std::uint8_t type = 0xff;
		std::uint8_t status = 0;

		std::uint8_t file_type = 0;
		std::uint32_t file_hash = 0;

		std::uint8_t service = 0xff;

		std::uint8_t channel = 0;

		std::string text;
	};

	struct Chat
	{
		std::uint64_t hindex = 0xffffffffffffffff;
		bool del = false;
		std::uint8_t state = 0;
		std::string name;
		std::string avatar;
		std::uint8_t chat_private = 0;
		std::uint64_t chat_id = 0xffffffffffffffff;
		std::uint64_t msg_index_last = 0;
		std::uint64_t start_msg_index = 0; 
		std::uint64_t msgs_count = 0;
		std::uint64_t msgs_del = 0;
		std::uint64_t msgs_rep_final = 0;
		std::uint64_t msgs_rep_read = 0;

		std::uint32_t not_viewed = 0;

		std::vector<std::string> nums;
		std::vector<std::string> nums_del; 
		std::vector<Msg> msgs;
	};

	struct Contact
	{
		std::string num;
		std::string rnum;
		std::string name;
		std::string fname;
		std::string lname;
		std::string avatar;
		bool messenger = false;
		std::uint64_t hindex = 0;
	};

	namespace pl_api
	{

		template <class Type>
		using object_ptr = ::pl::tl_object_ptr<Type>;

		class Function : public PLObject
		{
			public:
		};

		class Object : public PLObject
		{
			public:
		};

		template <class Type, class... Args>
		object_ptr<Type> make_object(Args &&... args)
		{
			return object_ptr<Type>(new Type(std::forward<Args>(args)...));
		}


		class FS_PutFile_Req : public Function
		{
			public:

				std::uint64_t full_size = 0;
				std::uint64_t offset = 0;
				std::string file_ext;
				std::uint32_t hash;

				std::uint64_t len = 0;
				signed char bin[PAPI_TxFileFragmetMaxSize];

		};
		class FS_GetFile_Req : public Function
		{
			public:

				std::uint64_t offset = 0;
				std::uint32_t hash;
				
				bool thumbnail = false;

 				std::uint64_t len = 0;
		};

		class FS_PutFile_Rsp  : public Object
		{
			public:

				std::uint16_t rsp = 0xffff;

				std::string file_ext;
				std::uint64_t post_size = 0;
				std::uint64_t offset = 0;
		};

		class FS_GetFile_Rsp : public Object
		{
			public:

				std::uint16_t rsp = 0xffff;

				std::uint64_t full_size = 0;
				std::uint64_t offset = 0;
				std::string  file_ext;

				std::int16_t len = 0;
				signed char bin[PAPI_RxFileFragmetMaxSize];
		};

		class SetSubcriberInfo_Req final : public Function
		{
			public:
				std::string number;
				std::string host;
				std::string port;

				std::string UID;
				std::string DevID;
				std::string user_agent;
				std::uint32_t reg_expires;

				std::string code;

				std::uint8_t user_type; 
				std::uint32_t user_version;
				std::uint8_t user_client;

				std::uint8_t log_level = 0;

#if defined(ANDROID_WRAP)
				PLAPI_DEF_CTORS_FUNCTION(SetSubcriberInfo_Req);
#endif

				std::int32_t get_id() const final
				{
					return (std::int32_t)MessengerAPI_Req::SetSubcriberInfo_Req;
				}
		};

		class Online_Req final : public Function
		{
			public:

#if defined(ANDROID_WRAP)
				PLAPI_DEF_CTORS_FUNCTION(Online_Req);
#endif

				std::int32_t get_id() const final
				{
					return (std::int32_t)MessengerAPI_Req::Online_Req;
				}
		};
		class Offline_Req final : public Function
		{
			public:

#if defined(ANDROID_WRAP)
				PLAPI_DEF_CTORS_FUNCTION(Offline_Req);
#endif

				std::int32_t get_id() const final
				{
					return (std::int32_t)MessengerAPI_Req::Offline_Req;
				}
		};
		class RegistrationRequired_Cnf final : public Function
		{
			public:

				std::string code;

#if defined(ANDROID_WRAP)
				PLAPI_DEF_CTORS_FUNCTION(RegistrationRequired_Cnf);
#endif

				std::int32_t get_id() const final
				{
					return (std::int32_t)MessengerAPI_Req::RegistrationRequired_Cnf;
				}
		};
		class Logout_Req final : public Function
		{
			public:

#if defined(ANDROID_WRAP)
				PLAPI_DEF_CTORS_FUNCTION(Logout_Req);
#endif

				std::int32_t get_id() const final
				{
					return (std::int32_t)MessengerAPI_Req::Logout_Req;
				}
		};
		class LogoutAll_Req final : public Function
		{
			public:

#if defined(ANDROID_WRAP)
				PLAPI_DEF_CTORS_FUNCTION(LogoutAll_Req);
#endif

				std::int32_t get_id() const final
				{
					return (std::int32_t)MessengerAPI_Req::LogoutAll_Req;
				}
		};
		class PushToken_Req final : public Function
		{
			public:

				std::string token;
				std::string token_voip;

#if defined(ANDROID_WRAP)
				PLAPI_DEF_CTORS_FUNCTION(PushToken_Req);
#endif

				std::int32_t get_id() const final
				{
					return (std::int32_t)MessengerAPI_Req::PushToken_Req;
				}
		};

		class GetSubsribersStatus_Req final : public Function
		{
			public:

#if defined(ANDROID_WRAP)
				PLAPI_DEF_CTORS_FUNCTION(GetSubsribersStatus_Req);
#endif

				std::int32_t get_id() const final
				{
					return (std::int32_t)MessengerAPI_Req::GetSubsribersStatus_Req;
				}
		};
		class SetUserStatus_Req final : public Function
		{
			public:

				std::uint16_t status;

#if defined(ANDROID_WRAP)
				PLAPI_DEF_CTORS_FUNCTION(SetUserStatus_Req);
#endif

				std::int32_t get_id() const final
				{
					return (std::int32_t)MessengerAPI_Req::SetUserStatus_Req;
				}
		};

		class Ping_Req final : public Function
		{
			public:

#if defined(ANDROID_WRAP)
				PLAPI_DEF_CTORS_FUNCTION(Ping_Req);
#endif

				std::int32_t get_id() const final
				{
					return (std::int32_t)MessengerAPI_Req::Ping_Req;
				}
		};
		class Ping_Cnf final : public Function
		{
			public:

				std::uint16_t rsp = 0xffff;

#if defined(ANDROID_WRAP)
				PLAPI_DEF_CTORS_FUNCTION(Ping_Cnf);
#endif

				std::int32_t get_id() const final
				{
					return (std::int32_t)MessengerAPI_Req::Ping_Cnf;
				}
		};

		class TestMsg_Req final : public Function
		{
			public:

				std::uint32_t req_id = 0;
				std::uint32_t len = 0;
				std::uint8_t data[PAPI_TestFragmetMaxSize];
				bool loop = false; 

#if defined(ANDROID_WRAP)
				PLAPI_DEF_CTORS_FUNCTION(TestMsg_Req);
#endif

				std::int32_t get_id() const final
				{
					return (std::int32_t)MessengerAPI_Req::TestMsg_Req;
				}
		};
		class TestMsg_Cnf final : public Function
		{
			public:

				std::uint16_t rsp = 0xffff;

				std::uint32_t req_id = 0;
				std::uint32_t len = 0;
				std::uint8_t data[PAPI_TestFragmetMaxSize];

#if defined(ANDROID_WRAP)
				PLAPI_DEF_CTORS_FUNCTION(TestMsg_Cnf);
#endif

				std::int32_t get_id() const final
				{
					return (std::int32_t)MessengerAPI_Req::TestMsg_Cnf;
				}
		};

		class GetRouteDirection_Req final : public Function
		{
			public:

#if defined(ANDROID_WRAP)
				PLAPI_DEF_CTORS_FUNCTION(GetRouteDirection_Req);
#endif

				std::int32_t get_id() const final
				{
					return (std::int32_t)MessengerAPI_Req::GetRouteDirection_Req;
				}
		};
		class SetRouteDirection_Req final : public Function
		{
			public:

				std::uint8_t direction;

#if defined(ANDROID_WRAP)
				PLAPI_DEF_CTORS_FUNCTION(SetRouteDirection_Req);
#endif

				std::int32_t get_id() const final
				{
					return (std::int32_t)MessengerAPI_Req::SetRouteDirection_Req;
				}
		};

		class Dial_Req final : public Function
		{
			public:

				std::string snum;
				std::string dnum;

				std::string call_id;

				std::uint8_t channel = 0;

#if defined(ANDROID_WRAP)
				PLAPI_DEF_CTORS_FUNCTION(Dial_Req);
#endif

				std::int32_t get_id() const final
				{
					return (std::int32_t)MessengerAPI_Req::Dial_Req;
				}
		};

		class Dial_Cnf final : public Function
		{
			public:

				std::uint16_t rsp = 0xffff;

				std::string snum;
				std::string dnum;

				std::string call_id;

#if defined(ANDROID_WRAP)
				PLAPI_DEF_CTORS_FUNCTION(Dial_Cnf);
#endif

				std::int32_t get_id() const final
				{
					return (std::int32_t)MessengerAPI_Req::Dial_Cnf;
				}
		};

		class Cancel_Req final : public Function
		{
			public:

				std::string snum;
				std::string dnum;

				std::string call_id;



#if defined(ANDROID_WRAP)
				PLAPI_DEF_CTORS_FUNCTION(Cancel_Req);
#endif

				std::int32_t get_id() const final
				{
					return (std::int32_t)MessengerAPI_Req::Cancel_Req;
				}
		};

		class PushClearCalls_Req final : public Function
		{
			public:

#if defined(ANDROID_WRAP)
				PLAPI_DEF_CTORS_FUNCTION(PushClearCalls_Req);
#endif

				std::int32_t get_id() const final
				{
					return (std::int32_t)MessengerAPI_Req::PushClearCalls_Req;
				}
		};

		class CallsBadge_Req final : public Function
		{
			public:

#if defined(ANDROID_WRAP)
			PLAPI_DEF_CTORS_FUNCTION(CallsBadge_Req);
#endif

			std::int32_t get_id() const final
			{
				return (std::int32_t)MessengerAPI_Req::CallsBadge_Req;
			}
		};

		class Msg_Req final : public Function
		{
			public:
				std::uint64_t chat_id = 0xffffffffffffffff;
				std::uint8_t chat_private = 0;

				std::string snum;
				std::string dnum;
				std::string text;

				std::uint8_t channel = 0;

				std::uint8_t file_type = 0;
				std::uint32_t file_hash = 0;

				bool repeat = false;







#if defined(ANDROID_WRAP)
				PLAPI_DEF_CTORS_FUNCTION(Msg_Req);
#endif






				std::int32_t get_id() const final
				{
					return (std::int32_t)MessengerAPI_Req::Msg_Req;
				}
		};
		class Msg_Cnf final : public Function
		{
			public:

				std::uint16_t rsp = 0xffff;

				std::uint64_t chat_id = 0xffffffffffffffff;
				std::uint64_t sm_id = 0xffffffffffffffff;
				std::uint64_t msg_index = 0xffffffffffffffff;

				std::string snum;
				std::string dnum;


#if defined(ANDROID_WRAP)
				PLAPI_DEF_CTORS_FUNCTION(Msg_Cnf);
#endif

				std::int32_t get_id() const final
				{
					return (std::int32_t)MessengerAPI_Req::Msg_Cnf;
				}
		};

		class Rep_Req final : public Function
		{
			public:
				std::uint64_t chat_id = 0xffffffffffffffff;
				std::uint8_t chat_private = 0;

				std::string snum;
				std::string dnum;

				std::uint64_t sm_id = 0xffffffffffffffff;
				std::uint64_t msg_index = 0xffffffffffffffff;

				std::uint8_t status = 0xff;

#if defined(ANDROID_WRAP)
				PLAPI_DEF_CTORS_FUNCTION(Rep_Req);
#endif

				std::int32_t get_id() const final
				{
					return (std::int32_t)MessengerAPI_Req::Rep_Req;
				}
		};
		class Rep_Cnf final : public Function
		{
			public:

				std::uint16_t rsp = 0xffff;

				std::uint64_t chat_id = 0xffffffffffffffff;
				std::uint64_t sm_id = 0xffffffffffffffff;
				std::uint64_t msg_index = 0xffffffffffffffff;

				std::string snum;
				std::string dnum;

				std::uint8_t status = 0xff;

#if defined(ANDROID_WRAP)
				PLAPI_DEF_CTORS_FUNCTION(Rep_Cnf);
#endif

				std::int32_t get_id() const final
				{
					return (std::int32_t)MessengerAPI_Req::Rep_Cnf;
				}
		};
		class DelMsg_Req final : public Function
		{
			public:

				std::uint64_t chat_id = 0xffffffffffffffff;


				std::uint64_t sm_id = 0xffffffffffffffff;
				std::uint64_t msg_index = 0xffffffffffffffff;

				std::uint8_t type; 


#if defined(ANDROID_WRAP)
				PLAPI_DEF_CTORS_FUNCTION(DelMsg_Req);
#endif

				std::int32_t get_id() const final
				{
					return (std::int32_t)MessengerAPI_Req::DelMsg_Req;
				}
		};
		class Typing_Req final : public Function
		{
			public:

				std::uint64_t chat_id = 0xffffffffffffffff;
 				std::uint8_t  chat_private = 0;

				std::string dnum;

				std::uint8_t type = 0; 

#if defined(ANDROID_WRAP)
				PLAPI_DEF_CTORS_FUNCTION(Typing_Req);
#endif

				std::int32_t get_id() const final
				{
					return (std::int32_t)MessengerAPI_Req::Typing_Req;
				}
		};

		class CS_PutMe_Req final : public Function
		{
			public:

				std::string name;
				std::string avatar;
				std::string lang;

				std::uint8_t pinbot = 0xff; 

#if defined(ANDROID_WRAP)
				PLAPI_DEF_CTORS_FUNCTION(CS_PutMe_Req);
#endif

				std::int32_t get_id() const final
				{
					return (std::int32_t)MessengerAPI_Req::CS_PutMe_Req;
				}
		};

		class CS_GetMe_Req final : public Function
		{
			public:

#if defined(ANDROID_WRAP)
				PLAPI_DEF_CTORS_FUNCTION(CS_GetMe_Req);
#endif

				std::int32_t get_id() const final
				{
					return (std::int32_t)MessengerAPI_Req::CS_GetMe_Req;
				}
		};

		class CS_PutContacts_Req final : public Function
		{
			public:

				std::vector<Contact> contacts;

#if defined(ANDROID_WRAP)
				PLAPI_DEF_CTORS_FUNCTION(CS_PutContacts_Req);
#endif

				std::int32_t get_id() const final
				{
					return (std::int32_t)MessengerAPI_Req::CS_PutContacts_Req;
				}
		};

		class CS_DelContacts_Req final : public Function
		{
			public:

				std::vector<Contact> contacts;

#if defined(ANDROID_WRAP)
				PLAPI_DEF_CTORS_FUNCTION(CS_DelContacts_Req);
#endif

				std::int32_t get_id() const final
				{
					return (std::int32_t)MessengerAPI_Req::CS_DelContacts_Req;
				}
		};

		class CS_GetProfiles_Req final : public Function
		{
			public:

				std::uint64_t index = 0xffffffffffffffff;

#if defined(ANDROID_WRAP)
				PLAPI_DEF_CTORS_FUNCTION(CS_GetProfiles_Req);
#endif

				std::int32_t get_id() const final
				{
					return (std::int32_t)MessengerAPI_Req::CS_GetProfiles_Req;
				}
		};

		class CS_GetContacts_Req final : public Function
		{
			public:

				std::uint64_t index = 0xffffffffffffffff;

#if defined(ANDROID_WRAP)
				PLAPI_DEF_CTORS_FUNCTION(CS_GetContacts_Req);
#endif

				std::int32_t get_id() const final
				{
					return (std::int32_t)MessengerAPI_Req::CS_GetContacts_Req;
				}
		};

		class CS_GetContactsHistory_Req final : public Function
		{
			public:

				std::uint64_t index = 0xffffffffffffffff;

#if defined(ANDROID_WRAP)
				PLAPI_DEF_CTORS_FUNCTION(CS_GetContactsHistory_Req);
#endif

				std::int32_t get_id() const final
				{
					return (std::int32_t)MessengerAPI_Req::CS_GetContactsHistory_Req;
				}
		};

		class Chat_CheckList_Req final : public Function
		{
			public:


				std::uint64_t index = 0xffffffffffffffff;

#if defined(ANDROID_WRAP)
				PLAPI_DEF_CTORS_FUNCTION(Chat_CheckList_Req);
#endif

				std::int32_t get_id() const final
				{
					return (std::int32_t)MessengerAPI_Req::Chat_CheckList_Req;
				}
		};

		class Chat_GetList_Req final : public Function
		{
			public:


				std::uint64_t index = 0xffffffffffffffff;

#if defined(ANDROID_WRAP)
				PLAPI_DEF_CTORS_FUNCTION(Chat_GetList_Req);
#endif

				std::int32_t get_id() const final
				{
					return (std::int32_t)MessengerAPI_Req::Chat_GetList_Req;
				}
		};

		class Chat_GetInfo_Req final : public Function
		{
			public:

				std::uint64_t chat_id = 0xffffffffffffffff;

#if defined(ANDROID_WRAP)
				PLAPI_DEF_CTORS_FUNCTION(Chat_GetInfo_Req);
#endif

				std::int32_t get_id() const final
				{
					return (std::int32_t)MessengerAPI_Req::Chat_GetInfo_Req;
				}
		};
		class Chat_CreatePrivate_Req final : public Function
		{
			public:

				std::string number;

#if defined(ANDROID_WRAP)
				PLAPI_DEF_CTORS_FUNCTION(Chat_CreatePrivate_Req);
#endif

				std::int32_t get_id() const final
				{
					return (std::int32_t)MessengerAPI_Req::Chat_CreatePrivate_Req;
				}
		};
		class Chat_CreateGroup_Req final : public Function
		{
			public:

				std::string name;

				std::vector <std::string> numbers;

#if defined(ANDROID_WRAP)
				PLAPI_DEF_CTORS_FUNCTION(Chat_CreateGroup_Req);
#endif

				std::int32_t get_id() const final
				{
					return (std::int32_t)MessengerAPI_Req::Chat_CreateGroup_Req;
				}
		};

		class Chat_Delete_Req final : public Function
		{
			public:

				std::uint64_t chat_id = 0xffffffffffffffff;

#if defined(ANDROID_WRAP)
				PLAPI_DEF_CTORS_FUNCTION(Chat_Delete_Req);
#endif

				std::int32_t get_id() const final
				{
					return (std::int32_t)MessengerAPI_Req::Chat_Delete_Req;
				}
		};

		class Chat_AddNum_Req final : public Function
		{
			public:

				std::uint64_t chat_id = 0xffffffffffffffff;
				std::vector <std::string> numbers;

#if defined(ANDROID_WRAP)
				PLAPI_DEF_CTORS_FUNCTION(Chat_AddNum_Req);
#endif

				std::int32_t get_id() const final
				{
					return (std::int32_t)MessengerAPI_Req::Chat_AddNum_Req;
				}
		};

		class Chat_DelNum_Req final : public Function
		{
			public:

				std::uint64_t chat_id = 0xffffffffffffffff;
				std::vector <std::string> numbers;

#if defined(ANDROID_WRAP)
				PLAPI_DEF_CTORS_FUNCTION(Chat_DelNum_Req);
#endif

				std::int32_t get_id() const final
				{
					return (std::int32_t)MessengerAPI_Req::Chat_DelNum_Req;
				}
		};

		class Chat_SetName_Req final : public Function
		{
			public:

				std::uint64_t chat_id = 0xffffffffffffffff;
				std::string name;

#if defined(ANDROID_WRAP)
				PLAPI_DEF_CTORS_FUNCTION(Chat_SetName_Req);
#endif

				std::int32_t get_id() const final
				{
					return (std::int32_t)MessengerAPI_Req::Chat_SetName_Req;
				}
		};

		class Chat_SetAvatar_Req final : public Function
		{
			public:

				std::uint64_t chat_id = 0xffffffffffffffff;
				std::string avatar;

#if defined(ANDROID_WRAP)
				PLAPI_DEF_CTORS_FUNCTION(Chat_SetAvatar_Req);
#endif

				std::int32_t get_id() const final
				{
					return (std::int32_t)MessengerAPI_Req::Chat_SetAvatar_Req;
				}
		};

		class Chat_GetMsgs_Req final : public Function
		{
			public:

				std::uint64_t chat_id = 0xffffffffffffffff;
				std::uint64_t msg_index = 0xffffffffffffffff;
				std::uint64_t msg_index_stop = 0xffffffffffffffff;
				std::uint32_t count = 0;
				std::uint8_t  dir = 0;


#if defined(ANDROID_WRAP)
				PLAPI_DEF_CTORS_FUNCTION(Chat_GetMsgs_Req);
#endif

				std::int32_t get_id() const final
				{
					return (std::int32_t)MessengerAPI_Req::Chat_GetMsgs_Req;
				}
		};

		class Chat_GetMsgsDel_Req final : public Function
		{
			public:

				std::uint64_t chat_id = 0xffffffffffffffff;
				std::uint64_t msg_index = 0xffffffffffffffff;
				std::uint64_t msg_index_stop = 0xffffffffffffffff;
				std::uint32_t count = 0;
				std::uint8_t  dir = 0;
				std::string   dt_start;
				std::string   dt_stop;


#if defined(ANDROID_WRAP)
				PLAPI_DEF_CTORS_FUNCTION(Chat_GetMsgsDel_Req);
#endif

				std::int32_t get_id() const final
				{
					return (std::int32_t)MessengerAPI_Req::Chat_GetMsgsDel_Req;
				}
		};

		class Chat_GetMsgsRepFinal_Req final : public Function
		{
			public:

				std::uint64_t chat_id = 0xffffffffffffffff;
				std::uint64_t msg_index = 0xffffffffffffffff;
				std::uint64_t msg_index_stop = 0xffffffffffffffff;
				std::uint32_t count = 0;
				std::uint8_t  dir = 0;
				std::uint8_t  full_msg = 0;
				std::string   dt_start; 
				std::string   dt_stop;


#if defined(ANDROID_WRAP)
				PLAPI_DEF_CTORS_FUNCTION(Chat_GetMsgsRepFinal_Req);
#endif

				std::int32_t get_id() const final
				{
					return (std::int32_t)MessengerAPI_Req::Chat_GetMsgsRepFinal_Req;
				}
		};

		class Chat_GetMsgsRepRead_Req final : public Function
		{
			public:

				std::uint64_t chat_id = 0xffffffffffffffff;
				std::uint64_t msg_index = 0xffffffffffffffff;
				std::uint64_t msg_index_stop = 0xffffffffffffffff;
				std::uint32_t count = 0;
				std::uint8_t  dir = 0;
				std::uint8_t  full_msg = 0;
				std::string   dt_start;
				std::string   dt_stop;


#if defined(ANDROID_WRAP)
				PLAPI_DEF_CTORS_FUNCTION(Chat_GetMsgsRepRead_Req);
#endif

				std::int32_t get_id() const final
				{
					return (std::int32_t)MessengerAPI_Req::Chat_GetMsgsRepRead_Req;
				}
		};

		class Chat_NotRead_Req final : public Function
		{
			public:

				std::uint64_t chat_id = 0xffffffffffffffff;

#if defined(ANDROID_WRAP)
				PLAPI_DEF_CTORS_FUNCTION(Chat_NotRead_Req);
#endif

				std::int32_t get_id() const final
				{
					return (std::int32_t)MessengerAPI_Req::Chat_NotRead_Req;
				}
		};
		class Chat_ReadAll_Req final : public Function
		{
			public:
				std::uint64_t chat_id = 0xffffffffffffffff;
				std::uint64_t msg_index = 0xffffffffffffffff; 

#if defined(ANDROID_WRAP)
				PLAPI_DEF_CTORS_FUNCTION(Chat_ReadAll_Req);
#endif

				std::int32_t get_id() const final
				{
					return (std::int32_t)MessengerAPI_Req::Chat_ReadAll_Req;
				}
		};

		class BL_Add_Req final : public Function
		{
			public:

				std::string block_user;

#if defined(ANDROID_WRAP)
				PLAPI_DEF_CTORS_FUNCTION(BL_Add_Req);
#endif

				std::int32_t get_id() const final
				{
					return (std::int32_t)MessengerAPI_Req::BL_Add_Req;
				}
		};

		class BL_Del_Req final : public Function
		{
			public:

				std::string block_user;

#if defined(ANDROID_WRAP)
				PLAPI_DEF_CTORS_FUNCTION(BL_Del_Req);
#endif

				std::int32_t get_id() const final
				{
					return (std::int32_t)MessengerAPI_Req::BL_Del_Req;
				}
		};

		class BL_Get_Req final : public Function
		{
			public:

#if defined(ANDROID_WRAP)
				PLAPI_DEF_CTORS_FUNCTION(BL_Get_Req);
#endif

				std::int32_t get_id() const final
				{
					return (std::int32_t)MessengerAPI_Req::BL_Get_Req;
				}
		};

		class FS_PutAvatar_Req final : public FS_PutFile_Req
		{
			public:









#if defined(ANDROID_WRAP)
				PLAPI_DEF_CTORS_FUNCTION(FS_PutAvatar_Req);
#endif

				std::int32_t get_id() const final
				{
					return (std::int32_t)MessengerAPI_Req::FS_PutAvatar_Req;
				}
		};

		class FS_GetAvatar_Req final : public FS_GetFile_Req
		{
			public:






				std::string number;

#if defined(ANDROID_WRAP)
				PLAPI_DEF_CTORS_FUNCTION(FS_GetAvatar_Req);
#endif

				std::int32_t get_id() const final
				{
					return (std::int32_t)MessengerAPI_Req::FS_GetAvatar_Req;
				}
		};

		class FS_PutChatAvatar_Req final : public FS_PutFile_Req
		{
			public:

				std::int64_t chat_id = 0xffffffffffffffff;

#if defined(ANDROID_WRAP)
				PLAPI_DEF_CTORS_FUNCTION(FS_PutChatAvatar_Req);
#endif

				std::int32_t get_id() const final
				{
					return (std::int32_t)MessengerAPI_Req::FS_PutChatAvatar_Req;
				}
		};

		class FS_GetChatAvatar_Req final : public FS_GetFile_Req
		{
			public:

				std::int64_t chat_id = 0xffffffffffffffff;

#if defined(ANDROID_WRAP)
				PLAPI_DEF_CTORS_FUNCTION(FS_GetChatAvatar_Req);
#endif

				std::int32_t get_id() const final
				{
					return (std::int32_t)MessengerAPI_Req::FS_GetChatAvatar_Req;
				}
		};

		class FS_PutMsgFile_Req final : public FS_PutFile_Req
		{
			public:

				std::int64_t sm_id = 0xffffffffffffffff;

#if defined(ANDROID_WRAP)
				PLAPI_DEF_CTORS_FUNCTION(FS_PutMsgFile_Req);
#endif

				std::int32_t get_id() const final
				{
					return (std::int32_t)MessengerAPI_Req::FS_PutMsgFile_Req;
				}
		};

		class FS_GetMsgFile_Req final : public FS_GetFile_Req
		{
			public:

				std::int64_t sm_id = 0xffffffffffffffff;

#if defined(ANDROID_WRAP)
				PLAPI_DEF_CTORS_FUNCTION(FS_GetMsgFile_Req);
#endif

				std::int32_t get_id() const final
				{
					return (std::int32_t)MessengerAPI_Req::FS_GetMsgFile_Req;
				}
		};


		class Billing_Balance_Req final : public Function
		{
			public:

#if defined(ANDROID_WRAP)
				PLAPI_DEF_CTORS_FUNCTION(Billing_Balance_Req);
#endif

				std::int32_t get_id() const final
				{
					return (std::int32_t)MessengerAPI_Req::Billing_Balance_Req;
				}
		};







		class ConnectStatus final : public Object
		{
			public:

				std::int8_t status;

#if defined(ANDROID_WRAP)
				PLAPI_DEF_CTORS_OBJECT(ConnectStatus);
#endif

				std::int32_t get_id() const final
				{
					return (std::int32_t)MessengerAPI_Ind::ConnectStatus;
				}

		};

		class SetSubcriberInfo_Rsp final : public Object
		{
			public:

				std::uint16_t rsp = 0xffff;

#if defined(ANDROID_WRAP)
				PLAPI_DEF_CTORS_OBJECT(SetSubcriberInfo_Rsp);
#endif

				std::int32_t get_id() const final
				{
					return (std::int32_t)MessengerAPI_Ind::SetSubcriberInfo_Rsp;
				}

		};

		class Online_Rsp final : public Object
		{
			public:

				std::uint16_t rsp = 0xffff;

#if defined(ANDROID_WRAP)
				PLAPI_DEF_CTORS_OBJECT(Online_Rsp);
#endif

				std::int32_t get_id() const final
				{
					return (std::int32_t)MessengerAPI_Ind::Online_Rsp;
				}

		};

		class Offline_Rsp final : public Object
		{
			public:

				std::uint16_t rsp = 0xffff;

#if defined(ANDROID_WRAP)
				PLAPI_DEF_CTORS_OBJECT(Offline_Rsp);
#endif

				std::int32_t get_id() const final
				{
					return (std::int32_t)MessengerAPI_Ind::Offline_Rsp;
				}

		};

		class RegistrationRequired_Ind final : public Object
		{
			public:

				bool error_code = false;

#if defined(ANDROID_WRAP)
				PLAPI_DEF_CTORS_OBJECT(RegistrationRequired_Ind);
#endif

				std::int32_t get_id() const final
				{
					return (std::int32_t)MessengerAPI_Ind::RegistrationRequired_Ind;
				}

		};

		class Logout_Rsp final : public Object
		{
			public:

				std::uint16_t rsp = 0xffff;

#if defined(ANDROID_WRAP)
				PLAPI_DEF_CTORS_OBJECT(Logout_Rsp);
#endif

				std::int32_t get_id() const final
				{
					return (std::int32_t)MessengerAPI_Ind::Logout_Rsp;
				}

		};

		class LogoutAll_Rsp final : public Object
		{
			public:

				std::uint16_t rsp = 0xffff;

#if defined(ANDROID_WRAP)
				PLAPI_DEF_CTORS_OBJECT(LogoutAll_Rsp);
#endif

				std::int32_t get_id() const final
				{
					return (std::int32_t)MessengerAPI_Ind::LogoutAll_Rsp;
				}

		};
		class PushToken_Rsp final : public Object
		{
			public:

				std::uint16_t rsp = 0xffff;

#if defined(ANDROID_WRAP)
				PLAPI_DEF_CTORS_OBJECT(PushToken_Rsp);
#endif

				std::int32_t get_id() const final
				{
					return (std::int32_t)MessengerAPI_Ind::PushToken_Rsp;
				}

		};

		class GetSubsribersStatus_Rsp final : public Object
		{
			public:

				std::uint16_t rsp = 0xffff;

#if defined(ANDROID_WRAP)
				PLAPI_DEF_CTORS_OBJECT(GetSubsribersStatus_Rsp);
#endif

				std::int32_t get_id() const final
				{
					return (std::int32_t)MessengerAPI_Ind::GetSubsribersStatus_Rsp;
				}

		};
		class ContactStatus_Ind final : public Object
		{
			public:

				std::string num;

				std::uint8_t status = 0xff; 
				std::string status_data_time; 

#if defined(ANDROID_WRAP)
				PLAPI_DEF_CTORS_OBJECT(ContactStatus_Ind);
#endif

				std::int32_t get_id() const final
				{
					return (std::int32_t)MessengerAPI_Ind::ContactStatus_Ind;
				}
		};
		class ContactUserStatus_Ind final : public Object
		{
			public:

				std::string num;

				std::uint8_t user_status = 0xff; 
				std::string user_status_data_time;

#if defined(ANDROID_WRAP)
				PLAPI_DEF_CTORS_OBJECT(ContactUserStatus_Ind);
#endif

				std::int32_t get_id() const final
				{
					return (std::int32_t)MessengerAPI_Ind::ContactUserStatus_Ind;
				}
		};

		class SetUserStatus_Rsp final : public Object
		{
			public:

				std::uint16_t rsp = 0xffff;

#if defined(ANDROID_WRAP)
				PLAPI_DEF_CTORS_OBJECT(SetUserStatus_Rsp);
#endif

				std::int32_t get_id() const final
				{
					return (std::int32_t)MessengerAPI_Ind::SetUserStatus_Rsp;
				}

		};


		class Ping_Rsp final : public Object
		{
			public:

				std::uint16_t rsp = 0xffff;

#if defined(ANDROID_WRAP)
				PLAPI_DEF_CTORS_OBJECT(Ping_Rsp);
#endif

				std::int32_t get_id() const final
				{
					return (std::int32_t)MessengerAPI_Ind::Ping_Rsp;
				}

		};
		class Ping_Ind final : public Object
		{
			public:

#if defined(ANDROID_WRAP)
				PLAPI_DEF_CTORS_OBJECT(Ping_Ind);
#endif

				std::int32_t get_id() const final
				{
					return (std::int32_t)MessengerAPI_Ind::Ping_Ind;
				}

		};

		class TestMsg_Rsp final : public Object
		{
			public:

				std::uint16_t rsp = 0xffff;

				std::uint32_t req_id = 0;
				std::uint32_t len = 0;
				std::uint8_t data[PAPI_TestFragmetMaxSize];

#if defined(ANDROID_WRAP)
				PLAPI_DEF_CTORS_OBJECT(TestMsg_Rsp);
#endif

				std::int32_t get_id() const final
				{
					return (std::int32_t)MessengerAPI_Ind::TestMsg_Rsp;
				}

		};
		class TestMsg_Ind final : public Object
		{
			public:

				std::uint32_t req_id = 0;
				std::uint32_t len = 0;
				std::uint8_t data[PAPI_TestFragmetMaxSize];

				bool loop = false;

#if defined(ANDROID_WRAP)
				PLAPI_DEF_CTORS_OBJECT(TestMsg_Ind);
#endif

				std::int32_t get_id() const final
				{
					return (std::int32_t)MessengerAPI_Ind::TestMsg_Ind;
				}

		};

		class GetRouteDirection_Rsp final : public Object
		{
			public:

				std::uint16_t rsp = 0xffff;

				std::uint8_t direction = 0xff;

#if defined(ANDROID_WRAP)
				PLAPI_DEF_CTORS_OBJECT(GetRouteDirection_Rsp);
#endif

				std::int32_t get_id() const final
				{
					return (std::int32_t)MessengerAPI_Ind::GetRouteDirection_Rsp;
				}

		};
		class SetRouteDirection_Rsp final : public Object
		{
			public:

				std::uint16_t rsp = 0xffff;

#if defined(ANDROID_WRAP)
				PLAPI_DEF_CTORS_OBJECT(SetRouteDirection_Rsp);
#endif

				std::int32_t get_id() const final
				{
					return (std::int32_t)MessengerAPI_Ind::SetRouteDirection_Rsp;
				}

		};

		class Dial_Rsp final : public Object
		{
			public:

				std::uint16_t rsp = 0xffff;

				std::string snum;
				std::string dnum;

				std::string voice_server;
				std::string call_id;

#if defined(ANDROID_WRAP)
				PLAPI_DEF_CTORS_OBJECT(Dial_Rsp);
#endif

				std::int32_t get_id() const final
				{
					return (std::int32_t)MessengerAPI_Ind::Dial_Rsp;
				}

		};
		class Dial_Ind final : public Object
		{
			public:

				std::string snum;
				std::string dnum;

				std::string voice_server;

				std::string call_id;

#if defined(ANDROID_WRAP)
				PLAPI_DEF_CTORS_OBJECT(Dial_Ind);
#endif

				std::int32_t get_id() const final
				{
					return (std::int32_t)MessengerAPI_Ind::Dial_Ind;
				}

		};
		class Cancel_Rsp final : public Object
		{
			public:

				std::uint16_t rsp = 0xffff;

				std::string snum;
				std::string dnum;


				std::string call_id;

#if defined(ANDROID_WRAP)
				PLAPI_DEF_CTORS_OBJECT(Cancel_Rsp);
#endif

				std::int32_t get_id() const final
				{
					return (std::int32_t)MessengerAPI_Ind::Cancel_Rsp;
				}

		};
		class PushClearCalls_Rsp final : public Object
		{
			public:

				std::uint16_t rsp = 0xffff;

#if defined(ANDROID_WRAP)
				PLAPI_DEF_CTORS_OBJECT(PushClearCalls_Rsp);
#endif

				std::int32_t get_id() const final
				{
					return (std::int32_t)MessengerAPI_Ind::PushClearCalls_Rsp;
				}

		};
		class CallsBadge_Rsp final : public Object
		{
			public:

				std::uint16_t rsp = 0xffff;

				std::uint32_t badge = 0;

#if defined(ANDROID_WRAP)
				PLAPI_DEF_CTORS_OBJECT(CallsBadge_Rsp);
#endif

				std::int32_t get_id() const final
				{
					return (std::int32_t)MessengerAPI_Ind::CallsBadge_Rsp;
				}

		};

		class Msg_Rsp final : public Object
		{
			public:

				std::uint16_t rsp = 0xffff;

				std::uint64_t chat_id = 0xffffffffffffffff;
				std::uint64_t sm_id = 0xffffffffffffffff;
				std::uint64_t msg_index = 0xffffffffffffffff;

				std::string snum;
				std::string dnum;

				std::string submit_date_time;

				std::uint8_t channel = 0;

#if defined(ANDROID_WRAP)
				PLAPI_DEF_CTORS_OBJECT(Msg_Rsp);
#endif

				std::int32_t get_id() const final
				{
					return (std::int32_t)MessengerAPI_Ind::Msg_Rsp;
				}

		};

		class Msg_Ind final : public Object
		{
			public:

				std::uint8_t  chat_private = 0;
				std::uint64_t chat_id = 0xffffffffffffffff;
				std::uint64_t sm_id = 0xffffffffffffffff;
				std::uint64_t msg_index = 0xffffffffffffffff;

				std::string snum;
				std::string dnum;
				std::string text;

				std::string submit_date_time;

				std::uint8_t channel = 0;

				std::uint8_t file_type = 0;
				std::uint32_t file_hash = 0;

				std::vector<std::string> buttons;

#if defined(ANDROID_WRAP)
				PLAPI_DEF_CTORS_OBJECT(Msg_Ind);
#endif

				std::int32_t get_id() const final
				{
					return (std::int32_t)MessengerAPI_Ind::Msg_Ind;
				}
		};

		class Rep_Rsp final : public Object
		{
			public:

				std::uint16_t rsp = 0xffff;

				std::uint64_t chat_id = 0xffffffffffffffff;
				std::uint64_t sm_id = 0xffffffffffffffff;
				std::uint64_t msg_index = 0xffffffffffffffff;

				std::string snum;
				std::string dnum;

				std::uint8_t status = 0xff;

#if defined(ANDROID_WRAP)
			PLAPI_DEF_CTORS_OBJECT(Rep_Rsp);
#endif

			std::int32_t get_id() const final
			{
				return (std::int32_t)MessengerAPI_Ind::Rep_Rsp;
			}

		};

		class Rep_Ind final : public Object
		{
			public:

				std::uint64_t chat_id = 0xffffffffffffffff;
				std::uint64_t sm_id = 0xffffffffffffffff;
				std::uint64_t msg_index = 0xffffffffffffffff;

				std::string snum;
				std::string dnum;

				std::uint8_t status = 0xff;

				std::string date_time;

#if defined(ANDROID_WRAP)
				PLAPI_DEF_CTORS_OBJECT(Rep_Ind);
#endif

				std::int32_t get_id() const final
				{
					return (std::int32_t)MessengerAPI_Ind::Rep_Ind;
				}
		};
		class DelMsg_Rsp final : public Object
		{
			public:

				std::uint16_t rsp = 0xffff;

				std::uint64_t chat_id = 0xffffffffffffffff;
				std::uint64_t sm_id = 0xffffffffffffffff;
				std::uint64_t msg_index = 0xffffffffffffffff;

#if defined(ANDROID_WRAP)
				PLAPI_DEF_CTORS_OBJECT(DelMsg_Rsp);
#endif

				std::int32_t get_id() const final
				{
					return (std::int32_t)MessengerAPI_Ind::DelMsg_Rsp;
				}
		};
		class Typing_Ind final : public Object
		{
			public:

				std::uint64_t chat_id = 0xffffffffffffffff;

				std::string snum;

				std::uint8_t type = 0;

#if defined(ANDROID_WRAP)
				PLAPI_DEF_CTORS_OBJECT(Typing_Ind);
#endif

				std::int32_t get_id() const final
				{
					return (std::int32_t)MessengerAPI_Ind::Typing_Ind;
				}
		};

		class CS_PutMe_Rsp final : public Object
		{
			public:

				std::uint16_t rsp = 0xffff;

#if defined(ANDROID_WRAP)
				PLAPI_DEF_CTORS_OBJECT(CS_PutMe_Rsp);
#endif

				std::int32_t get_id() const final
				{
					return (std::int32_t)MessengerAPI_Ind::CS_PutMe_Rsp;
				}
		};

		class CS_GetMe_Rsp final : public Object
		{
			public:

				std::uint16_t rsp = 0xffff;

				std::string name;
				std::string avatar;

				std::uint8_t pinbot = 0xff; 

#if defined(ANDROID_WRAP)
				PLAPI_DEF_CTORS_OBJECT(CS_GetMe_Rsp);
#endif

				std::int32_t get_id() const final
				{
					return (std::int32_t)MessengerAPI_Ind::CS_GetMe_Rsp;
				}
		};

		class CS_PutContacts_Rsp final : public Object
		{
			public:

				std::uint16_t rsp = 0xffff;


				std::vector<Contact> contacts;

#if defined(ANDROID_WRAP)
				PLAPI_DEF_CTORS_OBJECT(CS_PutContacts_Rsp);
#endif

				std::int32_t get_id() const final
				{
					return (std::int32_t)MessengerAPI_Ind::CS_PutContacts_Rsp;
				}
		};

		class CS_DelContacts_Rsp final : public Object
		{
			public:

				std::uint16_t rsp = 0xffff;



#if defined(ANDROID_WRAP)
				PLAPI_DEF_CTORS_OBJECT(CS_DelContacts_Rsp);
#endif

				std::int32_t get_id() const final
				{
					return (std::int32_t)MessengerAPI_Ind::CS_DelContacts_Rsp;
				}
		};

		class CS_GetProfiles_Rsp final : public Object
		{
			public:

				std::uint16_t rsp = 0xffff;



				std::vector<Contact> contacts;
				bool last = false;


#if defined(ANDROID_WRAP)
				PLAPI_DEF_CTORS_OBJECT(CS_GetProfiles_Rsp);
#endif

				std::int32_t get_id() const final
				{
					return (std::int32_t)MessengerAPI_Ind::CS_GetProfiles_Rsp;
				}
		};

		class CS_GetContacts_Rsp final : public Object
		{
			public:

				std::uint16_t rsp = 0xffff;

				std::vector<Contact> contacts;
				bool last = false;


#if defined(ANDROID_WRAP)
				PLAPI_DEF_CTORS_OBJECT(CS_GetContacts_Rsp);
#endif

				std::int32_t get_id() const final
				{
					return (std::int32_t)MessengerAPI_Ind::CS_GetContacts_Rsp;
				}
		};


		class CS_GetContactsHistory_Rsp final : public Object
		{
			public:

				std::uint16_t rsp = 0xffff;

				std::vector<Contact> contacts;
				bool last = false;


#if defined(ANDROID_WRAP)
				PLAPI_DEF_CTORS_OBJECT(CS_GetContactsHistory_Rsp);
#endif

				std::int32_t get_id() const final
				{
					return (std::int32_t)MessengerAPI_Ind::CS_GetContactsHistory_Rsp;
				}
		};

		class Chat_CheckList_Rsp final : public Object
		{
			public:

				std::uint16_t rsp = 0xffff;

#if defined(ANDROID_WRAP)
				PLAPI_DEF_CTORS_OBJECT(Chat_CheckList_Rsp);
#endif

				std::int32_t get_id() const final
				{
					return (std::int32_t)MessengerAPI_Ind::Chat_CheckList_Rsp;
				}
		};

		class Chat_GetList_Rsp final : public Object
		{
			public:

				std::uint16_t rsp = 0xffff;

				std::vector<Chat> chats;

				std::string server_dt;

#if defined(ANDROID_WRAP)
				PLAPI_DEF_CTORS_OBJECT(Chat_GetList_Rsp);
#endif

				std::int32_t get_id() const final
				{
					return (std::int32_t)MessengerAPI_Ind::Chat_GetList_Rsp;
				}
		};

		class Chat_GetInfo_Rsp final : public Object
		{
			public:
				std::uint64_t chat_id = 0xffffffffffffffff;

				std::uint16_t rsp = 0xffff;


				std::vector<Chat> chats;

#if defined(ANDROID_WRAP)
				PLAPI_DEF_CTORS_OBJECT(Chat_GetInfo_Rsp);
#endif

				std::int32_t get_id() const final
				{
					return (std::int32_t)MessengerAPI_Ind::Chat_GetInfo_Rsp;
				}
		};

		class Chat_CreatePrivate_Rsp final : public Object
		{
			public:

				std::uint16_t rsp = 0xffff;

				std::uint64_t chat_id = 0xffffffffffffffff;
				std::uint64_t hindex = 0xffffffffffffffff;
				std::uint64_t start_msg_ibdex = 0;

				std::vector <std::string> numbers;

#if defined(ANDROID_WRAP)
				PLAPI_DEF_CTORS_OBJECT(Chat_CreatePrivate_Rsp);
#endif

				std::int32_t get_id() const final
				{
					return (std::int32_t)MessengerAPI_Ind::Chat_CreatePrivate_Rsp;
				}
		};

		class Chat_CreateGroup_Rsp final : public Object
		{
			public:
				std::uint16_t rsp = 0xffff;

				std::uint64_t chat_id = 0xffffffffffffffff;
				std::uint64_t hindex = 0xffffffffffffffff;

				std::vector <std::string> numbers;

#if defined(ANDROID_WRAP)
				PLAPI_DEF_CTORS_OBJECT(Chat_CreateGroup_Rsp);
#endif

				std::int32_t get_id() const final
				{
					return (std::int32_t)MessengerAPI_Ind::Chat_CreateGroup_Rsp;
				}
		};
		class Chat_Delete_Rsp final : public Object
		{
			public:

				std::uint16_t rsp = 0xffff;

				std::uint64_t chat_id = 0xffffffffffffffff;
				std::uint64_t hindex = 0xffffffffffffffff;

#if defined(ANDROID_WRAP)
				PLAPI_DEF_CTORS_OBJECT(Chat_Delete_Rsp);
#endif

				std::int32_t get_id() const final
				{
					return (std::int32_t)MessengerAPI_Ind::Chat_Delete_Rsp;
				}
		};

		class Chat_AddNum_Rsp final : public Object
		{
			public:
				std::uint64_t chat_id = 0xffffffffffffffff;
				std::uint64_t hindex = 0xffffffffffffffff;

				std::uint16_t rsp = 0xffff;

#if defined(ANDROID_WRAP)
				PLAPI_DEF_CTORS_OBJECT(Chat_AddNum_Rsp);
#endif

				std::int32_t get_id() const final
				{
					return (std::int32_t)MessengerAPI_Ind::Chat_AddNum_Rsp;
				}
		};

		class Chat_DelNum_Rsp final : public Object
		{
			public:

				std::uint16_t rsp = 0xffff;

				std::uint64_t chat_id = 0xffffffffffffffff;
				std::uint64_t hindex = 0xffffffffffffffff;

#if defined(ANDROID_WRAP)
				PLAPI_DEF_CTORS_OBJECT(Chat_DelNum_Rsp);
#endif

				std::int32_t get_id() const final
				{
					return (std::int32_t)MessengerAPI_Ind::Chat_DelNum_Rsp;
				}
		};

		class Chat_SetName_Rsp final : public Object
		{
			public:

				std::uint16_t rsp = 0xffff;

				std::uint64_t chat_id = 0xffffffffffffffff;
				std::uint64_t hindex = 0xffffffffffffffff;


#if defined(ANDROID_WRAP)
				PLAPI_DEF_CTORS_OBJECT(Chat_SetName_Rsp);
#endif

				std::int32_t get_id() const final
				{
					return (std::int32_t)MessengerAPI_Ind::Chat_SetName_Rsp;
				}
		};

		class Chat_SetAvatar_Rsp final : public Object
		{
			public:

				std::uint16_t rsp = 0xffff;

				std::uint64_t chat_id = 0xffffffffffffffff;
				std::uint64_t hindex = 0xffffffffffffffff;


#if defined(ANDROID_WRAP)
				PLAPI_DEF_CTORS_OBJECT(Chat_SetAvatar_Rsp);
#endif

				std::int32_t get_id() const final
				{
					return (std::int32_t)MessengerAPI_Ind::Chat_SetAvatar_Rsp;
				}
		};

		class Chat_GetMsgs_Rsp final : public Object
		{
			public:

				std::uint16_t rsp = 0xffff;

				std::uint64_t chat_id = 0xffffffffffffffff;

				std::vector<Msg> msgs;

#if defined(ANDROID_WRAP)
				PLAPI_DEF_CTORS_OBJECT(Chat_GetMsgs_Rsp);
#endif

				std::int32_t get_id() const final
				{
					return (std::int32_t)MessengerAPI_Ind::Chat_GetMsgs_Rsp;
				}
		};

		class Chat_GetMsgsDel_Rsp final : public Object
		{
			public:

				std::uint16_t rsp = 0xffff;

				std::uint64_t chat_id = 0xffffffffffffffff;

				std::vector<Msg> msgs;

#if defined(ANDROID_WRAP)
				PLAPI_DEF_CTORS_OBJECT(Chat_GetMsgsDel_Rsp);
#endif

				std::int32_t get_id() const final
				{
					return (std::int32_t)MessengerAPI_Ind::Chat_GetMsgsDel_Rsp;
				}
		};

		class Chat_GetMsgsRepFinal_Rsp final : public Object
		{
			public:

				std::uint16_t rsp = 0xffff;

				std::uint64_t chat_id = 0xffffffffffffffff;

				std::vector<Msg> msgs;

#if defined(ANDROID_WRAP)
				PLAPI_DEF_CTORS_OBJECT(Chat_GetMsgsRepFinal_Rsp);
#endif

				std::int32_t get_id() const final
				{
					return (std::int32_t)MessengerAPI_Ind::Chat_GetMsgsRepFinal_Rsp;
				}
		};

		class Chat_GetMsgsRepRead_Rsp final : public Object
		{
			public:

				std::uint16_t rsp = 0xffff;

				std::uint64_t chat_id = 0xffffffffffffffff;

				std::vector<Msg> msgs;

#if defined(ANDROID_WRAP)
				PLAPI_DEF_CTORS_OBJECT(Chat_GetMsgsRepRead_Rsp);
#endif

				std::int32_t get_id() const final
				{
					return (std::int32_t)MessengerAPI_Ind::Chat_GetMsgsRepRead_Rsp;
				}
		};

		class Chat_ChangeInfo_Ind final : public Object
		{
			public:

				std::uint64_t index = 0xffffffffffffffff;
				std::uint64_t chat_id = 0xffffffffffffffff;
				std::uint8_t  change_type = 0xff;

				bool chat_info = false; 
				Chat chat; 

#if defined(ANDROID_WRAP)
				PLAPI_DEF_CTORS_OBJECT(Chat_ChangeInfo_Ind);
#endif

				std::int32_t get_id() const final
				{
					return (std::int32_t)MessengerAPI_Ind::Chat_ChangeInfo_Ind;
				}
		};
		class Chat_NotRead_Rsp final : public Object
		{
			public:

				std::uint16_t rsp = 0xffff;

				std::uint64_t chat_id = 0xffffffffffffffff;

				std::uint32_t not_read = 0;

#if defined(ANDROID_WRAP)
				PLAPI_DEF_CTORS_OBJECT(Chat_NotRead_Rsp);
#endif

				std::int32_t get_id() const final
				{
					return (std::int32_t)MessengerAPI_Ind::Chat_NotRead_Rsp;
				}
		};
		class Chat_ReadAll_Rsp final : public Object
		{
			public:

				std::uint16_t rsp = 0xffff; 

				std::uint64_t chat_id = 0xffffffffffffffff;

#if defined(ANDROID_WRAP)
				PLAPI_DEF_CTORS_OBJECT(Chat_ReadAll_Rsp);
#endif

				std::int32_t get_id() const final
				{
					return (std::int32_t)MessengerAPI_Ind::Chat_ReadAll_Rsp;
				}
		};

		class BL_Add_Rsp final : public Object
		{
			public:

				std::uint16_t rsp = 0xffff;

#if defined(ANDROID_WRAP)
				PLAPI_DEF_CTORS_OBJECT(BL_Add_Rsp);
#endif

				std::int32_t get_id() const final
				{
					return (std::int32_t)MessengerAPI_Ind::BL_Add_Rsp;
				}
		};

		class BL_Del_Rsp final : public Object
		{
			public:

				std::uint16_t rsp = 0xffff;

#if defined(ANDROID_WRAP)
				PLAPI_DEF_CTORS_OBJECT(BL_Del_Rsp);
#endif

				std::int32_t get_id() const final
				{
					return (std::int32_t)MessengerAPI_Ind::BL_Del_Rsp;
				}
		};

		class BL_Get_Rsp final : public Object
		{
			public:

				std::uint16_t rsp = 0xffff;

				std::vector<std::string> list;

#if defined(ANDROID_WRAP)
				PLAPI_DEF_CTORS_OBJECT(BL_Get_Rsp);
#endif

				std::int32_t get_id() const final
				{
					return (std::int32_t)MessengerAPI_Ind::BL_Get_Rsp;
				}
		};



		class FS_PutAvatar_Rsp final : public FS_PutFile_Rsp 
		{
			public:






#if defined(ANDROID_WRAP)
				PLAPI_DEF_CTORS_OBJECT(FS_PutAvatar_Rsp);
#endif

				std::int32_t get_id() const final
				{
					return (std::int32_t)MessengerAPI_Ind::FS_PutAvatar_Rsp;
				}
		};

		class FS_GetAvatar_Rsp final : public FS_GetFile_Rsp 
		{
			public:










#if defined(ANDROID_WRAP)
				PLAPI_DEF_CTORS_OBJECT(FS_GetAvatar_Rsp);
#endif

				std::int32_t get_id() const final
				{
					return (std::int32_t)MessengerAPI_Ind::FS_GetAvatar_Rsp;
				}
		};

		class FS_PutChatAvatar_Rsp final : public FS_PutFile_Rsp 
		{
			public:

 				std::int64_t chat_id = 0xffffffffffffffff;

#if defined(ANDROID_WRAP)
				PLAPI_DEF_CTORS_OBJECT(FS_PutChatAvatar_Rsp);
#endif

				std::int32_t get_id() const final
				{
					return (std::int32_t)MessengerAPI_Ind::FS_PutChatAvatar_Rsp;
				}
		};

		class FS_GetChatAvatar_Rsp final : public FS_GetFile_Rsp 
		{
			public:

				std::int64_t chat_id = 0xffffffffffffffff;

#if defined(ANDROID_WRAP)
				PLAPI_DEF_CTORS_OBJECT(FS_GetChatAvatar_Rsp);
#endif

				std::int32_t get_id() const final
				{
					return (std::int32_t)MessengerAPI_Ind::FS_GetChatAvatar_Rsp;
				}
		};

		class FS_PutMsgFile_Rsp final : public FS_PutFile_Rsp
		{
			public:

 				std::int64_t sm_id = 0xffffffffffffffff;

#if defined(ANDROID_WRAP)
				PLAPI_DEF_CTORS_OBJECT(FS_PutMsgFile_Rsp);
#endif

				std::int32_t get_id() const final
				{
					return (std::int32_t)MessengerAPI_Ind::FS_PutMsgFile_Rsp;
				}
		};

		class FS_GetMsgFile_Rsp final : public FS_GetFile_Rsp
		{
			public:

				std::int64_t sm_id = 0xffffffffffffffff;

#if defined(ANDROID_WRAP)
				PLAPI_DEF_CTORS_OBJECT(FS_GetMsgFile_Rsp);
#endif

				std::int32_t get_id() const final
				{
					return (std::int32_t)MessengerAPI_Ind::FS_GetMsgFile_Rsp;
				}
		};

		class Billing_Balance_Rsp final : public Object
		{
			public:

				std::uint16_t rsp = 0xffff;

				std::string balance;

#if defined(ANDROID_WRAP)
				PLAPI_DEF_CTORS_OBJECT(Billing_Balance_Rsp);
#endif

				std::int32_t get_id() const final
				{
					return (std::int32_t)MessengerAPI_Ind::Billing_Balance_Rsp;
				}
		};

	}

}  
