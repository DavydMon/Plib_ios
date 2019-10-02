#pragma once

#include <thread>
#include <queue>
#include <map>
#include <functional>
#include <mutex>

#include "utils/port/config.h"
#include "utils/MpscPollableQueue.h"

#include "Client.h"



#define _stds_pl_d(v) (void*)v.c_str(),(std::uint32_t)v.length()
#define _stds_pl_w(v) (void*)v.c_str(),(std::uint16_t)v.length()

namespace pl
{

	using InputQueue = MpscPollableQueue<Client::Request>;
	using OutputQueue = MpscPollableQueue<Client::Indicator>;

	class Client::Impl
	{
		enum class PConnectState
		{
			offline,

			start,
			wait_connect,
			connect,
			wait_open,
			open,
			wait_reg,
			wait_reg_newreg,
			wait_reg_auth,
			need_req,
			need_auth,

			wait_disconnect,

			online,

			Max
		};

		struct RegInfoData
		{
			std::string realm;
			std::string nonce;
			std::string opaque;
			bool need_uid;
			RegInfoData()
			{
				need_uid = false;
			}
		};

		struct TimerItem
		{
			PAPI_ID id;
			std::uint32_t tick;
		};

		enum class LogLevel : std::uint8_t
		{
			Off,
			Error,
			Debug,
			Verbose
		};

		public:
#if defined(ANDROID_WRAP)
            std::function<bool(std::uint32_t)> sock_command;
            std::function<bool(const std::string &)> str_stream;
            std::function<bool(const void*, std::uint32_t)> bin_stream;
			std::function<bool(const std::string &)> log_stream;
#else
			Client::CallbackSockCommand sock_command;
			Client::CallbackStrStream str_stream;
			Client::CallbackBinStream bin_stream;
			Client::CallbackLog       log_stream;
#endif
			std::string GetStrState(PConnectState state);
			void ChangeConnectState(PConnectState state);
			PConnectState GetConnectState();
			bool TestChangeConnectState(PConnectState test, PConnectState state);

			void send(Request request);
			Indicator receive(std::uint32_t timeout);

			void OnConnect(std::string addr, std::uint16_t port, unsigned ver);
			void OnDisconnect(std::string _addr, std::uint16_t _port, unsigned ver);
			bool StrStream(const char* str, std::uint32_t len);
			bool BinStream(const unsigned char* msg, std::uint32_t len);

			bool OnRegister(std::uint32_t req_id, const unsigned char* msg, std::uint32_t len);
			bool OnRegister_RegInfo(RegInfoData &data, const unsigned char* msg, std::uint32_t len);
			bool OnRegisterAuth(std::uint32_t req_id, const unsigned char* msg, std::uint32_t len);
			bool OnRegisterReg(std::uint32_t req_id, const unsigned char* msg, std::uint32_t len);
			bool OnLogoutRsp(std::uint32_t req_id, const unsigned char* msg, std::uint32_t len);
			bool OnLogoutAllRsp(std::uint32_t req_id, const unsigned char* msg, std::uint32_t len);
			bool OnPushTokenRsp(std::uint32_t req_id, const unsigned char* msg, std::uint32_t len);
			bool OnContactStatusInd(std::uint32_t req_id, const unsigned char* msg, std::uint32_t len);
			bool OnContactUserStatusInd(std::uint32_t req_id, const unsigned char* msg, std::uint32_t len);
			bool OnGetSubsribersStatus_Rsp(std::uint32_t req_id, const unsigned char* msg, std::uint32_t len);
			bool OnSetUserStatus_Rsp(std::uint32_t req_id, const unsigned char* msg, std::uint32_t len);
			bool OnGetRouteDirection_Rsp(std::uint32_t req_id, const unsigned char* msg, std::uint32_t len);
			bool OnSetRouteDirection_Rsp(std::uint32_t req_id, const unsigned char* msg, std::uint32_t len);
			bool OnPingRsp(std::uint32_t req_id, const unsigned char* msg, std::uint32_t len);
			bool OnPingInd(std::uint32_t req_id, const unsigned char* msg, std::uint32_t len);
			bool OnPingExInd(std::uint32_t req_id, const unsigned char* msg, std::uint32_t len);
			bool OnTestMsgRsp(std::uint32_t req_id, const unsigned char* msg, std::uint32_t len);
			bool OnTestMsgInd(std::uint32_t req_id, const unsigned char* msg, std::uint32_t len);
			bool OnTestMsgExInd(std::uint32_t req_id, const unsigned char* msg, std::uint32_t len);
			bool OnDialRsp(std::uint32_t req_id, const unsigned char* msg, std::uint32_t len);
			bool OnCancelRsp(std::uint32_t req_id, const unsigned char* msg, std::uint32_t len);
			bool OnDialInd(std::uint32_t req_id, const unsigned char* msg, std::uint32_t len);
			bool OnPushClearCallsRsp(std::uint32_t req_id, const unsigned char* msg, std::uint32_t len);
			bool OnCallsBadgeRsp(std::uint32_t req_id, const unsigned char* msg, std::uint32_t len);
			bool OnMsgRsp(std::uint32_t req_id, const unsigned char* msg, std::uint32_t len);
			bool OnMsgReq(std::uint32_t req_id, const unsigned char* msg, std::uint32_t len);
			bool OnRepRsp(std::uint32_t req_id, const unsigned char* msg, std::uint32_t len);
			bool OnRepReq(std::uint32_t req_id, const unsigned char* msg, std::uint32_t len);
			bool OnDelMsgRsp(std::uint32_t req_id, const unsigned char* msg, std::uint32_t len);
			bool OnTypingReq(std::uint32_t req_id, const unsigned char* msg, std::uint32_t len);

			bool OnBL_Add_Rsp(std::uint32_t req_id, const unsigned char* msg, std::uint32_t len);
			bool OnBL_Del_Rsp(std::uint32_t req_id, const unsigned char* msg, std::uint32_t len);
			bool OnBL_Get_Rsp_List(std::vector<std::string> &list, const unsigned char* msg, std::uint32_t len);
			bool OnBL_Get_Rsp(std::uint32_t req_id, const unsigned char* msg, std::uint32_t len);

			bool OnCS_PutMe_Rsp(std::uint32_t req_id, const unsigned char* msg, std::uint32_t len);
			bool OnCS_GetMe_Rsp(std::uint32_t req_id, const unsigned char* msg, std::uint32_t len);
			bool OnCS_GetProfileRsp_Rsp(std::uint32_t req_id, const unsigned char* msg, std::uint32_t len);
			bool OnCS_GetContactsRsp_Rsp(std::uint32_t req_id, const unsigned char* msg, std::uint32_t len);
			bool OnCS_GetContactsHistoryRsp_Rsp(std::uint32_t req_id, const unsigned char* msg, std::uint32_t len);

			bool OnCS_GetProfileRsp_Rsp_Contacts(std::vector<Contact> &contacts, bool &last, unsigned char* msg, std::uint32_t len);
			bool OnCS_PutContacts_Rsp(std::uint32_t req_id, const unsigned char* msg, std::uint32_t len);
			bool OnCS_DelContacts_Rsp(std::uint32_t req_id, const unsigned char* msg, std::uint32_t len);
			bool OnChatCheckListRsp(std::uint32_t req_id, const unsigned char* msg, std::uint32_t len);
			bool OnChatGetListRsp_ListNums(std::vector<std::string> &nums, unsigned char* msg, std::uint32_t len);
			bool OnChatGetListRsp_Msgs(std::vector<pl::Msg> &msgs, unsigned char* msg, std::uint32_t len);
			bool OnChatGetListRsp_ChatInfo(Chat &chat, unsigned char* msg, std::uint32_t len);

			bool OnChatGetListRsp_List(std::vector<Chat> &chats, unsigned char* msg, std::uint32_t len);
			bool OnChatGetListRsp(std::uint32_t req_id, const unsigned char* msg, std::uint32_t len);
			bool OnChatGetInfoRsp(std::uint32_t req_id, const unsigned char* msg, std::uint32_t len);
			bool OnChatCreateRsp_Nums(std::vector<std::string> &nums, unsigned char* msg, std::uint32_t len);
			bool OnChatCreateRsp(std::uint32_t req_id, const unsigned char* msg, std::uint32_t len);
			bool OnChatDelRsp(std::uint32_t req_id, const unsigned char* msg, std::uint32_t len);
			bool OnChatAddNumRsp(std::uint32_t req_id, const unsigned char* msg, std::uint32_t len);
			bool OnChatDelNumRsp(std::uint32_t req_id, const unsigned char* msg, std::uint32_t len);
			bool OnChatSetNameRsp(std::uint32_t req_id, const unsigned char* msg, std::uint32_t len);
			bool OnChatSetAvatarRsp(std::uint32_t req_id, const unsigned char* msg, std::uint32_t len);
			bool OnChatGetMsgsRsp(std::uint32_t req_id, const unsigned char* msg, std::uint32_t len);
			bool OnChatGetMsgsDelRsp(std::uint32_t req_id, const unsigned char* msg, std::uint32_t len);
			bool OnChatGetMsgsRepFinalRsp(std::uint32_t req_id, const unsigned char* msg, std::uint32_t len);
			bool OnChatGetMsgsRepReadRsp(std::uint32_t req_id, const unsigned char* msg, std::uint32_t len);
			bool OnChatChangeInfoInd(std::uint32_t req_id, const unsigned char* msg, std::uint32_t len);
			bool OnChatNotReadRsp(std::uint32_t req_id, const unsigned char* msg, std::uint32_t len);
			bool OnChatReadAllRsp(std::uint32_t req_id, const unsigned char* msg, std::uint32_t len);
			bool OnFSPutAvaterRsp(std::uint32_t req_id, const unsigned char* msg, std::uint32_t len);
			bool OnFSGetAvaterRsp(std::uint32_t req_id, const unsigned char* msg, std::uint32_t len);
			bool OnFSPutChatAvaterRsp(std::uint32_t req_id, const unsigned char* msg, std::uint32_t len);
			bool OnFSGetChatAvaterRsp(std::uint32_t req_id, const unsigned char* msg, std::uint32_t len);
			bool OnFSPutFileRsp(std::uint32_t req_id, const unsigned char* msg, std::uint32_t len);
			bool OnFSGetFileRsp(std::uint32_t req_id, const unsigned char* msg, std::uint32_t len);
			bool OnBillingBalanceRsp(std::uint32_t req_id, const unsigned char* msg, std::uint32_t len);


			Impl();
			~Impl();

		private:
			std::shared_ptr<InputQueue> input_queue_;
			std::shared_ptr<OutputQueue> output_queue_;
			int output_queue_ready_cnt_{0};



 			std::vector<std::thread> procc;

			bool on = false;
			PConnectState constate = PConnectState::offline;
			std::string number;
			std::string addr;
			std::uint16_t port;
			unsigned ip_ver;

			std::string host_name = "salom.beeline.tj";
			std::string UID = "0268dc6f-338e-4572-a426-5cf4f8922db9";
			std::string DevID = "356440085568177";
			std::string user_agent = "desktop tester";
			std::uint8_t user_type = 0xff; 
			std::uint32_t user_version = 0xffffffff;
			std::uint8_t user_client = 0xff;
			std::uint32_t reg_expires = 3600;

			std::string code;

			std::string realm;
			std::string nonce;
			std::string opaque;
			bool need_uid = false;

			PAPI_ID Request_Online_id = 0;

			std::int8_t log_level = 0;

			std::int32_t reg_tick;
			std::queue<TimerItem> itimers;
			std::map<PAPI_ID,std::int32_t> timers;
			std::mutex timers_lock;

			bool loop();
			void TestTimers();
			void OnTimer(PAPI_ID req_id, std::int32_t command_id);

			void StartTimer(PAPI_ID req_id, std::int32_t command_id);

			void Log(LogLevel level, std::int32_t req_id, char *str, std::int32_t len);
			void LogStr(LogLevel level, std::int32_t req_id, char *str, std::int32_t len, char *vstr, std::int32_t vlen);

			void SendResp(Indicator &&rsp);

			void SetConnectStatus(PAPI_ID req_id, MessengerAPI_ConnectStatus status);

			void SendOnlineRsp(PAPI_ID req_id, std::int16_t _rsp);
			void Send_RegistrationRequired_Ind(PAPI_ID req_id,bool error_code);

			bool OnRequest(Client::Request &&req);
			void OnSetSubcriberInfo(Client::Request &&req);
			void OnRequest_Online(Client::Request &&req);
			bool OnRequest_Offline();
			void OnRequest_Offline(Client::Request &&req);
			void OnRegistrationRequired_Cnf(Client::Request &&req);
			bool OnLogout_Req(Client::Request &&req);
			bool OnLogoutAll_Req(Client::Request &&req);
			bool OnPushToken_Req(Client::Request &&req);
			bool OnGetSubsribersStatus_Req(Client::Request &&req);
			bool OnSetUserStatus_Req(Client::Request &&req);
			bool OnGetRouteDirectionReq(Client::Request &&req);
			bool OnSetRouteDirectionReq(Client::Request &&req);
			bool OnRequest_PingReq(Client::Request &&req);
			bool OnRequest_PingCnf(Client::Request &&req);
			bool OnRequest_TestMsgReq(Client::Request &&req);
			bool OnRequest_TestMsgCnf(Client::Request &&req);
			bool OnRequest_DialReq(Client::Request &&req);
			bool OnRequest_DialCnf(Client::Request &&req);
			bool OnRequest_CancelReq(Client::Request &&req);
			bool OnRequest_PushClearCallsReq(Client::Request &&req);
			bool OnRequest_CallsBadgeReq(Client::Request &&req);
			bool OnRequest_MsgReq(Client::Request &&req);
			bool OnRequest_MsgCnf(Client::Request &&req);
			bool OnRequest_RepReq(Client::Request &&req);
			bool OnRequest_RepCnf(Client::Request &&req);
			bool OnRequest_DelMsgReq(Client::Request &&req);
			bool OnRequest_TypingReq(Client::Request &&req);
			bool OnRequest_BL_Add_Req(Client::Request &&req);
			bool OnRequest_BL_Del_Req(Client::Request &&req);
			bool OnRequest_BL_Get_Req(Client::Request &&req);
			bool OnRequest_CS_PutMe(Client::Request &&req);
			bool OnRequest_CS_GetMe(Client::Request &&req);
			bool OnRequest_CS_PutContacts(Client::Request &&req);
			bool OnRequest_CS_DelContacts(Client::Request &&req);
			bool OnRequest_CS_GetProfiles(Client::Request &&req);
			bool OnRequest_CS_GetContactsHistory(Client::Request &&req);
			bool OnRequest_CS_GetContacts(Client::Request &&req);
			bool OnRequest_Chat_CheckList(Client::Request &&req);
			bool OnRequest_Chat_GetList(Client::Request &&req);
			bool OnRequest_Chat_GetInfo(Client::Request &&req);
			bool OnRequest_Chat_Create_Private(Client::Request &&req);
			bool OnRequest_Chat_Create_Group(Client::Request &&req);
			bool OnRequest_Chat_Delete(Client::Request &&req);
			bool OnRequest_Chat_AddNum(Client::Request &&req);
			bool OnRequest_Chat_DelNum(Client::Request &&req);
			bool OnRequest_Chat_SetName(Client::Request &&req);
			bool OnRequest_Chat_SetAvatar(Client::Request &&req);
			bool OnRequest_Chat_GetMsgs(Client::Request &&req);
			bool OnRequest_Chat_GetMsgsDel(Client::Request &&req);
			bool OnRequest_Chat_GetMsgsRepFinal(Client::Request &&req);
			bool OnRequest_Chat_GetMsgsRepRead(Client::Request &&req);
			bool OnRequest_Chat_NotRead(Client::Request &&req);
			bool OnRequest_Chat_ReadAllReq(Client::Request &&req);
			bool OnRequest_FS_PutAvatar(Client::Request &&req);
			bool OnRequest_FS_GetAvatar(Client::Request &&req);
			bool OnRequest_FS_PutChatAvatar(Client::Request &&req);
			bool OnRequest_FS_GetChatAvatar(Client::Request &&req);
			bool OnRequest_FS_PutFile(Client::Request &&req);
			bool OnRequest_FS_GetFile(Client::Request &&req);
			bool OnRequest_Billing_Balance(Client::Request &&req);

			bool SIP_Message_Cnf_ex(std::uint32_t req_id, std::uint64_t chat_id, std::uint64_t msg_index, std::uint64_t sm_id, std::string &snum, std::string &dnum, std::int32_t status);
			bool SIP_Report_Cnf_ex (std::uint32_t req_id, std::uint64_t chat_id, std::uint64_t msg_index, std::uint64_t sm_id, std::string &snum, std::string &dnum, std::int32_t status);


			bool GenOpen();
			bool GenRegister();
			bool GenRegisterNewReg();
			bool GenRegisterAuth();
			void GenMD5(std::uint8_t *hash, std::string digestURI);


			void Procc();

			void init();




	};

}


