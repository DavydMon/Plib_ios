#ifndef papi_defH
#define papi_defH


#ifdef LINUX
#include <cstdint>
#endif

#pragma pack(push,mystruct,1)
typedef struct _PAPI_STREAM_HEADER
{
#ifndef LINUX
	unsigned char  ver;
	unsigned char  service;
	unsigned short command_id;
	unsigned long  req_id;
#else
	std::uint8_t   ver;
	std::uint8_t   service;
	std::uint16_t  command_id;
	std::uint32_t  req_id;
#endif
}PAPI_STREAM_HEADER;
#pragma pack(pop,mystruct)


enum class PAPI_Service
{
	service,
	registrar,
	message,
	chats, 
	cs,    
	fs,    
	dial,
	bl,
	billing,

	Max
};

enum class PAPI_ComService
{
	ping_req,
	ping_rsp,
	ping_ex_req,
	ping_ex_rsp,
	test_req,
	test_rsp,
	test_msg_req,
	test_msg_rsp,
	test_msg_ex_req,
	test_msg_ex_rsp,

	Max
};

enum class PAPI_ComRegistrar
{
	init_req,
	init_rsp,
	reg_req,
	reg_rsp,
	auth_req,
	auth_rsp,
	logout_req,
	logout_rsp,
	logout_all_req,
	logout_all_rsp,
	push_token_req,
	push_token_rsp,

	get_susbs_status_req,
	get_susbs_status_rsp,
	set_user_status_req,
	set_user_status_rsp,
	status_ind,
	user_status_ind,

	get_route_direction_req,
	get_route_direction_rsp,
	set_route_direction_req,
	set_route_direction_rsp,

	Max
};

enum class PAPI_ComDial
{
	dial_req,
	dial_rsp,
	cancel_req,
	cancel_rsp,
	push_clear_calls_req,
	push_clear_calls_rsp,
	calls_badge_req,
	calls_badge_rsp,

	Max
};

enum class PAPI_ComMessage
{
	message_req,
	message_rsp,
	report_req,
	report_rsp,
	del_msg_req,
	del_msg_rsp,
	del_msgs_req,
	del_msgs_rsp,

	typing_req,

	Max
};

enum class PAPI_ComBlackList
{
	module_name,
	add_req,
	add_rsp,
	del_req,
	del_rsp,
	get_req,
	get_rsp,

	Max
};

enum class PAPI_ComCS
{
	reserv,

	put_contacts_req,
	put_contacts_rsp,
	contacts_del_req,
	contacts_del_rsp,
	get_profiles_req,
	get_profiles_rsp,
	get_profile_req,
	get_profile_rsp,
	get_contacts_req,
	get_contacts_rsp,
	get_contacts_dt_req,
	get_contacts_dt_rsp,
	get_contacts_user_req,
	get_contacts_user_rsp,
	put_me_req,
	put_me_rsp,
	get_me_req,
	get_me_rsp,

	get_profilesex_req,
	get_profilesex_rsp,

	get_followers_req,
	get_followers_rsp,
	get_poly_contacts_req,
	get_poly_contacts_rsp,

	get_contacts_history_req,
	get_contacts_history_rsp,

	Max
};

enum class PAPI_ComChat
{
	reserv,

	chat_check_list_req,
	chat_check_list_rsp,
	chat_get_list_req,
	chat_get_list_rsp,

	chat_get_info_req,
	chat_get_info_rsp,
	chat_create_req,
	chat_create_rsp,
	chat_del_req,
	chat_del_rsp,
	chat_add_num_req,
	chat_add_num_rsp,
	chat_del_num_req,
	chat_del_num_rsp,
	chat_set_name_req,
	chat_set_name_rsp,
	chat_set_avatar_req,
	chat_set_avatar_rsp,

	chat_get_msg_index_req,
	chat_get_msg_index_rsp,

	put_msg_req,
	put_msg_rsp,
	put_rep_req,
	put_rep_rsp,

	get_msg_req,
	get_msg_rsp,

	chat_change_req,
	chat_change_rsp,

	chat_get_info_ex_req,
	chat_get_info_ex_rsp,

	chat_get_info_push_req,
	chat_get_info_push_rsp,

	chat_get_unread_req,
	chat_get_unread_rsp,

	find_chat_req,
	find_chat_rsp,
	find_msg_req,
	find_msg_rsp,

	chat_del_all_req,
	chat_del_all_rsp,

	del_msg_req,
	del_msg_rsp,

	get_msg_del_req,
	get_msg_del_rsp,

	get_repd_req,
	get_repd_rsp,
	get_repv_req,
	get_repv_rsp,

	get_msg_map_req,
	get_msg_map_rsp,

	set_read_req,
	set_read_rsp,

	read_ind_req,
	read_ind_rsp,

	Max
};

enum class PAPI_ComFS
{
	reserv,

	get_file_req,
	get_file_rsp,
	get_file_info_req,
	get_file_info_rsp,
	put_file_req,
	put_file_rsp,

	get_avatar_req,
	get_avatar_rsp,
	put_avatar_req,
	put_avatar_rsp,

	get_chat_avatar_req,
	get_chat_avatar_rsp,
	put_chat_avatar_req,
	put_chat_avatar_rsp,
	forward_file_req,
	forward_file_rsp,

	Max
};

enum class  PAPI_ComBilling
{
	balance_req, 
	balance_rsp, 

	Max    
};



enum class PAPI_Param
{
	rsp
};

enum class PAPI_ParamReg
{
	rsp,

	num,
	uid,
	dev_uid,

	reg_info,

	token,
	token_voip,

	status,
	status_date_time,
	user_status,
	user_status_date_time,

	direction,

	error_code,

	tid,

	Max
};

enum class PAPI_ParamRegInfo
{
	code,
	nonce,
	opaque,
	realm,
	digestURI,
	algorithm,
	resp,

	user_agent,
	user_type,
	user_ver,
	user_ver2,
	user_ver3,
	user_ver4,
	user_client,

	need_uid,

	Max
};


enum class PAPI_ParamService
{
	rsp,

	num,

	id,
	command,
	result,
	loop,
	data,

	tid,

	Max
};


enum class PAPI_ParamDial
{
	rsp,

	num,

	sour_num,
	dest_num,

	voice_server,
	channel,

	call_id,

	badge,

	tid,

	Max
};


enum class PAPI_ParamMsg
{
	rsp,

	num,



	sour_num,
	dest_num,


	chat_private,
	chat_id,
	sm_id,
	msg_index,
	text,

	rep_status,

	submit_date_time,
	date_time,

	file_type,
	file_hash,

	channel,

	del_type,

	typing_type,

	repeat,

	tid,

	Max
};

enum class PAPI_ParamBlackList
{
	module_name,

	rsp,

	num,
	block_num,

	sm_id,
	list,

	tid,

	Max
};

enum class PAPI_BlackListParamList
{
	count,
	num,

	Max
};


enum class PAPI_ParamCS
{
	rsp,

	num_own,
	name,
	avatar,
	hindex,
	contacts,
	lang,
	pinboot,


	tid,

	Max
};

enum class PAPI_ParamCS_Contacts
{
	count,
	last,
	contact,

	Max
};

enum class PAPI_ParamCS_Contact
{
	num,
	name,
	fname,
	lname,
	avatar,
	messenger,
	hindex,
	rnum,

	Max
};

enum class PAPI_ParamChat
{
	reserv,

	rsp,

	index,

	chat_id,
	chat_name,
	chat_avatar,

	chat_private,

	num_own,
	nums,

	lists,

	msg_index,

	msg,
	msgs,
	rep,
	reps,

	msg_count,
	dir, 

	get_chat_info,

	all_unread,
	chat_unread,

	sour_num,
	dest_num,
	sm_id,

	start_msg_index,


	change_type,

	module_type,

	msg_type, 
	file_type,
	file_hash,

	del_chat,

	stop_msg_index,

	rep_list,

	num_state, 

	full_msg,

	dt_start,
	dt_stop,

	server_time,

	tid,

	Max
};

enum class PAPI_ParamLists
{
	list,

	Max
};

enum class PAPI_ParamList
{
	chat_info,

	Max
};

enum class PAPI_ParamChatInfo
{
	del,
	chat_private,
	chat_id,
	name,
	avatar,
	nums,

	msg_index_last,
	msgs,

	hindex,

	unread,
	start_msg_index,

	num_state, 

	nums_del,
	nums_viewer,

	msgs_del,
	repd,
	repv,
	msgs_cnt,

	Max
};


enum class PAPI_ParamChatNums
{
	num,

	Max
};

enum class PAPI_ParamChatMsgs
{
	msg,

	Max
};

enum class PAPI_ParamChatMsg
{
	state,
	msg_index,
	sm_id,
	sour_addr,
	dest_addr,
	submit_dt,
	text,
	type, 
	status,
	file_type,
	file_hash,
	chat_id,
	service,

	done_dt,
	view_dt,

	submit_dt_ex,
	del_dt,

	Max
};


enum class PAPI_ParamFS
{
	module_name,

	rsp,

	sm_id,
	chat_id,
	hash,
	is_thumbnail,
	num,
	dest_num,
	new_sm_id,
	uid,
	file_size_full,
	offset,
	post_size,
	fragment,
	file_ext,
 	frag_len,

	tid,

	Max
};

enum class PAPI_ParamBilling
{
	reserv,

	rsp,

	num,
	balance,


	tid,

	Max
};


enum class PAPI_Result
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


#endif
