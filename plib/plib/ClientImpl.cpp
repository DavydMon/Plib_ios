#ifndef LINUX
#include <windows.h>
#endif

#include <thread>
#include <deque>


#include "utils/port/config.h"
#include "utils/MpscPollableQueue.h"

#include "d-mon/dmain.h"
#include "d-mon/dfstring.h"
#include "d-mon/dptlv.h"
#include "d-mon/dptlvt.h"
#include "d-mon/dttablepoint.h"

#include "def/md5.h"


#include "sip/papi.h"

#include "ClientImpl.h"



#if defined(ANDROID_WRAP) || defined(IOS)

void Sleep(int sec)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(sec));
}

using namespace dmon;

#else






#endif

namespace pl
{



Client::Impl::Impl()
{
	init();
}

Client::Impl::~Impl()
{
	input_queue_->writer_put({ 0, nullptr });
}


std::string Client::Impl::GetStrState(PConnectState state)
{
	std::string str;

	switch (state)
	{
		case PConnectState::offline:
			str = "offline";
			break;
		case PConnectState::start:
			str = "start";
			break;
		case PConnectState::wait_connect:
			str = "wait_connect";
			break;
		case PConnectState::connect:
			str = "connect";
			break;
		case PConnectState::wait_open:
			str = "wait_open";
			break;
		case PConnectState::open:
			str = "open";
			break;
		case PConnectState::wait_reg:
			str = "wait_reg";
			break;
		case PConnectState::wait_reg_newreg:
			str = "wait_reg_newreg";
			break;
		case PConnectState::wait_reg_auth:
			str = "wait_reg_auth";
			break;
		case PConnectState::need_req:
			str = "need_req";
			break;
		case PConnectState::need_auth:
			str = "need_auth";
			break;
		case PConnectState::wait_disconnect:
			str = "wait_disconnect";
			break;
		case PConnectState::online:
			str = "online";
			break;
		default:
			str = "unknown";
			break;
	}

	return str;
}


void Client::Impl::ChangeConnectState(PConnectState state)
{
	if (log_level <= (std::int8_t)LogLevel::Verbose)
	{
		std::string str;
		str = "state: " + GetStrState(constate) + " > " + GetStrState(state);

		Log(LogLevel::Verbose, Request_Online_id, (char*)str.c_str(), str.length());
	}

	constate = state;
}

Client::Impl::PConnectState Client::Impl::GetConnectState()
{
	return constate;
}

bool Client::Impl::TestChangeConnectState(PConnectState test, PConnectState state)
{
	bool r;
	
	r = constate == test;

	if (r)
		constate = state;

	return r;
}


void Client::Impl::send(Client::Request request)
{

	if (request.function == nullptr)
	{

		Log(LogLevel::Error, request.id, _strl("Drop wrong request"));
		return;
	}

	input_queue_->writer_put(std::move(request));
}

Client::Indicator Client::Impl::receive(std::uint32_t timeout)
{














	if (!timeout)
		timeout = 10;
	while(timeout)
	{
		if (output_queue_ready_cnt_ == 0)
		{
			output_queue_ready_cnt_ = output_queue_->reader_wait_nonblock();
		}
		if (output_queue_ready_cnt_ > 0)
		{
			output_queue_ready_cnt_--;
			return output_queue_->reader_get_unsafe();
		}
		if (timeout != 0)
		{
			Sleep(10);
			if (timeout <= 10)
				timeout = 0;
			else
				timeout -= 10;
		}
	}

	return{ false, 0, nullptr };
}

void Client::Impl::Log(LogLevel level, std::int32_t req_id, char *str,std::int32_t len)
{
	if (!log_level)
		return;
	if ((std::int8_t)level > log_level)
		return;

	DFixString4096 lstr;

	lstr.AddHexDword(req_id);
	lstr.AddSpace(1);
	lstr.Add(str, len);

#if defined(ANDROID_WRAP)
	if(!log_stream)
		return;

	std::string _str(lstr, lstr.Len());
	log_stream(_str);
#else
	if (!log_stream)
		return;
	log_stream(lstr, lstr.Len());
#endif
}

void Client::Impl::LogStr(LogLevel level, std::int32_t req_id, char *str, std::int32_t len, char *vstr, std::int32_t vlen)
{
	if (!log_level)
		return;
	if ((std::int8_t)level > log_level)
		return;

	DFixString4096 lstr;

	lstr.AddHexDword(req_id);
	lstr.AddSpace(1);
	lstr.Add(str, len);
	lstr.Add(vstr, vlen);

#if defined(ANDROID_WRAP)
	if (!log_stream)
		return;
	
	std::string _str(lstr, lstr.Len());
	log_stream(_str);
#else
	if (!log_stream)
		return;
	log_stream(lstr, lstr.Len());
#endif
}

void Client::Impl::SendResp(Indicator &&rsp)
{
	output_queue_->writer_put(std::move(rsp));
}



void Client::Impl::SendOnlineRsp(PAPI_ID req_id, std::int16_t _rsp)
{
	auto rsp = pl::pl_api::make_object<pl::pl_api::Online_Rsp>();
	rsp->rsp = _rsp;
	SendResp({ true, req_id, std::move(rsp) });
}

void Client::Impl::Send_RegistrationRequired_Ind(PAPI_ID req_id, bool error_code)
{
	code.clear();
	auto req = pl::pl_api::make_object<pl::pl_api::RegistrationRequired_Ind>();
	req->error_code = error_code;
	SendResp({ true, req_id, std::move(req) });
}


void Client::Impl::SetConnectStatus(PAPI_ID req_id, MessengerAPI_ConnectStatus status)
{
	auto rsp = pl::pl_api::make_object<pl::pl_api::ConnectStatus>();
	rsp->status = std::int8_t(status);
	SendResp({ true, req_id, std::move(rsp) });
}


void Client::Impl::StartTimer(PAPI_ID req_id, std::int32_t command_id)
{
	timers_lock.lock();

	TimerItem t;
	t.id = req_id;
	t.tick = GetTickCount();
	itimers.push(std::move(t));



	timers.emplace(req_id, command_id);

	timers_lock.unlock();
}


bool Client::Impl::GenOpen()
{
	std::string str;

	str = "Open " + number;
#if defined(ANDROID_WRAP)
    return str_stream(str);
#else
	if (!str_stream(str.c_str(), (std::uint32_t)str.length()))
		return false;

	return true;
#endif
}


bool Client::Impl::SIP_Message_Cnf_ex(std::uint32_t req_id, std::uint64_t chat_id, std::uint64_t msg_index, std::uint64_t sm_id, std::string &snum, std::string &dnum, std::int32_t status)
{
	PAPI_STREAM_HEADER *header;
	tDPrimitveTLV <512> msg;

	if (!bin_stream)
		return false;
	if (GetConnectState() != PConnectState::online)
	{
		return false;
	}

	header = (PAPI_STREAM_HEADER*)msg.SetHeader(sizeof(PAPI_STREAM_HEADER));
	header->ver = 0;
	header->service = (byte)PAPI_Service::message;
	header->command_id = (word)PAPI_ComMessage::message_rsp;
	header->req_id = req_id;

	msg.AddwTwLV((word)PAPI_ParamMsg::num, _stds_pl_w(number));

	msg.AddwTwLV((word)PAPI_ParamMsg::chat_id, &chat_id, 8);
	msg.AddwTwLV((word)PAPI_ParamMsg::msg_index, &msg_index, 8);
	msg.AddwTwLV((word)PAPI_ParamMsg::sm_id, &sm_id, 8);

	msg.AddwTwLV((word)PAPI_ParamMsg::sour_num, _stds_pl_w(snum));
	msg.AddwTwLV((word)PAPI_ParamMsg::dest_num, _stds_pl_w(dnum));

	msg.AddwTwLV((word)PAPI_ParamMsg::rsp, &status, 4);

	if (!bin_stream(msg, msg.Len()))
		return false;

	return true;
}

bool Client::Impl::SIP_Report_Cnf_ex(std::uint32_t req_id, std::uint64_t chat_id, std::uint64_t msg_index, std::uint64_t sm_id, std::string &snum, std::string &dnum, std::int32_t status)
{
	PAPI_STREAM_HEADER *header;
	tDPrimitveTLV <512> msg;

	if (!bin_stream)
		return false;
	if (GetConnectState() != PConnectState::online)
	{
		return false;
	}

	header = (PAPI_STREAM_HEADER*)msg.SetHeader(sizeof(PAPI_STREAM_HEADER));
	header->ver = 0;
	header->service = (byte)PAPI_Service::message;
	header->command_id = (word)PAPI_ComMessage::report_rsp;
	header->req_id = req_id;

	msg.AddwTwLV((word)PAPI_ParamMsg::num, _stds_pl_w(number));

	msg.AddwTwLV((word)PAPI_ParamMsg::chat_id, &chat_id, 8);
	msg.AddwTwLV((word)PAPI_ParamMsg::msg_index, &msg_index, 8);
	msg.AddwTwLV((word)PAPI_ParamMsg::sm_id, &sm_id, 8);

	msg.AddwTwLV((word)PAPI_ParamMsg::sour_num, _stds_pl_w(snum));
	msg.AddwTwLV((word)PAPI_ParamMsg::dest_num, _stds_pl_w(dnum));

	msg.AddwTwLV((word)PAPI_ParamMsg::rsp, &status, 4);

	if (!bin_stream(msg, msg.Len()))
		return false;

	return true;
}


bool Client::Impl::GenRegister()
{
	PAPI_STREAM_HEADER *header;
	tDPrimitveTLV <512> msg;

	if (!bin_stream)
	{
		Log(LogLevel::Error, Request_Online_id, _strl("GenRegister. error stream"));
		return false;
	}

	Log(LogLevel::Verbose, Request_Online_id, _strl("GenRegister"));

	header = (PAPI_STREAM_HEADER*)msg.SetHeader(sizeof(PAPI_STREAM_HEADER));
	header->ver = 0;
	header->service = (byte)PAPI_Service::registrar;
	header->command_id = (word)PAPI_ComRegistrar::init_req;
	header->req_id = (unsigned long)Request_Online_id;

	msg.AddwTwLV((word)PAPI_ParamReg::num, _stds_pl_w(number));

	if (!bin_stream(msg, msg.Len()))
	{
		Log(LogLevel::Error, Request_Online_id, _strl("GenRegister. error send"));
		return false;
	}

	return true;
}

bool Client::Impl::GenRegisterNewReg()
{
	PAPI_STREAM_HEADER *header;
	tDPrimitveTLV <4096> msg;
	DPrimitveTLV _data;
	std::string str;

	if (!bin_stream)
	{
		Log(LogLevel::Error, Request_Online_id, _strl("GenRegisterNewReg. error stream"));
		return false;
	}

	Log(LogLevel::Verbose, Request_Online_id, _strl("GenRegisterNewReg"));

	header = (PAPI_STREAM_HEADER*)msg.SetHeader(sizeof(PAPI_STREAM_HEADER));
	header->ver = 0;
	header->service = (byte)PAPI_Service::registrar;
	header->command_id = (word)PAPI_ComRegistrar::reg_req;
	header->req_id = (unsigned long)Request_Online_id;

	msg.AddwTwLV((word)PAPI_ParamReg::num, _stds_pl_w(number));
	msg.AddwTwLV((word)PAPI_ParamReg::uid, _stds_pl_w(UID));
	msg.AddwTwLV((word)PAPI_ParamReg::dev_uid, _stds_pl_w(DevID));

	_data.Init(msg.GetPS(), msg.EmptySize(), 4);

	_data.AddwTwLV((word)PAPI_ParamRegInfo::code, _stds_pl_w(code));

	if (user_agent.length())
		_data.AddwTwLV((word)PAPI_ParamRegInfo::user_agent, _stds_pl_w(user_agent));
	if (user_type != 0xff)
		_data.AddwTwLV((word)PAPI_ParamRegInfo::user_type, &user_type, 1);
	if (user_version != 0xffffffff)
		_data.AddwTwLV((word)PAPI_ParamRegInfo::user_ver, &user_version, 4);
	if (user_client != 0xff)
		_data.AddwTwLV((word)PAPI_ParamRegInfo::user_client, &user_client, 1);

	msg.AddWord((word)PAPI_ParamReg::reg_info);
	msg.AddWord((word)_data.Len());
	msg.Seek(_data.Len());

	if (!bin_stream(msg, msg.Len()))
	{
		Log(LogLevel::Error, Request_Online_id, _strl("GenRegisterNewReg. error send"));
		return false;
	}

	code.clear();

	return true;
}

void Client::Impl::GenMD5(std::uint8_t *hash, std::string digestURI)
{
	MD5_CTX md5;
	std::string _h1, _h2, _hash;
	byte h1[16];
	byte h2[16];
	DFixString256 d_h1, d_h2;
	std::string method = "REGISTER";
	std::string password = UID;


	_h1 = number + ":" + realm + ":" + password;
	_h2 = method + ":" + digestURI;

	MD5Init(&md5);
	MD5Update(&md5, (unsigned char*)_stds_pl_d(_h1));
	MD5Final(h1, &md5);

	MD5Init(&md5);
	MD5Update(&md5, (unsigned char*)_stds_pl_d(_h2));
	MD5Final(h2, &md5);

	d_h1.AddDumpByteA(h1, 16, 0);
	d_h2.AddDumpByteA(h2, 16, 0);

	_hash.assign(_str_pl(d_h1));
	_hash += ":" + nonce + ":";
	_hash.append(_str_pl(d_h2));

	MD5Init(&md5);
	MD5Update(&md5, (unsigned char*)_stds_pl_d(_hash));
	MD5Final(hash, &md5);
}

bool Client::Impl::GenRegisterAuth()
{
	PAPI_STREAM_HEADER *header;
	tDPrimitveTLV <4096> msg;
	DPrimitveTLV _data;
	std::string str;
	std::string username = number;
	DFixString256 d_hash;
	std::string digestURI = "sips:" + host_name + ";transport=tls;lr";


	if (!bin_stream)
	{
		Log(LogLevel::Error, Request_Online_id, _strl("GenRegisterAuth. error stream"));
		return false;
	}

	Log(LogLevel::Verbose, Request_Online_id, _strl("GenRegisterAuth"));

	header = (PAPI_STREAM_HEADER*)msg.SetHeader(sizeof(PAPI_STREAM_HEADER));
	header->ver = 0;
	header->service = (byte)PAPI_Service::registrar;
	header->command_id = (word)PAPI_ComRegistrar::auth_req;
	header->req_id = (unsigned long)Request_Online_id;

	msg.AddwTwLV((word)PAPI_ParamReg::num, _stds_pl_w(number));

	if (need_uid)
		msg.AddwTwLV((word)PAPI_ParamReg::uid, _stds_pl_w(UID));

	
	
	

	{




		byte hash[16];

























		GenMD5(hash, digestURI);

		d_hash.AddDumpByteA(hash, 16, 0);
	}

	_data.Init(msg.GetPS(), msg.EmptySize(), 4);

	_data.AddwTwLV((word)PAPI_ParamRegInfo::realm, _stds_pl_w(realm));
	_data.AddwTwLV((word)PAPI_ParamRegInfo::nonce, _stds_pl_w(nonce));
	_data.AddwTwLV((word)PAPI_ParamRegInfo::opaque, _stds_pl_w(opaque));
	_data.AddwTwLV((word)PAPI_ParamRegInfo::digestURI, _stds_pl_w(digestURI));
	_data.AddwTwLV((word)PAPI_ParamRegInfo::algorithm, (void*)_strl("md5")); 
	_data.AddwTwLV((word)PAPI_ParamRegInfo::resp, _str_plw(d_hash));

	if(user_agent.length())
		_data.AddwTwLV((word)PAPI_ParamRegInfo::user_agent, _stds_pl_w(user_agent));
	if(user_type != 0xff)
		_data.AddwTwLV((word)PAPI_ParamRegInfo::user_type, &user_type,1);
	if (user_version != 0xffffffff)
		_data.AddwTwLV((word)PAPI_ParamRegInfo::user_ver, &user_version, 4);
	if (user_client != 0xff)
		_data.AddwTwLV((word)PAPI_ParamRegInfo::user_client, &user_client, 1);

	msg.AddWord((word)PAPI_ParamReg::reg_info);
	msg.AddWord((word)_data.Len());
	msg.Seek(_data.Len());

	if (!bin_stream(msg, msg.Len()))
	{
		Log(LogLevel::Error, Request_Online_id, _strl("GenRegisterAuth. error send"));
		return false;
	}

	need_uid = false;

	return true;
}

void Client::Impl::OnSetSubcriberInfo(Client::Request &&req)
{
	Log(LogLevel::Verbose, Request_Online_id, _strl("OnSetSubcriberInfo"));






 	auto rr = req.function.release();
	auto r = (pl_api::SetSubcriberInfo_Req*)rr;
	number = r->number;
	host_name = r->host;

	UID = r->UID;
	DevID = r->DevID;
	if(r->user_agent.length())
		user_agent = r->user_agent;
	user_type = r->user_type;
	user_version = r->user_version;
	user_client = r->user_client;
	if(r->reg_expires)
		reg_expires = r->reg_expires;

	if (r->code.length())
		code = r->code;

	log_level = r->log_level;

	auto rsp = pl::pl_api::make_object<pl::pl_api::SetSubcriberInfo_Rsp>();
	rsp->rsp = 0;



	SendResp({ true, req.id, std::move(rsp)});
}


void Client::Impl::OnRequest_Online(Client::Request &&req)
{
	Log(LogLevel::Verbose,req.id,_strl("Online_Req"));

	switch (GetConnectState())
	{
		case PConnectState::offline:
		case PConnectState::wait_disconnect:
		{
			if(number.empty())
			{
				Log(LogLevel::Error, req.id, _strl("Online_Req error. user number is empty"));
				auto _rsp = pl::pl_api::make_object<pl::pl_api::Online_Rsp>();
				_rsp->rsp = 0xfff1;
				SendResp({ true, req.id, std::move(_rsp) });
				return;
			}
			if (Request_Online_id)
			{
				Log(LogLevel::Error, req.id, _strl("Online_Req error. previous request not processed yet"));

			}
			Request_Online_id = req.id;
			on = true;
			ChangeConnectState(PConnectState::start);
			SetConnectStatus(Request_Online_id, MessengerAPI_ConnectStatus::connecting);
			break;
		}
	}
}

bool Client::Impl::OnRequest_Offline()
{
	on = false;
	switch (GetConnectState())
	{
		case PConnectState::offline:
			Log(LogLevel::Debug, Request_Online_id, _strl("Offline warning. state = offline"));
			break;




		default:
		{
			reg_tick = GetTickCount();
			ChangeConnectState(PConnectState::wait_disconnect);
			if (!sock_command((std::uint32_t)Client::SockCommand::disconnect))
			{
				return false;
			}
		}
	}
	return true;
}

void Client::Impl::OnRequest_Offline(Client::Request &&req)
{
	std::uint32_t _rsp = 0;

	Log(LogLevel::Verbose, req.id, _strl("Offline_Req"));

	if (!OnRequest_Offline())
	{
		Log(LogLevel::Error, Request_Online_id, _strl("Offline_Req error. sock"));
		_rsp = 0xfff0;
	}

	Request_Online_id = 0;

	auto rsp = pl::pl_api::make_object<pl::pl_api::Offline_Rsp>();
	rsp->rsp = _rsp;
	SendResp({ true, req.id, std::move(rsp) });
}

void Client::Impl::OnRegistrationRequired_Cnf(Client::Request &&req)
{



	auto rr = req.function.release();
	auto r = (pl_api::RegistrationRequired_Cnf*)rr;

	code = r->code;





}


bool Client::Impl::OnLogout_Req(Client::Request &&req)
{
	PAPI_STREAM_HEADER *header;
	tDPrimitveTLV <65535> msg;

	bool result = false;

	while (true)
	{
		if (!bin_stream)
		{
			Log(LogLevel::Error, req.id, _strl("OnLogout_Req. error stream"));
			break;
		}
		if (GetConnectState() != PConnectState::online)
		{
			Log(LogLevel::Error, req.id, _strl("OnLogout_Req. error connect"));
			break;
		}

		Log(LogLevel::Verbose, req.id, _strl("OnLogout_Req"));

		auto r = move_tl_object_as<pl_api::Logout_Req>(req.function);

		header = (PAPI_STREAM_HEADER*)msg.SetHeader(sizeof(PAPI_STREAM_HEADER));
		header->ver = 0;
		header->service = (byte)PAPI_Service::registrar;
		header->command_id = (word)PAPI_ComRegistrar::logout_req;
		header->req_id = (unsigned long)req.id;

		msg.AddwTwLV((word)PAPI_ParamReg::num, _stds_pl_w(number));
		msg.AddwTwLV((word)PAPI_ParamReg::uid, _stds_pl_w(UID));
		msg.AddwTwLV((word)PAPI_ParamReg::dev_uid, _stds_pl_w(DevID));

		if (!bin_stream(msg, msg.Len()))
		{
			Log(LogLevel::Error, req.id, _strl("OnLogout_Req. error send"));
			break;
		}

		StartTimer(req.id, (uint32_t)MessengerAPI_Req::Logout_Req);

		result = true;
		break;
	}

	if (!result)
	{
		Log(LogLevel::Verbose, req.id, _strl("Send Logout_Rsp"));

		auto _rsp = pl::pl_api::make_object<pl::pl_api::Logout_Rsp>();
		_rsp->rsp = 0xfff0;
		SendResp({ true, req.id, std::move(_rsp) });
	}

	return true;
}

bool Client::Impl::OnLogoutAll_Req(Client::Request &&req)
{
	PAPI_STREAM_HEADER *header;
	tDPrimitveTLV <65535> msg;
	bool result = false;

	while (true)
	{
		if (!bin_stream)
		{
			Log(LogLevel::Error, req.id, _strl("OnLogoutAll_Req. error stream"));
			break;
		}
		if (GetConnectState() != PConnectState::online)
		{
			Log(LogLevel::Error, req.id, _strl("OnLogoutAll_Req. error connect"));
			break;
		}

		Log(LogLevel::Verbose, req.id, _strl("OnLogoutAll_Req"));

		auto r = move_tl_object_as<pl_api::LogoutAll_Req>(req.function);


		header = (PAPI_STREAM_HEADER*)msg.SetHeader(sizeof(PAPI_STREAM_HEADER));
		header->ver = 0;
		header->service = (byte)PAPI_Service::registrar;
		header->command_id = (word)PAPI_ComRegistrar::logout_all_req;
		header->req_id = (unsigned long)req.id;

		msg.AddwTwLV((word)PAPI_ParamReg::num, _stds_pl_w(number));
		msg.AddwTwLV((word)PAPI_ParamReg::uid, _stds_pl_w(UID));
		msg.AddwTwLV((word)PAPI_ParamReg::dev_uid, _stds_pl_w(DevID));

		if (!bin_stream(msg, msg.Len()))
		{
			Log(LogLevel::Error, req.id, _strl("OnLogoutAll_Req. error send"));
			break;
		}

		StartTimer(req.id, (uint32_t)MessengerAPI_Req::LogoutAll_Req);

		result = true;
		break;
	}

	if (!result)
	{
		Log(LogLevel::Verbose, req.id, _strl("Send LogoutAll_Rsp"));

		auto _rsp = pl::pl_api::make_object<pl::pl_api::LogoutAll_Rsp>();
		_rsp->rsp = 0xfff0;
		SendResp({ true, req.id, std::move(_rsp) });
	}

	return true;
}


bool Client::Impl::OnPushToken_Req(Client::Request &&req)
{
	PAPI_STREAM_HEADER *header;
	tDPrimitveTLV <65535> msg;
	bool result = false;

	while (true)
	{
		if (!bin_stream)
		{
			Log(LogLevel::Error, req.id, _strl("OnPushToken_Req. error stream"));
			break;
		}
		if (GetConnectState() != PConnectState::online)
		{
			Log(LogLevel::Error, req.id, _strl("OnPushToken_Req. error connect"));
			break;
		}

		Log(LogLevel::Verbose, req.id, _strl("OnPushToken_Req"));

		auto r = move_tl_object_as<pl_api::PushToken_Req>(req.function);

		header = (PAPI_STREAM_HEADER*)msg.SetHeader(sizeof(PAPI_STREAM_HEADER));
		header->ver = 0;
		header->service = (byte)PAPI_Service::registrar;
		header->command_id = (word)PAPI_ComRegistrar::push_token_req;
		header->req_id = (unsigned long)req.id;

		msg.AddwTwLV((word)PAPI_ParamReg::num, _stds_pl_w(number));
	
		if(r->token.length())
			msg.AddwTwLV((word)PAPI_ParamReg::token, _stds_pl_w(r->token));
		if (r->token_voip.length())
			msg.AddwTwLV((word)PAPI_ParamReg::token_voip, _stds_pl_w(r->token_voip));

		if (!bin_stream(msg, msg.Len()))
		{
			Log(LogLevel::Error, req.id, _strl("OnPushToken_Req. error send"));
			break;
		}

		StartTimer(req.id, (uint32_t)MessengerAPI_Req::PushToken_Req);

		result = true;
		break;
	}

	if (!result)
	{
		Log(LogLevel::Verbose, req.id, _strl("Send PushToken_Rsp"));

		auto _rsp = pl::pl_api::make_object<pl::pl_api::PushToken_Rsp>();
		_rsp->rsp = 0xfff0;
		SendResp({ true, req.id, std::move(_rsp) });
	}

	return true;
}


bool Client::Impl::OnGetSubsribersStatus_Req(Client::Request &&req)
{
	PAPI_STREAM_HEADER *header;
	tDPrimitveTLV <65535> msg;
	bool result = false;

	while (true)
	{
		if (!bin_stream)
		{
			Log(LogLevel::Error, req.id, _strl("OnGetSubsribersStatus_Req. error stream"));
			break;
		}
		if (GetConnectState() != PConnectState::online)
		{
			Log(LogLevel::Error, req.id, _strl("OnGetSubsribersStatus_Req. error connect"));
			break;
		}

		Log(LogLevel::Verbose, req.id, _strl("OnGetSubsribersStatus_Req"));


		auto r = move_tl_object_as<pl_api::GetSubsribersStatus_Req>(req.function);

		header = (PAPI_STREAM_HEADER*)msg.SetHeader(sizeof(PAPI_STREAM_HEADER));
		header->ver = 0;
		header->service = (byte)PAPI_Service::registrar;
		header->command_id = (word)PAPI_ComRegistrar::get_susbs_status_req;
		header->req_id = (unsigned long)req.id;

		msg.AddwTwLV((word)PAPI_ParamReg::num, _stds_pl_w(number));

		if (!bin_stream(msg, msg.Len()))
		{
			Log(LogLevel::Error, req.id, _strl("OnGetSubsribersStatus_Req. error send"));
			break;
		}

		StartTimer(req.id, (uint32_t)MessengerAPI_Req::GetSubsribersStatus_Req);

		result = true;
		break;
	}

	if (!result)
	{
		Log(LogLevel::Verbose, req.id, _strl("Send GetSubsribersStatus_Rsp"));

		auto _rsp = pl::pl_api::make_object<pl::pl_api::GetSubsribersStatus_Rsp>();
		_rsp->rsp = 0xfff0;
		SendResp({ true, req.id, std::move(_rsp) });
	}

	return true;
}

bool Client::Impl::OnSetUserStatus_Req(Client::Request &&req)
{
	PAPI_STREAM_HEADER *header;
	tDPrimitveTLV <65535> msg;
	bool result = false;

	while (true)
	{
		if (!bin_stream)
		{
			Log(LogLevel::Error, req.id, _strl("OnSetUserStatus_Req. error stream"));
			break;
		}
		if (GetConnectState() != PConnectState::online)
		{
			Log(LogLevel::Error, req.id, _strl("OnSetUserStatus_Req. error connect"));
			break;
		}

		Log(LogLevel::Verbose, req.id, _strl("OnSetUserStatus_Req"));


		auto r = move_tl_object_as<pl_api::SetUserStatus_Req>(req.function);

		header = (PAPI_STREAM_HEADER*)msg.SetHeader(sizeof(PAPI_STREAM_HEADER));
		header->ver = 0;
		header->service = (byte)PAPI_Service::registrar;
		header->command_id = (word)PAPI_ComRegistrar::set_user_status_req;
		header->req_id = (unsigned long)req.id;

		msg.AddwTwLV((word)PAPI_ParamReg::num, _stds_pl_w(number));
	
		msg.AddwTwLV((word)PAPI_ParamReg::status, &r->status,1);

		if (!bin_stream(msg, msg.Len()))
		{
			Log(LogLevel::Error, req.id, _strl("OnSetUserStatus_Req. error send"));
			break;
		}

		StartTimer(req.id, (uint32_t)MessengerAPI_Req::SetUserStatus_Req);

		result = true;
		break;
	}

	if (!result)
	{
		Log(LogLevel::Verbose, req.id, _strl("Send SetUserStatus_Rsp"));

		auto _rsp = pl::pl_api::make_object<pl::pl_api::SetUserStatus_Rsp>();
		_rsp->rsp = 0xfff0;
		SendResp({ true, req.id, std::move(_rsp) });
	}

	return true;
}


bool Client::Impl::OnGetRouteDirectionReq(Client::Request &&req)
{
	PAPI_STREAM_HEADER *header;
	tDPrimitveTLV <512> msg;
	bool result = false;

	while (true)
	{
		if (!bin_stream)
		{
			Log(LogLevel::Error, req.id, _strl("OnGetRouteDirectionReq. error stream"));
			break;
		}
		if (GetConnectState() != PConnectState::online)
		{
			Log(LogLevel::Error, req.id, _strl("OnGetRouteDirectionReq. error connect"));
			break;
		}

		Log(LogLevel::Verbose, req.id, _strl("OnGetRouteDirectionReq"));


		auto r = move_tl_object_as<pl_api::GetRouteDirection_Req>(req.function);

		header = (PAPI_STREAM_HEADER*)msg.SetHeader(sizeof(PAPI_STREAM_HEADER));
		header->ver = 0;
		header->service = (byte)PAPI_Service::registrar;
		header->command_id = (word)PAPI_ComRegistrar::get_route_direction_req;
		header->req_id = (unsigned long)req.id;

		msg.AddwTwLV((word)PAPI_ParamReg::num, _stds_pl_w(number));

		if (!bin_stream(msg, msg.Len()))
		{
			Log(LogLevel::Error, req.id, _strl("OnGetRouteDirectionReq. error send"));
			break;
		}

		StartTimer(req.id, (uint32_t)MessengerAPI_Req::GetRouteDirection_Req);

		result = true;
		break;
	}

	if (!result)
	{
		Log(LogLevel::Verbose, req.id, _strl("Send GetRouteDirection_Rsp"));

		auto _rsp = pl::pl_api::make_object<pl::pl_api::GetRouteDirection_Rsp>();
		_rsp->rsp = 0xfff0;
		SendResp({ true, req.id, std::move(_rsp) });
	}

	return true;
}

bool Client::Impl::OnSetRouteDirectionReq(Client::Request &&req)
{
	PAPI_STREAM_HEADER *header;
	tDPrimitveTLV <512> msg;
	bool result = false;

	while (true)
	{
		if (!bin_stream)
		{
			Log(LogLevel::Error, req.id, _strl("OnSetRouteDirectionReq. error stream"));
			break;
		}
		if (GetConnectState() != PConnectState::online)
		{
			Log(LogLevel::Error, req.id, _strl("OnSetRouteDirectionReq. error connect"));
			break;
		}

		Log(LogLevel::Verbose, req.id, _strl("OnSetRouteDirectionReq"));


		auto r = move_tl_object_as<pl_api::SetRouteDirection_Req>(req.function);

		header = (PAPI_STREAM_HEADER*)msg.SetHeader(sizeof(PAPI_STREAM_HEADER));
		header->ver = 0;
		header->service = (byte)PAPI_Service::registrar;
		header->command_id = (word)PAPI_ComRegistrar::set_route_direction_req;
		header->req_id = (unsigned long)req.id;

		msg.AddwTwLV((word)PAPI_ParamReg::num, _stds_pl_w(number));

		msg.AddwTwLV((word)PAPI_ParamReg::direction, &r->direction,1);

		if (!bin_stream(msg, msg.Len()))
		{
			Log(LogLevel::Error, req.id, _strl("OnSetRouteDirectionReq. error send"));
			break;
		}

		StartTimer(req.id, (uint32_t)MessengerAPI_Req::SetRouteDirection_Req);

		result = true;
		break;
	}

	if (!result)
	{
		Log(LogLevel::Verbose, req.id, _strl("Send SetRouteDirection_Rsp"));

		auto _rsp = pl::pl_api::make_object<pl::pl_api::SetRouteDirection_Rsp>();
		_rsp->rsp = 0xfff0;
		SendResp({ true, req.id, std::move(_rsp) });
	}

	return true;
}


bool Client::Impl::OnRequest_PingReq(Client::Request &&req)
{
	PAPI_STREAM_HEADER *header;
	tDPrimitveTLV <512> msg;
	bool result = false;

	while (true)
	{
		if (!bin_stream)
		{
			Log(LogLevel::Error, req.id, _strl("OnRequest_PingReq. error stream"));
			break;
		}
		Client::Impl::PConnectState state = GetConnectState();
		if ((state == PConnectState::offline) || (state == PConnectState::start) || (state == PConnectState::wait_connect) || (state == PConnectState::wait_disconnect) || (state == PConnectState::connect) || (state == PConnectState::wait_open))
		{
			Log(LogLevel::Error, req.id, _strl("OnRequest_PingReq. error connect"));
			break;
		}


		Log(LogLevel::Verbose, req.id, _strl("OnRequest_PingReq"));


		auto r = move_tl_object_as<pl_api::Ping_Req>(req.function);

		header = (PAPI_STREAM_HEADER*)msg.SetHeader(sizeof(PAPI_STREAM_HEADER));
		header->ver = 0;
		header->service = (byte)PAPI_Service::service;
		header->command_id = (word)PAPI_ComService::ping_req;
		header->req_id = (unsigned long)req.id;

 		msg.AddwTwLV((word)PAPI_ParamService::num, _stds_pl_w(number));

		if (!bin_stream(msg, msg.Len()))
		{
			Log(LogLevel::Error, req.id, _strl("OnRequest_PingReq. error send"));
			break;
		}

 		StartTimer(req.id, (uint32_t)MessengerAPI_Req::Ping_Req);

		result = true;
		break;
	}

	if (!result)
	{
		Log(LogLevel::Verbose, req.id, _strl("Send Ping_Rsp"));

		auto _rsp = pl::pl_api::make_object<pl::pl_api::Ping_Rsp>();
		_rsp->rsp = 0xfff0;
		SendResp({ true, req.id, std::move(_rsp) });
	}

	return true;
}

bool Client::Impl::OnRequest_PingCnf(Client::Request &&req)
{
	PAPI_STREAM_HEADER *header;
	tDPrimitveTLV <512> msg;

	if (!bin_stream)
	{
		Log(LogLevel::Error, req.id, _strl("OnRequest_PingCnf. error stream"));
		return false;
	}
	Client::Impl::PConnectState state = GetConnectState();
	if ((state == PConnectState::offline) || (state == PConnectState::start) || (state == PConnectState::wait_connect) || (state == PConnectState::wait_disconnect) || (state == PConnectState::connect) || (state == PConnectState::wait_open))
	{
		Log(LogLevel::Error, req.id, _strl("OnRequest_PingCnf. error connect"));
		return false;
	}

	Log(LogLevel::Verbose, req.id, _strl("OnRequest_PingCnf"));


	auto r = move_tl_object_as<pl_api::Ping_Cnf>(req.function);


	header = (PAPI_STREAM_HEADER*)msg.SetHeader(sizeof(PAPI_STREAM_HEADER));
	header->ver = 0;
	header->service = (byte)PAPI_Service::service;
	header->command_id = (word)PAPI_ComService::ping_rsp;
	header->req_id = (unsigned long)req.id;

 	msg.AddwTwLV((word)PAPI_ParamService::num, _stds_pl_w(number));


	if (!bin_stream(msg, msg.Len()))
	{
		Log(LogLevel::Error, req.id, _strl("OnRequest_PingCnf. error send"));
		return false;
	}

	return true;
}

bool Client::Impl::OnRequest_TestMsgReq(Client::Request &&req)
{
	PAPI_STREAM_HEADER *header;
	tDPrimitveTLV <65535> msg;
	bool result = false;

	while (true)
	{
		if (!bin_stream)
		{
			Log(LogLevel::Error, req.id, _strl("OnRequest_TestMsgReq. error stream"));
			break;
		}
		Client::Impl::PConnectState state = GetConnectState();
		if ((state == PConnectState::offline) || (state == PConnectState::start) || (state == PConnectState::wait_connect) || (state == PConnectState::wait_disconnect) || (state == PConnectState::connect) || (state == PConnectState::wait_open))
		{
			Log(LogLevel::Error, req.id, _strl("OnRequest_TestMsgReq. error connect"));
			break;
		}





		auto r = move_tl_object_as<pl_api::TestMsg_Req>(req.function);

		header = (PAPI_STREAM_HEADER*)msg.SetHeader(sizeof(PAPI_STREAM_HEADER));
		header->ver = 0;
		header->service = (byte)PAPI_Service::service;
		header->command_id = (word)PAPI_ComService::test_msg_req;
		header->req_id = (unsigned long)req.id;

 		msg.AddwTwLV((word)PAPI_ParamService::num, _stds_pl_w(number));

		msg.AddwTwLV((word)PAPI_ParamService::id, &r->req_id, 4);
		msg.AddwTwLV((word)PAPI_ParamService::loop, &r->loop, 1);

		if(r->len)
			msg.AddwTwLV((word)PAPI_ParamService::data, r->data, r->len);

		if (!bin_stream(msg, msg.Len()))
		{
			Log(LogLevel::Error, req.id, _strl("OnRequest_TestMsgReq. error send"));
			break;
		}

 		StartTimer(req.id, (uint32_t)MessengerAPI_Req::TestMsg_Req);

		result = true;
		break;
	}

	if (!result)
	{


		auto _rsp = pl::pl_api::make_object<pl::pl_api::TestMsg_Rsp>();
		_rsp->rsp = 0xfff0;
		SendResp({ true, req.id, std::move(_rsp) });
	}

	return true;
}

bool Client::Impl::OnRequest_TestMsgCnf(Client::Request &&req)
{
	PAPI_STREAM_HEADER *header;
	tDPrimitveTLV <65535> msg;
	bool result = false;

	if (!bin_stream)
	{
		Log(LogLevel::Error, req.id, _strl("OnRequest_TestMsgCnf. error stream"));
		return false;
	}
	Client::Impl::PConnectState state = GetConnectState();
	if ((state == PConnectState::offline) || (state == PConnectState::start) || (state == PConnectState::wait_connect) || (state == PConnectState::wait_disconnect) || (state == PConnectState::connect) || (state == PConnectState::wait_open))
	{
		Log(LogLevel::Error, req.id, _strl("OnRequest_TestMsgCnf. error connect"));
		return false;
	}





	auto r = move_tl_object_as<pl_api::TestMsg_Req>(req.function);

	header = (PAPI_STREAM_HEADER*)msg.SetHeader(sizeof(PAPI_STREAM_HEADER));
	header->ver = 0;
	header->service = (byte)PAPI_Service::service;
	header->command_id = (word)PAPI_ComService::test_msg_rsp;
	header->req_id = (unsigned long)req.id;

 	msg.AddwTwLV((word)PAPI_ParamService::num, _stds_pl_w(number));

	msg.AddwTwLV((word)PAPI_ParamService::id, &r->req_id, 4);
	msg.AddwTwLV((word)PAPI_ParamService::loop, &r->loop, 1);

	if (r->len)
		msg.AddwTwLV((word)PAPI_ParamService::data, r->data, r->len);

	if (!bin_stream(msg, msg.Len()))
	{
		Log(LogLevel::Error, req.id, _strl("OnRequest_TestMsgCnf. error send"));
		return false;
	}

	return true;
}


bool Client::Impl::OnRequest_DialReq(Client::Request &&req)
{
	PAPI_STREAM_HEADER *header;
	tDPrimitveTLV <512> msg;
	bool result = false;

	while (true)
	{
		if (!bin_stream)
		{
			Log(LogLevel::Error, req.id, _strl("OnRequest_DialReq. error stream"));
			break;
		}
		if (GetConnectState() != PConnectState::online)
		{
			Log(LogLevel::Error, req.id, _strl("OnRequest_DialReq. error connect"));
			break;
		}

		Log(LogLevel::Verbose, req.id, _strl("OnRequest_DialReq"));

		auto r = move_tl_object_as<pl_api::Dial_Req>(req.function);
		
		header = (PAPI_STREAM_HEADER*)msg.SetHeader(sizeof(PAPI_STREAM_HEADER));
		header->ver = 0;
		header->service = (byte)PAPI_Service::dial;
		header->command_id = (word)PAPI_ComDial::dial_req;
		header->req_id = (unsigned long)req.id;



		msg.AddwTwLV((word)PAPI_ParamDial::sour_num, _stds_pl_w(r->snum));
		msg.AddwTwLV((word)PAPI_ParamDial::dest_num, _stds_pl_w(r->dnum));

		msg.AddwTwLV((word)PAPI_ParamDial::call_id, _stds_pl_w(r->call_id));

		if(r->channel)
			msg.AddwTwLV((word)PAPI_ParamDial::channel, &r->channel,1);

		if (!bin_stream(msg, msg.Len()))
		{
			Log(LogLevel::Error, req.id, _strl("OnRequest_DialReq. error send"));
			break;
		}

		StartTimer(req.id, (uint32_t)MessengerAPI_Req::Dial_Req);

		result = true;
		break;
	}

	if (!result)
	{
		Log(LogLevel::Verbose, req.id, _strl("Send Dial_Rsp"));

		auto _rsp = pl::pl_api::make_object<pl::pl_api::Dial_Rsp>();
		_rsp->rsp = 0xfff0;
		SendResp({ true, req.id, std::move(_rsp) });
	}

	return true;
}

bool Client::Impl::OnRequest_DialCnf(Client::Request &&req)
{
	PAPI_STREAM_HEADER *header;
	tDPrimitveTLV <512> msg;

	if (!bin_stream)
	{
		Log(LogLevel::Error, req.id, _strl("OnRequest_DialCnf. error stream"));
		return false;
	}
	if (GetConnectState() != PConnectState::online)
	{
		Log(LogLevel::Error, req.id, _strl("OnRequest_DialCnf. error connect"));
		return false;
	}

	Log(LogLevel::Verbose, req.id, _strl("OnReqOnRequest_DialCnfuest_PingCnf"));

	auto r = move_tl_object_as<pl_api::Dial_Cnf>(req.function);


	header = (PAPI_STREAM_HEADER*)msg.SetHeader(sizeof(PAPI_STREAM_HEADER));
	header->ver = 0;
	header->service = (byte)PAPI_Service::dial;
	header->command_id = (word)PAPI_ComDial::dial_rsp;
	header->req_id = (unsigned long)req.id;



	msg.AddwTwLV((word)PAPI_ParamDial::sour_num, _stds_pl_w(r->snum));
	msg.AddwTwLV((word)PAPI_ParamDial::dest_num, _stds_pl_w(r->dnum));

	msg.AddwTwLV((word)PAPI_ParamDial::rsp, (byte*)&r->rsp, 2);

	if(r->call_id.length())
		msg.AddwTwLV((word)PAPI_ParamDial::call_id, _stds_pl_w(r->call_id));

	if (!bin_stream(msg, msg.Len()))
	{
		Log(LogLevel::Error, req.id, _strl("OnRequest_DialCnf. error send"));
		return false;
	}



	return true;
}

bool Client::Impl::OnRequest_CancelReq(Client::Request &&req)
{
	PAPI_STREAM_HEADER *header;
	tDPrimitveTLV <512> msg;
	bool result = false;

	while (true)
	{
		if (!bin_stream)
		{
			Log(LogLevel::Error, req.id, _strl("OnRequest_CancelReq. error stream"));
			break;
		}
		if (GetConnectState() != PConnectState::online)
		{
			Log(LogLevel::Error, req.id, _strl("OnRequest_CancelReq. error connect"));
			break;
		}

		Log(LogLevel::Verbose, req.id, _strl("OnRequest_CancelReq"));

		auto r = move_tl_object_as<pl_api::Cancel_Req>(req.function);
		
		header = (PAPI_STREAM_HEADER*)msg.SetHeader(sizeof(PAPI_STREAM_HEADER));
		header->ver = 0;
		header->service = (byte)PAPI_Service::dial;
		header->command_id = (word)PAPI_ComDial::cancel_req;
		header->req_id = (unsigned long)req.id;



		msg.AddwTwLV((word)PAPI_ParamDial::sour_num, _stds_pl_w(r->snum));
		msg.AddwTwLV((word)PAPI_ParamDial::dest_num, _stds_pl_w(r->dnum));

		msg.AddwTwLV((word)PAPI_ParamDial::call_id, _stds_pl_w(r->call_id));




		if (!bin_stream(msg, msg.Len()))
		{
			Log(LogLevel::Error, req.id, _strl("OnRequest_CancelReq. error send"));
			break;
		}

		StartTimer(req.id, (uint32_t)MessengerAPI_Req::Cancel_Req);

		result = true;
		break;
	}

	if (!result)
	{
		Log(LogLevel::Verbose, req.id, _strl("Send Cancel_Rsp"));

		auto _rsp = pl::pl_api::make_object<pl::pl_api::Cancel_Rsp>();
		_rsp->rsp = 0xfff0;
		SendResp({ true, req.id, std::move(_rsp) });
	}

	return true;
}

bool Client::Impl::OnRequest_PushClearCallsReq(Client::Request &&req)
{
	PAPI_STREAM_HEADER *header;
	tDPrimitveTLV <512> msg;
	bool result = false;

	while (true)
	{
		if (!bin_stream)
		{
			Log(LogLevel::Error, req.id, _strl("OnRequest_PushClearCallsReq. error stream"));
			break;
		}
		if (GetConnectState() != PConnectState::online)
		{
			Log(LogLevel::Error, req.id, _strl("OnRequest_PushClearCallsReq. error connect"));
			break;
		}

		Log(LogLevel::Verbose, req.id, _strl("OnRequest_PushClearCallsReq"));

		auto r = move_tl_object_as<pl_api::PushClearCalls_Req>(req.function);

		header = (PAPI_STREAM_HEADER*)msg.SetHeader(sizeof(PAPI_STREAM_HEADER));
		header->ver = 0;
		header->service = (byte)PAPI_Service::dial;
		header->command_id = (word)PAPI_ComDial::push_clear_calls_req;
		header->req_id = (unsigned long)req.id;

		if (!bin_stream(msg, msg.Len()))
		{
			Log(LogLevel::Error, req.id, _strl("OnRequest_PushClearCallsReq. error send"));
			break;
		}

		StartTimer(req.id, (uint32_t)MessengerAPI_Req::PushClearCalls_Req);
		
		result = true;
		break;
	}

	if (!result)
	{
		Log(LogLevel::Verbose, req.id, _strl("Send PushClearCalls_Rsp"));

		auto _rsp = pl::pl_api::make_object<pl::pl_api::PushClearCalls_Rsp>();
		_rsp->rsp = 0xfff0;
		SendResp({ true, req.id, std::move(_rsp) });
	}

	return true;
}

bool Client::Impl::OnRequest_CallsBadgeReq(Client::Request &&req)
{
	PAPI_STREAM_HEADER *header;
	tDPrimitveTLV <512> msg;
	bool result = false;

	while (true)
	{
		if (!bin_stream)
		{
			Log(LogLevel::Error, req.id, _strl("OnRequest_CallsBadgeReq. error stream"));
			break;
		}
		if (GetConnectState() != PConnectState::online)
		{
			Log(LogLevel::Error, req.id, _strl("OnRequest_CallsBadgeReq. error connect"));
			break;
		}

		Log(LogLevel::Verbose, req.id, _strl("OnRequest_CallsBadgeReq"));

		auto r = move_tl_object_as<pl_api::CallsBadge_Req>(req.function);

		header = (PAPI_STREAM_HEADER*)msg.SetHeader(sizeof(PAPI_STREAM_HEADER));
		header->ver = 0;
		header->service = (byte)PAPI_Service::dial;
		header->command_id = (word)PAPI_ComDial::calls_badge_req;
		header->req_id = (unsigned long)req.id;

		if (!bin_stream(msg, msg.Len()))
		{
			Log(LogLevel::Error, req.id, _strl("OnRequest_CallsBadgeReq. error send"));
			break;
		}

		StartTimer(req.id, (uint32_t)MessengerAPI_Req::CallsBadge_Req);

		result = true;
		break;
	}

	if (!result)
	{
		Log(LogLevel::Verbose, req.id, _strl("Send CallsBadge_Rsp"));

		auto _rsp = pl::pl_api::make_object<pl::pl_api::CallsBadge_Rsp>();
		_rsp->rsp = 0xfff0;
		SendResp({ true, req.id, std::move(_rsp) });
	}

	return true;
}



bool Client::Impl::OnRequest_MsgReq(Client::Request &&req)
{
	PAPI_STREAM_HEADER *header;
	tDPrimitveTLV <65535> msg;
	bool result = false;

	while (true)
	{
		if (!bin_stream)
		{
			Log(LogLevel::Error, req.id, _strl("OnRequest_MsgReq. error stream"));
			break;
		}
		if (GetConnectState() != PConnectState::online)
		{
			Log(LogLevel::Error, req.id, _strl("OnRequest_MsgReq. error connect"));
			break;
		}

		Log(LogLevel::Verbose, req.id, _strl("OnRequest_MsgReq"));

		auto r = move_tl_object_as<pl_api::Msg_Req>(req.function);

		header = (PAPI_STREAM_HEADER*)msg.SetHeader(sizeof(PAPI_STREAM_HEADER));
		header->ver = 0;
		header->service = (byte)PAPI_Service::message;
		header->command_id = (word)PAPI_ComMessage::message_req;
		header->req_id = (unsigned long)req.id;

		msg.AddwTwLV((word)PAPI_ParamMsg::num, _stds_pl_w(number));

		msg.AddwTwLV((word)PAPI_ParamMsg::chat_private, (byte*)&r->chat_private,1);
		msg.AddwTwLV((word)PAPI_ParamMsg::chat_id, (byte*)&r->chat_id, 8);

		msg.AddwTwLV((word)PAPI_ParamMsg::sour_num, _stds_pl_w(r->snum));
		msg.AddwTwLV((word)PAPI_ParamMsg::dest_num, _stds_pl_w(r->dnum));

		if(r->channel)
			msg.AddwTwLV((word)PAPI_ParamMsg::channel, &r->channel, 1);

		if(r->file_type)
		{
			msg.AddwTwLV((word)PAPI_ParamMsg::file_type, &r->file_type, 1);
			msg.AddwTwLV((word)PAPI_ParamMsg::file_hash, (byte*)&r->file_hash, 4);
		}

		if(r->repeat)
			msg.AddwTwLV((word)PAPI_ParamMsg::repeat, &r->repeat, 1);


		msg.AddwTwLV((word)PAPI_ParamMsg::text, _stds_pl_w(r->text));


		if (!bin_stream(msg, msg.Len()))
		{
			Log(LogLevel::Error, req.id, _strl("OnRequest_MsgReq. error send"));
			break;
		}

		StartTimer(req.id, (uint32_t)MessengerAPI_Req::Msg_Req);

		result = true;
		break;
	}

	if (!result)
	{
		Log(LogLevel::Verbose, req.id, _strl("Send Msg_Rsp"));

		auto _rsp = pl::pl_api::make_object<pl::pl_api::Msg_Rsp>();
		_rsp->rsp = 0xfff0;
		SendResp({ true, req.id, std::move(_rsp) });
	}

	return true;
}

bool Client::Impl::OnRequest_MsgCnf(Client::Request &&req)
{
	PAPI_STREAM_HEADER *header;
	tDPrimitveTLV <512> msg;

	if (!bin_stream)
	{
		Log(LogLevel::Error, req.id, _strl("OnRequest_MsgCnf. error stream"));
		return false;
	}
	if (GetConnectState() != PConnectState::online)
	{
		Log(LogLevel::Error, req.id, _strl("OnRequest_MsgCnf. error connect"));
		return false;
	}

	Log(LogLevel::Verbose, req.id, _strl("OnRequest_MsgCnf"));

	auto r = move_tl_object_as<pl_api::Msg_Cnf>(req.function);

	header = (PAPI_STREAM_HEADER*)msg.SetHeader(sizeof(PAPI_STREAM_HEADER));
	header->ver = 0;
	header->service = (byte)PAPI_Service::message;
	header->command_id = (word)PAPI_ComMessage::message_rsp;
	header->req_id = (unsigned long)req.id;

	msg.AddwTwLV((word)PAPI_ParamMsg::num, _stds_pl_w(number));

	msg.AddwTwLV((word)PAPI_ParamMsg::chat_id, (byte*)&r->chat_id, 8);
	msg.AddwTwLV((word)PAPI_ParamMsg::msg_index, (byte*)&r->msg_index, 8);
	msg.AddwTwLV((word)PAPI_ParamMsg::sm_id, (byte*)&r->sm_id, 8);

	msg.AddwTwLV((word)PAPI_ParamMsg::sour_num, _stds_pl_w(r->snum));
	msg.AddwTwLV((word)PAPI_ParamMsg::dest_num, _stds_pl_w(r->dnum));

	msg.AddwTwLV((word)PAPI_ParamMsg::rsp, (byte*)&r->rsp, 2);

	if (!bin_stream(msg, msg.Len()))
	{
		Log(LogLevel::Error, req.id, _strl("OnRequest_MsgCnf. error send"));
		return false;
	}



	return true;
}

bool Client::Impl::OnRequest_RepReq(Client::Request &&req)
{
	PAPI_STREAM_HEADER *header;
	tDPrimitveTLV <512> msg;
	bool result = false;

	while (true)
	{
		if (!bin_stream)
		{
			Log(LogLevel::Error, req.id, _strl("OnRequest_RepReq. error stream"));
			break;
		}
		if (GetConnectState() != PConnectState::online)
		{
			Log(LogLevel::Error, req.id, _strl("OnRequest_RepReq. error connect"));
			break;
		}

		Log(LogLevel::Verbose, req.id, _strl("OnRequest_RepReq"));

		auto r = move_tl_object_as<pl_api::Rep_Req>(req.function);

		header = (PAPI_STREAM_HEADER*)msg.SetHeader(sizeof(PAPI_STREAM_HEADER));
		header->ver = 0;
		header->service = (byte)PAPI_Service::message;
		header->command_id = (word)PAPI_ComMessage::report_req;
		header->req_id = (unsigned long)req.id;

		msg.AddwTwLV((word)PAPI_ParamMsg::num, _stds_pl_w(number));

		msg.AddwTwLV((word)PAPI_ParamMsg::sour_num, _stds_pl_w(r->snum));
		msg.AddwTwLV((word)PAPI_ParamMsg::dest_num, _stds_pl_w(r->dnum));

		msg.AddwTwLV((word)PAPI_ParamMsg::chat_private, (byte*)&r->chat_private, 1);
		msg.AddwTwLV((word)PAPI_ParamMsg::chat_id, &r->chat_id, 8);
		msg.AddwTwLV((word)PAPI_ParamMsg::msg_index, &r->msg_index, 8);
		msg.AddwTwLV((word)PAPI_ParamMsg::sm_id, &r->sm_id, 8);

		msg.AddwTwLV((word)PAPI_ParamMsg::rep_status, &r->status, 1);

		if (!bin_stream(msg, msg.Len()))
		{
			Log(LogLevel::Error, req.id, _strl("OnRequest_RepReq. error send"));
			break;
		}

		StartTimer(req.id, (uint32_t)MessengerAPI_Req::Rep_Req);

		result = true;
		break;
	}

	if (!result)
	{
		Log(LogLevel::Verbose, req.id, _strl("Send Rep_Rsp"));

		auto _rsp = pl::pl_api::make_object<pl::pl_api::Rep_Rsp>();
		_rsp->rsp = 0xfff0;
		SendResp({ true, req.id, std::move(_rsp) });
	}

	return true;
}

bool Client::Impl::OnRequest_RepCnf(Client::Request &&req)
{
	PAPI_STREAM_HEADER *header;
	tDPrimitveTLV <512> msg;

	if (!bin_stream)
	{
		Log(LogLevel::Error, req.id, _strl("OnRequest_RepCnf. error stream"));
		return false;
	}
	if (GetConnectState() != PConnectState::online)
	{
		Log(LogLevel::Error, req.id, _strl("OnRequest_RepCnf. error connect"));
		return false;
	}

	Log(LogLevel::Verbose, req.id, _strl("OnRequest_RepCnf"));

	auto r = move_tl_object_as<pl_api::Rep_Cnf>(req.function);

	header = (PAPI_STREAM_HEADER*)msg.SetHeader(sizeof(PAPI_STREAM_HEADER));
	header->ver = 0;
	header->service = (byte)PAPI_Service::message;
	header->command_id = (word)PAPI_ComMessage::report_rsp;
	header->req_id = (unsigned long)req.id;

	msg.AddwTwLV((word)PAPI_ParamMsg::num, _stds_pl_w(number));

	msg.AddwTwLV((word)PAPI_ParamMsg::chat_id, (byte*)&r->chat_id, 8);
	msg.AddwTwLV((word)PAPI_ParamMsg::msg_index, (byte*)&r->msg_index, 8);
	msg.AddwTwLV((word)PAPI_ParamMsg::sm_id, (byte*)&r->sm_id, 8);

	msg.AddwTwLV((word)PAPI_ParamMsg::sour_num, _stds_pl_w(r->snum));
	msg.AddwTwLV((word)PAPI_ParamMsg::dest_num, _stds_pl_w(r->dnum));

	msg.AddwTwLV((word)PAPI_ParamMsg::rep_status, &r->status, 1);

	msg.AddwTwLV((word)PAPI_ParamMsg::rsp, (byte*)&r->rsp, 2);

	if (!bin_stream(msg, msg.Len()))
	{
		Log(LogLevel::Error, req.id, _strl("OnRequest_RepCnf. error send"));
		return false;
	}



	return true;
}

bool Client::Impl::OnRequest_DelMsgReq(Client::Request &&req)
{
	PAPI_STREAM_HEADER *header;
	tDPrimitveTLV <512> msg;
	bool result = false;

	while (true)
	{
		if (!bin_stream)
		{
			Log(LogLevel::Error, req.id, _strl("OnRequest_DelMsgReq. error stream"));
			break;
		}
		if (GetConnectState() != PConnectState::online)
		{
			Log(LogLevel::Error, req.id, _strl("OnRequest_DelMsgReq. error connect"));
			break;
		}

		Log(LogLevel::Verbose, req.id, _strl("OnRequest_DelMsgReq"));

		auto r = move_tl_object_as<pl_api::DelMsg_Req>(req.function);

		header = (PAPI_STREAM_HEADER*)msg.SetHeader(sizeof(PAPI_STREAM_HEADER));
		header->ver = 0;
		header->service = (byte)PAPI_Service::message;
		header->command_id = (word)PAPI_ComMessage::del_msg_req;
		header->req_id = (unsigned long)req.id;

		msg.AddwTwLV((word)PAPI_ParamMsg::num, _stds_pl_w(number));


		msg.AddwTwLV((word)PAPI_ParamMsg::chat_id, &r->chat_id, 8);
		msg.AddwTwLV((word)PAPI_ParamMsg::msg_index, &r->msg_index, 8);
		msg.AddwTwLV((word)PAPI_ParamMsg::sm_id, &r->sm_id, 8);



 		msg.AddwTwLV((word)PAPI_ParamMsg::del_type, &r->type, 1);

		if (!bin_stream(msg, msg.Len()))
		{
			Log(LogLevel::Error, req.id, _strl("OnRequest_DelMsgReq. error send"));
			break;
		}

		StartTimer(req.id, (uint32_t)MessengerAPI_Req::DelMsg_Req);

		result = true;
		break;
	}

	if (!result)
	{
		Log(LogLevel::Verbose, req.id, _strl("Send DelMsg_Rsp"));

		auto _rsp = pl::pl_api::make_object<pl::pl_api::DelMsg_Rsp>();
		_rsp->rsp = 0xfff0;
		SendResp({ true, req.id, std::move(_rsp) });
	}

	return true;
}

bool Client::Impl::OnRequest_TypingReq(Client::Request &&req)
{
	PAPI_STREAM_HEADER *header;
	tDPrimitveTLV <512> msg;

	if (!bin_stream)
	{
		Log(LogLevel::Error, req.id, _strl("OnRequest_TypingReq. error stream"));
		return false;
	}
	if (GetConnectState() != PConnectState::online)
	{
		Log(LogLevel::Error, req.id, _strl("OnRequest_TypingReq. error connect"));
		return false;
	}

	Log(LogLevel::Verbose, req.id, _strl("OnRequest_TypingReq"));

	auto r = move_tl_object_as<pl_api::Typing_Req>(req.function);

	header = (PAPI_STREAM_HEADER*)msg.SetHeader(sizeof(PAPI_STREAM_HEADER));
	header->ver = 0;
	header->service = (byte)PAPI_Service::message;
	header->command_id = (word)PAPI_ComMessage::typing_req;
	header->req_id = (unsigned long)req.id;

	msg.AddwTwLV((word)PAPI_ParamMsg::num, _stds_pl_w(number));

	msg.AddwTwLV((word)PAPI_ParamMsg::dest_num, _stds_pl_w(r->dnum));

	msg.AddwTwLV((word)PAPI_ParamMsg::chat_private, &r->chat_private, 1);

	msg.AddwTwLV((word)PAPI_ParamMsg::chat_id, &r->chat_id, 8);

	msg.AddwTwLV((word)PAPI_ParamMsg::typing_type, &r->type, 1);

	if (!bin_stream(msg, msg.Len()))
	{
		Log(LogLevel::Error, req.id, _strl("OnRequest_TypingReq. error send"));
		return false;
	}

	return true;
}



bool Client::Impl::OnRequest_BL_Add_Req(Client::Request &&req)
{
	PAPI_STREAM_HEADER *header;
	tDPrimitveTLV <256> msg;
	bool result = false;

	while (true)
	{
		if (!bin_stream)
		{
			Log(LogLevel::Error, req.id, _strl("OnRequest_BL_Add_Req. error stream"));
			break;
		}
		if (GetConnectState() != PConnectState::online)
		{
			Log(LogLevel::Error, req.id, _strl("OnRequest_BL_Add_Req. error connect"));
			break;
		}

		Log(LogLevel::Verbose, req.id, _strl("OnRequest_BL_Add_Req"));

		auto r = move_tl_object_as<pl_api::BL_Add_Req>(req.function);

		header = (PAPI_STREAM_HEADER*)msg.SetHeader(sizeof(PAPI_STREAM_HEADER));
		header->ver = 0;
		header->service = (byte)PAPI_Service::bl;
		header->command_id = (word)PAPI_ComBlackList::add_req;
		header->req_id = (unsigned long)req.id;

		msg.AddwTwLV((word)PAPI_ParamBlackList::num, _stds_pl_w(number));

		msg.AddwTwLV((word)PAPI_ParamBlackList::block_num, _stds_pl_w(r->block_user));


		if (!bin_stream(msg, msg.Len()))
		{
			Log(LogLevel::Error, req.id, _strl("OnRequest_BL_Add_Req. error send"));
			break;
		}

		StartTimer(req.id, (uint32_t)MessengerAPI_Req::BL_Add_Req);

		result = true;
		break;
	}

	if (!result)
	{
		Log(LogLevel::Verbose, req.id, _strl("Send BL_Add_Rsp"));

		auto _rsp = pl::pl_api::make_object<pl::pl_api::BL_Add_Rsp>();
		_rsp->rsp = 0xfff0;
		SendResp({ true, req.id, std::move(_rsp) });
	}

	return true;
}

bool Client::Impl::OnRequest_BL_Del_Req(Client::Request &&req)
{
	PAPI_STREAM_HEADER *header;
	tDPrimitveTLV <256> msg;
	bool result = false;

	while (true)
	{
		if (!bin_stream)
		{
			Log(LogLevel::Error, req.id, _strl("OnRequest_BL_Del_Req. error stream"));
			break;
		}
		if (GetConnectState() != PConnectState::online)
		{
			Log(LogLevel::Error, req.id, _strl("OnRequest_BL_Del_Req. error connect"));
			break;
		}

		Log(LogLevel::Verbose, req.id, _strl("OnRequest_BL_Del_Req"));

		auto r = move_tl_object_as<pl_api::BL_Del_Req>(req.function);

		header = (PAPI_STREAM_HEADER*)msg.SetHeader(sizeof(PAPI_STREAM_HEADER));
		header->ver = 0;
		header->service = (byte)PAPI_Service::bl;
		header->command_id = (word)PAPI_ComBlackList::del_req;
		header->req_id = (unsigned long)req.id;

		msg.AddwTwLV((word)PAPI_ParamBlackList::num, _stds_pl_w(number));

		msg.AddwTwLV((word)PAPI_ParamBlackList::block_num, _stds_pl_w(r->block_user));


		if (!bin_stream(msg, msg.Len()))
		{
			Log(LogLevel::Error, req.id, _strl("OnRequest_BL_Del_Req. error send"));
			break;
		}

		StartTimer(req.id, (uint32_t)MessengerAPI_Req::BL_Del_Req);

		result = true;
		break;
	}

	if (!result)
	{
		Log(LogLevel::Verbose, req.id, _strl("Send BL_Del_Rsp"));

		auto _rsp = pl::pl_api::make_object<pl::pl_api::BL_Del_Rsp>();
		_rsp->rsp = 0xfff0;
		SendResp({ true, req.id, std::move(_rsp) });
	}

	return true;
}

bool Client::Impl::OnRequest_BL_Get_Req(Client::Request &&req)
{
	PAPI_STREAM_HEADER *header;
	tDPrimitveTLV <256> msg;
	bool result = false;

	while (true)
	{
		if (!bin_stream)
		{
			Log(LogLevel::Error, req.id, _strl("OnRequest_BL_Get_Req. error stream"));
			break;
		}
		if (GetConnectState() != PConnectState::online)
		{
			Log(LogLevel::Error, req.id, _strl("OnRequest_BL_Get_Req. error connect"));
			break;
		}

		Log(LogLevel::Verbose, req.id, _strl("OnRequest_BL_Get_Req"));

		auto r = move_tl_object_as<pl_api::BL_Get_Req>(req.function);

		header = (PAPI_STREAM_HEADER*)msg.SetHeader(sizeof(PAPI_STREAM_HEADER));
		header->ver = 0;
		header->service = (byte)PAPI_Service::bl;
		header->command_id = (word)PAPI_ComBlackList::get_req;
		header->req_id = (unsigned long)req.id;

		msg.AddwTwLV((word)PAPI_ParamBlackList::num, _stds_pl_w(number));


		if (!bin_stream(msg, msg.Len()))
		{
			Log(LogLevel::Error, req.id, _strl("OnRequest_BL_Get_Req. error send"));
			break;
		}

		StartTimer(req.id, (uint32_t)MessengerAPI_Req::BL_Get_Req);

		result = true;
		break;
	}

	if (!result)
	{
		Log(LogLevel::Verbose, req.id, _strl("Send BL_Get_Rsp"));

		auto _rsp = pl::pl_api::make_object<pl::pl_api::BL_Get_Rsp>();
		_rsp->rsp = 0xfff0;
		SendResp({ true, req.id, std::move(_rsp) });
	}

	return true;
}


bool Client::Impl::OnRequest_CS_PutMe(Client::Request &&req)
{
	PAPI_STREAM_HEADER *header;
	tDPrimitveTLV <512> msg;
	bool result = false;

	while (true)
	{
		if (!bin_stream)
		{
			Log(LogLevel::Error, req.id, _strl("OnRequest_CS_PutMe. error stream"));
			break;
		}
		if (GetConnectState() != PConnectState::online)
		{
			Log(LogLevel::Error, req.id, _strl("OnRequest_CS_PutMe. error connect"));
			break;
		}

		Log(LogLevel::Verbose, req.id, _strl("OnRequest_CS_PutMe"));

		auto r = move_tl_object_as<pl_api::CS_PutMe_Req>(req.function);

		header = (PAPI_STREAM_HEADER*)msg.SetHeader(sizeof(PAPI_STREAM_HEADER));
		header->ver = 0;
		header->service = (byte)PAPI_Service::cs;
		header->command_id = (word)PAPI_ComCS::put_me_req;
		header->req_id = (unsigned long)req.id;

		msg.AddwTwLV((word)PAPI_ParamCS::num_own, _stds_pl_w(number));
		msg.AddwTwLV((word)PAPI_ParamCS::name, _stds_pl_w(r->name));
		msg.AddwTwLV((word)PAPI_ParamCS::avatar, _stds_pl_w(r->avatar));
		msg.AddwTwLV((word)PAPI_ParamCS::lang, _stds_pl_w(r->lang));
		if(r->pinbot != 0xff)
			msg.AddwTwLV((word)PAPI_ParamCS::pinboot, &r->pinbot, 1);

		if (!bin_stream(msg, msg.Len()))
		{
			Log(LogLevel::Error, req.id, _strl("OnRequest_CS_PutMe. error send"));
			break;
		}

		StartTimer(req.id, (uint32_t)MessengerAPI_Req::CS_PutMe_Req);

		result = true;
		break;
	}

	if (!result)
	{
		Log(LogLevel::Verbose, req.id, _strl("Send CS_PutMe_Rsp"));

		auto _rsp = pl::pl_api::make_object<pl::pl_api::CS_PutMe_Rsp>();
		_rsp->rsp = 0xfff0;
		SendResp({ true, req.id, std::move(_rsp) });
	}

	return true;
}

bool Client::Impl::OnRequest_CS_GetMe(Client::Request &&req)
{
	PAPI_STREAM_HEADER *header;
	tDPrimitveTLV <512> msg;
	bool result = false;

	while (true)
	{
		if (!bin_stream)
		{
			Log(LogLevel::Error, req.id, _strl("OnRequest_CS_GetMe. error stream"));
			break;
		}
		if (GetConnectState() != PConnectState::online)
		{
			Log(LogLevel::Error, req.id, _strl("OnRequest_CS_GetMe. error connect"));
			break;
		}

		Log(LogLevel::Verbose, req.id, _strl("OnRequest_CS_GetMe"));

		header = (PAPI_STREAM_HEADER*)msg.SetHeader(sizeof(PAPI_STREAM_HEADER));
		header->ver = 0;
		header->service = (byte)PAPI_Service::cs;
		header->command_id = (word)PAPI_ComCS::get_me_req;
		header->req_id = (unsigned long)req.id;

		msg.AddwTwLV((word)PAPI_ParamCS::num_own, _stds_pl_w(number));

		if (!bin_stream(msg, msg.Len()))
		{
			Log(LogLevel::Error, req.id, _strl("OnRequest_CS_GetMe. error send"));
			break;
		}

		StartTimer(req.id, (uint32_t)MessengerAPI_Req::CS_GetMe_Req);

		result = true;
		break;
	}

	if (!result)
	{
		Log(LogLevel::Verbose, req.id, _strl("Send CS_GetMe_Rsp"));

		auto _rsp = pl::pl_api::make_object<pl::pl_api::CS_GetMe_Rsp>();
		_rsp->rsp = 0xfff0;
		SendResp({ true, req.id, std::move(_rsp) });
	}

	return true;
}

bool Client::Impl::OnRequest_CS_PutContacts(Client::Request &&req)
{
	PAPI_STREAM_HEADER *header;
	tDPrimitveTLV <65535> msg;
	DPrimitveTLV data;
	bool result = false;

	while (true)
	{
		if (!bin_stream)
		{
			Log(LogLevel::Error, req.id, _strl("OnRequest_CS_PutContacts. error stream"));
			break;
		}
		if (GetConnectState() != PConnectState::online)
		{
			Log(LogLevel::Error, req.id, _strl("OnRequest_CS_PutContacts. error connect"));
			break;
		}

		Log(LogLevel::Verbose, req.id, _strl("OnRequest_CS_PutContacts"));

		auto r = move_tl_object_as<pl_api::CS_PutContacts_Req>(req.function);

		header = (PAPI_STREAM_HEADER*)msg.SetHeader(sizeof(PAPI_STREAM_HEADER));
		header->ver = 0;
		header->service = (byte)PAPI_Service::cs;
		header->command_id = (word)PAPI_ComCS::put_contacts_req;
		header->req_id = (unsigned long)req.id;

		msg.AddwTwLV((word)PAPI_ParamCS::num_own, _stds_pl_w(number));


		{
			DPrimitveTLV _data;

			_data.Init(msg.GetPS(), msg.EmptySize(), 4);

			for (auto &c: r->contacts)
			{
				DPrimitveTLV cdata;

				cdata.Init(_data.GetPS(), _data.EmptySize(), 4);

				cdata.AddwTwLV((word)PAPI_ParamCS_Contact::num, _stds_pl_w(c.num));
				if(c.name.length())
					cdata.AddwTwLV((word)PAPI_ParamCS_Contact::name, _stds_pl_w(c.name));
				if (c.fname.length())
					cdata.AddwTwLV((word)PAPI_ParamCS_Contact::fname, _stds_pl_w(c.fname));
				if (c.lname.length())
					cdata.AddwTwLV((word)PAPI_ParamCS_Contact::lname, _stds_pl_w(c.lname));

				_data.AddWord((word)PAPI_ParamCS_Contacts::contact);
				_data.AddWord((word)cdata.Len());
				_data.Seek(cdata.Len());
			}

			msg.AddWord((word)PAPI_ParamCS::contacts);
			msg.AddWord((word)_data.Len());
			msg.Seek(_data.Len());
		}

		if (!bin_stream(msg, msg.Len()))
		{
			Log(LogLevel::Error, req.id, _strl("OnRequest_CS_PutContacts. error send"));
			break;
		}

		StartTimer(req.id, (uint32_t)MessengerAPI_Req::CS_PutContacts_Req);

		result = true;
		break;
	}

	if (!result)
	{
		Log(LogLevel::Verbose, req.id, _strl("Send CS_PutContacts_Rsp"));

		auto _rsp = pl::pl_api::make_object<pl::pl_api::CS_PutContacts_Rsp>();
		_rsp->rsp = 0xfff0;
		SendResp({ true, req.id, std::move(_rsp) });
	}

	return true;
}

bool Client::Impl::OnRequest_CS_DelContacts(Client::Request &&req)
{
	PAPI_STREAM_HEADER *header;
	tDPrimitveTLV <512> msg;
	bool result = false;

	while (true)
	{
		if (!bin_stream)
		{
			Log(LogLevel::Error, req.id, _strl("OnRequest_CS_DelContacts. error stream"));
			break;
		}
		if (GetConnectState() != PConnectState::online)
		{
			Log(LogLevel::Error, req.id, _strl("OnRequest_CS_DelContacts. error connect"));
			break;
		}

		Log(LogLevel::Verbose, req.id, _strl("OnRequest_CS_DelContacts"));

		auto r = move_tl_object_as<pl_api::CS_DelContacts_Req>(req.function);

		header = (PAPI_STREAM_HEADER*)msg.SetHeader(sizeof(PAPI_STREAM_HEADER));
		header->ver = 0;
		header->service = (byte)PAPI_Service::cs;
		header->command_id = (word)PAPI_ComCS::contacts_del_req;
		header->req_id = (unsigned long)req.id;

		msg.AddwTwLV((word)PAPI_ParamCS::num_own, _stds_pl_w(number));

		{
			DPrimitveTLV _data;

			_data.Init(msg.GetPS(), msg.EmptySize(), 4);

			for (auto &c : r->contacts)
			{
				DPrimitveTLV cdata;

				cdata.Init(_data.GetPS(), _data.EmptySize(), 4);

				cdata.AddwTwLV((word)PAPI_ParamCS_Contact::num, _stds_pl_w(c.num));

				_data.AddWord((word)PAPI_ParamCS_Contacts::contact);
				_data.AddWord((word)cdata.Len());
				_data.Seek(cdata.Len());
			}

			msg.AddWord((word)PAPI_ParamCS::contacts);
			msg.AddWord((word)_data.Len());
			msg.Seek(_data.Len());
		}

		if (!bin_stream(msg, msg.Len()))
		{
			Log(LogLevel::Error, req.id, _strl("OnRequest_CS_DelContacts. error send"));
			break;
		}

		StartTimer(req.id, (uint32_t)MessengerAPI_Req::CS_DelContacts_Req);

		result = true;
		break;
	}

	if (!result)
	{
		Log(LogLevel::Verbose, req.id, _strl("Send CS_DelContacts_Rsp"));

		auto _rsp = pl::pl_api::make_object<pl::pl_api::CS_DelContacts_Rsp>();
		_rsp->rsp = 0xfff0;
		SendResp({ true, req.id, std::move(_rsp) });
	}

	return true;
}

bool Client::Impl::OnRequest_CS_GetProfiles(Client::Request &&req)
{
	PAPI_STREAM_HEADER *header;
	tDPrimitveTLV <512> msg;
	bool result = false;

	while (true)
	{
		if (!bin_stream)
		{
			Log(LogLevel::Error, req.id, _strl("OnRequest_CS_GetProfiles. error stream"));
			break;
		}
		if (GetConnectState() != PConnectState::online)
		{
			Log(LogLevel::Error, req.id, _strl("OnRequest_CS_GetProfiles. error connect"));
			break;
		}

		Log(LogLevel::Verbose, req.id, _strl("OnRequest_CS_GetProfiles"));

		auto r = move_tl_object_as<pl_api::CS_GetProfiles_Req>(req.function);

		header = (PAPI_STREAM_HEADER*)msg.SetHeader(sizeof(PAPI_STREAM_HEADER));
		header->ver = 0;
		header->service = (byte)PAPI_Service::cs;

		header->command_id = (word)PAPI_ComCS::get_profiles_req;
		header->req_id = (unsigned long)req.id;

		msg.AddwTwLV((word)PAPI_ParamCS::num_own, _stds_pl_w(number));
		msg.AddwTwLV((word)PAPI_ParamCS::hindex, &r->index,8);

		if (!bin_stream(msg, msg.Len()))
		{
			Log(LogLevel::Error, req.id, _strl("OnRequest_CS_GetProfiles. error send"));
			break;
		}

		StartTimer(req.id, (uint32_t)MessengerAPI_Req::CS_GetProfiles_Req);

		result = true;
		break;
	}

	if (!result)
	{
		Log(LogLevel::Verbose, req.id, _strl("Send CS_GetProfiles_Rsp"));

		auto _rsp = pl::pl_api::make_object<pl::pl_api::CS_GetProfiles_Rsp>();
		_rsp->rsp = 0xfff0;
		SendResp({ true, req.id, std::move(_rsp) });
	}

	return true;
}

bool Client::Impl::OnRequest_CS_GetContactsHistory(Client::Request &&req)
{
	PAPI_STREAM_HEADER *header;
	tDPrimitveTLV <512> msg;
	bool result = false;

	while (true)
	{
		if (!bin_stream)
		{
			Log(LogLevel::Error, req.id, _strl("OnRequest_CS_GetContactsHistory. error stream"));
			break;
		}
		if (GetConnectState() != PConnectState::online)
		{
			Log(LogLevel::Error, req.id, _strl("OnRequest_CS_GetContactsHistory. error connect"));
			break;
		}

		Log(LogLevel::Verbose, req.id, _strl("OnRequest_CS_GetContactsHistory"));

		auto r = move_tl_object_as<pl_api::CS_GetContactsHistory_Req>(req.function);

		header = (PAPI_STREAM_HEADER*)msg.SetHeader(sizeof(PAPI_STREAM_HEADER));
		header->ver = 0;
		header->service = (byte)PAPI_Service::cs;
		header->command_id = (word)PAPI_ComCS::get_contacts_history_req;
		header->req_id = (unsigned long)req.id;

		msg.AddwTwLV((word)PAPI_ParamCS::num_own, _stds_pl_w(number));
		msg.AddwTwLV((word)PAPI_ParamCS::hindex, &r->index, 8);

		if (!bin_stream(msg, msg.Len()))
		{
			Log(LogLevel::Error, req.id, _strl("OnRequest_CS_GetContactsHistory. error send"));
			break;
		}

		StartTimer(req.id, (uint32_t)MessengerAPI_Req::CS_GetContactsHistory_Req);

		result = true;
		break;
	}

	if (!result)
	{
		Log(LogLevel::Verbose, req.id, _strl("Send CS_GetContactsHistory_Rsp"));

		auto _rsp = pl::pl_api::make_object<pl::pl_api::CS_GetContactsHistory_Rsp>();
		_rsp->rsp = 0xfff0;
		SendResp({ true, req.id, std::move(_rsp) });
	}

	return true;
}

bool Client::Impl::OnRequest_CS_GetContacts(Client::Request &&req)
{
	PAPI_STREAM_HEADER *header;
	tDPrimitveTLV <512> msg;
	bool result = false;

	while (true)
	{
		if (!bin_stream)
		{
			Log(LogLevel::Error, req.id, _strl("OnRequest_CS_GetContacts. error stream"));
			break;
		}
		if (GetConnectState() != PConnectState::online)
		{
			Log(LogLevel::Error, req.id, _strl("OnRequest_CS_GetContacts. error connect"));
			break;
		}

		Log(LogLevel::Verbose, req.id, _strl("OnRequest_CS_GetContacts"));

		auto r = move_tl_object_as<pl_api::CS_GetContacts_Req>(req.function);

		header = (PAPI_STREAM_HEADER*)msg.SetHeader(sizeof(PAPI_STREAM_HEADER));
		header->ver = 0;
		header->service = (byte)PAPI_Service::cs;
		header->command_id = (word)PAPI_ComCS::get_contacts_req;
		header->req_id = (unsigned long)req.id;

		msg.AddwTwLV((word)PAPI_ParamCS::num_own, _stds_pl_w(number));

		if (!bin_stream(msg, msg.Len()))
		{
			Log(LogLevel::Error, req.id, _strl("OnRequest_CS_GetContacts. error send"));
			break;
		}

		StartTimer(req.id, (uint32_t)MessengerAPI_Req::CS_GetContacts_Req);

		result = true;
		break;
	}

	if (!result)
	{
		Log(LogLevel::Verbose, req.id, _strl("Send CS_GetContacts_Rsp"));

		auto _rsp = pl::pl_api::make_object<pl::pl_api::CS_GetContacts_Rsp>();
		_rsp->rsp = 0xfff0;
		SendResp({ true, req.id, std::move(_rsp) });
	}

	return true;
}

bool Client::Impl::OnRequest_Chat_CheckList(Client::Request &&req)
{
	PAPI_STREAM_HEADER *header;
	tDPrimitveTLV <512> msg;

	bool result = false;

	while (true)
	{
		if (!bin_stream)
		{
			Log(LogLevel::Error, req.id, _strl("OnRequest_Chat_CheckList. error stream"));
			break;
		}
		if (GetConnectState() != PConnectState::online)
		{
			Log(LogLevel::Error, req.id, _strl("OnRequest_Chat_CheckList. error connect"));
			break;
		}

		Log(LogLevel::Verbose, req.id, _strl("OnRequest_Chat_CheckList"));

		auto r = move_tl_object_as<pl_api::Chat_CheckList_Req>(req.function);

		header = (PAPI_STREAM_HEADER*)msg.SetHeader(sizeof(PAPI_STREAM_HEADER));
		header->ver = 0;
		header->service = (byte)PAPI_Service::chats;
		header->command_id = (word)PAPI_ComChat::chat_check_list_req;
		header->req_id = (unsigned long)req.id;

		msg.AddwTwLV((word)PAPI_ParamChat::num_own, _stds_pl_w(number));

		if(r->index != 0xffffffffffffffff)
			msg.AddwTwLV((word)PAPI_ParamChat::index, &r->index, 8);

		if (!bin_stream(msg, msg.Len()))
		{
			Log(LogLevel::Error, req.id, _strl("OnRequest_Chat_CheckList. error send"));
			break;
		}

		StartTimer(req.id, (uint32_t)MessengerAPI_Req::Chat_CheckList_Req);

		result = true;
		break;
	}

	if (!result)
	{
		Log(LogLevel::Verbose, req.id, _strl("Send Chat_CheckList_Rsp"));

		auto _rsp = pl::pl_api::make_object<pl::pl_api::Chat_CheckList_Rsp>();
		_rsp->rsp = 0xfff0;
		SendResp({ true, req.id, std::move(_rsp) });
	}

	return true;
}

bool Client::Impl::OnRequest_Chat_GetList(Client::Request &&req)
{
	PAPI_STREAM_HEADER *header;
	tDPrimitveTLV <512> msg;
	bool result = false;

	while (true)
	{
		if (!bin_stream)
		{
			Log(LogLevel::Error, req.id, _strl("OnRequest_Chat_GetList. error stream"));
			break;
		}
		if (GetConnectState() != PConnectState::online)
		{
			Log(LogLevel::Error, req.id, _strl("OnRequest_Chat_GetList. error connect"));
			break;
		}

		Log(LogLevel::Verbose, req.id, _strl("OnRequest_Chat_GetList"));

		auto r = move_tl_object_as<pl_api::Chat_GetList_Req>(req.function);

		header = (PAPI_STREAM_HEADER*)msg.SetHeader(sizeof(PAPI_STREAM_HEADER));
		header->ver = 0;
		header->service = (byte)PAPI_Service::chats;
		header->command_id = (word)PAPI_ComChat::chat_get_list_req;
		header->req_id = (unsigned long)req.id;

		msg.AddwTwLV((word)PAPI_ParamChat::num_own, _stds_pl_w(number));

		if (r->index != 0xffffffffffffffff)
			msg.AddwTwLV((word)PAPI_ParamChat::index, &r->index, 8);

		if (!bin_stream(msg, msg.Len()))
		{
			Log(LogLevel::Error, req.id, _strl("OnRequest_Chat_GetList. error send"));
			break;
		}

		StartTimer(req.id, (uint32_t)MessengerAPI_Req::Chat_GetList_Req);

		result = true;
		break;
	}

	if (!result)
	{
		Log(LogLevel::Verbose, req.id, _strl("Send Chat_GetList_Rsp"));

		auto _rsp = pl::pl_api::make_object<pl::pl_api::Chat_GetList_Rsp>();
		_rsp->rsp = 0xfff0;
		SendResp({ true, req.id, std::move(_rsp) });
	}

	return true;
}

bool Client::Impl::OnRequest_Chat_GetInfo(Client::Request &&req)
{
	PAPI_STREAM_HEADER *header;
	tDPrimitveTLV <512> msg;
	bool result = false;

	while (true)
	{
		if (!bin_stream)
		{
			Log(LogLevel::Error, req.id, _strl("OnRequest_Chat_GetInfo. error stream"));
			break;
		}
		if (GetConnectState() != PConnectState::online)
		{
			Log(LogLevel::Error, req.id, _strl("OnRequest_Chat_GetInfo. error connect"));
			break;
		}

		Log(LogLevel::Verbose, req.id, _strl("OnRequest_Chat_GetInfo"));

		auto r = move_tl_object_as<pl_api::Chat_GetInfo_Req>(req.function);

		header = (PAPI_STREAM_HEADER*)msg.SetHeader(sizeof(PAPI_STREAM_HEADER));
		header->ver = 0;
		header->service = (byte)PAPI_Service::chats;
		header->command_id = (word)PAPI_ComChat::chat_get_info_ex_req;
		header->req_id = (unsigned long)req.id;

		msg.AddwTwLV((word)PAPI_ParamChat::num_own, _stds_pl_w(number));

		msg.AddwTwLV((word)PAPI_ParamChat::chat_id, &r->chat_id, 8);

		if (!bin_stream(msg, msg.Len()))
		{
			Log(LogLevel::Error, req.id, _strl("OnRequest_Chat_GetInfo. error send"));
			break;
		}

		StartTimer(req.id, (uint32_t)MessengerAPI_Req::Chat_GetInfo_Req);

		result = true;
		break;
	}

	if (!result)
	{
		Log(LogLevel::Verbose, req.id, _strl("Send Chat_GetInfo_Rsp"));

		auto _rsp = pl::pl_api::make_object<pl::pl_api::Chat_GetInfo_Rsp>();
		_rsp->rsp = 0xfff0;
		SendResp({ true, req.id, std::move(_rsp) });
	}

	return true;
}


bool Client::Impl::OnRequest_Chat_Create_Private(Client::Request &&req)
{
	PAPI_STREAM_HEADER *header;
	tDPrimitveTLV <512> msg;
	byte priv = 1;
	bool result = false;

	while (true)
	{
		if (!bin_stream)
		{
			Log(LogLevel::Error, req.id, _strl("OnRequest_Chat_Create_Private. error stream"));
			break;
		}
		if (GetConnectState() != PConnectState::online)
		{
			Log(LogLevel::Error, req.id, _strl("OnRequest_Chat_Create_Private. error connect"));
			break;
		}

		Log(LogLevel::Verbose, req.id, _strl("OnRequest_Chat_Create_Private"));

		auto r = move_tl_object_as<pl_api::Chat_CreatePrivate_Req>(req.function);

		header = (PAPI_STREAM_HEADER*)msg.SetHeader(sizeof(PAPI_STREAM_HEADER));
		header->ver = 0;
		header->service = (byte)PAPI_Service::chats;
		header->command_id = (word)PAPI_ComChat::chat_create_req;
		header->req_id = (unsigned long)req.id;

		msg.AddwTwLV((word)PAPI_ParamChat::num_own, _stds_pl_w(number));

		{
			DPrimitveTLV _data;

			_data.Init(msg.GetPS(), msg.EmptySize(), 4);

			_data.AddwTwLV((word)PAPI_ParamChatNums::num, _stds_pl_w(r->number));

			msg.AddWord((word)PAPI_ParamChat::nums);

			msg.AddWord((word)_data.Len());
			msg.Seek(_data.Len());
		}

		msg.AddwTwLV((word)PAPI_ParamChat::chat_private, &priv, 1);

		if (!bin_stream(msg, msg.Len()))
		{
			Log(LogLevel::Error, req.id, _strl("OnRequest_Chat_Create_Private. error send"));
			break;
		}

		StartTimer(req.id, (uint32_t)MessengerAPI_Req::Chat_CreatePrivate_Req);

		result = true;
		break;
	}

	if (!result)
	{
		Log(LogLevel::Verbose, req.id, _strl("Send Chat_CreatePrivate_Rsp"));

		auto _rsp = pl::pl_api::make_object<pl::pl_api::Chat_CreatePrivate_Rsp>();
		_rsp->rsp = 0xfff0;
		SendResp({ true, req.id, std::move(_rsp) });
	}

	return true;
}

bool Client::Impl::OnRequest_Chat_Create_Group(Client::Request &&req)
{
	PAPI_STREAM_HEADER *header;
	tDPrimitveTLV <65535> msg;
	byte priv = 0;
	bool result = false;

	while (true)
	{
		if (!bin_stream)
		{
			Log(LogLevel::Error, req.id, _strl("OnRequest_Chat_Create_Group. error stream"));
			break;
		}
		if (GetConnectState() != PConnectState::online)
		{
			Log(LogLevel::Error, req.id, _strl("OnRequest_Chat_Create_Group. error connect"));
			break;
		}

		Log(LogLevel::Verbose, req.id, _strl("OnRequest_Chat_Create_Group"));

		auto r = move_tl_object_as<pl_api::Chat_CreateGroup_Req>(req.function);

		header = (PAPI_STREAM_HEADER*)msg.SetHeader(sizeof(PAPI_STREAM_HEADER));
		header->ver = 0;
		header->service = (byte)PAPI_Service::chats;
		header->command_id = (word)PAPI_ComChat::chat_create_req;
		header->req_id = (unsigned long)req.id;

		msg.AddwTwLV((word)PAPI_ParamChat::num_own, _stds_pl_w(number));
		msg.AddwTwLV((word)PAPI_ParamChat::chat_name, _stds_pl_w(r->name));

		{
			DPrimitveTLV _data;

			_data.Init(msg.GetPS(), msg.EmptySize(), 4);

			for (auto &num : r->numbers)
			{
				_data.AddwTwLV((word)PAPI_ParamChatNums::num, _stds_pl_w(num));
			}

			msg.AddWord((word)PAPI_ParamChat::nums);

			msg.AddWord((word)_data.Len());
			msg.Seek(_data.Len());
		}

		msg.AddwTwLV((word)PAPI_ParamChat::chat_private, &priv, 1);

		if (!bin_stream(msg, msg.Len()))
		{
			Log(LogLevel::Error, req.id, _strl("OnRequest_Chat_Create_Group. error send"));
			break;
		}

		StartTimer(req.id, (uint32_t)MessengerAPI_Req::Chat_CreateGroup_Req);

		result = true;
		break;
	}

	if (!result)
	{
		Log(LogLevel::Verbose, req.id, _strl("Send Chat_CreateGroup_Rsp"));

		auto _rsp = pl::pl_api::make_object<pl::pl_api::Chat_CreateGroup_Rsp>();
		_rsp->rsp = 0xfff0;
		SendResp({ true, req.id, std::move(_rsp) });
	}

	return true;
}

bool Client::Impl::OnRequest_Chat_Delete(Client::Request &&req)
{
	PAPI_STREAM_HEADER *header;
	tDPrimitveTLV <512> msg;
	bool result = false;

	while (true)
	{
		if (!bin_stream)
		{
			Log(LogLevel::Error, req.id, _strl("OnRequest_Chat_Delete. error stream"));
			break;
		}
		if (GetConnectState() != PConnectState::online)
		{
			Log(LogLevel::Error, req.id, _strl("OnRequest_Chat_Delete. error connect"));
			break;
		}

		Log(LogLevel::Verbose, req.id, _strl("OnRequest_Chat_Delete"));

		auto r = move_tl_object_as<pl_api::Chat_Delete_Req>(req.function);

		header = (PAPI_STREAM_HEADER*)msg.SetHeader(sizeof(PAPI_STREAM_HEADER));
		header->ver = 0;
		header->service = (byte)PAPI_Service::chats;
		header->command_id = (word)PAPI_ComChat::chat_del_req;
		header->req_id = (unsigned long)req.id;

		msg.AddwTwLV((word)PAPI_ParamChat::num_own, _stds_pl_w(number));
		msg.AddwTwLV((word)PAPI_ParamChat::chat_id, &r->chat_id,8);

		if (!bin_stream(msg, msg.Len()))
		{
			Log(LogLevel::Error, req.id, _strl("OnRequest_Chat_Delete. error send"));
			break;
		}

		StartTimer(req.id, (uint32_t)MessengerAPI_Req::Chat_Delete_Req);

		result = true;
		break;
	}

	if (!result)
	{
		Log(LogLevel::Verbose, req.id, _strl("Send Chat_Delete_Rsp"));

		auto _rsp = pl::pl_api::make_object<pl::pl_api::Chat_Delete_Rsp>();
		_rsp->rsp = 0xfff0;
		SendResp({ true, req.id, std::move(_rsp) });
	}

	return true;
}

bool Client::Impl::OnRequest_Chat_AddNum(Client::Request &&req)
{
	PAPI_STREAM_HEADER *header;
	tDPrimitveTLV <512> msg;

	bool result = false;

	while (true)
	{
		if (!bin_stream)
		{
			Log(LogLevel::Error, req.id, _strl("OnRequest_Chat_AddNum. error stream"));
			break;
		}
		if (GetConnectState() != PConnectState::online)
		{
			Log(LogLevel::Error, req.id, _strl("OnRequest_Chat_AddNum. error connect"));
			break;
		}

		Log(LogLevel::Verbose, req.id, _strl("OnRequest_Chat_AddNum"));

		auto r = move_tl_object_as<pl_api::Chat_AddNum_Req>(req.function);

		header = (PAPI_STREAM_HEADER*)msg.SetHeader(sizeof(PAPI_STREAM_HEADER));
		header->ver = 0;
		header->service = (byte)PAPI_Service::chats;
		header->command_id = (word)PAPI_ComChat::chat_add_num_req;
		header->req_id = (unsigned long)req.id;

		msg.AddwTwLV((word)PAPI_ParamChat::num_own, _stds_pl_w(number));
		msg.AddwTwLV((word)PAPI_ParamChat::chat_id, &r->chat_id, 8);

		{
			DPrimitveTLV _data;

			_data.Init(msg.GetPS(), msg.EmptySize(), 4);

			for (auto &num : r->numbers)
			{
				_data.AddwTwLV((word)PAPI_ParamChatNums::num, _stds_pl_w(num));
			}

			msg.AddWord((word)PAPI_ParamChat::nums);

			msg.AddWord((word)_data.Len());
			msg.Seek(_data.Len());
		}

		if (!bin_stream(msg, msg.Len()))
		{
			Log(LogLevel::Error, req.id, _strl("OnRequest_Chat_AddNum. error send"));
			break;
		}

		StartTimer(req.id, (uint32_t)MessengerAPI_Req::Chat_AddNum_Req);

		result = true;
		break;
	}

	if (!result)
	{
		Log(LogLevel::Verbose, req.id, _strl("Send Chat_AddNum_Rsp"));

		auto _rsp = pl::pl_api::make_object<pl::pl_api::Chat_AddNum_Rsp>();
		_rsp->rsp = 0xfff0;
		SendResp({ true, req.id, std::move(_rsp) });
	}

	return true;
}

bool Client::Impl::OnRequest_Chat_DelNum(Client::Request &&req)
{
	PAPI_STREAM_HEADER *header;
	tDPrimitveTLV <512> msg;

	bool result = false;

	while (true)
	{
		if (!bin_stream)
		{
			Log(LogLevel::Error, req.id, _strl("OnRequest_Chat_DelNum. error stream"));
			break;
		}
		if (GetConnectState() != PConnectState::online)
		{
			Log(LogLevel::Error, req.id, _strl("OnRequest_Chat_DelNum. error connect"));
			break;
		}

		Log(LogLevel::Verbose, req.id, _strl("OnRequest_Chat_DelNum"));

		auto r = move_tl_object_as<pl_api::Chat_DelNum_Req>(req.function);

		header = (PAPI_STREAM_HEADER*)msg.SetHeader(sizeof(PAPI_STREAM_HEADER));
		header->ver = 0;
		header->service = (byte)PAPI_Service::chats;
		header->command_id = (word)PAPI_ComChat::chat_del_num_req;
		header->req_id = (unsigned long)req.id;

		msg.AddwTwLV((word)PAPI_ParamChat::num_own, _stds_pl_w(number));
		msg.AddwTwLV((word)PAPI_ParamChat::chat_id, &r->chat_id, 8);

		{
			DPrimitveTLV _data;

			_data.Init(msg.GetPS(), msg.EmptySize(), 4);

			for (auto &num : r->numbers)
			{
				_data.AddwTwLV((word)PAPI_ParamChatNums::num, _stds_pl_w(num));
			}

			msg.AddWord((word)PAPI_ParamChat::nums);

			msg.AddWord((word)_data.Len());
			msg.Seek(_data.Len());
		}

		if (!bin_stream(msg, msg.Len()))
		{
			Log(LogLevel::Error, req.id, _strl("OnRequest_Chat_DelNum. error send"));
			break;
		}

		StartTimer(req.id, (uint32_t)MessengerAPI_Req::Chat_DelNum_Req);

		result = true;
		break;
	}

	if (!result)
	{
		Log(LogLevel::Verbose, req.id, _strl("Send Chat_DelNum_Rsp"));

		auto _rsp = pl::pl_api::make_object<pl::pl_api::Chat_DelNum_Rsp>();
		_rsp->rsp = 0xfff0;
		SendResp({ true, req.id, std::move(_rsp) });
	}

	return true;
}

bool Client::Impl::OnRequest_Chat_SetName(Client::Request &&req)
{
	PAPI_STREAM_HEADER *header;
	tDPrimitveTLV <512> msg;
	bool result = false;

	while (true)
	{
		if (!bin_stream)
		{
			Log(LogLevel::Error, req.id, _strl("OnRequest_Chat_SetName. error stream"));
			break;
		}
		if (GetConnectState() != PConnectState::online)
		{
			Log(LogLevel::Error, req.id, _strl("OnRequest_Chat_SetName. error connect"));
			break;
		}

		Log(LogLevel::Verbose, req.id, _strl("OnRequest_Chat_SetName"));

		auto r = move_tl_object_as<pl_api::Chat_SetName_Req>(req.function);

		header = (PAPI_STREAM_HEADER*)msg.SetHeader(sizeof(PAPI_STREAM_HEADER));
		header->ver = 0;
		header->service = (byte)PAPI_Service::chats;
		header->command_id = (word)PAPI_ComChat::chat_set_name_req;
		header->req_id = (unsigned long)req.id;

		msg.AddwTwLV((word)PAPI_ParamChat::num_own, _stds_pl_w(number));
		msg.AddwTwLV((word)PAPI_ParamChat::chat_id, &r->chat_id, 8);
		msg.AddwTwLV((word)PAPI_ParamChat::chat_name, _stds_pl_w(r->name));

		if (!bin_stream(msg, msg.Len()))
		{
			Log(LogLevel::Error, req.id, _strl("OnRequest_Chat_SetName. error send"));
			break;
		}

		StartTimer(req.id, (uint32_t)MessengerAPI_Req::Chat_SetName_Req);

		result = true;
		break;
	}

	if (!result)
	{
		Log(LogLevel::Verbose, req.id, _strl("Send Chat_SetName_Rsp"));

		auto _rsp = pl::pl_api::make_object<pl::pl_api::Chat_SetName_Rsp>();
		_rsp->rsp = 0xfff0;
		SendResp({ true, req.id, std::move(_rsp) });
	}

	return true;
}

bool Client::Impl::OnRequest_Chat_SetAvatar(Client::Request &&req)
{
	PAPI_STREAM_HEADER *header;
	tDPrimitveTLV <512> msg;
	bool result = false;

	while (true)
	{
		if (!bin_stream)
		{
			Log(LogLevel::Error, req.id, _strl("OnRequest_Chat_SetAvatar. error stream"));
			break;
		}
		if (GetConnectState() != PConnectState::online)
		{
			Log(LogLevel::Error, req.id, _strl("OnRequest_Chat_SetAvatar. error connect"));
			break;
		}

		Log(LogLevel::Verbose, req.id, _strl("OnRequest_Chat_SetAvatar"));

		auto r = move_tl_object_as<pl_api::Chat_SetAvatar_Req>(req.function);

		header = (PAPI_STREAM_HEADER*)msg.SetHeader(sizeof(PAPI_STREAM_HEADER));
		header->ver = 0;
		header->service = (byte)PAPI_Service::chats;
		header->command_id = (word)PAPI_ComChat::chat_set_avatar_req;
		header->req_id = (unsigned long)req.id;

		msg.AddwTwLV((word)PAPI_ParamChat::num_own, _stds_pl_w(number));
		msg.AddwTwLV((word)PAPI_ParamChat::chat_id, &r->chat_id, 8);
 		msg.AddwTwLV((word)PAPI_ParamChat::chat_avatar, _stds_pl_w(r->avatar));

		if (!bin_stream(msg, msg.Len()))
		{
			Log(LogLevel::Error, req.id, _strl("OnRequest_Chat_SetAvatar. error send"));
			break;
		}

		StartTimer(req.id, (uint32_t)MessengerAPI_Req::Chat_SetAvatar_Req);

		result = true;
		break;
	}

	if (!result)
	{
		Log(LogLevel::Verbose, req.id, _strl("Send Chat_SetAvatar_Rsp"));

		auto _rsp = pl::pl_api::make_object<pl::pl_api::Chat_SetAvatar_Rsp>();
		_rsp->rsp = 0xfff0;
		SendResp({ true, req.id, std::move(_rsp) });
	}

	return true;
}


bool Client::Impl::OnRequest_Chat_GetMsgs(Client::Request &&req)
{
	PAPI_STREAM_HEADER *header;
	tDPrimitveTLV <512> msg;
	bool result = false;

	while (true)
	{
		if (!bin_stream)
		{
			Log(LogLevel::Error, req.id, _strl("OnRequest_Chat_GetMsgs. error stream"));
			break;
		}
		if (GetConnectState() != PConnectState::online)
		{
			Log(LogLevel::Error, req.id, _strl("OnRequest_Chat_GetMsgs. error connect"));
			break;
		}

		Log(LogLevel::Verbose, req.id, _strl("OnRequest_Chat_GetMsgs"));

		auto r = move_tl_object_as<pl_api::Chat_GetMsgs_Req>(req.function);

		header = (PAPI_STREAM_HEADER*)msg.SetHeader(sizeof(PAPI_STREAM_HEADER));
		header->ver = 0;
		header->service = (byte)PAPI_Service::chats;
		header->command_id = (word)PAPI_ComChat::get_msg_req;
		header->req_id = (unsigned long)req.id;

		msg.AddwTwLV((word)PAPI_ParamChat::num_own, _stds_pl_w(number));
		msg.AddwTwLV((word)PAPI_ParamChat::chat_id, &r->chat_id, 8);
		msg.AddwTwLV((word)PAPI_ParamChat::msg_index, &r->msg_index, 8);
		if(r->msg_index_stop != 0xffffffffffffffff)
			msg.AddwTwLV((word)PAPI_ParamChat::stop_msg_index, &r->msg_index_stop, 8);
		msg.AddwTwLV((word)PAPI_ParamChat::msg_count, &r->count, 4);
		msg.AddwTwLV((word)PAPI_ParamChat::dir, &r->dir, 1);

		if (!bin_stream(msg, msg.Len()))
		{
			Log(LogLevel::Error, req.id, _strl("OnRequest_Chat_GetMsgs. error send"));
			break;
		}

		StartTimer(req.id, (uint32_t)MessengerAPI_Req::Chat_GetMsgs_Req);

		result = true;
		break;
	}

	if (!result)
	{
		Log(LogLevel::Verbose, req.id, _strl("Send Chat_GetMsgs_Rsp"));

		auto _rsp = pl::pl_api::make_object<pl::pl_api::Chat_GetMsgs_Rsp>();
		_rsp->rsp = 0xfff0;
		SendResp({ true, req.id, std::move(_rsp) });
	}

	return true;
}

bool Client::Impl::OnRequest_Chat_GetMsgsDel(Client::Request &&req)
{
	PAPI_STREAM_HEADER *header;
	tDPrimitveTLV <512> msg;
	bool result = false;

	while (true)
	{
		if (!bin_stream)
		{
			Log(LogLevel::Error, req.id, _strl("OnRequest_Chat_GetMsgsDel. error stream"));
			break;
		}
		if (GetConnectState() != PConnectState::online)
		{
			Log(LogLevel::Error, req.id, _strl("OnRequest_Chat_GetMsgsDel. error connect"));
			break;
		}

		Log(LogLevel::Verbose, req.id, _strl("OnRequest_Chat_GetMsgsDel"));

		auto r = move_tl_object_as<pl_api::Chat_GetMsgsDel_Req>(req.function);

		header = (PAPI_STREAM_HEADER*)msg.SetHeader(sizeof(PAPI_STREAM_HEADER));
		header->ver = 0;
		header->service = (byte)PAPI_Service::chats;
		header->command_id = (word)PAPI_ComChat::get_msg_del_req;
		header->req_id = (unsigned long)req.id;

		msg.AddwTwLV((word)PAPI_ParamChat::num_own, _stds_pl_w(number));
		msg.AddwTwLV((word)PAPI_ParamChat::chat_id, &r->chat_id, 8);
		msg.AddwTwLV((word)PAPI_ParamChat::msg_index, &r->msg_index, 8);
		if (r->msg_index_stop != 0xffffffffffffffff)
			msg.AddwTwLV((word)PAPI_ParamChat::stop_msg_index, &r->msg_index_stop, 8);
		msg.AddwTwLV((word)PAPI_ParamChat::msg_count, &r->count, 4);
		msg.AddwTwLV((word)PAPI_ParamChat::dir, &r->dir, 1);
		if (r->dt_start.length())
			msg.AddwTwLV((word)PAPI_ParamChat::dt_start, _stds_pl_w(r->dt_start));
		if (r->dt_stop.length())
			msg.AddwTwLV((word)PAPI_ParamChat::dt_stop, _stds_pl_w(r->dt_stop));

		if (!bin_stream(msg, msg.Len()))
		{
			Log(LogLevel::Error, req.id, _strl("OnRequest_Chat_GetMsgsDel. error send"));
			break;
		}

		StartTimer(req.id, (uint32_t)MessengerAPI_Req::Chat_GetMsgsDel_Req);

		result = true;
		break;
	}

	if (!result)
	{
		Log(LogLevel::Verbose, req.id, _strl("Send Chat_GetMsgsDel_Rsp"));

		auto _rsp = pl::pl_api::make_object<pl::pl_api::Chat_GetMsgsDel_Rsp>();
		_rsp->rsp = 0xfff0;
		SendResp({ true, req.id, std::move(_rsp) });
	}

	return true;
}

bool Client::Impl::OnRequest_Chat_GetMsgsRepFinal(Client::Request &&req)
{
	PAPI_STREAM_HEADER *header;
	tDPrimitveTLV <512> msg;
	bool result = false;

	while (true)
	{
		if (!bin_stream)
		{
			Log(LogLevel::Error, req.id, _strl("OnRequest_Chat_GetMsgsRepFinal. error stream"));
			break;
		}
		if (GetConnectState() != PConnectState::online)
		{
			Log(LogLevel::Error, req.id, _strl("OnRequest_Chat_GetMsgsRepFinal. error connect"));
			break;
		}

		Log(LogLevel::Verbose, req.id, _strl("OnRequest_Chat_GetMsgsRepFinal"));

		auto r = move_tl_object_as<pl_api::Chat_GetMsgsRepFinal_Req>(req.function);

		header = (PAPI_STREAM_HEADER*)msg.SetHeader(sizeof(PAPI_STREAM_HEADER));
		header->ver = 0;
		header->service = (byte)PAPI_Service::chats;
		header->command_id = (word)PAPI_ComChat::get_repd_req;
		header->req_id = (unsigned long)req.id;

		msg.AddwTwLV((word)PAPI_ParamChat::num_own, _stds_pl_w(number));
		msg.AddwTwLV((word)PAPI_ParamChat::chat_id, &r->chat_id, 8);
		msg.AddwTwLV((word)PAPI_ParamChat::msg_index, &r->msg_index, 8);
		if (r->msg_index_stop != 0xffffffffffffffff)
			msg.AddwTwLV((word)PAPI_ParamChat::stop_msg_index, &r->msg_index_stop, 8);
		msg.AddwTwLV((word)PAPI_ParamChat::msg_count, &r->count, 4);
		msg.AddwTwLV((word)PAPI_ParamChat::dir, &r->dir, 1);
		msg.AddwTwLV((word)PAPI_ParamChat::full_msg, &r->full_msg, 1);
		if (r->dt_start.length())
			msg.AddwTwLV((word)PAPI_ParamChat::dt_start, _stds_pl_w(r->dt_start));
		if (r->dt_stop.length())
			msg.AddwTwLV((word)PAPI_ParamChat::dt_stop, _stds_pl_w(r->dt_stop));

		if (!bin_stream(msg, msg.Len()))
		{
			Log(LogLevel::Error, req.id, _strl("OnRequest_Chat_GetMsgsRepFinal. error send"));
			break;
		}

		StartTimer(req.id, (uint32_t)MessengerAPI_Req::Chat_GetMsgsRepFinal_Req);

		result = true;
		break;
	}

	if (!result)
	{
		Log(LogLevel::Verbose, req.id, _strl("Send Chat_GetMsgsRepFinal_Rsp"));

		auto _rsp = pl::pl_api::make_object<pl::pl_api::Chat_GetMsgsRepFinal_Rsp>();
		_rsp->rsp = 0xfff0;
		SendResp({ true, req.id, std::move(_rsp) });
	}

	return true;
}

bool Client::Impl::OnRequest_Chat_GetMsgsRepRead(Client::Request &&req)
{
	PAPI_STREAM_HEADER *header;
	tDPrimitveTLV <512> msg;
	bool result = false;

	while (true)
	{
		if (!bin_stream)
		{
			Log(LogLevel::Error, req.id, _strl("OnRequest_Chat_GetMsgsRepRead. error stream"));
			break;
		}
		if (GetConnectState() != PConnectState::online)
		{
			Log(LogLevel::Error, req.id, _strl("OnRequest_Chat_GetMsgsRepRead. error connect"));
			break;
		}

		Log(LogLevel::Verbose, req.id, _strl("OnRequest_Chat_GetMsgsRepRead"));

		auto r = move_tl_object_as<pl_api::Chat_GetMsgsRepRead_Req>(req.function);

		header = (PAPI_STREAM_HEADER*)msg.SetHeader(sizeof(PAPI_STREAM_HEADER));
		header->ver = 0;
		header->service = (byte)PAPI_Service::chats;
		header->command_id = (word)PAPI_ComChat::get_repv_req;
		header->req_id = (unsigned long)req.id;

		msg.AddwTwLV((word)PAPI_ParamChat::num_own, _stds_pl_w(number));
		msg.AddwTwLV((word)PAPI_ParamChat::chat_id, &r->chat_id, 8);
		msg.AddwTwLV((word)PAPI_ParamChat::msg_index, &r->msg_index, 8);
		if (r->msg_index_stop != 0xffffffffffffffff)
			msg.AddwTwLV((word)PAPI_ParamChat::stop_msg_index, &r->msg_index_stop, 8);
		msg.AddwTwLV((word)PAPI_ParamChat::msg_count, &r->count, 4);
		msg.AddwTwLV((word)PAPI_ParamChat::dir, &r->dir, 1);
		msg.AddwTwLV((word)PAPI_ParamChat::full_msg, &r->full_msg, 1);
		if(r->dt_start.length())
			msg.AddwTwLV((word)PAPI_ParamChat::dt_start, _stds_pl_w(r->dt_start));
		if (r->dt_stop.length())
			msg.AddwTwLV((word)PAPI_ParamChat::dt_stop, _stds_pl_w(r->dt_stop));

		if (!bin_stream(msg, msg.Len()))
		{
			Log(LogLevel::Error, req.id, _strl("OnRequest_Chat_GetMsgsRepRead. error send"));
			break;
		}

		StartTimer(req.id, (uint32_t)MessengerAPI_Req::Chat_GetMsgsRepRead_Req);

		result = true;
		break;
	}

	if (!result)
	{
		Log(LogLevel::Verbose, req.id, _strl("Send Chat_GetMsgsRepRead_Rsp"));

		auto _rsp = pl::pl_api::make_object<pl::pl_api::Chat_GetMsgsRepRead_Rsp>();
		_rsp->rsp = 0xfff0;
		SendResp({ true, req.id, std::move(_rsp) });
	}

	return true;
}


bool Client::Impl::OnRequest_Chat_NotRead(Client::Request &&req)
{
	PAPI_STREAM_HEADER *header;
	tDPrimitveTLV <512> msg;
	bool result = false;

	while (true)
	{
		if (!bin_stream)
		{
			Log(LogLevel::Error, req.id, _strl("OnRequest_Chat_NotRead. error stream"));
			break;
		}
		if (GetConnectState() != PConnectState::online)
		{
			Log(LogLevel::Error, req.id, _strl("OnRequest_Chat_NotRead. error connect"));
			break;
		}

		Log(LogLevel::Verbose, req.id, _strl("OnRequest_Chat_NotRead"));

		auto r = move_tl_object_as<pl_api::Chat_NotRead_Req>(req.function);

		header = (PAPI_STREAM_HEADER*)msg.SetHeader(sizeof(PAPI_STREAM_HEADER));
		header->ver = 0;
		header->service = (byte)PAPI_Service::chats;
		header->command_id = (word)PAPI_ComChat::chat_get_unread_req;
		header->req_id = (unsigned long)req.id;

		msg.AddwTwLV((word)PAPI_ParamChat::num_own, _stds_pl_w(number));
		if(r->chat_id != 0xffffffffffffffff)
			msg.AddwTwLV((word)PAPI_ParamChat::chat_id, &r->chat_id, 8);

		if (!bin_stream(msg, msg.Len()))
		{
			Log(LogLevel::Error, req.id, _strl("OnRequest_Chat_NotRead. error send"));
			break;
		}

		StartTimer(req.id, (uint32_t)MessengerAPI_Req::Chat_NotRead_Req);

		result = true;
		break;
	}

	if (!result)
	{
		Log(LogLevel::Verbose, req.id, _strl("Send Chat_NotRead_Rsp"));

		auto _rsp = pl::pl_api::make_object<pl::pl_api::Chat_NotRead_Rsp>();
		_rsp->rsp = 0xfff0;
		SendResp({ true, req.id, std::move(_rsp) });
	}

	return true;
}

bool Client::Impl::OnRequest_Chat_ReadAllReq(Client::Request &&req)
{
	PAPI_STREAM_HEADER *header;
	tDPrimitveTLV <512> msg;
	bool result = false;

	while (true)
	{
		if (!bin_stream)
		{
			Log(LogLevel::Error, req.id, _strl("OnRequest_Chat_ReadAllReq. error stream"));
			break;
		}
		if (GetConnectState() != PConnectState::online)
		{
			Log(LogLevel::Error, req.id, _strl("OnRequest_Chat_ReadAllReq. error connect"));
			break;
		}

		Log(LogLevel::Verbose, req.id, _strl("OnRequest_Chat_ReadAllReq"));

		auto r = move_tl_object_as<pl_api::Chat_ReadAll_Req>(req.function);

		header = (PAPI_STREAM_HEADER*)msg.SetHeader(sizeof(PAPI_STREAM_HEADER));
		header->ver = 0;
		header->service = (byte)PAPI_Service::chats;
		header->command_id = (word)PAPI_ComChat::set_read_req;
		header->req_id = (unsigned long)req.id;

		msg.AddwTwLV((word)PAPI_ParamChat::num_own, _stds_pl_w(number));

		msg.AddwTwLV((word)PAPI_ParamChat::chat_id, &r->chat_id, 8);

		if (r->msg_index != 0xffffffffffffffff)
			msg.AddwTwLV((word)PAPI_ParamChat::msg_index, &r->msg_index, 8);

		if (!bin_stream(msg, msg.Len()))
		{
			Log(LogLevel::Error, req.id, _strl("OnRequest_Chat_ReadAllReq. error send"));
			break;
		}

		StartTimer(req.id, (uint32_t)MessengerAPI_Req::Chat_ReadAll_Req);

		result = true;
		break;
	}

	if (!result)
	{
		Log(LogLevel::Verbose, req.id, _strl("Send Chat_ReadAll_Rsp"));

		auto _rsp = pl::pl_api::make_object<pl::pl_api::Chat_ReadAll_Rsp>();
		_rsp->rsp = 0xfff0;
		SendResp({ true, req.id, std::move(_rsp) });
	}

	return true;
}


bool Client::Impl::OnRequest_FS_PutAvatar(Client::Request &&req)
{
	PAPI_STREAM_HEADER *header;
	tDPrimitveTLV <65535> msg;
	bool result = false;

	while (true)
	{
		if (!bin_stream)
		{
			Log(LogLevel::Error, req.id, _strl("OnRequest_FS_PutAvatar. error stream"));
			break;
		}
		if (GetConnectState() != PConnectState::online)
		{
			Log(LogLevel::Error, req.id, _strl("OnRequest_FS_PutAvatar. error connect"));
			break;
		}

		Log(LogLevel::Verbose, req.id, _strl("OnRequest_FS_PutAvatar"));

		auto r = move_tl_object_as<pl_api::FS_PutAvatar_Req>(req.function);

		header = (PAPI_STREAM_HEADER*)msg.SetHeader(sizeof(PAPI_STREAM_HEADER));
		header->ver = 0;
		header->service = (byte)PAPI_Service::fs;
		header->command_id = (word)PAPI_ComFS::put_avatar_req;
		header->req_id = (unsigned long)req.id;

		msg.AddwTwLV((word)PAPI_ParamFS::num, _stds_pl_w(number));
		msg.AddwTwLV((word)PAPI_ParamFS::uid, _stds_pl_w(UID));
		msg.AddwTwLV((word)PAPI_ParamFS::file_size_full, &r->full_size, 8);
		msg.AddwTwLV((word)PAPI_ParamFS::offset, &r->offset, 8);
		msg.AddwTwLV((word)PAPI_ParamFS::file_ext, _stds_pl_w(r->file_ext));
	
		DFixString15 hash;
		hash.AddHexDword(r->hash);
		msg.AddwTwLV((word)PAPI_ParamFS::hash, _str_plw(hash));


#ifndef SHARP_WRAP
		msg.AddwTwLV((word)PAPI_ParamFS::fragment, r->bin,(word)r->len);
#else
		msg.AddwTwLV((word)PAPI_ParamFS::fragment, const_cast<void*>(r->bin), (word)r->len);
#endif

		if (!bin_stream(msg, msg.Len()))
		{
			Log(LogLevel::Error, req.id, _strl("OnRequest_FS_PutAvatar. error send"));
			break;
		}

		StartTimer(req.id, (uint32_t)MessengerAPI_Req::FS_PutAvatar_Req);

		result = true;
		break;
	}

	if (!result)
	{
		Log(LogLevel::Verbose, req.id, _strl("Send FS_PutAvatar_Rsp"));

		auto _rsp = pl::pl_api::make_object<pl::pl_api::FS_PutAvatar_Rsp>();
		_rsp->rsp = 0xfff0;
		SendResp({ true, req.id, std::move(_rsp) });
	}

	return true;
}

bool Client::Impl::OnRequest_FS_GetAvatar(Client::Request &&req)
{
	PAPI_STREAM_HEADER *header;
	tDPrimitveTLV <65535> msg;
	bool result = false;

	while (true)
	{
		if (!bin_stream)
		{
			Log(LogLevel::Error, req.id, _strl("OnRequest_FS_GetAvatar. error stream"));
			break;
		}
		if (GetConnectState() != PConnectState::online)
		{
			Log(LogLevel::Error, req.id, _strl("OnRequest_FS_GetAvatar. error connect"));
			break;
		}

		Log(LogLevel::Verbose, req.id, _strl("OnRequest_FS_GetAvatar"));

		auto r = move_tl_object_as<pl_api::FS_GetAvatar_Req>(req.function);

		header = (PAPI_STREAM_HEADER*)msg.SetHeader(sizeof(PAPI_STREAM_HEADER));
		header->ver = 0;
		header->service = (byte)PAPI_Service::fs;
		header->command_id = (word)PAPI_ComFS::get_avatar_req;
		header->req_id = (unsigned long)req.id;

		if(!r->number.length())
			msg.AddwTwLV((word)PAPI_ParamFS::num, _stds_pl_w(number));
		else
			msg.AddwTwLV((word)PAPI_ParamFS::num, _stds_pl_w(r->number));

		msg.AddwTwLV((word)PAPI_ParamFS::uid, _stds_pl_w(UID));
		msg.AddwTwLV((word)PAPI_ParamFS::offset, &r->offset, 8);
		if(r->len)
			msg.AddwTwLV((word)PAPI_ParamFS::frag_len, &r->len, 8);

		DFixString15 hash;
		hash.AddHexDword(r->hash);
		msg.AddwTwLV((word)PAPI_ParamFS::hash, _str_plw(hash));

		if (r->thumbnail)
		{
			uint8_t thumbnail = 1;
			msg.AddwTwLV((word)PAPI_ParamFS::is_thumbnail, &thumbnail, 1);
		}

		if (!bin_stream(msg, msg.Len()))
		{
			Log(LogLevel::Error, req.id, _strl("OnRequest_FS_GetAvatar. error send"));
			break;
		}

		StartTimer(req.id, (uint32_t)MessengerAPI_Req::FS_GetAvatar_Req);

		result = true;
		break;
	}

	if (!result)
	{
		Log(LogLevel::Verbose, req.id, _strl("Send FS_GetAvatar_Rsp"));

		auto _rsp = pl::pl_api::make_object<pl::pl_api::FS_GetAvatar_Rsp>();
		_rsp->rsp = 0xfff0;
		SendResp({ true, req.id, std::move(_rsp) });
	}

	return true;
}

bool Client::Impl::OnRequest_FS_PutChatAvatar(Client::Request &&req)
{
	PAPI_STREAM_HEADER *header;
	tDPrimitveTLV <65535> msg;
	bool result = false;

	while (true)
	{
		if (!bin_stream)
		{
			Log(LogLevel::Error, req.id, _strl("OnRequest_FS_PutChatAvatar. error stream"));
			break;
		}
		if (GetConnectState() != PConnectState::online)
		{
			Log(LogLevel::Error, req.id, _strl("OnRequest_FS_PutChatAvatar. error connect"));
			break;
		}

		Log(LogLevel::Verbose, req.id, _strl("OnRequest_FS_PutChatAvatar"));

		auto r = move_tl_object_as<pl_api::FS_PutChatAvatar_Req>(req.function);

		header = (PAPI_STREAM_HEADER*)msg.SetHeader(sizeof(PAPI_STREAM_HEADER));
		header->ver = 0;
		header->service = (byte)PAPI_Service::fs;
		header->command_id = (word)PAPI_ComFS::put_chat_avatar_req;
		header->req_id = (unsigned long)req.id;

		msg.AddwTwLV((word)PAPI_ParamFS::num, _stds_pl_w(number));
		msg.AddwTwLV((word)PAPI_ParamFS::uid, _stds_pl_w(UID));
		msg.AddwTwLV((word)PAPI_ParamFS::file_size_full, &r->full_size, 8);
		msg.AddwTwLV((word)PAPI_ParamFS::offset, &r->offset, 8);
		msg.AddwTwLV((word)PAPI_ParamFS::file_ext, _stds_pl_w(r->file_ext));
	
		msg.AddwTwLV((word)PAPI_ParamFS::chat_id, &r->chat_id, 8);

		DFixString15 hash;
		hash.AddHexDword(r->hash);
		msg.AddwTwLV((word)PAPI_ParamFS::hash, _str_plw(hash));


#ifndef SHARP_WRAP
		msg.AddwTwLV((word)PAPI_ParamFS::fragment, r->bin,(word)r->len);
#else
		msg.AddwTwLV((word)PAPI_ParamFS::fragment, const_cast<void*>(r->bin), (word)r->len);
#endif

		if (!bin_stream(msg, msg.Len()))
		{
			Log(LogLevel::Error, req.id, _strl("OnRequest_FS_PutChatAvatar. error send"));
			break;
		}

		StartTimer(req.id, (uint32_t)MessengerAPI_Req::FS_PutChatAvatar_Req);

		result = true;
		break;
	}

	if (!result)
	{
		Log(LogLevel::Verbose, req.id, _strl("Send FS_PutChatAvatar_Rsp"));

		auto _rsp = pl::pl_api::make_object<pl::pl_api::FS_PutChatAvatar_Rsp>();
		_rsp->rsp = 0xfff0;
		SendResp({ true, req.id, std::move(_rsp) });
	}

	return true;
}

bool Client::Impl::OnRequest_FS_GetChatAvatar(Client::Request &&req)
{
	PAPI_STREAM_HEADER *header;
	tDPrimitveTLV <65535> msg;
	bool result = false;

	while (true)
	{
		if (!bin_stream)
		{
			Log(LogLevel::Error, req.id, _strl("OnRequest_FS_GetChatAvatar. error stream"));
			break;
		}
		if (GetConnectState() != PConnectState::online)
		{
			Log(LogLevel::Error, req.id, _strl("OnRequest_FS_GetChatAvatar. error connect"));
			break;
		}

		Log(LogLevel::Verbose, req.id, _strl("OnRequest_FS_GetChatAvatar"));

		auto r = move_tl_object_as<pl_api::FS_GetChatAvatar_Req>(req.function);

		header = (PAPI_STREAM_HEADER*)msg.SetHeader(sizeof(PAPI_STREAM_HEADER));
		header->ver = 0;
		header->service = (byte)PAPI_Service::fs;
		header->command_id = (word)PAPI_ComFS::get_chat_avatar_req;
		header->req_id = (unsigned long)req.id;

		msg.AddwTwLV((word)PAPI_ParamFS::num, _stds_pl_w(number));
		msg.AddwTwLV((word)PAPI_ParamFS::uid, _stds_pl_w(UID));
		msg.AddwTwLV((word)PAPI_ParamFS::offset, &r->offset, 8);
		if (r->len)
			msg.AddwTwLV((word)PAPI_ParamFS::frag_len, &r->len, 8);

		msg.AddwTwLV((word)PAPI_ParamFS::chat_id, &r->chat_id, 8);

		DFixString15 hash;
		hash.AddHexDword(r->hash);
		msg.AddwTwLV((word)PAPI_ParamFS::hash, _str_plw(hash));

		if (r->thumbnail)
		{
			uint8_t thumbnail = 1;
			msg.AddwTwLV((word)PAPI_ParamFS::is_thumbnail, &thumbnail, 1);
		}

		if (!bin_stream(msg, msg.Len()))
		{
			Log(LogLevel::Error, req.id, _strl("OnRequest_FS_GetChatAvatar. error send"));
			break;
		}

		StartTimer(req.id, (uint32_t)MessengerAPI_Req::FS_GetChatAvatar_Req);

		result = true;
		break;
	}

	if (!result)
	{
		Log(LogLevel::Verbose, req.id, _strl("Send FS_GetChatAvatar_Rsp"));

		auto _rsp = pl::pl_api::make_object<pl::pl_api::FS_GetChatAvatar_Rsp>();
		_rsp->rsp = 0xfff0;
		SendResp({ true, req.id, std::move(_rsp) });
	}

	return true;
}

bool Client::Impl::OnRequest_FS_PutFile(Client::Request &&req)
{
	PAPI_STREAM_HEADER *header;
	tDPrimitveTLV <65535> msg;
	bool result = false;

	while (true)
	{
		if (!bin_stream)
		{
			Log(LogLevel::Error, req.id, _strl("OnRequest_FS_PutFile. error stream"));
			break;
		}
		if (GetConnectState() != PConnectState::online)
		{
			Log(LogLevel::Error, req.id, _strl("OnRequest_FS_PutFile. error connect"));
			break;
		}

		Log(LogLevel::Verbose, req.id, _strl("OnRequest_FS_PutFile"));

		auto r = move_tl_object_as<pl_api::FS_PutMsgFile_Req>(req.function);

		header = (PAPI_STREAM_HEADER*)msg.SetHeader(sizeof(PAPI_STREAM_HEADER));
		header->ver = 0;
		header->service = (byte)PAPI_Service::fs;
		header->command_id = (word)PAPI_ComFS::put_file_req;
		header->req_id = (unsigned long)req.id;

		msg.AddwTwLV((word)PAPI_ParamFS::num, _stds_pl_w(number));
		msg.AddwTwLV((word)PAPI_ParamFS::uid, _stds_pl_w(UID));
		msg.AddwTwLV((word)PAPI_ParamFS::file_size_full, &r->full_size, 8);
		msg.AddwTwLV((word)PAPI_ParamFS::offset, &r->offset, 8);
		msg.AddwTwLV((word)PAPI_ParamFS::file_ext, _stds_pl_w(r->file_ext));
	
		msg.AddwTwLV((word)PAPI_ParamFS::sm_id, &r->sm_id, 8);

		DFixString15 hash;
		hash.AddHexDword(r->hash);
		msg.AddwTwLV((word)PAPI_ParamFS::hash, _str_plw(hash));


#ifndef SHARP_WRAP
		msg.AddwTwLV((word)PAPI_ParamFS::fragment, r->bin,(word)r->len);
#else
		msg.AddwTwLV((word)PAPI_ParamFS::fragment, const_cast<void*>(r->bin), (word)r->len);
#endif

		if (!bin_stream(msg, msg.Len()))
		{
			Log(LogLevel::Error, req.id, _strl("OnRequest_FS_PutFile. error send"));
			break;
		}

		StartTimer(req.id, (uint32_t)MessengerAPI_Req::FS_PutMsgFile_Req);

		result = true;
		break;
	}

	if (!result)
	{
		Log(LogLevel::Verbose, req.id, _strl("Send FS_PutMsgFile_Rsp"));

		auto _rsp = pl::pl_api::make_object<pl::pl_api::FS_PutMsgFile_Rsp>();
		_rsp->rsp = 0xfff0;
		SendResp({ true, req.id, std::move(_rsp) });
	}

	return true;
}

bool Client::Impl::OnRequest_FS_GetFile(Client::Request &&req)
{
	PAPI_STREAM_HEADER *header;
	tDPrimitveTLV <65535> msg;
	bool result = false;

	while (true)
	{
		if (!bin_stream)
		{
			Log(LogLevel::Error, req.id, _strl("OnRequest_FS_GetFile. error stream"));
			break;
		}
		if (GetConnectState() != PConnectState::online)
		{
			Log(LogLevel::Error, req.id, _strl("OnRequest_FS_GetFile. error connect"));
			break;
		}

		Log(LogLevel::Verbose, req.id, _strl("OnRequest_FS_GetFile"));

		auto r = move_tl_object_as<pl_api::FS_GetMsgFile_Req>(req.function);

		header = (PAPI_STREAM_HEADER*)msg.SetHeader(sizeof(PAPI_STREAM_HEADER));
		header->ver = 0;
		header->service = (byte)PAPI_Service::fs;
		header->command_id = (word)PAPI_ComFS::get_file_req;
		header->req_id = (unsigned long)req.id;

		msg.AddwTwLV((word)PAPI_ParamFS::num, _stds_pl_w(number));
		msg.AddwTwLV((word)PAPI_ParamFS::uid, _stds_pl_w(UID));
		msg.AddwTwLV((word)PAPI_ParamFS::offset, &r->offset, 8);
		if (r->len)
			msg.AddwTwLV((word)PAPI_ParamFS::frag_len, &r->len, 8);

		msg.AddwTwLV((word)PAPI_ParamFS::sm_id, &r->sm_id, 8);

		DFixString15 hash;
		hash.AddHexDword(r->hash);
		msg.AddwTwLV((word)PAPI_ParamFS::hash, _str_plw(hash));

		if (r->thumbnail)
		{
			uint8_t thumbnail = 1;
			msg.AddwTwLV((word)PAPI_ParamFS::is_thumbnail, &thumbnail, 1);
		}

		if (!bin_stream(msg, msg.Len()))
		{
			Log(LogLevel::Error, req.id, _strl("OnRequest_FS_GetFile. error send"));
			break;
		}

		StartTimer(req.id, (uint32_t)MessengerAPI_Req::FS_GetMsgFile_Req);

		result = true;
		break;
	}

	if (!result)
	{
		Log(LogLevel::Verbose, req.id, _strl("Send FS_GetMsgFile_Rsp"));

		auto _rsp = pl::pl_api::make_object<pl::pl_api::FS_GetMsgFile_Rsp>();
		_rsp->rsp = 0xfff0;
		SendResp({ true, req.id, std::move(_rsp) });
	}

	return true;
}


bool Client::Impl::OnRequest_Billing_Balance(Client::Request &&req)
{
	PAPI_STREAM_HEADER *header;
	tDPrimitveTLV <65535> msg;
	bool result = false;

	while (true)
	{
		if (!bin_stream)
		{
			Log(LogLevel::Error, req.id, _strl("OnRequest_Billing_Balance. error stream"));
			break;
		}
		if (GetConnectState() != PConnectState::online)
		{
			Log(LogLevel::Error, req.id, _strl("OnRequest_Billing_Balance. error connect"));
			break;
		}

		Log(LogLevel::Verbose, req.id, _strl("OnRequest_Billing_Balance"));

		auto r = move_tl_object_as<pl_api::Billing_Balance_Req>(req.function);

		header = (PAPI_STREAM_HEADER*)msg.SetHeader(sizeof(PAPI_STREAM_HEADER));
		header->ver = 0;
		header->service = (byte)PAPI_Service::billing;
		header->command_id = (word)PAPI_ComBilling::balance_req;
		header->req_id = (unsigned long)req.id;

		msg.AddwTwLV((word)PAPI_ParamBilling::num, _stds_pl_w(number));

		if (!bin_stream(msg, msg.Len()))
		{
			Log(LogLevel::Error, req.id, _strl("OnRequest_Billing_Balance. error send"));
			break;
		}

		StartTimer(req.id, (uint32_t)MessengerAPI_Req::Billing_Balance_Req);

		result = true;
		break;
	}

	if (!result)
	{
		Log(LogLevel::Verbose, req.id, _strl("Send Billing_Balance_Rsp"));

		auto _rsp = pl::pl_api::make_object<pl::pl_api::Billing_Balance_Rsp>();
		_rsp->rsp = 0xfff0;
		SendResp({ true, req.id, std::move(_rsp) });
	}

	return true;
}


bool Client::Impl::OnRequest(Client::Request &&req)
{
	bool r = false;

	switch((MessengerAPI_Req)req.function->get_id())
	{
		case MessengerAPI_Req::SetSubcriberInfo_Req:
		{
			OnSetSubcriberInfo(std::move(req));
			break;
		}
		case MessengerAPI_Req::Online_Req:
		{
			OnRequest_Online(std::move(req));
			break;
		}
		case MessengerAPI_Req::Offline_Req:
		{
			OnRequest_Offline(std::move(req));
			break;
		}
		case MessengerAPI_Req::RegistrationRequired_Cnf:
		{
			OnRegistrationRequired_Cnf(std::move(req));
			break;
		}
		case MessengerAPI_Req::Logout_Req:
		{
			OnLogout_Req(std::move(req));
			break;
		}
		case MessengerAPI_Req::LogoutAll_Req:
		{
			OnLogoutAll_Req(std::move(req));
			break;
		}
		case MessengerAPI_Req::PushToken_Req:
		{
			OnPushToken_Req(std::move(req));
			break;
		}
		case MessengerAPI_Req::GetSubsribersStatus_Req:
		{
			OnGetSubsribersStatus_Req(std::move(req));
			break;
		}
		case MessengerAPI_Req::SetUserStatus_Req:
		{
			OnSetUserStatus_Req(std::move(req));
			break;
		}
		case MessengerAPI_Req::GetRouteDirection_Req:
		{
			OnGetRouteDirectionReq(std::move(req));
			break;
		}
		case MessengerAPI_Req::SetRouteDirection_Req:
		{
			OnSetRouteDirectionReq(std::move(req));
			break;
		}

		case MessengerAPI_Req::Ping_Req:
		{
			r = OnRequest_PingReq(std::move(req));
			break;
		}
		case MessengerAPI_Req::Ping_Cnf:
		{
			r = OnRequest_PingCnf(std::move(req));
			break;
		}
		case MessengerAPI_Req::TestMsg_Req:
		{
			r = OnRequest_TestMsgReq(std::move(req));
			break;
		}
		case MessengerAPI_Req::TestMsg_Cnf:
		{
			r = OnRequest_TestMsgCnf(std::move(req));
			break;
		}

		case MessengerAPI_Req::Dial_Req:
		{
			r = OnRequest_DialReq(std::move(req));
			break;
		}
		case MessengerAPI_Req::Dial_Cnf:
		{
			r = OnRequest_DialCnf(std::move(req));
			break;
		}
		case MessengerAPI_Req::Cancel_Req:
		{
			r = OnRequest_CancelReq(std::move(req));
			break;
		}
		case MessengerAPI_Req::PushClearCalls_Req:
		{
			r = OnRequest_PushClearCallsReq(std::move(req));
			break;
		}
		case MessengerAPI_Req::CallsBadge_Req:
		{
			r = OnRequest_CallsBadgeReq(std::move(req));
			break;
		}

		case MessengerAPI_Req::Msg_Req:
		{
			r = OnRequest_MsgReq(std::move(req));
			break;
		}
		case MessengerAPI_Req::Msg_Cnf:
		{
			r = OnRequest_MsgCnf(std::move(req));
			break;
		}
		case MessengerAPI_Req::Rep_Req:
		{
			r = OnRequest_RepReq(std::move(req));
			break;
		}
		case MessengerAPI_Req::Rep_Cnf:
		{
			r = OnRequest_RepCnf(std::move(req));
			break;
		}
		case MessengerAPI_Req::DelMsg_Req:
		{
			r = OnRequest_DelMsgReq(std::move(req));
			break;
		}
		case MessengerAPI_Req::Typing_Req:
		{
			r = OnRequest_TypingReq(std::move(req));
			break;
		}

		case MessengerAPI_Req::CS_PutMe_Req:
		{
			r = OnRequest_CS_PutMe(std::move(req));
			break;
		}
		case MessengerAPI_Req::CS_GetMe_Req:
		{
			r = OnRequest_CS_GetMe(std::move(req));
			break;
		}
		case MessengerAPI_Req::CS_PutContacts_Req:
		{
			r = OnRequest_CS_PutContacts(std::move(req));
			break;
		}
		case MessengerAPI_Req::CS_DelContacts_Req:
		{
			r = OnRequest_CS_DelContacts(std::move(req));
			break;
		}
		case MessengerAPI_Req::CS_GetProfiles_Req:
		{
			r = OnRequest_CS_GetProfiles(std::move(req));
			break;
		}
		case MessengerAPI_Req::CS_GetContacts_Req:
		{
			r = OnRequest_CS_GetContacts(std::move(req));
			break;
		}
		case MessengerAPI_Req::CS_GetContactsHistory_Req:
		{
			r = OnRequest_CS_GetContactsHistory(std::move(req));
			break;
		}

		case MessengerAPI_Req::Chat_CheckList_Req:
		{
			r = OnRequest_Chat_CheckList(std::move(req));
			break;
		}
		case MessengerAPI_Req::Chat_GetList_Req:
		{
			r = OnRequest_Chat_GetList(std::move(req));
			break;
		}
		case MessengerAPI_Req::Chat_GetInfo_Req:
		{
			r = OnRequest_Chat_GetInfo(std::move(req));
			break;
		}
		case MessengerAPI_Req::Chat_CreatePrivate_Req:
		{
			r = OnRequest_Chat_Create_Private(std::move(req));
			break;
		}
		case MessengerAPI_Req::Chat_CreateGroup_Req:
		{
			r = OnRequest_Chat_Create_Group(std::move(req));
			break;
		}
		case MessengerAPI_Req::Chat_Delete_Req:
		{
			r = OnRequest_Chat_Delete(std::move(req));
			break;
		}
		case MessengerAPI_Req::Chat_AddNum_Req:
		{
			r = OnRequest_Chat_AddNum(std::move(req));
			break;
		}
		case MessengerAPI_Req::Chat_DelNum_Req:
		{
			r = OnRequest_Chat_DelNum(std::move(req));
			break;
		}
		case MessengerAPI_Req::Chat_SetName_Req:
		{
			r = OnRequest_Chat_SetName(std::move(req));
			break;
		}
		case MessengerAPI_Req::Chat_SetAvatar_Req:
		{
			r = OnRequest_Chat_SetAvatar(std::move(req));
			break;
		}
		case MessengerAPI_Req::Chat_GetMsgs_Req:
		{
			r = OnRequest_Chat_GetMsgs(std::move(req));
			break;
		}
		case MessengerAPI_Req::Chat_GetMsgsDel_Req:
		{
			r = OnRequest_Chat_GetMsgsDel(std::move(req));
			break;
		}
		case MessengerAPI_Req::Chat_GetMsgsRepFinal_Req:
		{
			r = OnRequest_Chat_GetMsgsRepFinal(std::move(req));
			break;
		}
		case MessengerAPI_Req::Chat_GetMsgsRepRead_Req:
		{
			r = OnRequest_Chat_GetMsgsRepRead(std::move(req));
			break;
		}
		case MessengerAPI_Req::Chat_NotRead_Req:
		{
			r = OnRequest_Chat_NotRead(std::move(req));
			break;
		}
		case MessengerAPI_Req::Chat_ReadAll_Req:
		{
			r = OnRequest_Chat_ReadAllReq(std::move(req));
			break;
		}

		case MessengerAPI_Req::BL_Add_Req:
		{
			r = OnRequest_BL_Add_Req(std::move(req));
			break;
		}
		case MessengerAPI_Req::BL_Del_Req:
		{
			r = OnRequest_BL_Del_Req(std::move(req));
			break;
		}
		case MessengerAPI_Req::BL_Get_Req:
		{
			r = OnRequest_BL_Get_Req(std::move(req));
			break;
		}
		case MessengerAPI_Req::FS_PutAvatar_Req:
		{
			r = OnRequest_FS_PutAvatar(std::move(req));
			break;
		}
		case MessengerAPI_Req::FS_GetAvatar_Req:
		{
			r = OnRequest_FS_GetAvatar(std::move(req));
			break;
		}
		case MessengerAPI_Req::FS_PutChatAvatar_Req:
		{
			r = OnRequest_FS_PutChatAvatar(std::move(req));
			break;
		}
		case MessengerAPI_Req::FS_GetChatAvatar_Req:
		{
			r = OnRequest_FS_GetChatAvatar(std::move(req));
			break;
		}
		case MessengerAPI_Req::FS_PutMsgFile_Req:
		{
			r = OnRequest_FS_PutFile(std::move(req));
			break;
		}
		case MessengerAPI_Req::FS_GetMsgFile_Req:
		{
			r = OnRequest_FS_GetFile(std::move(req));
			break;
		}
		case MessengerAPI_Req::Billing_Balance_Req:
		{
			r = OnRequest_Billing_Balance(std::move(req));
			break;
		}
	}

	return r;
}

bool Client::Impl::loop()
{
	while (true)
	{
		int size = input_queue_->reader_wait_nonblock();
		if (size == 0)
			return false;
		for (int i = 0; i < size; i++)
		{
			auto request = input_queue_->reader_get_unsafe();

			if (request.function == nullptr)
			{
				return true;
			}
			OnRequest(std::move(request));
		}
	}
	return false;
}

void Client::Impl::OnTimer(PAPI_ID req_id, std::int32_t command_id)
{









#define CASE(n)\
		case MessengerAPI_Req::n##_Req:\
		{\
			Log(LogLevel::Verbose, req_id, _strl("Send "#n"_Rsp (timeout)"));\
			auto rsp = pl::pl_api::make_object<pl::pl_api::n##_Rsp>();\
			rsp->rsp = 0xfffe;\
			SendResp({ true, req_id, std::move(rsp) });\
			break;\
		}


	switch ((MessengerAPI_Req)command_id)
	{









		CASE(Logout);
		CASE(LogoutAll);

		CASE(PushToken);

		CASE(GetSubsribersStatus);
		CASE(SetUserStatus);

		CASE(GetRouteDirection);
		CASE(SetRouteDirection);

		CASE(Dial);
		CASE(Cancel);
		CASE(PushClearCalls);
		CASE(CallsBadge);

		CASE(Msg);
		CASE(Rep);
		CASE(DelMsg);

		CASE(BL_Add);
		CASE(BL_Del);
		CASE(BL_Get);

		CASE(CS_PutMe);
		CASE(CS_GetMe);
		CASE(CS_PutContacts);
		CASE(CS_DelContacts);
		CASE(CS_GetProfiles);
		CASE(CS_GetContacts);
		CASE(CS_GetContactsHistory);

		CASE(Chat_CheckList);
		CASE(Chat_GetList);
		CASE(Chat_GetInfo);
		CASE(Chat_CreatePrivate);
		CASE(Chat_CreateGroup);
		CASE(Chat_Delete);
		CASE(Chat_AddNum);
		CASE(Chat_DelNum);
		CASE(Chat_SetName);
		CASE(Chat_SetAvatar);
		CASE(Chat_GetMsgs);
		CASE(Chat_GetMsgsRepFinal);
		CASE(Chat_GetMsgsRepRead);
		CASE(Chat_GetMsgsDel);
		CASE(Chat_NotRead);
		CASE(Chat_ReadAll);

		CASE(FS_PutAvatar);
		CASE(FS_GetAvatar);
		CASE(FS_PutChatAvatar);
		CASE(FS_GetChatAvatar);
		CASE(FS_PutMsgFile);
		CASE(FS_GetMsgFile);

		CASE(Billing_Balance);
	}

#undef CASE
}

void Client::Impl::TestTimers()
{
	auto tick = GetTickCount();

	timers_lock.lock();

	while(itimers.size())
	{
		auto t = itimers.front();
		auto d = tick - t.tick;
		if(d < 15000)
			break;

		std::map<PAPI_ID,std::int32_t>::iterator it;
		
		if (timers.size())
		{
			it = timers.find(t.id);
			if (it != timers.end())
			{
				OnTimer(t.id, it->second);
				timers.erase(it);
			}
		}

		itimers.pop();
	}

	timers_lock.unlock();
}

void Client::Impl::Procc()
{
	while(true)
	{
		if (loop())
			break;

		if(itimers.size())
		{
			TestTimers();
		}

		{
			if(on)
			{
				if (GetConnectState() != PConnectState::online)
				{
					PConnectState state = GetConnectState();
					switch (state)
					{
						case PConnectState::offline:
							break;
						case PConnectState::start:
						{
							if(sock_command)
							{
								reg_tick = GetTickCount();
								ChangeConnectState(PConnectState::wait_connect);

								if (!sock_command((std::uint32_t)Client::SockCommand::connect))
								{
									ChangeConnectState(PConnectState::start);

								}


							}
							else
							{

							}
							break;
						}
						case PConnectState::connect:
						{
							if (sock_command)
							{ 
								reg_tick = GetTickCount();
								ChangeConnectState(PConnectState::wait_open);
								if(!GenOpen())
									ChangeConnectState(PConnectState::connect);

							}
							break;
						}
						case PConnectState::open:
						{
							if (sock_command)
							{
								reg_tick = GetTickCount();
								ChangeConnectState(PConnectState::wait_reg);

								if(!GenRegister())
								{
									
								}

							}
							break;
						}
						case PConnectState::need_req:
						{
							if (!code.length())
								break;

							if (sock_command)
							{
								reg_tick = GetTickCount();
								ChangeConnectState(PConnectState::wait_reg_newreg);

								if (!GenRegisterNewReg())
									ChangeConnectState(PConnectState::open);

							}
							break;
						}
						case PConnectState::need_auth:
						{
							if (sock_command)
							{
								reg_tick = GetTickCount();
								ChangeConnectState(PConnectState::wait_reg_auth);

								if (!GenRegisterAuth())
									ChangeConnectState(PConnectState::open);

							}
							break;
						}
						case PConnectState::wait_connect:
						case PConnectState::wait_open:
						case PConnectState::wait_reg:
						case PConnectState::wait_reg_newreg:
						case PConnectState::wait_reg_auth:
						{
							int32_t tick = GetTickCount();
							tick -= reg_tick;
							if (tick >= 10000)
							{
								Log(LogLevel::Verbose, Request_Online_id, _strl("state timeout"));
								switch (state)
								{
									case PConnectState::wait_connect:
										ChangeConnectState(PConnectState::start);
										break;
									case PConnectState::wait_open:
										ChangeConnectState(PConnectState::connect);
										break;
									default:
										ChangeConnectState(PConnectState::open);
								}
							}
							break;
						}
					}
				}
			}
			else
			{

			}
		}

		Sleep(10);
	}
}

void Client::Impl::init()
{
    input_queue_ = std::make_shared<InputQueue>();
    input_queue_->init();
    output_queue_ = std::make_shared<OutputQueue>();
    output_queue_->init();

 	procc.emplace_back(&Impl::Procc, this);
 }







void Client::Impl::OnConnect(std::string _addr, std::uint16_t _port, unsigned ver)
{
	addr = _addr;
	port = _port;
	ip_ver = ver;

	Log(LogLevel::Verbose, Request_Online_id, _strl("OnConnect"));

	TestChangeConnectState(PConnectState::wait_connect, PConnectState::connect);
}

void Client::Impl::OnDisconnect(std::string _addr, std::uint16_t _port, unsigned ver)
{
	Log(LogLevel::Verbose, Request_Online_id, _strl("OnDisconnect"));

	if (on)
	{
		ChangeConnectState(PConnectState::start);
		SetConnectStatus(Request_Online_id, MessengerAPI_ConnectStatus::connecting);
	}
	else
	{
		ChangeConnectState(PConnectState::offline);
		SetConnectStatus(Request_Online_id, MessengerAPI_ConnectStatus::offline);
	}
}

bool Client::Impl::StrStream(const char* str, std::uint32_t len)
{
	PConnectState state;

	if(!str || !len)
		return true;

	
	state = GetConnectState();

	if (log_level <= (std::int8_t)LogLevel::Verbose)
	{
		std::string _str;
		_str = "StrStream. state: " + GetStrState(constate) + "  str: ";
		_str.append(str, len);

		Log(LogLevel::Verbose, Request_Online_id, (char*)_str.c_str(), _str.length());
	}

	switch(state)
	{
		case PConnectState::wait_open:
		{
			std::string _str;
			_str.assign(str, len);
			if (_str == "Open OK")
			{
				ChangeConnectState(PConnectState::open);
				SetConnectStatus(Request_Online_id, MessengerAPI_ConnectStatus::registrtarion);
			}
			else
			{
				ChangeConnectState(PConnectState::connect);
				SetConnectStatus(Request_Online_id, MessengerAPI_ConnectStatus::connecting);
			}
			break;
		}
	}

	return true;
}

bool Client::Impl::BinStream(const unsigned char* msg, std::uint32_t len)
{
	PAPI_STREAM_HEADER *h;

	if (!msg || (len < sizeof(PAPI_STREAM_HEADER)))
		return false;;

	h = (PAPI_STREAM_HEADER*)msg;

	msg += sizeof(PAPI_STREAM_HEADER);
	len -= sizeof(PAPI_STREAM_HEADER);

	switch((PAPI_Service)h->service)
	{
		case PAPI_Service::registrar:
		{
			switch((PAPI_ComRegistrar)h->command_id)
			{
				case PAPI_ComRegistrar::init_rsp:
				{
					OnRegister(h->req_id,msg,len);
					break;
				}
				case PAPI_ComRegistrar::auth_rsp:
				{
					OnRegisterAuth(h->req_id, msg, len);
					break;
				}
				case PAPI_ComRegistrar::reg_rsp:
				{
					OnRegisterReg(h->req_id, msg, len);
					break;
				}
				case PAPI_ComRegistrar::logout_rsp:
				{
					OnLogoutRsp(h->req_id, msg, len);
					break;
				}
				case PAPI_ComRegistrar::logout_all_rsp:
				{
					OnLogoutAllRsp(h->req_id, msg, len);
					break;
				}
				case PAPI_ComRegistrar::push_token_rsp:
				{
					OnPushTokenRsp(h->req_id, msg, len);
					break;
				}
				case PAPI_ComRegistrar::status_ind:
				{
					OnContactStatusInd(h->req_id, msg, len);
					break;
				}
				case PAPI_ComRegistrar::user_status_ind:
				{
					OnContactUserStatusInd(h->req_id, msg, len);
					break;
				}
				case PAPI_ComRegistrar::get_susbs_status_rsp:
				{
					OnGetSubsribersStatus_Rsp(h->req_id, msg, len);
					break;
				}
				case PAPI_ComRegistrar::set_user_status_rsp:
				{
					OnSetUserStatus_Rsp(h->req_id, msg, len);
					break;
				}
				case PAPI_ComRegistrar::get_route_direction_rsp:
				{
					OnGetRouteDirection_Rsp(h->req_id, msg, len);
					break;
				}
				case PAPI_ComRegistrar::set_route_direction_rsp:
				{
					OnSetRouteDirection_Rsp(h->req_id, msg, len);
					break;
				}
			}
			break;
		}
		case PAPI_Service::service:
		{
			switch ((PAPI_ComService)h->command_id)
			{
				case PAPI_ComService::ping_rsp:
				{
					OnPingRsp(h->req_id, msg, len);
					break;
				}
				case PAPI_ComService::ping_req:
				{
					OnPingInd(h->req_id, msg, len);
					break;
				}
				case PAPI_ComService::ping_ex_req:
				{
					OnPingExInd(h->req_id, msg, len);
					break;
				}
				case PAPI_ComService::test_msg_rsp:
				{
					OnTestMsgRsp(h->req_id, msg, len);
					break;
				}
				case PAPI_ComService::test_msg_req:
				{
					OnTestMsgInd(h->req_id, msg, len);
					break;
				}
				case PAPI_ComService::test_msg_ex_req:
				{
					OnTestMsgExInd(h->req_id, msg, len);
					break;
				}
			}
			break;
		}
		case PAPI_Service::dial:
		{
			switch ((PAPI_ComDial)h->command_id)
			{
				case PAPI_ComDial::dial_rsp:
				{
					OnDialRsp(h->req_id, msg, len);
					break;
				}
				case PAPI_ComDial::dial_req:
				{
					OnDialInd(h->req_id, msg, len);
					break;
				}
				case PAPI_ComDial::cancel_req:
				{
					OnCancelRsp(h->req_id, msg, len);
					break;
				}
				case PAPI_ComDial::push_clear_calls_rsp:
				{
					OnPushClearCallsRsp(h->req_id, msg, len);
					break;
				}
				case PAPI_ComDial::calls_badge_rsp:
				{
					OnCallsBadgeRsp(h->req_id, msg, len);
					break;
				}
			}
			break;
		}
		case PAPI_Service::message:
		{
			switch ((PAPI_ComMessage)h->command_id)
			{
				case PAPI_ComMessage::message_rsp:
				{
					OnMsgRsp(h->req_id, msg, len);
					break;
				}
				case PAPI_ComMessage::message_req:
				{
					OnMsgReq(h->req_id, msg, len);
					break;
				}
				case PAPI_ComMessage::report_rsp:
				{
					OnRepRsp(h->req_id, msg, len);
					break;
				}
				case PAPI_ComMessage::report_req:
				{
					OnRepReq(h->req_id, msg, len);
					break;
				}
				case PAPI_ComMessage::del_msg_rsp:
				{
					OnDelMsgRsp(h->req_id, msg, len);
					break;
				}
				case PAPI_ComMessage::typing_req:
				{
					OnTypingReq(h->req_id, msg, len);
					break;
				}
			}
			break;
		}
		case PAPI_Service::cs:
		{
			switch ((PAPI_ComCS)h->command_id)
			{
				case PAPI_ComCS::put_me_rsp:
				{
					OnCS_PutMe_Rsp(h->req_id, msg, len);
					break;
				}
				case PAPI_ComCS::get_me_rsp:
				{
					OnCS_GetMe_Rsp(h->req_id, msg, len);
					break;
				}
				case PAPI_ComCS::get_profiles_rsp:
				{
					OnCS_GetProfileRsp_Rsp(h->req_id, msg, len);
					break;
				}
				case PAPI_ComCS::put_contacts_rsp:
				{
					OnCS_PutContacts_Rsp(h->req_id, msg, len);
					break;
				}
				case PAPI_ComCS::contacts_del_rsp:
				{
					OnCS_DelContacts_Rsp(h->req_id, msg, len);
					break;
				}
				case PAPI_ComCS::get_contacts_rsp:
				{
					OnCS_GetContactsRsp_Rsp(h->req_id, msg, len);
					break;
				}
				case PAPI_ComCS::get_contacts_history_rsp:
				{
					OnCS_GetContactsHistoryRsp_Rsp(h->req_id, msg, len);
					break;
				}
			}
			break;
		}
		case PAPI_Service::chats:
		{
			switch ((PAPI_ComChat)h->command_id)
			{
				case PAPI_ComChat::chat_check_list_rsp:
				{
					OnChatCheckListRsp(h->req_id, msg, len);
					break;
				}
				case PAPI_ComChat::chat_get_list_rsp:
				{
					OnChatGetListRsp(h->req_id, msg, len);
					break;
				}
				case PAPI_ComChat::chat_get_info_ex_rsp:
				{
					OnChatGetInfoRsp(h->req_id, msg, len);
					break;
				}
				case PAPI_ComChat::chat_create_rsp:
				{
					OnChatCreateRsp(h->req_id, msg, len);
					break;
				}
				case PAPI_ComChat::chat_del_rsp:
				{
					OnChatDelRsp(h->req_id, msg, len);
					break;
				}
				case PAPI_ComChat::chat_add_num_rsp:
				{
					OnChatAddNumRsp(h->req_id, msg, len);
					break;
				}
				case PAPI_ComChat::chat_del_num_rsp:
				{
					OnChatDelNumRsp(h->req_id, msg, len);
					break;
				}
				case PAPI_ComChat::chat_set_name_rsp:
				{
					OnChatSetNameRsp(h->req_id, msg, len);
					break;
				}
				case PAPI_ComChat::chat_set_avatar_rsp:
				{
					OnChatSetAvatarRsp(h->req_id, msg, len);
					break;
				}
				case PAPI_ComChat::get_msg_rsp:
				{
					OnChatGetMsgsRsp(h->req_id, msg, len);
					break;
				}
				case PAPI_ComChat::get_msg_del_rsp:
				{
					OnChatGetMsgsDelRsp(h->req_id, msg, len);
					break;
				}
				case PAPI_ComChat::get_repd_rsp:
				{
					OnChatGetMsgsRepFinalRsp(h->req_id, msg, len);
					break;
				}
				case PAPI_ComChat::get_repv_rsp:
				{
					OnChatGetMsgsRepReadRsp(h->req_id, msg, len);
					break;
				}
				case PAPI_ComChat::chat_change_req:
				{
					OnChatChangeInfoInd(h->req_id, msg, len);
					break;
				}
				case PAPI_ComChat::chat_get_unread_rsp:
				{
					OnChatNotReadRsp(h->req_id, msg, len);
					break;
				}
				case PAPI_ComChat::set_read_rsp:
				{
					OnChatReadAllRsp(h->req_id, msg, len);
					break;
				}
				break;
			}
			break;
		}
		case PAPI_Service::bl:
		{
			switch ((PAPI_ComBlackList)h->command_id)
			{
				case PAPI_ComBlackList::add_rsp:
				{
					OnBL_Add_Rsp(h->req_id, msg, len);
					break;
				}
				case PAPI_ComBlackList::del_rsp:
				{
					OnBL_Del_Rsp(h->req_id, msg, len);
					break;
				}
				case PAPI_ComBlackList::get_rsp:
				{
					OnBL_Get_Rsp(h->req_id, msg, len);
					break;
				}
			}
			break;
		}
		case PAPI_Service::fs:
		{
			switch ((PAPI_ComFS)h->command_id)
			{
				case PAPI_ComFS::put_avatar_rsp:
				{
					OnFSPutAvaterRsp(h->req_id, msg, len);
					break;
				}
				case PAPI_ComFS::get_avatar_rsp:
				{
					OnFSGetAvaterRsp(h->req_id, msg, len);
					break;
				}
				case PAPI_ComFS::put_chat_avatar_rsp:
				{
					OnFSPutChatAvaterRsp(h->req_id, msg, len);
					break;
				}
				case PAPI_ComFS::get_chat_avatar_rsp:
				{
					OnFSGetChatAvaterRsp(h->req_id, msg, len);
					break;
				}
				case PAPI_ComFS::put_file_rsp:
				{
					OnFSPutFileRsp(h->req_id, msg, len);
					break;
				}
				case PAPI_ComFS::get_file_rsp:
				{
					OnFSGetFileRsp(h->req_id, msg, len);
					break;
				}
			}
			break;
		}
		case PAPI_Service::billing:
		{
			switch ((PAPI_ComBilling)h->command_id)
			{
				case PAPI_ComBilling::balance_rsp:
				{
					OnBillingBalanceRsp(h->req_id, msg, len);
					break;
				}
			}
			break;
		}
	}

	timers_lock.lock();
	std::map<PAPI_ID, std::int32_t>::iterator it;
	it = timers.find(h->req_id);
	if (it != timers.end())
		timers.erase(it);
	timers_lock.unlock();

	return true;
}


bool Client::Impl::OnRegister_RegInfo(RegInfoData &data, const unsigned char* msg, std::uint32_t len)
{
	word pn;
	word pl;

	while (len)
	{

		if (len < 4)
		{
			break;
		}
		
		pn = *(word*)msg;
		msg += 2;
		len -= 2;
		



		pl = *(word*)msg;
		msg += 2;
		len -= 2;
		if (len < pl)
			break;

		switch(pn)
		{
			case (word)PAPI_ParamRegInfo::realm:
			{
				data.realm.assign((char*)msg,pl);
				break;
			}
			case (word)PAPI_ParamRegInfo::nonce:
			{
				data.nonce.assign((char*)msg, pl);
				break;
			}
			case (word)PAPI_ParamRegInfo::opaque:
			{
				data.opaque.assign((char*)msg, pl);
				break;
			}
			case (word)PAPI_ParamRegInfo::need_uid:
			{
				if(pl)
					data.need_uid = *msg != 0;
				break;
			}
		}

		msg += pl;
		len -= pl;
	}

	return true;
}

bool Client::Impl::OnRegister(std::uint32_t req_id,const unsigned char* msg, std::uint32_t len)
{
	RegInfoData data;
	std::uint16_t rsp = 0xffff;
	bool error_code = false;

	Log(LogLevel::Verbose, req_id, _strl("OnRegister_Rsp"));

	tDTablePointerWord <(int)PAPI_ParamReg::Max> p;

	p.Parse((byte*)msg, len);

	if(p.data[(int)PAPI_ParamReg::rsp].len == 2)
		rsp = *(uint16_t*)p.data[(int)PAPI_ParamReg::rsp].msg;

	if (p.data[(int)PAPI_ParamReg::error_code].len)
		error_code = *p.data[(int)PAPI_ParamReg::error_code].msg != 0;

	if (p.data[(int)PAPI_ParamReg::reg_info].Exist())
		OnRegister_RegInfo(data, p.data[(int)PAPI_ParamReg::reg_info].msg, p.data[(int)PAPI_ParamReg::reg_info].len);

	switch((PAPI_Result)rsp)
	{
		case PAPI_Result::not_found: 
		{
			ChangeConnectState(PConnectState::need_req);
			SetConnectStatus(Request_Online_id, MessengerAPI_ConnectStatus::registrtarion);
			Send_RegistrationRequired_Ind(Request_Online_id, error_code);
			break;
		}
		case PAPI_Result::unauthorized: 
		{
			realm  = data.realm;
			nonce  = data.nonce;
			opaque = data.opaque;
			need_uid = data.need_uid;
			ChangeConnectState(PConnectState::need_auth);
			SetConnectStatus(Request_Online_id, MessengerAPI_ConnectStatus::registrtarion);
			break;
		}
		default:
		{
			ChangeConnectState(PConnectState::open);
			SetConnectStatus(Request_Online_id, MessengerAPI_ConnectStatus::registrtarion);
			break;
		}
	}

	return true;
}

bool Client::Impl::OnRegisterAuth(std::uint32_t req_id, const unsigned char* msg, std::uint32_t len)
{
	RegInfoData data;
	std::uint16_t rsp = 0xffff;
	bool error_code = false;


	tDTablePointerWord <(int)PAPI_ParamReg::Max> p;

	Log(LogLevel::Verbose, req_id, _strl("OnRegisterAuth_Rsp"));

	p.Parse((byte*)msg, len);

	if (p.data[(int)PAPI_ParamReg::rsp].len == 2)
		rsp = *(std::uint16_t*)p.data[(int)PAPI_ParamReg::rsp].msg;

	if (p.data[(int)PAPI_ParamReg::error_code].len)
		error_code = *p.data[(int)PAPI_ParamReg::error_code].msg != 0;

	if (p.data[(int)PAPI_ParamReg::reg_info].Exist())
		OnRegister_RegInfo(data, p.data[(int)PAPI_ParamReg::reg_info].msg, p.data[(int)PAPI_ParamReg::reg_info].len);

	switch((PAPI_Result)rsp)
	{
		case PAPI_Result::ok: 
		{
			switch (GetConnectState())
			{

				case PConnectState::wait_reg_auth:
				{
					ChangeConnectState(PConnectState::online);
					if (Request_Online_id)
						SendOnlineRsp(Request_Online_id, 0);
					else
					{

					}
					SetConnectStatus(Request_Online_id, MessengerAPI_ConnectStatus::online);
					break;
				}
				default:
				{
					ChangeConnectState(PConnectState::open);
				}
			}
			break;
		}
		case PAPI_Result::not_found: 
		{
			ChangeConnectState(PConnectState::need_req);
			SetConnectStatus(Request_Online_id, MessengerAPI_ConnectStatus::registrtarion);
			Send_RegistrationRequired_Ind(Request_Online_id, error_code);
			break;
		}
		default:
		{
			ChangeConnectState(PConnectState::open);
			SetConnectStatus(Request_Online_id, MessengerAPI_ConnectStatus::registrtarion);
			break;
		}
	}

	return true;
}

bool Client::Impl::OnRegisterReg(std::uint32_t req_id, const unsigned char* msg, std::uint32_t len)
{
	std::uint16_t rsp = 0xffff;
	bool error_code = false;

	tDTablePointerWord <(int)PAPI_ParamReg::Max> p;

	Log(LogLevel::Verbose, req_id, _strl("OnRegisterReg_Rsp"));

	p.Parse((byte*)msg, len);

	if (p.data[(int)PAPI_ParamReg::rsp].len == 2)
		rsp = *(std::uint16_t*)p.data[(int)PAPI_ParamReg::rsp].msg;

	if (p.data[(int)PAPI_ParamReg::error_code].len)
		error_code = *p.data[(int)PAPI_ParamReg::error_code].msg != 0;

	switch ((PAPI_Result)rsp)
	{
		case PAPI_Result::ok: 
		{
			switch (GetConnectState())
			{
 				case PConnectState::wait_reg_newreg:

				{
					ChangeConnectState(PConnectState::online);
					if (Request_Online_id)
						SendOnlineRsp(Request_Online_id, 0);
					else
					{

					}
					SetConnectStatus(Request_Online_id, MessengerAPI_ConnectStatus::online);
					break;
				}
				default:
				{
					ChangeConnectState(PConnectState::open);
				}
			}
			break;
		}
		case PAPI_Result::not_found: 
		{
			ChangeConnectState(PConnectState::need_req);
			SetConnectStatus(Request_Online_id, MessengerAPI_ConnectStatus::registrtarion);
			Send_RegistrationRequired_Ind(Request_Online_id, error_code);
			break;
		}
		default:
		{
			ChangeConnectState(PConnectState::open);
			SetConnectStatus(Request_Online_id, MessengerAPI_ConnectStatus::registrtarion);
			break;
		}
	}

	return true;
}


bool Client::Impl::OnLogoutRsp(std::uint32_t req_id, const unsigned char* msg, std::uint32_t len)
{
	tDTablePointerWord <(int)PAPI_ParamReg::Max> p;
	std::uint16_t _rsp = 0xffff;

	Log(LogLevel::Verbose, req_id, _strl("OnLogoutRsp"));

	p.Parse((byte*)msg, len);

	auto rsp = pl::pl_api::make_object<pl::pl_api::Logout_Rsp>();

	if (p.data[(int)PAPI_ParamReg::rsp].len == 2)
		_rsp = *(std::uint16_t*)p.data[(int)PAPI_ParamReg::rsp].msg;

	rsp->rsp = _rsp;

	Log(LogLevel::Verbose, req_id, _strl("Send Logout_Rsp"));

	SendResp({ true, req_id, std::move(rsp) });

	if (_rsp == 0)
		OnRequest_Offline();

	return true;
}

bool Client::Impl::OnLogoutAllRsp(std::uint32_t req_id, const unsigned char* msg, std::uint32_t len)
{
	tDTablePointerWord <(int)PAPI_ParamReg::Max> p;
	std::uint16_t _rsp = 0xffff;

	Log(LogLevel::Verbose, req_id, _strl("OnLogoutAllRsp"));

	p.Parse((byte*)msg, len);

	auto rsp = pl::pl_api::make_object<pl::pl_api::LogoutAll_Rsp>();

	if (p.data[(int)PAPI_ParamReg::rsp].len == 2)
		_rsp = *(std::uint16_t*)p.data[(int)PAPI_ParamReg::rsp].msg;

	rsp->rsp = _rsp;

	Log(LogLevel::Verbose, req_id, _strl("Send LogoutAll_Rsp"));

	SendResp({ true, req_id, std::move(rsp) });

	if (_rsp == 0)
		OnRequest_Offline();

	return true;
}


bool Client::Impl::OnPushTokenRsp(std::uint32_t req_id, const unsigned char* msg, std::uint32_t len)
{
	tDTablePointerWord <(int)PAPI_ParamReg::Max> p;
	std::uint16_t _rsp = 0xffff;

	Log(LogLevel::Verbose, req_id, _strl("OnPushTokenRsp"));

	p.Parse((byte*)msg, len);

	auto rsp = pl::pl_api::make_object<pl::pl_api::PushToken_Rsp>();

	if (p.data[(int)PAPI_ParamReg::rsp].len == 2)
		_rsp = *(std::uint16_t*)p.data[(int)PAPI_ParamReg::rsp].msg;

	rsp->rsp = _rsp;

	Log(LogLevel::Verbose, req_id, _strl("Send PushToken_Rsp"));

	SendResp({ true, req_id, std::move(rsp) });

	return true;
}


bool Client::Impl::OnContactStatusInd(std::uint32_t req_id, const unsigned char* msg, std::uint32_t len)
{
	tDTablePointerWord <(int)PAPI_ParamReg::Max> p;

	Log(LogLevel::Verbose, req_id, _strl("OnContactStatusInd"));

	p.Parse((byte*)msg, len);

	auto rsp = pl::pl_api::make_object<pl::pl_api::ContactStatus_Ind>();

	rsp->num.assign((char*)DTP_PPARAMETERB(PAPI_ParamReg::num));

	if (p.data[(int)PAPI_ParamReg::status].len)
		rsp->status = *p.data[(int)PAPI_ParamReg::status].msg;

	rsp->status_data_time.assign((char*)DTP_PPARAMETERB(PAPI_ParamReg::status_date_time));

	Log(LogLevel::Verbose, req_id, _strl("Send ContactStatus_Ind"));

	SendResp({ true, req_id, std::move(rsp) });

	return true;
}

bool Client::Impl::OnContactUserStatusInd(std::uint32_t req_id, const unsigned char* msg, std::uint32_t len)
{
	tDTablePointerWord <(int)PAPI_ParamReg::Max> p;

	Log(LogLevel::Verbose, req_id, _strl("OnContactUserStatusInd"));

	p.Parse((byte*)msg, len);

	auto rsp = pl::pl_api::make_object<pl::pl_api::ContactUserStatus_Ind>();

	rsp->num.assign((char*)DTP_PPARAMETERB(PAPI_ParamReg::num));

	if (p.data[(int)PAPI_ParamReg::user_status].len)
		rsp->user_status = *p.data[(int)PAPI_ParamReg::user_status].msg;

	rsp->user_status_data_time.assign((char*)DTP_PPARAMETERB(PAPI_ParamReg::user_status_date_time));

	Log(LogLevel::Verbose, req_id, _strl("Send ContactUserStatus_Ind"));

	SendResp({ true, req_id, std::move(rsp) });

	return true;
}

bool Client::Impl::OnGetSubsribersStatus_Rsp(std::uint32_t req_id, const unsigned char* msg, std::uint32_t len)
{
	tDTablePointerWord <(int)PAPI_ParamReg::Max> p;

	Log(LogLevel::Verbose, req_id, _strl("OnGetSubsribersStatus_Rsp"));

	p.Parse((byte*)msg, len);

	auto rsp = pl::pl_api::make_object<pl::pl_api::GetSubsribersStatus_Rsp>();


	if (p.data[(int)PAPI_ParamReg::rsp].len == 2)
		rsp->rsp = *(std::uint16_t*)p.data[(int)PAPI_ParamReg::rsp].msg;

	Log(LogLevel::Verbose, req_id, _strl("Send GetSubsribersStatus_Rsp"));

	SendResp({ true, req_id, std::move(rsp) });

	return true;
}

bool Client::Impl::OnSetUserStatus_Rsp(std::uint32_t req_id, const unsigned char* msg, std::uint32_t len)
{
	tDTablePointerWord <(int)PAPI_ParamReg::Max> p;

	Log(LogLevel::Verbose, req_id, _strl("OnSetUserStatus_Rsp"));

	p.Parse((byte*)msg, len);

	auto rsp = pl::pl_api::make_object<pl::pl_api::SetUserStatus_Rsp>();


	if (p.data[(int)PAPI_ParamReg::rsp].len == 2)
		rsp->rsp = *(std::uint16_t*)p.data[(int)PAPI_ParamReg::rsp].msg;

	Log(LogLevel::Verbose, req_id, _strl("Send SetUserStatus_Rsp"));

	SendResp({ true, req_id, std::move(rsp) });

	return true;
}


bool Client::Impl::OnGetRouteDirection_Rsp(std::uint32_t req_id, const unsigned char* msg, std::uint32_t len)
{
	tDTablePointerWord <(int)PAPI_ParamReg::Max> p;

	Log(LogLevel::Verbose, req_id, _strl("OnGetRouteDirection_Rsp"));

	p.Parse((byte*)msg, len);

	auto rsp = pl::pl_api::make_object<pl::pl_api::GetRouteDirection_Rsp>();


	if (p.data[(int)PAPI_ParamReg::rsp].len == 2)
		rsp->rsp = *(std::uint16_t*)p.data[(int)PAPI_ParamReg::rsp].msg;

	if (p.data[(int)PAPI_ParamReg::direction].len)
		rsp->direction = *p.data[(int)PAPI_ParamReg::direction].msg;

	Log(LogLevel::Verbose, req_id, _strl("Send GetRouteDirection_Rsp"));

	SendResp({ true, req_id, std::move(rsp) });

	return true;
}

bool Client::Impl::OnSetRouteDirection_Rsp(std::uint32_t req_id, const unsigned char* msg, std::uint32_t len)
{
	tDTablePointerWord <(int)PAPI_ParamReg::Max> p;

	Log(LogLevel::Verbose, req_id, _strl("OnSetRouteDirection_Rsp"));

	p.Parse((byte*)msg, len);

	auto rsp = pl::pl_api::make_object<pl::pl_api::SetRouteDirection_Rsp>();


	if (p.data[(int)PAPI_ParamReg::rsp].len == 2)
		rsp->rsp = *(std::uint16_t*)p.data[(int)PAPI_ParamReg::rsp].msg;

	Log(LogLevel::Verbose, req_id, _strl("Send SetRouteDirection_Rsp"));

	SendResp({ true, req_id, std::move(rsp) });

	return true;
}



bool Client::Impl::OnPingRsp(std::uint32_t req_id, const unsigned char* msg, std::uint32_t len)
{
	tDTablePointerWord <(int)PAPI_ParamService::Max> p;

	Log(LogLevel::Verbose, req_id, _strl("OnPingRsp"));

	p.Parse((byte*)msg, len);

	auto _rsp = pl::pl_api::make_object<pl::pl_api::Ping_Rsp>();

	_rsp->rsp = 1;

	if (p.data[(int)PAPI_ParamService::rsp].len == 2)
	{
		_rsp->rsp = *(std::uint16_t*)p.data[(int)PAPI_ParamService::rsp].msg;
	}

	Log(LogLevel::Verbose, req_id, _strl("Send Ping_Rsp"));

	SendResp({ true, req_id, std::move(_rsp) });

	return true;
}

bool Client::Impl::OnPingInd(std::uint32_t req_id, const unsigned char* msg, std::uint32_t len)
{
	tDTablePointerWord <(int)PAPI_ParamService::Max> p;

	Log(LogLevel::Verbose, req_id, _strl("OnPingInd"));

	p.Parse((byte*)msg, len);

	auto _rsp = pl::pl_api::make_object<pl::pl_api::Ping_Ind>();


	Log(LogLevel::Verbose, req_id, _strl("Send Ping_Ind"));

	SendResp({ true, req_id, std::move(_rsp) });

	return true;
}

bool Client::Impl::OnPingExInd(std::uint32_t req_id, const unsigned char* msg, std::uint32_t len)
{






	PAPI_STREAM_HEADER *header;
	tDPrimitveTLV <512> _msg;

	if (!bin_stream)
	{
		return true;
	}
	Client::Impl::PConnectState state = GetConnectState();
	if ((state == PConnectState::offline) || (state == PConnectState::start) || (state == PConnectState::wait_connect) || (state == PConnectState::wait_disconnect))
	{
		return true;
	}

	header = (PAPI_STREAM_HEADER*)_msg.SetHeader(sizeof(PAPI_STREAM_HEADER));
	header->ver = 0;
	header->service = (byte)PAPI_Service::service;
	header->command_id = (word)PAPI_ComService::ping_ex_rsp;
	header->req_id = (unsigned long)req_id;

	_msg.AddwTwLV((word)PAPI_ParamService::num, _stds_pl_w(number));

	if (!bin_stream(_msg, _msg.Len()))
	{

		return true;
	}






	return true;
}

bool Client::Impl::OnTestMsgRsp(std::uint32_t req_id, const unsigned char* msg, std::uint32_t len)
{
	tDTablePointerWord <(int)PAPI_ParamService::Max> p;



	p.Parse((byte*)msg, len);

	auto _rsp = pl::pl_api::make_object<pl::pl_api::TestMsg_Rsp>();

	if (p.data[(int)PAPI_ParamService::rsp].len == 2)
		_rsp->rsp = *(std::uint16_t*)p.data[(int)PAPI_ParamService::rsp].msg;

	if (p.data[(int)PAPI_ParamService::id].len == 4)
		_rsp->req_id = *(std::uint32_t*)p.data[(int)PAPI_ParamService::id].msg;

	if (p.data[(int)PAPI_ParamService::data].len)
	{
		if (p.data[(int)PAPI_ParamService::data].len <= PAPI_TestFragmetMaxSize)
		{
			memcpy(_rsp->data, DTP_PPARAMETER(PAPI_ParamService::data));
			_rsp->len = p.data[(int)PAPI_ParamService::data].len;
		}
	}



	SendResp({ true, req_id, std::move(_rsp) });

	return true;
}

bool Client::Impl::OnTestMsgInd(std::uint32_t req_id, const unsigned char* msg, std::uint32_t len)
{
	tDTablePointerWord <(int)PAPI_ParamService::Max> p;



	p.Parse((byte*)msg, len);

	auto _rsp = pl::pl_api::make_object<pl::pl_api::TestMsg_Ind>();


	if (p.data[(int)PAPI_ParamService::id].len == 4)
		_rsp->req_id = *(std::uint32_t*)p.data[(int)PAPI_ParamService::id].msg;

	if (p.data[(int)PAPI_ParamService::data].len)
	{
		if (p.data[(int)PAPI_ParamService::data].len <= PAPI_TestFragmetMaxSize)
		{
			memcpy(_rsp->data, DTP_PPARAMETER(PAPI_ParamService::data));
			_rsp->len = p.data[(int)PAPI_ParamService::data].len;
		}
	}

	if (p.data[(int)PAPI_ParamService::loop].len)
		_rsp->loop = *p.data[(int)PAPI_ParamService::id].msg != 0;



	SendResp({ true, req_id, std::move(_rsp) });

	return true;
}

bool Client::Impl::OnTestMsgExInd(std::uint32_t req_id, const unsigned char* msg, std::uint32_t len)
{
	tDTablePointerWord <(int)PAPI_ParamService::Max> p;
	PAPI_STREAM_HEADER *header;
	tDPrimitveTLV <65535> _msg;
	bool loop = false;




	p.Parse((byte*)msg, len);

	header = (PAPI_STREAM_HEADER*)_msg.SetHeader(sizeof(PAPI_STREAM_HEADER));
	header->ver = 0;
	header->service = (byte)PAPI_Service::service;
	header->command_id = (word)PAPI_ComService::test_msg_ex_rsp;
	header->req_id = (unsigned long)req_id;

	_msg.AddwTwLV((word)PAPI_ParamService::num, _stds_pl_w(number));

	if (p.data[(int)PAPI_ParamService::id].len == 4)
		_msg.AddwTwLV((word)PAPI_ParamService::id, DTP_PPARAMETER(PAPI_ParamService::id));

 	if (p.data[(int)PAPI_ParamService::loop].len)
 		loop = *p.data[(int)PAPI_ParamService::id].msg != 0;

	if (loop)
	{
		if (p.data[(int)PAPI_ParamService::data].len)
			_msg.AddwTwLV((word)PAPI_ParamService::data, DTP_PPARAMETER(PAPI_ParamService::data));
	}





	return true;
}




bool Client::Impl::OnDialRsp(std::uint32_t req_id, const unsigned char* msg, std::uint32_t len)
{
	tDTablePointerWord <(int)PAPI_ParamDial::Max> p;

	Log(LogLevel::Verbose, req_id, _strl("OnDialRsp"));

	p.Parse((byte*)msg, len);

	auto _rsp = pl::pl_api::make_object<pl::pl_api::Dial_Rsp>();

	if (p.data[(int)PAPI_ParamDial::rsp].len == 2)
		_rsp->rsp  = *(std::uint16_t*)p.data[(int)PAPI_ParamDial::rsp].msg;

	_rsp->snum.assign((char*)DTP_PPARAMETERB(PAPI_ParamDial::sour_num));
	_rsp->dnum.assign((char*)DTP_PPARAMETERB(PAPI_ParamDial::dest_num));

	_rsp->voice_server.assign((char*)DTP_PPARAMETERB(PAPI_ParamDial::voice_server));
	_rsp->call_id.assign((char*)DTP_PPARAMETERB(PAPI_ParamDial::call_id));

	Log(LogLevel::Verbose, req_id, _strl("Send PAPI_ParamDial"));

	SendResp({ true, req_id, std::move(_rsp) });

	return true;
}

bool Client::Impl::OnDialInd(std::uint32_t req_id, const unsigned char* msg, std::uint32_t len)
{
	tDTablePointerWord <(int)PAPI_ParamDial::Max> p;

	Log(LogLevel::Verbose, req_id, _strl("OnDialInd"));

	p.Parse((byte*)msg, len);

	auto _rsp = pl::pl_api::make_object<pl::pl_api::Dial_Ind>();

	_rsp->snum.assign((char*)DTP_PPARAMETERB(PAPI_ParamDial::sour_num));
	_rsp->dnum.assign((char*)DTP_PPARAMETERB(PAPI_ParamDial::dest_num));

	_rsp->voice_server.assign((char*)DTP_PPARAMETERB(PAPI_ParamDial::voice_server));

	_rsp->call_id.assign((char*)DTP_PPARAMETERB(PAPI_ParamDial::call_id));

	Log(LogLevel::Verbose, req_id, _strl("Send Dial_Ind"));

	SendResp({ true, req_id, std::move(_rsp) });

	return true;
}

bool Client::Impl::OnCancelRsp(std::uint32_t req_id, const unsigned char* msg, std::uint32_t len)
{
	tDTablePointerWord <(int)PAPI_ParamDial::Max> p;

	Log(LogLevel::Verbose, req_id, _strl("OnCancelRsp"));

	p.Parse((byte*)msg, len);

	auto _rsp = pl::pl_api::make_object<pl::pl_api::Cancel_Rsp>();

	if (p.data[(int)PAPI_ParamDial::rsp].len == 2)
		_rsp->rsp = *(std::uint16_t*)p.data[(int)PAPI_ParamDial::rsp].msg;

	_rsp->snum.assign((char*)DTP_PPARAMETERB(PAPI_ParamDial::sour_num));
	_rsp->dnum.assign((char*)DTP_PPARAMETERB(PAPI_ParamDial::dest_num));


	_rsp->call_id.assign((char*)DTP_PPARAMETERB(PAPI_ParamDial::call_id));

	Log(LogLevel::Verbose, req_id, _strl("Send PAPI_ParamCancel"));

	SendResp({ true, req_id, std::move(_rsp) });

	return true;
}

bool Client::Impl::OnPushClearCallsRsp(std::uint32_t req_id, const unsigned char* msg, std::uint32_t len)
{
	tDTablePointerWord <(int)PAPI_ParamDial::Max> p;

	Log(LogLevel::Verbose, req_id, _strl("OnPushClearCallsRsp"));

	p.Parse((byte*)msg, len);

	auto _rsp = pl::pl_api::make_object<pl::pl_api::PushClearCalls_Rsp>();

	if (p.data[(int)PAPI_ParamDial::rsp].len == 2)
		_rsp->rsp = *(std::uint16_t*)p.data[(int)PAPI_ParamDial::rsp].msg;

	Log(LogLevel::Verbose, req_id, _strl("Send PushClearCalls_Rsp"));

	SendResp({ true, req_id, std::move(_rsp) });

	return true;
}

bool Client::Impl::OnCallsBadgeRsp(std::uint32_t req_id, const unsigned char* msg, std::uint32_t len)
{
	tDTablePointerWord <(int)PAPI_ParamDial::Max> p;

	Log(LogLevel::Verbose, req_id, _strl("OnCallsBadgeRsp"));

	p.Parse((byte*)msg, len);

	auto _rsp = pl::pl_api::make_object<pl::pl_api::CallsBadge_Rsp>();

	if (p.data[(int)PAPI_ParamDial::rsp].len == 2)
		_rsp->rsp = *(std::uint16_t*)p.data[(int)PAPI_ParamDial::rsp].msg;
	if (p.data[(int)PAPI_ParamDial::badge].len == 4)
		_rsp->badge = *(std::uint32_t*)p.data[(int)PAPI_ParamDial::badge].msg;

	Log(LogLevel::Verbose, req_id, _strl("Send CallsBadge_Rsp"));

	SendResp({ true, req_id, std::move(_rsp) });

	return true;
}



bool Client::Impl::OnMsgRsp(std::uint32_t req_id, const unsigned char* msg, std::uint32_t len)
{
	tDTablePointerWord <(int)PAPI_ParamMsg::Max> p;

	Log(LogLevel::Verbose, req_id, _strl("OnMsgRsp"));

	p.Parse((byte*)msg, len);

	auto _rsp = pl::pl_api::make_object<pl::pl_api::Msg_Rsp>();

	_rsp->rsp = 1;

	if (p.data[(int)PAPI_ParamMsg::rsp].len == 2)
	{




		_rsp->rsp  = *(std::uint16_t*)p.data[(int)PAPI_ParamMsg::rsp].msg;
	}

	if (p.data[(int)PAPI_ParamMsg::chat_id].len == 8)
		_rsp->chat_id = *(std::uint64_t*)p.data[(int)PAPI_ParamMsg::chat_id].msg;
	if (p.data[(int)PAPI_ParamMsg::sm_id].len == 8)
		_rsp->sm_id = *(std::uint64_t*)p.data[(int)PAPI_ParamMsg::sm_id].msg;
	if (p.data[(int)PAPI_ParamMsg::msg_index].len == 8)
		_rsp->msg_index = *(std::uint64_t*)p.data[(int)PAPI_ParamMsg::msg_index].msg;


	_rsp->snum.assign((char*)DTP_PPARAMETERB(PAPI_ParamMsg::sour_num));
	_rsp->dnum.assign((char*)DTP_PPARAMETERB(PAPI_ParamMsg::dest_num));

	_rsp->submit_date_time.assign((char*)DTP_PPARAMETERB(PAPI_ParamMsg::submit_date_time));

	Log(LogLevel::Verbose, req_id, _strl("Send Msg_Rsp"));

	SendResp({ true, req_id, std::move(_rsp) });

	return true;
}

bool Client::Impl::OnMsgReq(std::uint32_t req_id, const unsigned char* msg, std::uint32_t len)
{
	tDTablePointerWord <(int)PAPI_ParamMsg::Max> p;

	Log(LogLevel::Verbose, req_id, _strl("OnMsgReq"));

	p.Parse((byte*)msg, len);

	auto rsp = pl::pl_api::make_object<pl::pl_api::Msg_Ind>();

	rsp->snum.assign((char*)DTP_PPARAMETERB(PAPI_ParamMsg::sour_num));
	rsp->dnum.assign((char*)DTP_PPARAMETERB(PAPI_ParamMsg::dest_num));

	rsp->submit_date_time.assign((char*)DTP_PPARAMETERB(PAPI_ParamMsg::submit_date_time));


	if (p.data[(int)PAPI_ParamMsg::chat_private].len)
		rsp->chat_private = *p.data[(int)PAPI_ParamMsg::chat_private].msg;
	if (p.data[(int)PAPI_ParamMsg::channel].len)
		rsp->channel = *p.data[(int)PAPI_ParamMsg::channel].msg;
	if (p.data[(int)PAPI_ParamMsg::chat_id].len == 8)
		rsp->chat_id = *(std::uint64_t*)p.data[(int)PAPI_ParamMsg::chat_id].msg;
	if (p.data[(int)PAPI_ParamMsg::msg_index].len == 8)
		rsp->msg_index = *(std::uint64_t*)p.data[(int)PAPI_ParamMsg::msg_index].msg;
	if (p.data[(int)PAPI_ParamMsg::sm_id].len == 8)
		rsp->sm_id = *(std::uint64_t*)p.data[(int)PAPI_ParamMsg::sm_id].msg;

	if (p.data[(int)PAPI_ParamMsg::file_type].len)
	{
		rsp->file_type = *p.data[(int)PAPI_ParamMsg::file_type].msg;
		if (p.data[(int)PAPI_ParamMsg::file_hash].len == 4)
			rsp->file_hash = *(std::uint32_t*)p.data[(int)PAPI_ParamMsg::file_hash].msg;
	}


		rsp->text.assign((char*)DTP_PPARAMETER(PAPI_ParamMsg::text));



	Log(LogLevel::Verbose, req_id, _strl("Send Msg_Ind"));

	SendResp({ true, req_id, std::move(rsp) });

	return true;
}

bool Client::Impl::OnRepRsp(std::uint32_t req_id, const unsigned char* msg, std::uint32_t len)
{
	tDTablePointerWord <(int)PAPI_ParamMsg::Max> p;

	Log(LogLevel::Verbose, req_id, _strl("OnRepRsp"));

	p.Parse((byte*)msg, len);

	auto _rsp = pl::pl_api::make_object<pl::pl_api::Rep_Rsp>();

	_rsp->rsp = 1;

	if (p.data[(int)PAPI_ParamMsg::rsp].len == 2)
	{




		_rsp->rsp  = *(word*)p.data[(int)PAPI_ParamMsg::rsp].msg;
	}

	if (p.data[(int)PAPI_ParamMsg::chat_id].len == 8)
		_rsp->chat_id = *(std::uint64_t*)p.data[(int)PAPI_ParamMsg::chat_id].msg;
	if (p.data[(int)PAPI_ParamMsg::sm_id].len == 8)
		_rsp->sm_id = *(std::uint64_t*)p.data[(int)PAPI_ParamMsg::sm_id].msg;
	if (p.data[(int)PAPI_ParamMsg::msg_index].len == 8)
		_rsp->msg_index = *(std::uint64_t*)p.data[(int)PAPI_ParamMsg::msg_index].msg;


	_rsp->snum.assign((char*)DTP_PPARAMETERB(PAPI_ParamMsg::sour_num));
	_rsp->dnum.assign((char*)DTP_PPARAMETERB(PAPI_ParamMsg::dest_num));

	Log(LogLevel::Verbose, req_id, _strl("Send Rep_Rsp"));

	SendResp({ true, req_id, std::move(_rsp) });

	return true;
}

bool Client::Impl::OnRepReq(std::uint32_t req_id, const unsigned char* msg, std::uint32_t len)
{
 	tDTablePointerWord <(int)PAPI_ParamMsg::Max> p;
 
	Log(LogLevel::Verbose, req_id, _strl("OnRepReq"));

	p.Parse((byte*)msg, len);
 
	auto rsp = pl::pl_api::make_object<pl::pl_api::Rep_Ind>();

	rsp->snum.assign((char*)DTP_PPARAMETERB(PAPI_ParamMsg::sour_num));
	rsp->dnum.assign((char*)DTP_PPARAMETERB(PAPI_ParamMsg::dest_num));



	if (p.data[(int)PAPI_ParamMsg::chat_id].len == 8)
		rsp->chat_id = *(std::uint64_t*)p.data[(int)PAPI_ParamMsg::chat_id].msg;
	if (p.data[(int)PAPI_ParamMsg::msg_index].len == 8)
		rsp->msg_index = *(std::uint64_t*)p.data[(int)PAPI_ParamMsg::msg_index].msg;
	if (p.data[(int)PAPI_ParamMsg::sm_id].len == 8)
		rsp->sm_id = *(std::uint64_t*)p.data[(int)PAPI_ParamMsg::sm_id].msg;

	if (p.data[(int)PAPI_ParamMsg::rep_status].len)
		rsp->status = *p.data[(int)PAPI_ParamMsg::rep_status].msg;

	rsp->date_time.assign((char*)DTP_PPARAMETERB(PAPI_ParamMsg::date_time));

	Log(LogLevel::Verbose, req_id, _strl("Send Rep_Ind"));

	SendResp({ true, req_id, std::move(rsp) });

	return true;
}

bool Client::Impl::OnDelMsgRsp(std::uint32_t req_id, const unsigned char* msg, std::uint32_t len)
{
	tDTablePointerWord <(int)PAPI_ParamMsg::Max> p;

	Log(LogLevel::Verbose, req_id, _strl("OnDelMsgRsp"));

	p.Parse((byte*)msg, len);

	auto _rsp = pl::pl_api::make_object<pl::pl_api::DelMsg_Rsp>();

	_rsp->rsp = 1;

	if (p.data[(int)PAPI_ParamMsg::rsp].len == 2)
		_rsp->rsp  = *(word*)p.data[(int)PAPI_ParamMsg::rsp].msg;

	if (p.data[(int)PAPI_ParamMsg::chat_id].len == 8)
		_rsp->chat_id = *(std::uint64_t*)p.data[(int)PAPI_ParamMsg::chat_id].msg;
	if (p.data[(int)PAPI_ParamMsg::sm_id].len == 8)
		_rsp->sm_id = *(std::uint64_t*)p.data[(int)PAPI_ParamMsg::sm_id].msg;
	if (p.data[(int)PAPI_ParamMsg::msg_index].len == 8)
		_rsp->msg_index = *(std::uint64_t*)p.data[(int)PAPI_ParamMsg::msg_index].msg;


	Log(LogLevel::Verbose, req_id, _strl("Send DelMsg_Rsp"));

	SendResp({ true, req_id, std::move(_rsp) });

	return true;
}

bool Client::Impl::OnTypingReq(std::uint32_t req_id, const unsigned char* msg, std::uint32_t len)
{
	tDTablePointerWord <(int)PAPI_ParamMsg::Max> p;

	Log(LogLevel::Verbose, req_id, _strl("OnTypingReq"));

	p.Parse((byte*)msg, len);

	auto rsp = pl::pl_api::make_object<pl::pl_api::Typing_Ind>();

	rsp->snum.assign((char*)DTP_PPARAMETERB(PAPI_ParamMsg::sour_num));

	if (p.data[(int)PAPI_ParamMsg::chat_id].len == 8)
		rsp->chat_id = *(std::uint64_t*)p.data[(int)PAPI_ParamMsg::chat_id].msg;

	if (p.data[(int)PAPI_ParamMsg::typing_type].len)
		rsp->type = *p.data[(int)PAPI_ParamMsg::typing_type].msg;

	Log(LogLevel::Verbose, req_id, _strl("Send Typing_Ind"));

	SendResp({ true, req_id, std::move(rsp) });

	return true;
}


bool Client::Impl::OnBL_Add_Rsp(std::uint32_t req_id, const unsigned char* msg, std::uint32_t len)
{
	tDTablePointerWord <(int)PAPI_ParamBlackList::Max> p;

	Log(LogLevel::Verbose, req_id, _strl("OnBL_Add_Rsp"));

	p.Parse((byte*)msg, len);

	auto _rsp = pl::pl_api::make_object<pl::pl_api::BL_Add_Rsp>();

	_rsp->rsp = 1;

	switch(p.data[(int)PAPI_ParamBlackList::rsp].len)
	{
		case 1:
		{
			_rsp->rsp  = *p.data[(int)PAPI_ParamBlackList::rsp].msg;
			break;
		}
		case 2:
		{
			_rsp->rsp = *(word*)p.data[(int)PAPI_ParamBlackList::rsp].msg;
			break;
		}
	}

	Log(LogLevel::Verbose, req_id, _strl("Send BL_Add_Rsp"));

	SendResp({ true, req_id, std::move(_rsp) });

	return true;
}

bool Client::Impl::OnBL_Del_Rsp(std::uint32_t req_id, const unsigned char* msg, std::uint32_t len)
{
	tDTablePointerWord <(int)PAPI_ParamBlackList::Max> p;

	Log(LogLevel::Verbose, req_id, _strl("OnBL_Del_Rsp"));

	p.Parse((byte*)msg, len);

	auto _rsp = pl::pl_api::make_object<pl::pl_api::BL_Del_Rsp>();

	_rsp->rsp = 1;

	switch(p.data[(int)PAPI_ParamBlackList::rsp].len)
	{
		case 1:
		{
			_rsp->rsp  = *p.data[(int)PAPI_ParamBlackList::rsp].msg;
			break;
		}
		case 2:
		{
			_rsp->rsp = *(word*)p.data[(int)PAPI_ParamBlackList::rsp].msg;
			break;
		}
	}

	Log(LogLevel::Verbose, req_id, _strl("Send BL_Del_Rsp"));

	SendResp({ true, req_id, std::move(_rsp) });

	return true;
}

bool Client::Impl::OnBL_Get_Rsp_List(std::vector<std::string> &list, const unsigned char* msg, std::uint32_t len)
{
	word pn;
	word pl;
	while (len)
	{
		if (len < 4)
		{
			break;
		}
		
		pn = *(word*)msg;
		msg += 2;
		len -= 2;
		
		pl = *(word*)msg;
		msg += 2;
		len -= 2;
		if (len < pl)
			break;

		switch (pn)
		{
			case (word)PAPI_BlackListParamList::num:
			{
				list.emplace_back((char*)msg,pl);
				break;
			}
		}

		msg += pl;
		len -= pl;
	}

	return true;
	return true;
}

bool Client::Impl::OnBL_Get_Rsp(std::uint32_t req_id, const unsigned char* msg, std::uint32_t len)
{
	tDTablePointerWord <(int)PAPI_ParamBlackList::Max> p;

	Log(LogLevel::Verbose, req_id, _strl("OnBL_Get_Rsp"));

	p.Parse((byte*)msg, len);

	auto _rsp = pl::pl_api::make_object<pl::pl_api::BL_Get_Rsp>();

	_rsp->rsp = 1;

	switch(p.data[(int)PAPI_ParamBlackList::rsp].len)
	{
		case 1:
		{
			_rsp->rsp  = *p.data[(int)PAPI_ParamBlackList::rsp].msg;
			break;
		}
		case 2:
		{
			_rsp->rsp = *(word*)p.data[(int)PAPI_ParamBlackList::rsp].msg;
			break;
		}
	}

	if (p.data[(int)PAPI_ParamBlackList::list].len)
		OnBL_Get_Rsp_List(_rsp->list, DTP_PPARAMETER(PAPI_ParamBlackList::list));

	Log(LogLevel::Verbose, req_id, _strl("Send BL_Get_Rsp"));

	SendResp({ true, req_id, std::move(_rsp) });

	return true;
}



bool Client::Impl::OnCS_PutMe_Rsp(std::uint32_t req_id, const unsigned char* msg, std::uint32_t len)
{
	tDTablePointerWord <(int)PAPI_ParamCS::Max> p;


	Log(LogLevel::Verbose, req_id, _strl("OnCS_PutMe_Rsp"));

	p.Parse((byte*)msg, len);

	auto rsp = pl::pl_api::make_object<pl::pl_api::CS_PutMe_Rsp>();

	if (p.data[(int)PAPI_ParamCS::rsp].len == 2)
		rsp->rsp = *(std::uint16_t*)p.data[(int)PAPI_ParamCS::rsp].msg;






	Log(LogLevel::Verbose, req_id, _strl("Send LogoCS_PutMe_Rsput_Rsp"));

	SendResp({ true, req_id, std::move(rsp) });

	return true;
}

bool Client::Impl::OnCS_GetMe_Rsp(std::uint32_t req_id, const unsigned char* msg, std::uint32_t len)
{
	tDTablePointerWord <(int)PAPI_ParamCS::Max> p;


	Log(LogLevel::Verbose, req_id, _strl("OnCS_GetMe_Rsp"));

	p.Parse((byte*)msg, len);

	auto rsp = pl::pl_api::make_object<pl::pl_api::CS_GetMe_Rsp>();

	if (p.data[(int)PAPI_ParamCS::rsp].len == 2)
		rsp->rsp = *(std::uint16_t*)p.data[(int)PAPI_ParamCS::rsp].msg;

	rsp->name.assign((char*)DTP_PPARAMETER(PAPI_ParamCS::name));
	rsp->avatar.assign((char*)DTP_PPARAMETER(PAPI_ParamCS::avatar));

	if (p.data[(int)PAPI_ParamCS::pinboot].len)
		rsp->pinbot = *p.data[(int)PAPI_ParamCS::pinboot].msg;






	Log(LogLevel::Verbose, req_id, _strl("Send CS_GetMe_Rsp"));

	SendResp({ true, req_id, std::move(rsp) });

	return true;
}


bool Client::Impl::OnCS_GetProfileRsp_Rsp_Contacts(std::vector<Contact> &contacts, bool &last, unsigned char* msg, std::uint32_t len)
{
	word pn;
	word pl;
	while (len)
	{
		if (len < 4)
		{
			break;
		}
		
		pn = *(word*)msg;
		msg += 2;
		len -= 2;
		
		pl = *(word*)msg;
		msg += 2;
		len -= 2;
		if (len < pl)
			break;

		switch (pn)
		{
			case (word)PAPI_ParamCS_Contacts::contact:
			{
				pl::Contact c;
				tDTablePointerWord <(int)PAPI_ParamCS_Contact::Max> p;
				p.Parse((byte*)msg, pl);

				c.num.assign((char*)DTP_PPARAMETER(PAPI_ParamCS_Contact::num));
				c.rnum.assign((char*)DTP_PPARAMETER(PAPI_ParamCS_Contact::rnum));
				c.name.assign((char*)DTP_PPARAMETER(PAPI_ParamCS_Contact::name));
				c.fname.assign((char*)DTP_PPARAMETER(PAPI_ParamCS_Contact::fname));
				c.lname.assign((char*)DTP_PPARAMETER(PAPI_ParamCS_Contact::lname));
				c.avatar.assign((char*)DTP_PPARAMETER(PAPI_ParamCS_Contact::avatar));

				if (p.data[(int)PAPI_ParamCS_Contact::messenger].len)
					c.messenger = *p.data[(int)PAPI_ParamCS_Contact::messenger].msg != 0;

				if (p.data[(int)PAPI_ParamCS_Contact::hindex].len == 8)
					c.hindex = *(std::uint64_t*)p.data[(int)PAPI_ParamCS_Contact::hindex].msg;


				contacts.push_back(std::move(c));

				break;
			}
			case (word)PAPI_ParamCS_Contacts::last:
			{
				if (pl)

					last = *msg != 0;
				break;
			}
		}

		msg += pl;
		len -= pl;
	}

	return true;
}

bool Client::Impl::OnCS_GetProfileRsp_Rsp(std::uint32_t req_id, const unsigned char* msg, std::uint32_t len)
{
	tDTablePointerWord <(int)PAPI_ParamCS::Max> p;


	Log(LogLevel::Verbose, req_id, _strl("OnCS_GetProfileRsp_Rsp"));

	p.Parse((byte*)msg, len);

	auto rsp = pl::pl_api::make_object<pl::pl_api::CS_GetProfiles_Rsp>();

	if (p.data[(int)PAPI_ParamCS::rsp].len == 2)
		rsp->rsp = *(std::uint16_t*)p.data[(int)PAPI_ParamCS::rsp].msg;









	if (p.data[(int)PAPI_ParamCS::contacts].len)
	{

		OnCS_GetProfileRsp_Rsp_Contacts(rsp->contacts,rsp->last, DTP_PPARAMETER(PAPI_ParamCS::contacts));
	}


	Log(LogLevel::Verbose, req_id, _strl("Send CS_GetProfiles_Rsp"));

	SendResp({ true, req_id, std::move(rsp) });

	return true;
}

bool Client::Impl::OnCS_GetContactsRsp_Rsp(std::uint32_t req_id, const unsigned char* msg, std::uint32_t len)
{
	tDTablePointerWord <(int)PAPI_ParamCS::Max> p;

	Log(LogLevel::Verbose, req_id, _strl("OnCS_GetContactsRsp_Rsp"));

	p.Parse((byte*)msg, len);

	auto rsp = pl::pl_api::make_object<pl::pl_api::CS_GetContacts_Rsp>();

	if (p.data[(int)PAPI_ParamCS::rsp].len == 2)
		rsp->rsp = *(std::uint16_t*)p.data[(int)PAPI_ParamCS::rsp].msg;

	if (p.data[(int)PAPI_ParamCS::contacts].len)
		OnCS_GetProfileRsp_Rsp_Contacts(rsp->contacts,rsp->last, DTP_PPARAMETER(PAPI_ParamCS::contacts));


	Log(LogLevel::Verbose, req_id, _strl("Send CS_GetContacts_Rsp"));

	SendResp({ true, req_id, std::move(rsp) });

	return true;
}

bool Client::Impl::OnCS_GetContactsHistoryRsp_Rsp(std::uint32_t req_id, const unsigned char* msg, std::uint32_t len)
{
	tDTablePointerWord <(int)PAPI_ParamCS::Max> p;

	Log(LogLevel::Verbose, req_id, _strl("OnCS_GetContactsHistoryRsp_Rsp"));

	p.Parse((byte*)msg, len);

	auto rsp = pl::pl_api::make_object<pl::pl_api::CS_GetContactsHistory_Rsp>();

	if (p.data[(int)PAPI_ParamCS::rsp].len == 2)
		rsp->rsp = *(std::uint16_t*)p.data[(int)PAPI_ParamCS::rsp].msg;

	if (p.data[(int)PAPI_ParamCS::contacts].len)
		OnCS_GetProfileRsp_Rsp_Contacts(rsp->contacts, rsp->last, DTP_PPARAMETER(PAPI_ParamCS::contacts));


	Log(LogLevel::Verbose, req_id, _strl("Send CS_GetContactsHistory_Rsp"));

	SendResp({ true, req_id, std::move(rsp) });

	return true;
}

bool Client::Impl::OnCS_PutContacts_Rsp(std::uint32_t req_id, const unsigned char* msg, std::uint32_t len)
{
	tDTablePointerWord <(int)PAPI_ParamCS::Max> p;


	Log(LogLevel::Verbose, req_id, _strl("OnCS_PutContacts_Rsp"));

	p.Parse((byte*)msg, len);

	auto rsp = pl::pl_api::make_object<pl::pl_api::CS_PutContacts_Rsp>();

	if (p.data[(int)PAPI_ParamCS::rsp].len == 2)
		rsp->rsp = *(std::uint16_t*)p.data[(int)PAPI_ParamCS::rsp].msg;

	if (p.data[(int)PAPI_ParamCS::contacts].len)
	{
		bool last;
		OnCS_GetProfileRsp_Rsp_Contacts(rsp->contacts, last, DTP_PPARAMETER(PAPI_ParamCS::contacts));
	}






	Log(LogLevel::Verbose, req_id, _strl("Send CS_PutContacts_Rsp"));

	SendResp({ true, req_id, std::move(rsp) });

	return true;
}

bool Client::Impl::OnCS_DelContacts_Rsp(std::uint32_t req_id, const unsigned char* msg, std::uint32_t len)
{
	tDTablePointerWord <(int)PAPI_ParamCS::Max> p;


	Log(LogLevel::Verbose, req_id, _strl("OnCS_DelContacts_Rsp"));

	p.Parse((byte*)msg, len);

	auto rsp = pl::pl_api::make_object<pl::pl_api::CS_DelContacts_Rsp>();

	if (p.data[(int)PAPI_ParamCS::rsp].len == 2)
		rsp->rsp = *(std::uint16_t*)p.data[(int)PAPI_ParamCS::rsp].msg;






	Log(LogLevel::Verbose, req_id, _strl("Send CS_DelContacts_Rsp"));

	SendResp({ true, req_id, std::move(rsp) });

	return true;
}



bool Client::Impl::OnChatCheckListRsp(std::uint32_t req_id, const unsigned char* msg, std::uint32_t len)
{
	tDTablePointerWord <(int)PAPI_ParamChat::Max> p;

	Log(LogLevel::Verbose, req_id, _strl("OnChatCheckListRsp"));

	p.Parse((byte*)msg, len);

	auto rsp = pl::pl_api::make_object<pl::pl_api::Chat_CheckList_Rsp>();

	if (p.data[(int)PAPI_ParamChat::rsp].len == 2)
		rsp->rsp = *(std::uint16_t*)p.data[(int)PAPI_ParamChat::rsp].msg;

	Log(LogLevel::Verbose, req_id, _strl("Send Chat_CheckList_Rsp"));

	SendResp({ true, req_id, std::move(rsp) });

	return true;
}

bool Client::Impl::OnChatGetListRsp_ListNums(std::vector<std::string> &nums, unsigned char* msg, std::uint32_t len)
{
	word pn;
	word pl;
	while (len)
	{
		if (len < 4)
		{
			break;
		}
		
		pn = *(word*)msg;
		msg += 2;
		len -= 2;
		
		pl = *(word*)msg;
		msg += 2;
		len -= 2;
		if (len < pl)
			break;

		switch (pn)
		{
			case (word)PAPI_ParamChatNums::num:
			{
				if (pl)
				{
					std::string num;
					num.assign((char*)msg, pl);
					nums.push_back(std::move(num));
				}

				break;
			}
		}

		msg += pl;
		len -= pl;
	}

	return true;
}

bool Client::Impl::OnChatGetListRsp_Msgs(std::vector<pl::Msg> &msgs, unsigned char* msg, std::uint32_t len)
{
	word pn;
	word pl;
	while (len)
	{
		if (len < 4)
		{
			break;
		}
		
		pn = *(word*)msg;
		msg += 2;
		len -= 2;
		
		pl = *(word*)msg;
		msg += 2;
		len -= 2;
		if (len < pl)
			break;

		switch (pn)
		{
			case (word)PAPI_ParamChatMsgs::msg:
			{
				if (pl)
				{
					pl::Msg _msg;
					tDTablePointerWord <(int)PAPI_ParamChatMsg::Max> p;
					p.Parse((byte*)msg, pl);

					if (p.data[(int)PAPI_ParamChatMsg::msg_index].len == 8)
						_msg.msg_index = *(std::uint64_t*)p.data[(int)PAPI_ParamChatMsg::msg_index].msg;
					if (p.data[(int)PAPI_ParamChatMsg::sm_id].len == 8)
						_msg.sm_id = *(std::uint64_t*)p.data[(int)PAPI_ParamChatMsg::sm_id].msg;

					if (p.data[(int)PAPI_ParamChatMsg::state].len)
						_msg.state = *p.data[(int)PAPI_ParamChatMsg::state].msg;
					if (p.data[(int)PAPI_ParamChatMsg::type].len)
						_msg.type = *p.data[(int)PAPI_ParamChatMsg::type].msg;
					if (p.data[(int)PAPI_ParamChatMsg::status].len)
						_msg.status = *p.data[(int)PAPI_ParamChatMsg::status].msg;
					if (p.data[(int)PAPI_ParamChatMsg::service].len)
						_msg.service = *p.data[(int)PAPI_ParamChatMsg::service].msg;

					if (p.data[(int)PAPI_ParamChatMsg::file_type].len)
						_msg.file_type = *p.data[(int)PAPI_ParamChatMsg::file_type].msg;
					if (p.data[(int)PAPI_ParamChatMsg::file_hash].len)
						_msg.file_hash = *(std::uint32_t*)p.data[(int)PAPI_ParamChatMsg::file_hash].msg;

					if (p.data[(int)PAPI_ParamChatMsg::sour_addr].len)
						_msg.sour_num.assign((char*)DTP_PPARAMETER(PAPI_ParamChatMsg::sour_addr));
					if (p.data[(int)PAPI_ParamChatMsg::sour_addr].len)
						_msg.dest_num.assign((char*)DTP_PPARAMETER(PAPI_ParamChatMsg::dest_addr));

					if (p.data[(int)PAPI_ParamChatMsg::submit_dt].len)
						_msg.submit_date_time.assign((char*)DTP_PPARAMETER(PAPI_ParamChatMsg::submit_dt));
					if (p.data[(int)PAPI_ParamChatMsg::done_dt].len)
						_msg.done_date_time.assign((char*)DTP_PPARAMETER(PAPI_ParamChatMsg::done_dt));
					if (p.data[(int)PAPI_ParamChatMsg::view_dt].len)
						_msg.view_date_time.assign((char*)DTP_PPARAMETER(PAPI_ParamChatMsg::view_dt));

					if (p.data[(int)PAPI_ParamChatMsg::text].len)
						_msg.text.assign((char*)DTP_PPARAMETER(PAPI_ParamChatMsg::text));

					if(_msg.state && (_msg.status == 0))
						_msg.status = 1;

					msgs.push_back(std::move(_msg));
				}

				break;
			}
		}

		msg += pl;
		len -= pl;
	}

	return true;
}

bool Client::Impl::OnChatGetListRsp_ChatInfo(Chat &chat, unsigned char* msg, std::uint32_t len)
{
	tDTablePointerWord <(int)PAPI_ParamChatInfo::Max> pp;
	
	if (!pp.Parse(msg, len))
		return false;

	if (pp.data[(int)PAPI_ParamChatInfo::hindex].len == 8)
		chat.hindex = *(std::uint64_t*)pp.data[(int)PAPI_ParamChatInfo::hindex].msg;

	if (pp.data[(int)PAPI_ParamChatInfo::del].len)
		chat.del = *pp.data[(int)PAPI_ParamChatInfo::del].msg != 0;

	if (pp.data[(int)PAPI_ParamChatInfo::num_state].len)
		chat.state = *pp.data[(int)PAPI_ParamChatInfo::num_state].msg;

	chat.name.assign((char*)DTP_PARAMETER(pp, PAPI_ParamChatInfo::name));
	chat.avatar.assign((char*)DTP_PARAMETER(pp, PAPI_ParamChatInfo::avatar));

	if (pp.data[(int)PAPI_ParamChatInfo::chat_private].len)
		chat.chat_private = *pp.data[(int)PAPI_ParamChatInfo::chat_private].msg;
	if (pp.data[(int)PAPI_ParamChatInfo::chat_id].len == 8)
		chat.chat_id = *(std::uint64_t*)pp.data[(int)PAPI_ParamChatInfo::chat_id].msg;
	if (pp.data[(int)PAPI_ParamChatInfo::msg_index_last].len == 8)
		chat.msg_index_last = *(std::uint64_t*)pp.data[(int)PAPI_ParamChatInfo::msg_index_last].msg;
	if (pp.data[(int)PAPI_ParamChatInfo::start_msg_index].len == 8)
		chat.start_msg_index = *(std::uint64_t*)pp.data[(int)PAPI_ParamChatInfo::start_msg_index].msg;
	if (pp.data[(int)PAPI_ParamChatInfo::unread].len == 4)
		chat.not_viewed = *(std::uint32_t*)pp.data[(int)PAPI_ParamChatInfo::unread].msg;
	if (pp.data[(int)PAPI_ParamChatInfo::msgs_cnt].len == 8)
		chat.msgs_count = *(std::uint64_t*)pp.data[(int)PAPI_ParamChatInfo::msgs_cnt].msg;
	if (pp.data[(int)PAPI_ParamChatInfo::msgs_del].len == 8)
		chat.msgs_del = *(std::uint64_t*)pp.data[(int)PAPI_ParamChatInfo::msgs_del].msg;
	if (pp.data[(int)PAPI_ParamChatInfo::repd].len == 8)
		chat.msgs_rep_final = *(std::uint64_t*)pp.data[(int)PAPI_ParamChatInfo::repd].msg;
	if (pp.data[(int)PAPI_ParamChatInfo::repv].len == 8)
		chat.msgs_rep_read = *(std::uint64_t*)pp.data[(int)PAPI_ParamChatInfo::repv].msg;

	if (pp.data[(int)PAPI_ParamChatInfo::nums].len)
	{
		OnChatGetListRsp_ListNums(chat.nums, DTP_PARAMETER(pp, PAPI_ParamChatInfo::nums));
	}
	if (pp.data[(int)PAPI_ParamChatInfo::nums_del].len)
	{
		OnChatGetListRsp_ListNums(chat.nums_del, DTP_PARAMETER(pp, PAPI_ParamChatInfo::nums_del));
	}
	if (pp.data[(int)PAPI_ParamChatInfo::nums_viewer].len)
	{
		OnChatGetListRsp_ListNums(chat.nums_del, DTP_PARAMETER(pp, PAPI_ParamChatInfo::nums_viewer));
	}

	if (pp.data[(int)PAPI_ParamChatInfo::msgs].len)
	{
		OnChatGetListRsp_Msgs(chat.msgs, DTP_PARAMETER(pp, PAPI_ParamChatInfo::msgs));
	}

	return true;
}


bool Client::Impl::OnChatGetListRsp_List(std::vector<Chat> &chats, unsigned char* msg, std::uint32_t len)
{
	word pn;
	word pl;
	while (len)
	{
		if (len < 4)
		{
			break;
		}
		
		pn = *(word*)msg;
		msg += 2;
		len -= 2;
		
		pl = *(word*)msg;
		msg += 2;
		len -= 2;
		if (len < pl)
			break;

		switch (pn)
		{
			case (word)PAPI_ParamLists::list:
			{
				tDTablePointerWord <(int)PAPI_ParamList::Max> p;
				p.Parse((byte*)msg, pl);

				if (p.data[(int)PAPI_ParamList::chat_info].len)
				{
					pl::Chat chat;


























					OnChatGetListRsp_ChatInfo(chat, DTP_PPARAMETER(PAPI_ParamList::chat_info));

					chats.push_back(std::move(chat));
				}

				break;
			}
		}

		msg += pl;
		len -= pl;
	}

	return true;
}

bool Client::Impl::OnChatGetListRsp(std::uint32_t req_id, const unsigned char* msg, std::uint32_t len)
{
	tDTablePointerWord <(int)PAPI_ParamChat::Max> p;

	Log(LogLevel::Verbose, req_id, _strl("OnChatGetListRsp"));

	p.Parse((byte*)msg, len);

	auto rsp = pl::pl_api::make_object<pl::pl_api::Chat_GetList_Rsp>();

	if (p.data[(int)PAPI_ParamChat::rsp].len == 2)
		rsp->rsp = *(std::uint16_t*)p.data[(int)PAPI_ParamChat::rsp].msg;

	if (p.data[(int)PAPI_ParamChat::lists].len)

		OnChatGetListRsp_List(rsp->chats, DTP_PPARAMETER(PAPI_ParamChat::lists));

	if (p.data[(int)PAPI_ParamChat::server_time].len)
		rsp->server_dt.assign((char*)DTP_PPARAMETER(PAPI_ParamChat::server_time));

	Log(LogLevel::Verbose, req_id, _strl("Send Chat_GetList_Rsp"));

	SendResp({ true, req_id, std::move(rsp) });

	return true;
}

bool Client::Impl::OnChatGetInfoRsp(std::uint32_t req_id, const unsigned char* msg, std::uint32_t len)
{
	tDTablePointerWord <(int)PAPI_ParamChat::Max> p;

	Log(LogLevel::Verbose, req_id, _strl("OnChatGetInfoRsp"));

	p.Parse((byte*)msg, len);

	auto rsp = pl::pl_api::make_object<pl::pl_api::Chat_GetInfo_Rsp>();

	if (p.data[(int)PAPI_ParamChat::chat_id].len == 8)
		rsp->chat_id = *(std::uint64_t*)p.data[(int)PAPI_ParamChat::chat_id].msg;

	if (p.data[(int)PAPI_ParamChat::rsp].len == 2)
		rsp->rsp = *(std::uint16_t*)p.data[(int)PAPI_ParamChat::rsp].msg;

	if (p.data[(int)PAPI_ParamChat::lists].len)
		OnChatGetListRsp_List(rsp->chats, DTP_PPARAMETER(PAPI_ParamChat::lists));

	Log(LogLevel::Verbose, req_id, _strl("Send Chat_GetInfo_Rsp"));

	SendResp({ true, req_id, std::move(rsp) });

	return true;
}

bool Client::Impl::OnChatCreateRsp_Nums(std::vector<std::string> &nums, unsigned char* msg, std::uint32_t len)
{
	word pn;
	word pl;
	while (len)
	{
		if (len < 4)
		{
			break;
		}
		
		pn = *(word*)msg;
		msg += 2;
		len -= 2;
		
		pl = *(word*)msg;
		msg += 2;
		len -= 2;
		if (len < pl)
			break;

		switch (pn)
		{
			case (word)PAPI_ParamChatNums::num:
			{
				nums.emplace_back((char*)msg,pl);
				break;
			}
		}

		msg += pl;
		len -= pl;
	}

	return true;
}

bool Client::Impl::OnChatCreateRsp(std::uint32_t req_id, const unsigned char* msg, std::uint32_t len)
{
 	tDTablePointerWord <(int)PAPI_ParamChat::Max> p;
	std::uint16_t _rsp = 0xffff;
	byte chat_private = 0;
	std::uint64_t chat_id = 0xffffffffffffffff;
	std::uint64_t hindex = 0xffffffffffffffff;

	Log(LogLevel::Verbose, req_id, _strl("OnChatCreateRsp"));

	p.Parse((byte*)msg, len);
 
	if (p.data[(int)PAPI_ParamChat::rsp].len == 2)
		_rsp = *(std::uint16_t*)p.data[(int)PAPI_ParamChat::rsp].msg;
	if (p.data[(int)PAPI_ParamChat::chat_private].len)
		chat_private = *p.data[(int)PAPI_ParamChat::chat_private].msg;

	if (p.data[(int)PAPI_ParamChat::chat_id].len == 8)
		chat_id = *(std::uint64_t*)p.data[(int)PAPI_ParamChat::chat_id].msg;
	if (p.data[(int)PAPI_ParamChat::index].len == 8)
		hindex = *(std::uint64_t*)p.data[(int)PAPI_ParamChat::index].msg;

	if (chat_private != 0)
	{
		auto rsp = pl::pl_api::make_object<pl::pl_api::Chat_CreatePrivate_Rsp>();

		rsp->rsp = _rsp;
		rsp->chat_id = chat_id;
		rsp->hindex = hindex;

		if (p.data[(int)PAPI_ParamChat::start_msg_index].len == 8)
			rsp->start_msg_ibdex = *(std::uint64_t*)p.data[(int)PAPI_ParamChat::start_msg_index].msg;

		if (p.data[(int)PAPI_ParamChat::nums].len)
		{
			OnChatCreateRsp_Nums(rsp->numbers, DTP_PPARAMETER(PAPI_ParamChat::nums));
		}

		Log(LogLevel::Verbose, req_id, _strl("Send Chat_CreatePrivate_Rsp"));

		SendResp({ true, req_id, std::move(rsp) });
	}
	else
	{
		auto rsp = pl::pl_api::make_object<pl::pl_api::Chat_CreateGroup_Rsp>();

		rsp->rsp = _rsp;
		rsp->chat_id = chat_id;
		rsp->hindex = hindex;

		if (p.data[(int)PAPI_ParamChat::nums].len)
		{
			OnChatCreateRsp_Nums(rsp->numbers, DTP_PPARAMETER(PAPI_ParamChat::nums));
		}

		Log(LogLevel::Verbose, req_id, _strl("Send Chat_CreateGroup_Rsp"));

		SendResp({ true, req_id, std::move(rsp) });
	}

	return true;
}

bool Client::Impl::OnChatDelRsp(std::uint32_t req_id, const unsigned char* msg, std::uint32_t len)
{
 	tDTablePointerWord <(int)PAPI_ParamChat::Max> p;

	Log(LogLevel::Verbose, req_id, _strl("OnChatDelRsp"));

	p.Parse((byte*)msg, len);
 
	auto rsp = pl::pl_api::make_object<pl::pl_api::Chat_Delete_Rsp>();

	if (p.data[(int)PAPI_ParamChat::chat_id].len == 8)
		rsp->chat_id = *(std::uint64_t*)p.data[(int)PAPI_ParamChat::chat_id].msg;
	if (p.data[(int)PAPI_ParamChat::index].len == 8)
		rsp->hindex = *(std::uint64_t*)p.data[(int)PAPI_ParamChat::index].msg;

	if (p.data[(int)PAPI_ParamChat::rsp].len == 2)
		rsp->rsp = *(std::uint16_t*)p.data[(int)PAPI_ParamChat::rsp].msg;

	Log(LogLevel::Verbose, req_id, _strl("Send Chat_Delete_Rsp"));

	SendResp({ true, req_id, std::move(rsp) });

	return true;
}

bool Client::Impl::OnChatAddNumRsp(std::uint32_t req_id, const unsigned char* msg, std::uint32_t len)
{
	tDTablePointerWord <(int)PAPI_ParamChat::Max> p;

	Log(LogLevel::Verbose, req_id, _strl("OnChatAddNumRsp"));

	p.Parse((byte*)msg, len);

	auto rsp = pl::pl_api::make_object<pl::pl_api::Chat_AddNum_Rsp>();

	if (p.data[(int)PAPI_ParamChat::chat_id].len == 8)
		rsp->chat_id = *(std::uint64_t*)p.data[(int)PAPI_ParamChat::chat_id].msg;
	if (p.data[(int)PAPI_ParamChat::index].len == 8)
		rsp->hindex = *(std::uint64_t*)p.data[(int)PAPI_ParamChat::index].msg;

	if (p.data[(int)PAPI_ParamChat::rsp].len == 2)
		rsp->rsp = *(std::uint16_t*)p.data[(int)PAPI_ParamChat::rsp].msg;

	Log(LogLevel::Verbose, req_id, _strl("Send Chat_AddNum_Rsp"));

	SendResp({ true, req_id, std::move(rsp) });

	return true;
}

bool Client::Impl::OnChatDelNumRsp(std::uint32_t req_id, const unsigned char* msg, std::uint32_t len)
{
	tDTablePointerWord <(int)PAPI_ParamChat::Max> p;

	Log(LogLevel::Verbose, req_id, _strl("OnChatDelNumRsp"));

	p.Parse((byte*)msg, len);

	auto rsp = pl::pl_api::make_object<pl::pl_api::Chat_DelNum_Rsp>();

	if (p.data[(int)PAPI_ParamChat::chat_id].len == 8)
		rsp->chat_id = *(std::uint64_t*)p.data[(int)PAPI_ParamChat::chat_id].msg;
	if (p.data[(int)PAPI_ParamChat::index].len == 8)
		rsp->hindex = *(std::uint64_t*)p.data[(int)PAPI_ParamChat::index].msg;

	if (p.data[(int)PAPI_ParamChat::rsp].len == 2)
		rsp->rsp = *(std::uint16_t*)p.data[(int)PAPI_ParamChat::rsp].msg;

	Log(LogLevel::Verbose, req_id, _strl("Send Chat_DelNum_Rsp"));

	SendResp({ true, req_id, std::move(rsp) });

	return true;
}

bool Client::Impl::OnChatSetNameRsp(std::uint32_t req_id, const unsigned char* msg, std::uint32_t len)
{
	tDTablePointerWord <(int)PAPI_ParamChat::Max> p;

	Log(LogLevel::Verbose, req_id, _strl("OnChatSetNameRsp"));

	p.Parse((byte*)msg, len);

	auto rsp = pl::pl_api::make_object<pl::pl_api::Chat_SetName_Rsp>();

	if (p.data[(int)PAPI_ParamChat::chat_id].len == 8)
		rsp->chat_id = *(std::uint64_t*)p.data[(int)PAPI_ParamChat::chat_id].msg;
	if (p.data[(int)PAPI_ParamChat::index].len == 8)
		rsp->hindex = *(std::uint64_t*)p.data[(int)PAPI_ParamChat::index].msg;

	if (p.data[(int)PAPI_ParamChat::rsp].len == 2)
		rsp->rsp = *(std::uint16_t*)p.data[(int)PAPI_ParamChat::rsp].msg;

	Log(LogLevel::Verbose, req_id, _strl("Send Chat_SetName_Rsp"));

	SendResp({ true, req_id, std::move(rsp) });

	return true;
}

bool Client::Impl::OnChatSetAvatarRsp(std::uint32_t req_id, const unsigned char* msg, std::uint32_t len)
{
	tDTablePointerWord <(int)PAPI_ParamChat::Max> p;

	Log(LogLevel::Verbose, req_id, _strl("OnChatSetAvatarRsp"));

	p.Parse((byte*)msg, len);

	auto rsp = pl::pl_api::make_object<pl::pl_api::Chat_SetAvatar_Rsp>();

	if (p.data[(int)PAPI_ParamChat::chat_id].len == 8)
		rsp->chat_id = *(std::uint64_t*)p.data[(int)PAPI_ParamChat::chat_id].msg;
	if (p.data[(int)PAPI_ParamChat::index].len == 8)
		rsp->hindex = *(std::uint64_t*)p.data[(int)PAPI_ParamChat::index].msg;

	if (p.data[(int)PAPI_ParamChat::rsp].len == 2)
		rsp->rsp = *(std::uint16_t*)p.data[(int)PAPI_ParamChat::rsp].msg;

	Log(LogLevel::Verbose, req_id, _strl("Send Chat_SetAvatar_Rsp"));

	SendResp({ true, req_id, std::move(rsp) });

	return true;
}


bool Client::Impl::OnChatGetMsgsRsp(std::uint32_t req_id, const unsigned char* msg, std::uint32_t len)
{
	tDTablePointerWord <(int)PAPI_ParamChat::Max> p;

	Log(LogLevel::Verbose, req_id, _strl("OnChatGetMsgsRsp"));

	p.Parse((byte*)msg, len);

	auto rsp = pl::pl_api::make_object<pl::pl_api::Chat_GetMsgs_Rsp>();

	if (p.data[(int)PAPI_ParamChat::chat_id].len == 8)
		rsp->chat_id = *(std::uint64_t*)p.data[(int)PAPI_ParamChat::chat_id].msg;

	if (p.data[(int)PAPI_ParamChat::rsp].len == 2)
		rsp->rsp = *(std::uint16_t*)p.data[(int)PAPI_ParamChat::rsp].msg;

	if (p.data[(int)PAPI_ParamChat::msgs].len)
	{
		OnChatGetListRsp_Msgs(rsp->msgs, DTP_PPARAMETER(PAPI_ParamChat::msgs));

	}

	Log(LogLevel::Verbose, req_id, _strl("Send Chat_GetMsgs_Rsp"));

	SendResp({ true, req_id, std::move(rsp) });

	return true;
}

bool Client::Impl::OnChatGetMsgsDelRsp(std::uint32_t req_id, const unsigned char* msg, std::uint32_t len)
{
	tDTablePointerWord <(int)PAPI_ParamChat::Max> p;

	Log(LogLevel::Verbose, req_id, _strl("OnChatGetMsgsDelRsp"));

	p.Parse((byte*)msg, len);

	auto rsp = pl::pl_api::make_object<pl::pl_api::Chat_GetMsgsDel_Rsp>();

	if (p.data[(int)PAPI_ParamChat::chat_id].len == 8)
		rsp->chat_id = *(std::uint64_t*)p.data[(int)PAPI_ParamChat::chat_id].msg;

	if (p.data[(int)PAPI_ParamChat::rsp].len == 2)
		rsp->rsp = *(std::uint16_t*)p.data[(int)PAPI_ParamChat::rsp].msg;

	if (p.data[(int)PAPI_ParamChat::msgs].len)
	{
		OnChatGetListRsp_Msgs(rsp->msgs, DTP_PPARAMETER(PAPI_ParamChat::msgs));

	}

	Log(LogLevel::Verbose, req_id, _strl("Send Chat_GetMsgsDel_Rsp"));

	SendResp({ true, req_id, std::move(rsp) });

	return true;
}

bool Client::Impl::OnChatGetMsgsRepFinalRsp(std::uint32_t req_id, const unsigned char* msg, std::uint32_t len)
{
	tDTablePointerWord <(int)PAPI_ParamChat::Max> p;

	Log(LogLevel::Verbose, req_id, _strl("OnChatGetMsgsRepFinalRsp"));

	p.Parse((byte*)msg, len);

	auto rsp = pl::pl_api::make_object<pl::pl_api::Chat_GetMsgsRepFinal_Rsp>();

	if (p.data[(int)PAPI_ParamChat::chat_id].len == 8)
		rsp->chat_id = *(std::uint64_t*)p.data[(int)PAPI_ParamChat::chat_id].msg;

	if (p.data[(int)PAPI_ParamChat::rsp].len == 2)
		rsp->rsp = *(std::uint16_t*)p.data[(int)PAPI_ParamChat::rsp].msg;

	if (p.data[(int)PAPI_ParamChat::msgs].len)
	{
		OnChatGetListRsp_Msgs(rsp->msgs, DTP_PPARAMETER(PAPI_ParamChat::msgs));

	}

	Log(LogLevel::Verbose, req_id, _strl("Send Chat_GetMsgsRepFinal_Rsp"));

	SendResp({ true, req_id, std::move(rsp) });

	return true;
}

bool Client::Impl::OnChatGetMsgsRepReadRsp(std::uint32_t req_id, const unsigned char* msg, std::uint32_t len)
{
	tDTablePointerWord <(int)PAPI_ParamChat::Max> p;

	Log(LogLevel::Verbose, req_id, _strl("OnChatGetMsgsRepReaRsp"));

	p.Parse((byte*)msg, len);

	auto rsp = pl::pl_api::make_object<pl::pl_api::Chat_GetMsgsRepRead_Rsp>();

	if (p.data[(int)PAPI_ParamChat::chat_id].len == 8)
		rsp->chat_id = *(std::uint64_t*)p.data[(int)PAPI_ParamChat::chat_id].msg;

	if (p.data[(int)PAPI_ParamChat::rsp].len == 2)
		rsp->rsp = *(std::uint16_t*)p.data[(int)PAPI_ParamChat::rsp].msg;

	if (p.data[(int)PAPI_ParamChat::msgs].len)
	{
		OnChatGetListRsp_Msgs(rsp->msgs, DTP_PPARAMETER(PAPI_ParamChat::msgs));

	}

	Log(LogLevel::Verbose, req_id, _strl("Send Chat_GetMsgsRepRead_Rsp"));

	SendResp({ true, req_id, std::move(rsp) });

	return true;
}

bool Client::Impl::OnChatChangeInfoInd(std::uint32_t req_id, const unsigned char* msg, std::uint32_t len)
{
	tDTablePointerWord <(int)PAPI_ParamChat::Max> p;

	Log(LogLevel::Verbose, req_id, _strl("OnChatChangeInfoInd"));

	p.Parse((byte*)msg, len);

	auto rsp = pl::pl_api::make_object<pl::pl_api::Chat_ChangeInfo_Ind>();

	if (p.data[(int)PAPI_ParamChat::index].len == 8)
		rsp->chat_id = *(std::uint64_t*)p.data[(int)PAPI_ParamChat::index].msg;
	if (p.data[(int)PAPI_ParamChat::chat_id].len == 8)
		rsp->chat_id = *(std::uint64_t*)p.data[(int)PAPI_ParamChat::chat_id].msg;

	if (p.data[(int)PAPI_ParamChat::change_type].len)
		rsp->change_type = *p.data[(int)PAPI_ParamChat::change_type].msg;

	if (p.data[(int)PAPI_ParamChat::get_chat_info].len)
	{
		rsp ->chat_info = OnChatGetListRsp_ChatInfo(rsp->chat, DTP_PPARAMETER(PAPI_ParamChat::get_chat_info));
	}


	Log(LogLevel::Verbose, req_id, _strl("Send Chat_ChangeInfo_Ind"));

	SendResp({ true, req_id, std::move(rsp) });

	return true;
}

bool Client::Impl::OnChatNotReadRsp(std::uint32_t req_id, const unsigned char* msg, std::uint32_t len)
{
	tDTablePointerWord <(int)PAPI_ParamChat::Max> p;

	Log(LogLevel::Verbose, req_id, _strl("OnChatNotReadRsp"));

	p.Parse((byte*)msg, len);

	auto rsp = pl::pl_api::make_object<pl::pl_api::Chat_NotRead_Rsp>();

	if (p.data[(int)PAPI_ParamChat::chat_id].len == 8)
		rsp->chat_id = *(std::uint64_t*)p.data[(int)PAPI_ParamChat::chat_id].msg;

	if (p.data[(int)PAPI_ParamChat::rsp].len == 2)
		rsp->rsp = *(std::uint16_t*)p.data[(int)PAPI_ParamChat::rsp].msg;

	if (p.data[(int)PAPI_ParamChat::chat_unread].len == 4)
		rsp->not_read = *(std::uint32_t*)p.data[(int)PAPI_ParamChat::chat_unread].msg;

	Log(LogLevel::Verbose, req_id, _strl("Send Chat_NotRead_Rsp"));

	SendResp({ true, req_id, std::move(rsp) });

	return true;
}

bool Client::Impl::OnChatReadAllRsp(std::uint32_t req_id, const unsigned char* msg, std::uint32_t len)
{
	tDTablePointerWord <(int)PAPI_ParamChat::Max> p;

	Log(LogLevel::Verbose, req_id, _strl("OnChatReadAllRsp"));

	p.Parse((byte*)msg, len);

	auto rsp = pl::pl_api::make_object<pl::pl_api::Chat_ReadAll_Rsp>();

	if (p.data[(int)PAPI_ParamChat::chat_id].len == 8)
		rsp->chat_id = *(std::uint64_t*)p.data[(int)PAPI_ParamChat::chat_id].msg;

	if (p.data[(int)PAPI_ParamChat::rsp].len == 2)
		rsp->rsp = *(std::uint16_t*)p.data[(int)PAPI_ParamChat::rsp].msg;


	Log(LogLevel::Verbose, req_id, _strl("Send Chat_ReadAll_Rsp"));

	SendResp({ true, req_id, std::move(rsp) });

	return true;
}


bool Client::Impl::OnFSPutAvaterRsp(std::uint32_t req_id, const unsigned char* msg, std::uint32_t len)
{
	tDTablePointerWord <(int)PAPI_ParamFS::Max> p;

	Log(LogLevel::Verbose, req_id, _strl("OnFSPutAvaterRsp"));

	p.Parse((byte*)msg, len);

	auto rsp = pl::pl_api::make_object<pl::pl_api::FS_PutAvatar_Rsp>();

	switch (p.data[(int)PAPI_ParamFS::rsp].len)
	{
		case 1:
		{
			rsp->rsp = *p.data[(int)PAPI_ParamFS::rsp].msg;
			break;
		}
		case 2:
		{
			rsp->rsp = *(word*)p.data[(int)PAPI_ParamFS::rsp].msg;
			break;
		}
		case 4:
		{
			std::uint32_t _rsp;
			_rsp = *(std::uint32_t*)p.data[(int)PAPI_ParamFS::rsp].msg;
			rsp->rsp = (std::uint16_t)_rsp;
			break;
		}
	}

	if (p.data[(int)PAPI_ParamFS::offset].len == 8)
		rsp->offset = *(std::uint64_t*)p.data[(int)PAPI_ParamFS::offset].msg;
	if (p.data[(int)PAPI_ParamFS::post_size].len == 8)
		rsp->post_size = *(std::uint64_t*)p.data[(int)PAPI_ParamFS::post_size].msg;

	Log(LogLevel::Verbose, req_id, _strl("Send FS_PutAvatar_Rsp"));

	SendResp({ true, req_id, std::move(rsp) });

	return true;
}

bool Client::Impl::OnFSGetAvaterRsp(std::uint32_t req_id, const unsigned char* msg, std::uint32_t len)
{
	tDTablePointerWord <(int)PAPI_ParamFS::Max> p;

	Log(LogLevel::Verbose, req_id, _strl("OnFSGetAvaterRsp"));

	p.Parse((byte*)msg, len);

	auto rsp = pl::pl_api::make_object<pl::pl_api::FS_GetAvatar_Rsp>();

	switch (p.data[(int)PAPI_ParamFS::rsp].len)
	{
		case 1:
		{
			rsp->rsp = *p.data[(int)PAPI_ParamFS::rsp].msg;
			break;
		}
		case 2:
		{
			rsp->rsp = *(word*)p.data[(int)PAPI_ParamFS::rsp].msg;
			break;
		}
		case 4:
		{
			std::uint32_t _rsp;
			_rsp = *(std::uint32_t*)p.data[(int)PAPI_ParamFS::rsp].msg;
			rsp->rsp = (std::uint16_t)_rsp;
			break;
		}
	}

	rsp->file_ext.assign((char*)DTP_PPARAMETER(PAPI_ParamFS::file_ext));

	if (p.data[(int)PAPI_ParamFS::offset].len == 8)
		rsp->offset = *(std::uint64_t*)p.data[(int)PAPI_ParamFS::offset].msg;
	
	if (p.data[(int)PAPI_ParamFS::file_size_full].len == 8)
 		rsp->full_size = *(std::uint64_t*)p.data[(int)PAPI_ParamFS::file_size_full].msg;

	if (p.data[(int)PAPI_ParamFS::fragment].len)
	{
		if (p.data[(int)PAPI_ParamFS::fragment].len <= PAPI_RxFileFragmetMaxSize)
		{
#ifndef SHARP_WRAP
			memcpy(rsp->bin, DTP_PPARAMETER(PAPI_ParamFS::fragment));
#else
			memcpy(rsp->bin, DTP_PPARAMETER(PAPI_ParamFS::fragment));
#endif
			rsp->len = p.data[(int)PAPI_ParamFS::fragment].len;
		}
		else
			rsp->rsp = 0xf000;
	}

	Log(LogLevel::Verbose, req_id, _strl("Send FS_GetAvatar_Rsp"));

	SendResp({ true, req_id, std::move(rsp) });

	return true;
}

bool Client::Impl::OnFSPutChatAvaterRsp(std::uint32_t req_id, const unsigned char* msg, std::uint32_t len)
{
	tDTablePointerWord <(int)PAPI_ParamFS::Max> p;

	Log(LogLevel::Verbose, req_id, _strl("OnFSPutChatAvaterRsp"));

	p.Parse((byte*)msg, len);

	auto rsp = pl::pl_api::make_object<pl::pl_api::FS_PutChatAvatar_Rsp>();

	switch (p.data[(int)PAPI_ParamFS::rsp].len)
	{
		case 1:
		{
			rsp->rsp = *p.data[(int)PAPI_ParamFS::rsp].msg;
			break;
		}
		case 2:
		{
			rsp->rsp = *(word*)p.data[(int)PAPI_ParamFS::rsp].msg;
			break;
		}
		case 4:
		{
			std::uint32_t _rsp;
			_rsp = *(std::uint32_t*)p.data[(int)PAPI_ParamFS::rsp].msg;
			rsp->rsp = (std::uint16_t)_rsp;
			break;
		}
	}

	if (p.data[(int)PAPI_ParamFS::chat_id].len == 8)
		rsp->chat_id = *(std::uint64_t*)p.data[(int)PAPI_ParamFS::chat_id].msg;

	if (p.data[(int)PAPI_ParamFS::offset].len == 8)
		rsp->offset = *(std::uint64_t*)p.data[(int)PAPI_ParamFS::offset].msg;
	if (p.data[(int)PAPI_ParamFS::post_size].len == 8)
		rsp->post_size = *(std::uint64_t*)p.data[(int)PAPI_ParamFS::post_size].msg;

	Log(LogLevel::Verbose, req_id, _strl("Send FS_PutChatAvatar_Rsp"));

	SendResp({ true, req_id, std::move(rsp) });

	return true;
}

bool Client::Impl::OnFSGetChatAvaterRsp(std::uint32_t req_id, const unsigned char* msg, std::uint32_t len)
{
	tDTablePointerWord <(int)PAPI_ParamFS::Max> p;

	Log(LogLevel::Verbose, req_id, _strl("OnFSGetChatAvaterRsp"));

	p.Parse((byte*)msg, len);

	auto rsp = pl::pl_api::make_object<pl::pl_api::FS_GetChatAvatar_Rsp>();

	switch (p.data[(int)PAPI_ParamFS::rsp].len)
	{
		case 1:
		{
			rsp->rsp = *p.data[(int)PAPI_ParamFS::rsp].msg;
			break;
		}
		case 2:
		{
			rsp->rsp = *(word*)p.data[(int)PAPI_ParamFS::rsp].msg;
			break;
		}
		case 4:
		{
			std::uint32_t _rsp;
			_rsp = *(std::uint32_t*)p.data[(int)PAPI_ParamFS::rsp].msg;
			rsp->rsp = (std::uint16_t)_rsp;
			break;
		}
	}

	rsp->file_ext.assign((char*)DTP_PPARAMETER(PAPI_ParamFS::file_ext));

	if (p.data[(int)PAPI_ParamFS::chat_id].len == 8)
		rsp->chat_id = *(std::uint64_t*)p.data[(int)PAPI_ParamFS::chat_id].msg;

	if (p.data[(int)PAPI_ParamFS::offset].len == 8)
		rsp->offset = *(std::uint64_t*)p.data[(int)PAPI_ParamFS::offset].msg;

	if (p.data[(int)PAPI_ParamFS::file_size_full].len == 8)
 		rsp->full_size = *(std::uint64_t*)p.data[(int)PAPI_ParamFS::file_size_full].msg;

	if (p.data[(int)PAPI_ParamFS::fragment].len)
	{
		if (p.data[(int)PAPI_ParamFS::fragment].len <= PAPI_RxFileFragmetMaxSize)
		{
#ifndef SHARP_WRAP
			memcpy(rsp->bin, DTP_PPARAMETER(PAPI_ParamFS::fragment));
#else
			memcpy(rsp->bin, DTP_PPARAMETER(PAPI_ParamFS::fragment));
#endif
			rsp->len = p.data[(int)PAPI_ParamFS::fragment].len;
		}
		else
			rsp->rsp = 0xf000;
	}

	Log(LogLevel::Verbose, req_id, _strl("Send FS_GetChatAvatar_Rsp"));

	SendResp({ true, req_id, std::move(rsp) });

	return true;
}

bool Client::Impl::OnFSPutFileRsp(std::uint32_t req_id, const unsigned char* msg, std::uint32_t len)
{
	tDTablePointerWord <(int)PAPI_ParamFS::Max> p;

	Log(LogLevel::Verbose, req_id, _strl("OnFSPutFileRsp"));

	p.Parse((byte*)msg, len);

	auto rsp = pl::pl_api::make_object<pl::pl_api::FS_PutMsgFile_Rsp>();

	switch (p.data[(int)PAPI_ParamFS::rsp].len)
	{
		case 1:
		{
			rsp->rsp = *p.data[(int)PAPI_ParamFS::rsp].msg;
			break;
		}
		case 2:
		{
			rsp->rsp = *(word*)p.data[(int)PAPI_ParamFS::rsp].msg;
			break;
		}
		case 4:
		{
			std::uint32_t _rsp;
			_rsp = *(std::uint32_t*)p.data[(int)PAPI_ParamFS::rsp].msg;
			rsp->rsp = (std::uint16_t)_rsp;
			break;
		}
	}

	if (p.data[(int)PAPI_ParamFS::sm_id].len == 8)
		rsp->sm_id = *(std::uint64_t*)p.data[(int)PAPI_ParamFS::sm_id].msg;

	if (p.data[(int)PAPI_ParamFS::offset].len == 8)
		rsp->offset = *(std::uint64_t*)p.data[(int)PAPI_ParamFS::offset].msg;
	if (p.data[(int)PAPI_ParamFS::post_size].len == 8)
		rsp->post_size = *(std::uint64_t*)p.data[(int)PAPI_ParamFS::post_size].msg;

	Log(LogLevel::Verbose, req_id, _strl("Send FS_PutMsgFile_Rsp"));

	SendResp({ true, req_id, std::move(rsp) });

	return true;
}

bool Client::Impl::OnFSGetFileRsp(std::uint32_t req_id, const unsigned char* msg, std::uint32_t len)
{
	tDTablePointerWord <(int)PAPI_ParamFS::Max> p;

	Log(LogLevel::Verbose, req_id, _strl("OnFSGetFileRsp"));

	p.Parse((byte*)msg, len);

	auto rsp = pl::pl_api::make_object<pl::pl_api::FS_GetMsgFile_Rsp>();

	switch (p.data[(int)PAPI_ParamFS::rsp].len)
	{
		case 1:
		{
			rsp->rsp = *p.data[(int)PAPI_ParamFS::rsp].msg;
			break;
		}
		case 2:
		{
			rsp->rsp = *(word*)p.data[(int)PAPI_ParamFS::rsp].msg;
			break;
		}
		case 4:
		{
			std::uint32_t _rsp;
			_rsp = *(std::uint32_t*)p.data[(int)PAPI_ParamFS::rsp].msg;
			rsp->rsp = (std::uint16_t)_rsp;
			break;
		}
	}

	rsp->file_ext.assign((char*)DTP_PPARAMETER(PAPI_ParamFS::file_ext));

	if (p.data[(int)PAPI_ParamFS::sm_id].len == 8)
		rsp->sm_id = *(std::uint64_t*)p.data[(int)PAPI_ParamFS::sm_id].msg;

	if (p.data[(int)PAPI_ParamFS::offset].len == 8)
		rsp->offset = *(std::uint64_t*)p.data[(int)PAPI_ParamFS::offset].msg;

	if (p.data[(int)PAPI_ParamFS::file_size_full].len == 8)
 		rsp->full_size = *(std::uint64_t*)p.data[(int)PAPI_ParamFS::file_size_full].msg;

	if (p.data[(int)PAPI_ParamFS::fragment].len)
	{
		if (p.data[(int)PAPI_ParamFS::fragment].len <= PAPI_RxFileFragmetMaxSize)
		{
#ifndef SHARP_WRAP
			memcpy(rsp->bin, DTP_PPARAMETER(PAPI_ParamFS::fragment));
#else
			memcpy(rsp->bin, DTP_PPARAMETER(PAPI_ParamFS::fragment));
#endif
			rsp->len = p.data[(int)PAPI_ParamFS::fragment].len;
		}
		else
			rsp->rsp = 0xf000;
	}

	Log(LogLevel::Verbose, req_id, _strl("Send FS_GetMsgFile_Rsp"));

	SendResp({ true, req_id, std::move(rsp) });

	return true;
}


bool Client::Impl::OnBillingBalanceRsp(std::uint32_t req_id, const unsigned char* msg, std::uint32_t len)
{
	tDTablePointerWord <(int)PAPI_ParamBilling::Max> p;

	Log(LogLevel::Verbose, req_id, _strl("OnBillingBalanceRsp"));

	p.Parse((byte*)msg, len);

	auto rsp = pl::pl_api::make_object<pl::pl_api::Billing_Balance_Rsp>();

	if (p.data[(int)PAPI_ParamBilling::rsp].len == 2)
		rsp->rsp = *(std::uint16_t*)p.data[(int)PAPI_ParamBilling::rsp].msg;

	rsp->balance.assign((char*)DTP_PPARAMETER(PAPI_ParamBilling::balance));


	Log(LogLevel::Verbose, req_id, _strl("Send Billing_Balance_Rsp"));

	SendResp({ true, req_id, std::move(rsp) });

	return true;
}



} 
