#pragma once

#ifndef _WIN32
#include <unistd.h>
#include <cstdlib>
#include <cstring>
#include <netdb.h>
#else
#include <winsock2.h>
#include <ws2tcpip.h>
#include <wspiapi.h>
#endif
#include <iostream>
#include <udt.h>
#include "cc.h"

class UDTServer
{

#ifndef _WIN32
	static void* recvdata(void* usocket);
#else
	static DWORD WINAPI recvdata(LPVOID usocket);
#endif
	static int Start(int port);
};