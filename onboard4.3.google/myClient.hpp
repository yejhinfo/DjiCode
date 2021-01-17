#ifndef MYCLIENT_HPP
#define MYCLIENT_HPP

int initSocket(); // 初始化串口

int SendCMD(char sendBuffer[], char recvCode[]);   // 传入要发送的命令参数，以及匹配返回的状态码是否正确

int closeSocket();

#endif // MYCLIENT_HPP
