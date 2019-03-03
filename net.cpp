﻿#include "net.h"
#include<vector>
#include<sstream>
#include <functional>
Net::Net(){
	SDLNet_Init();
	sockets = SDLNet_AllocSocketSet(1);
}

Net::~Net(){
	SDLNet_TCP_Close(connection);
	SDLNet_TCP_DelSocket(sockets, connection);
	SDLNet_FreeSocketSet(sockets);
	SDLNet_Quit();
}

int Net::connect(std::string ip){
	SDLNet_ResolveHost(&server, ip.c_str(), 45451);
	connection = SDLNet_TCP_Open(&server);
	if(!connection)
		return -1;
	SDLNet_TCP_AddSocket(sockets, connection);
	return 0;
}

std::tuple<int, int> Net::login(int room){
	std::string request = "LOGIN " + std::to_string(room);
	char data;
	std::string reply;
	int result, length = request.length();
	SDLNet_TCP_Send(connection, request.c_str(), length);
	if(length > result)
		return std::make_tuple<int, int>(-1, -1);
	while(SDLNet_CheckSockets(sockets, 500) == 1){
		SDLNet_TCP_Recv(connection, &data, 1);
		reply += data;
	}
	std::stringstream stream(reply);
	std::string temp;
	std::vector<std::string> replys;
	while(std::getline(stream, temp)){
		if(!temp.empty()){
			replys.push_back(temp);
		}
	}
	if(!replys[0].compare("SUCCESS")){
		return std::make_tuple<int, int>(std::stoi(replys[1]), std::stoi(replys[2]));
	}
	return std::make_tuple<int, int>(-1, -1);
}

std::tuple<int, int> Net::login(int room, std::string pass){
	std::string request = "LOGIN " + std::to_string(room) + "\r\nPASSWORD" + pass;
	char data;
	std::string reply;
	int result, length = request.length();
	SDLNet_TCP_Send(connection, request.c_str(), length);
	if(length > result)
		return std::make_tuple<int, int>(-1, -1);
	while(SDLNet_CheckSockets(sockets, 500) == 1){
		SDLNet_TCP_Recv(connection, &data, 1);
		reply += data;
	}
	std::stringstream stream(reply);
	std::string temp;
	std::vector<std::string> replys;
	while(std::getline(stream, temp)){
		if(!temp.empty()){
			replys.push_back(temp);
		}
	}
	if(!replys[0].compare("SUCCESS")){
		return std::make_tuple<int, int>(std::stoi(replys[1]), std::stoi(replys[2]));
	}
	return std::make_tuple<int, int>(-1, -1);
}

int Net::makeroom(){
	std::string request = "ROOM";
	char data;
	std::string reply;
	int result, length = request.length();
	SDLNet_TCP_Send(connection, request.c_str(), length);
	if(length > result)
		return -1;
	while(SDLNet_CheckSockets(sockets, 500) == 1){
		SDLNet_TCP_Recv(connection, &data, 1);
		reply += data;
	}
	std::stringstream stream(reply);
	std::string temp;
	std::vector<std::string> replys;
	while(std::getline(stream, temp)){
		if(!temp.empty()){
			replys.push_back(temp);
		}
	}
	if(!replys[0].compare("SUCCESS")){
		return std::stoi(replys[1]);
	}
	return -1;
}

int Net::setpassword(std::string pass){
	std::string request = "SETPASSWORD" + pass;
	char data;
	std::string reply;
	int result, length = request.length();
	SDLNet_TCP_Send(connection, request.c_str(), length);
	if(length > result)
		return -1;
	while(SDLNet_CheckSockets(sockets, 500) == 1){
		SDLNet_TCP_Recv(connection, &data, 1);
		reply += data;
	}
	if(!reply.compare("SUCCESS"))return 0;
	return -1;
}

int Net::setboardsize(int x, int y){
	std::string request = "SETBOARD" + std::to_string(x)+std::to_string(y);
	char data;
	std::string reply;
	int result, length = request.length();
	SDLNet_TCP_Send(connection, request.c_str(), length);
	if(length > result)
		return -1;
	while(SDLNet_CheckSockets(sockets, 500) == 1){
		SDLNet_TCP_Recv(connection, &data, 1);
		reply += data;
	}
	if(!reply.compare("SUCCESS"))return 0;
	return -1;
}

int Net::put(int x, int y){
	std::string request = "PUT " + std::to_string(x) + " " + std::to_string(y);
	int result, length = request.length();
	SDLNet_TCP_Send(connection, request.c_str(), length);
	if(length > result)
		return -1;
	return 0;
}

std::tuple<const char*, int, int> Net::get(){
	char data;
	std::string ret;
	while(SDLNet_CheckSockets(sockets, 0) == 1){
		SDLNet_TCP_Recv(connection, &data, 1);
		ret += data;
	}
	std::vector<std::string> parsed;
	std::stringstream stream{ret};
	std::string buf;
	while(std::getline(stream, buf, ' ')){
		parsed.push_back(buf);
	}
	return std::make_tuple(parsed[0].c_str(), std::stoi(parsed[1]), std::stoi(parsed[2]));
}