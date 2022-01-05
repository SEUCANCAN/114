#include <ros/ros.h>
#include <stdio.h>   
#include <sys/types.h>   
#include <sys/socket.h>   
#include <netinet/in.h>   
#include <unistd.h>   
#include <errno.h>   
#include <string.h>   
#include <stdlib.h>
#include <geometry_msgs/Twist.h>  
#include <iostream>
#define SERV_PORT   8080   
uint8_t speed_cmd=0x00;
uint8_t steer_cmd=0x46;
uint8_t nocontorltime=0;
int enable=0;
/////
int speed_try ;
int steer_try ;
void timerCallback(const ros::TimerEvent& event)
{
    //ROS_INFO("enable= %d",enable); 
    if (nocontorltime == 10)
    {
        enable = 0;
        //speed_cmd=0x00;
        //steer_cmd=0x3a;
        speed_cmd=speed_try;
        steer_cmd=steer_try;
       
    }
    else
    {
        nocontorltime = nocontorltime + 1;
        //steer_cmd=0x46;
        enable = 1;
    }

}

void cmdCallback(const geometry_msgs::Twist::ConstPtr& msg)
{
  nocontorltime = 0;
     
    //ROS_INFO("enable=1");
    if (msg->linear.x < 0 || msg->linear.x > 50)
    {
       speed_cmd=0x00;
    }
    else
    {
       speed_cmd = msg->linear.x;
    }
    if (msg->angular.x < 0 || msg->angular.x > 150)
    {
       steer_cmd=0x46;
    }
    else
    {
       steer_cmd=msg->angular.x;
    }
    
    if (msg->linear.y == 1 )
    {
       speed_cmd=0x00;
       steer_cmd=0x46; 
    }
 

    return;
}

int main(int argc, char** argv)  
{  
    ros::init(argc, argv, "socket");
    ros::NodeHandle n; //创建句柄（虽然后面没用到这个句柄，但如果不创建，运行时进程会出错）
    ros::Timer timer = n.createTimer(ros::Duration(0.02), timerCallback);
    ros::Subscriber sub = n.subscribe("/cmd",1000,cmdCallback);
    ros::param::set("/sp",0);
    ros::param::set("/st",0);  
  /* sock_fd --- socket文件描述符 创建udp套接字*/  
    int sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if(sock_fd < 0)  
    {  
      perror("socket");  
      exit(1);  
    }  
 
  /* 将套接字和IP、端口绑定 */  
    struct sockaddr_in addr_serv;  
    int len;  
    memset(&addr_serv, 0, sizeof(struct sockaddr_in));  
    addr_serv.sin_family = AF_INET;   
    addr_serv.sin_port = htons(SERV_PORT);
    addr_serv.sin_addr.s_addr = htonl(INADDR_ANY);  
    ROS_INFO("%f",addr_serv.sin_addr.s_addr);
    len = sizeof(addr_serv);  
  

    int opt = 1;
    // sockfd为需要端口复用的套接字
    setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, (const void *)&opt, sizeof(opt));

    struct timeval timeout;
    timeout.tv_sec = 0; timeout.tv_usec=10000;

    setsockopt(sock_fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)); 

    /* 绑定socket */  
    if(bind(sock_fd, (struct sockaddr *)&addr_serv, sizeof(addr_serv)) < 0)  
    {  
      perror("bind error:");  
      exit(1);  
    }  
    
      
    int recv_num;  
    int send_num;
    uint8_t recv_buf[500];  
    struct sockaddr_in addr_client;  
    ros::Rate loop_rate(80);

/////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
    printf("server wait:\n"); 
    recv_num = recvfrom(sock_fd, recv_buf, sizeof(recv_buf), 0, (struct sockaddr *)&addr_client, (socklen_t *)&len);   
    if(recv_num < 0)  
    {  
      //perror("recvfrom error:");  
      //exit(1);  
    }  
    char id = recv_buf[0];   
    printf("server receive %d bytes. id :%d\n", recv_num, id);  

*/
/////////////////////////////////////////////////////////////////////////////////////////////////////////

  while(ros::ok())  
  {  
    ros::spinOnce();
    ros::param::get("/sp",speed_try);
    ros::param::get("/st",steer_try);
    uint8_t send_buf[4]={0x0d,0x0a,speed_cmd,steer_cmd}; 
    //uint8_t send_buf[4]={0x0d,0x0a,0x19,steer_cmd}; 
    /*for(int i=0; i<4; i++)
    {
                //16进制的方式打印到屏幕
        std::cout << std::hex << (send_buf[i] & 0xff) << " ";
    }
    std::cout << std::endl; */
////注意一下这以下！！！！

     //printf("server wait:\n"); 
    recv_num = recvfrom(sock_fd, recv_buf, sizeof(recv_buf), 0, (struct sockaddr *)&addr_client, (socklen_t *)&len);
    
    if(recv_num == 3)  
    {  
      char id = recv_buf[0];   
      //printf("server receive %d bytes. id :%d\n", recv_num, id);  
      printf("feedback info: id: %d  speed:%d  steer: %d\n", id, recv_buf[1], recv_buf[2]);
    }
    else
    {
      ROS_INFO("No vaild data.");
    }
    

  ///////注意以下这以上！！！
    //if (id == 1)
    if (1 == 1)
    {
       send_num = sendto(sock_fd, send_buf, 4, 0, (struct sockaddr *)&addr_client, len);  
        for(int i=0; i<4; i++)
        {
              //send_num = sendto(sock_fd, send_buf[i], recv_num, 0, (struct sockaddr *)&addr_client, len);
                //16进制的方式打印到屏幕
            std::cout << /*std::hex << */(send_buf[i] & 0xff) << " ";
        }
        std::cout << std::endl;
    } 
    if(send_num < 0)  
    {  
      //perror("sendto error:");  
      //exit(1);  
    }  
    loop_rate.sleep();
  }  
    
  close(sock_fd);  
    
  return 0;  
}
