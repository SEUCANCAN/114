//serial_port.cpp
#include <ros/ros.h>
#include <serial/serial.h>
#include <geometry_msgs/Twist.h>
#include <iostream>
#include <ros/time.h>
#include <ros/duration.h>
uint8_t speed_cmd=0x00;
uint8_t steer_cmd=0x46;
uint8_t nocontorltime=0;
int enable=0; 

void timerCallback(const ros::TimerEvent& event)
{
    ROS_INFO("enalbe= %f",enable);
    if (nocontorltime == 5)
    {
        enable = 0;
        speed_cmd=0x00;
        steer_cmd=0x46;
    }
    else
    {
        nocontorltime = nocontorltime + 1;
        enable = 1;
        
    }

}



void cmdCallback(const geometry_msgs::Twist::ConstPtr& msg)
{
  nocontorltime = 0;
     
    ROS_INFO("enalbe=1");
    if (msg->linear.x < 0 || msg->linear.x > 32)
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

    ros::init(argc, argv, "serial_port");
    ros::NodeHandle n; //创建句柄（虽然后面没用到这个句柄，但如果不创建，运行时进程会出错）
    
    ros::Timer timer = n.createTimer(ros::Duration(0.1), timerCallback);
    
    ros::Subscriber sub = n.subscribe("/cmd",1000,cmdCallback);
    serial::Serial sp;//创建一个serial类
    serial::Timeout to = serial::Timeout::simpleTimeout(1000);//创建timeout
    sp.setPort("/dev/ttyUSB0");//设置要打开的串口名称
    sp.setBaudrate(115200);//设置串口通信的波特率
    sp.setTimeout(to);//串口设置timeout
    try
    {
        sp.open();
    }
    catch(serial::IOException& e)
    {
        ROS_ERROR_STREAM("Unable to open port.");
        return -1;
    }
    if(sp.isOpen())
    {
        ROS_INFO_STREAM("/dev/ttyUSB0 is opened.");
    }
    else
    {
        return -1;
    }
    ros::Rate loop_rate(500);
    while(ros::ok())
    {
        ros::spinOnce();
        //获取缓冲区内的字节数
        size_t n = 4;
        if(n!=0)
        {
            uint8_t buffer[4]={speed_cmd,steer_cmd,0x0d,0x0a};
            for(int i=0; i<4; i++)
            {
                //16进制的方式打印到屏幕
                std::cout << std::hex << (buffer[i] & 0xff) << " ";
            }
            std::cout << std::endl;
            //把数据发送回去
            sp.write(buffer, n);
        }
        loop_rate.sleep();
    }
    
    //关闭串口
    sp.close();
 
    return 0;
}
