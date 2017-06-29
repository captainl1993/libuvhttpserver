#include "HttpMgr.h"
#include "tinyxml.h"
#include <fstream>
#include "WXNotifyMgr.h"

http_parser *HttpMgr::parser;
http_parser_settings HttpMgr::settings_null;
WXNotiFy HttpMgr::wxNotiFy;

void WXNotiFy::clear()
{
	 appid = "";
	 attach = "";
	 bank_type = "";
	 fee_type = "";
	 is_subscribe = "";
	 mch_id = "";
	 nonce_str = "";
	 out_trade_no = "";
	 result_code = "";
	 return_code = "";
	 sign = "";
	 sub_mch_id = "";
	 time_end = "";
	 total_fee = "";
	 trade_type = "";
	 transaction_id = "";
}
int HttpMgr::on_message_begin(http_parser* _) {
	(void)_;
	wxNotiFy.clear();
	return 0;
}

int HttpMgr::on_headers_complete(http_parser* _) {
	(void)_;
	return 0;
}

int HttpMgr::on_message_complete(http_parser* _) {
	(void)_;
	return 0;
}

int HttpMgr::on_url(http_parser* _, const char* at, size_t length) {
	(void)_;
	return 0;
}

int HttpMgr::on_header_field(http_parser* _, const char* at, size_t length) {
	(void)_;
	return 0;
}

int HttpMgr::on_header_value(http_parser* _, const char* at, size_t length) {
	(void)_;
	return 0;
}
int HttpMgr::on_body(http_parser* _, const char* at, size_t length) {
	(void)_;
	//输出到文件  
	std::ofstream os;
	os.open("data.xml", std::ios_base::out);
	os << std::string(at, (int)length);
	os.close();

	TiXmlDocument doc;
	doc.LoadFile("data.xml");
	if (doc.Error())
	{
		return 0;
	}
	TiXmlHandle docHandle(&doc);
	TiXmlElement* root = doc.RootElement();
	if (root == nullptr)
		return 0;
	
	for (TiXmlElement *pModuleElem = root->FirstChildElement(); pModuleElem != nullptr; pModuleElem = pModuleElem->NextSiblingElement())
	{
		std::string nodename = pModuleElem->Value();
		std::string nodeValue = pModuleElem->GetText();
		if (nodename == "appid")
		{
			wxNotiFy.appid = nodeValue;
		}
		else if (nodename == "attach")
		{
			wxNotiFy.attach = nodeValue;
		}
		else if (nodename == "bank_type")
		{
			wxNotiFy.bank_type = nodeValue;
		}
		else if (nodename == "fee_type")
		{
			wxNotiFy.fee_type = nodeValue;
		}
		else if (nodename == "is_subscribe")
		{
			wxNotiFy.is_subscribe = nodeValue;
		}
		else if (nodename == "mch_id")
		{
			wxNotiFy.mch_id = nodeValue;
		}
		else if (nodename == "nonce_str")
		{
			wxNotiFy.nonce_str = nodeValue;
		}
		else if (nodename == "out_trade_no")
		{
			wxNotiFy.out_trade_no = nodeValue;
		}
		else if (nodename == "result_code")
		{
			wxNotiFy.result_code = nodeValue;
		}
		else if (nodename == "return_code")
		{
			wxNotiFy.return_code = nodeValue;
		}
		else if (nodename == "sign")
		{
			wxNotiFy.sign = nodeValue;
		}
		else if (nodename == "sub_mch_id")
		{
			wxNotiFy.sub_mch_id = nodeValue;
		}
		else if (nodename == "time_end")
		{
			wxNotiFy.time_end = nodeValue;
		}
		else if (nodename == "total_fee")
		{
			wxNotiFy.total_fee = nodeValue;
		}
		else if (nodename == "trade_type")
		{
			wxNotiFy.trade_type = nodeValue;
		}
		else if (nodename == "transaction_id")
		{
			wxNotiFy.transaction_id = nodeValue;
		}
	}
	return 0;
}
std::string HttpMgr::MakeRespone(std::string data)
{
	std::string result = "HTTP/1.1 200\r\n"
		"Content-Type:text;charset=utf-8\r\n"
		"Content-Length:" + std::to_string(data.size()) + "\r\n\r\n";
	result += data;
	return result;
}
//网络消息处理

//读取连接数据
void HttpMgr::OnRead(uv_stream_t * session, std::string data)
{
	//输出到文件  
	std::ofstream os;
	os.open("requst.txt", std::ios_base::out);
	os << data;
	os.close();
	parser = (http_parser *)malloc(sizeof(http_parser));  //分配一个http_parser
	http_parser_init(parser, HTTP_REQUEST);  //初始化parser为Request类型
	size_t parsed = http_parser_execute(parser, &settings_null, data.c_str(), data.size());
	if (wxNotiFy.return_code == "SUCCESS")
	{
		std::string responestr = MakeRespone("<xml>\
		<return_code><![CDATA[SUCCESS]]>< / return_code>\
		<return_msg><![CDATA[OK]]>< / return_msg>\
		< / xml>");
		NetServer::SendData(session, responestr);

		//提交给WXNotifyMgr
		WXNotifyMgr::GetInstance()->pushWBMsg(wxNotiFy);
	}
	else
	{
		std::string responestr = MakeRespone("error");
		NetServer::SendData(session, responestr);
	}
	NetServer::CloseSession(session);
}
//连接进入
void HttpMgr::OnConnected(uv_stream_t * session)
{

}
//连接断开
void HttpMgr::OnDisConnected(uv_stream_t * session)
{

}

void HttpMgr::Start()
{
	settings_null.on_body = on_body;
	settings_null.on_message_begin = on_message_begin;
	settings_null.on_header_field = on_header_field;
	settings_null.on_header_value = on_header_value;
	settings_null.on_url = on_url;
	settings_null.on_status = 0;
	settings_null.on_body = on_body;
	settings_null.on_headers_complete = on_headers_complete;
	settings_null.on_message_complete = on_message_complete;

	NetServer::OnConnected = HttpMgr::OnConnected;
	NetServer::OnRead = HttpMgr::OnRead;
	NetServer::OnDisConnected = HttpMgr::OnDisConnected;
	NetServer::RunServer("0.0.0.0", 5555, nullptr);
}