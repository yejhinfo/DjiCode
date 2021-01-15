#ifndef UART_HPP
#define UART_HPP

//宏定义  
#define FALSE  -1  
#define TRUE   0  
#define DEV_NAME  "/dev/ttyUSB0"


//串口相关的头文件  
#include<stdio.h>      /*标准输入输出定义*/  
#include<stdlib.h>     /*标准函数库定义*/  
#include<unistd.h>     /*Unix 标准函数定义*/  
#include<sys/types.h>   
#include<sys/stat.h>     
#include<fcntl.h>      /*文件控制定义*/  
#include<termios.h>    /*PPSIX 终端控制定义*/  
#include<errno.h>      /*错误号定义*/  
#include<string.h>  
#include<math.h>
#include<stdlib.h>
#include <fcntl.h>


//串口参数
struct Uart_Set
{
  int speed;
  int flow_ctrl;
  int databits;
  int stopbits;
  int parity; 
};


int  UART_Set(int fd,int speed,int flow_ctrl,int databits,int stopbits,int parity); 
void Printf_Buf(char hex_buf[],int len);
int  UART_Recv(int fd, char *buf,int size);
int  UART_Send(int fd, char *send_buf,int data_len);

#endif // UART_HPP
