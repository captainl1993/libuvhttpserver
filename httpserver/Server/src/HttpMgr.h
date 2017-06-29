#pragma once
#include <iostream>
#include <stdlib.h>
#include <memory>
#include "http_parser.h"
#include "NetServer.h"
#include <string>

class WXNotiFy {
public:
	void clear();
	std::string appid = "";
	std::string attach = "";
	std::string bank_type = "";
	std::string fee_type = "";
	std::string is_subscribe = "";
	std::string mch_id = "";
	std::string nonce_str = "";
	std::string out_trade_no = "";
	std::string result_code = "";
	std::string return_code = "";
	std::string sign = "";
	std::string sub_mch_id = "";
	std::string time_end = "";
	std::string total_fee = "";
	std::string trade_type = "";
	std::string transaction_id = "";

};


class HttpMgr {
private:
	static WXNotiFy wxNotiFy;
	static http_parser *parser;
	static http_parser_settings settings_null;
	static int on_message_begin(http_parser* _);

	static int on_headers_complete(http_parser* _);

	static int on_message_complete(http_parser* _);
	static int on_url(http_parser* _, const char* at, size_t length);
	static int on_header_field(http_parser* _, const char* at, size_t length);

	static int on_header_value(http_parser* _, const char* at, size_t length);
	static int on_body(http_parser* _, const char* at, size_t length);

	//读取连接数据
	static void OnRead(uv_stream_t * session, std::string data);
	//连接进入
	static  void OnConnected(uv_stream_t * session);
	//连接断开
	static  void OnDisConnected(uv_stream_t * session) ;

	static std::string MakeRespone(std::string data);
public:
	static void Start();
};