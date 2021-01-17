#include <stdio.h>
#include <Windows.h>

#include <QDebug>

#pragma comment(lib, "ws2_32.lib")
#define Port 5000
#define IP_ADDRESS "192.168.1.201"
#define BUFFER_SIZE 20

WSADATA s; // 用来储存调用AfxSocketInit全局函数返回的Windows Sockets初始化信息

SOCKET ClientSocket;
struct sockaddr_in ClientAddr;
// 一个sockaddr_in型的结构体对象
int ret = 0;
//char SendBuffer[MAX_PATH];   // Windows的MAX_PATH默认是260
int lenth = 0;
char RecvBuffer[BUFFER_SIZE];


int initSocket()
{
    // 初始化Windows Socket
    // WSAStartup函数对Winsock服务的初始化
    if (WSAStartup(MAKEWORD(2, 2), &s) != 0) // 通过连接两个给定的无符号参数,首个参数为低字节
    {
        qDebug() << "Init Windows Socket Failed! Error: %d\n", GetLastError();
        getchar();
        return -1;
    }



   return 0;
}


int SendCMD(char sendBuffer[], char recvCode[]){

    // 创建一个套接口
    // 如果这样一个套接口用connect()与一个指定端口连接
    // 则可用send()和recv()与该端口进行数据报的发送与接收
    // 当会话结束后，调用closesocket()
    ClientSocket = socket(AF_INET, // 只支持ARPA Internet地址格式
        SOCK_STREAM, // 新套接口的类型描述
        IPPROTO_TCP); // 套接口所用的协议
    if (ClientSocket == INVALID_SOCKET)
    {
        qDebug() << "Create Socket Failed! Error: %d\n", GetLastError();
        getchar();
        return -1;
    }

    ClientAddr.sin_family = AF_INET;
    ClientAddr.sin_addr.s_addr = inet_addr(IP_ADDRESS); // 定义IP地址
    ClientAddr.sin_port = htons(Port); // 将主机的无符号短整形数转换成网络字节顺序
    memset(ClientAddr.sin_zero, 0X00, 8); // 函数通常为新申请的内存做初始化工作




    // 连接Socket
    ret = connect(ClientSocket,
        (struct sockaddr*)&ClientAddr,
        sizeof(ClientAddr));
    if (ret == SOCKET_ERROR)
    {
        qDebug() << "Socket Connect Failed! Error:%d\n", GetLastError();
        getchar();
        return -1;
    }
    else
    {
        qDebug() << "Socket Connect Succeed!";
    }

            // 发送数据至服务器
            ret = send(ClientSocket,
                sendBuffer,
                (int)strlen(sendBuffer), // 返回发送缓冲区数据长度
                0);

            if (ret == SOCKET_ERROR)
            {
                qDebug() << "Send Information Failed! Error:%d\n", GetLastError();
                //getchar();
                //break;
            }

            // wait for answer
            lenth = recv(ClientSocket, RecvBuffer, BUFFER_SIZE, 0);
            if (lenth < 0)
            {
                qDebug() << "Didn't get the answer! Waitting...\n";
            }
            printf("%s\n", RecvBuffer);
            if (RecvBuffer == recvCode)
                qDebug() << "It's the right answer!\n";
            else
                qDebug() << "THe answer is wrong!";
            //break;

            // 关闭socket
            closesocket(ClientSocket);

    return 0;

}




int closeSocket(){



    qDebug() << "Quit!\n";
    //break;

    //break;


    WSACleanup();
    //getchar();

    return 0;
}


