/*! @file missions/main.cpp
 *  @version 3.3
 *  @date Jun 05 2017
 *
 *  @brief
 *  main for GPS Missions API usage in a Linux environment.
 *  Shows example usage of the Waypoint Missions and Hotpoint Missions through
 *  the
 *  Mission Manager API.
 *
 *  @Copyright (c) 2017 DJI
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 */

#include "mission_sample.hpp"
#include "uart.hpp"
#include <string>

using namespace DJI::OSDK;
using namespace DJI::OSDK::Telemetry;
using namespace std;




int
main(int argc, char** argv)
{
  // Initialize variables
  int functionTimeout = 1;

  // Setup OSDK.
  LinuxSetup linuxEnvironment(argc, argv);
  Vehicle*   vehicle = linuxEnvironment.getVehicle();
  if (vehicle == NULL)
  {
    std::cout << "Vehicle not initialized, exiting.\n";
    return -1;
  }

  // Obtain Control Authority
  vehicle->obtainCtrlAuthority(functionTimeout);

  // Setup variables for use
  int wayptPolygonSides;
  int     hotptInitRadius;
  int     responseTimeout = 1;

  // Display interactive prompt
  std::cout
    << "| Available commands:                                            |"
    << std::endl;


  int sumpost =0;
  bool flag1 = true;
  bool flag2 = false;
  int len = 0;
  int fd;
  char hex_buf[210];//存放读入的数据
  struct post p[99];
 	struct Uart_Set Uart4; //初始化串口参数
    Uart4.speed = 9600;
    Uart4.flow_ctrl = 0;
    Uart4.databits = 8;
    Uart4.stopbits = 1;
    Uart4.parity = 'N';
	  fd = open(DEV_NAME, O_RDWR);
    if(fd < 0) {
            perror(DEV_NAME);
            return -1;
    }
    UART_Set(fd,Uart4.speed,Uart4.flow_ctrl,Uart4.databits,Uart4.stopbits,Uart4.parity);  
    printf("----------UART_TEST-------------\n");

    //runWaypointMission(vehicle, 5, responseTimeout,p);
    while(1){
      memset(hex_buf,-1,sizeof hex_buf);
      len = UART_Recv(fd,hex_buf, 17);
      Printf_Buf(hex_buf,len);
      if(hex_buf[0]=='s'&&hex_buf[1]=='t'&&hex_buf[2]=='a'){
      	flag1 = true; 
      	flag2 = true;
        UART_Send(fd,(char*)"start\n\r",15);
      } 
      if(flag1 &&flag2){
      	if(hex_buf[0]=='l'&&hex_buf[1]=='a'){
      	 	addPostLatitude(hex_buf,len,p);
      	}
      	if(hex_buf[0]=='l'&&hex_buf[1]=='o'){
      		addPostLongitude(hex_buf,len,p);
      	}
      }
      if(hex_buf[0]=='e'&&hex_buf[1]=='n'&&hex_buf[2]=='d'){
      	flag1 = false; 
      	flag2 = false;
	      wayptPolygonSides = (hex_buf[6]-'0')*10+hex_buf[7]-'0';
		    UART_Send(fd,(char*)"ending\n\r",15);
      } 
      
      if(!flag1&&!flag2){
      	for(int i=0;i<wayptPolygonSides;i++){
      		cout<<"k: "<<i<<" "<<"p[k].latitude: ";
      		printf("%lf\n", p[i].latitude);
      		cout<<"k: "<<i<<" "<<"p[k].longitude: ";
      		printf("%lf\n", p[i].longitude);
      	}
      }
      if(!flag1&&!flag2){
	  		cout<<"wayptPolygonSides: "<<wayptPolygonSides<<endl;
      	 if(hex_buf[0]=='f'&&hex_buf[1]=='l'&&hex_buf[2]=='y'){
	        UART_Send(fd,(char*)"flying\n\r",15);
          runWaypointMission(vehicle, wayptPolygonSides, responseTimeout,p);
	        flag1=true;
      		flag2=false;
        }
      }
      if(hex_buf[0]=='t'&&hex_buf[1]=='a'&&hex_buf[2]=='k'&&hex_buf[3]=='e'){
      	 monitoredTakeoff(vehicle);
		     UART_Send(fd,(char*)"takeoff\n\r",15);
      }
      if(hex_buf[0]=='L'&&hex_buf[1]=='a'&&hex_buf[2]=='n'&&hex_buf[3]=='d'){
      	 monitoredLanding(vehicle);
		     UART_Send(fd,(char*)"landing\n\r",15);
      }
      if(hex_buf[0]=='G'&&hex_buf[1]=='o'&&hex_buf[2]=='H'&&hex_buf[3]=='o'){
      	vehicle->control->goHome(6);
	    	UART_Send(fd,(char*)"gohome\n\r",15);
      }
 
  }


  return 0;
}