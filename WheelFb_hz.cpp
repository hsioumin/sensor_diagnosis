#include "ros/ros.h"
#include "std_msgs/String.h"
#include "std_msgs/Float32.h"
#include "andbot/WheelFb.h"
#include <sys/time.h>
#include <iostream>
#include <ctime>
#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include <pthread.h>

#define MICRO_PERSEC    1000000
#define BUFF_SIZE               1000

using namespace std;

static int64_t Count=0;

void* publish_WheelFb(void* pVoid)
{
        struct timeval current_time, prev_time;
        int64_t nCurr, nPrev, nDiff;
        int64_t prev_Count = 0;

        ros::NodeHandle n;
        ros::Publisher wheelfb_pub = n.advertise<std_msgs::Float32>("WheelFb_hz", BUFF_SIZE);

        while(ros::ok())
        {
                gettimeofday(&current_time,NULL);
                nCurr = ((int64_t)current_time.tv_sec)*MICRO_PERSEC + (int64_t)current_time.tv_usec;
                ::usleep(MICRO_PERSEC);
                gettimeofday(&prev_time,NULL);
                nPrev = ((int64_t)prev_time.tv_sec)*MICRO_PERSEC + (int64_t)prev_time.tv_usec;
                nDiff = nPrev - nCurr;
                std_msgs::Float32 wheelfb_hz;
                wheelfb_hz.data=(((float)(Count - prev_Count))/(float)nDiff)*MICRO_PERSEC;
                prev_Count = Count;
                wheelfb_pub.publish(wheelfb_hz);
                ros::spinOnce();
        }

        return NULL;
}


void wheelfb_Callback(const andbot::WheelFb WheelFbData)
{
        ++Count;
}

void* subscribe_WheelFb(void* pVoid)
{
        ros::NodeHandle n;
        ros::Subscriber sub_wheelfb = n.subscribe("/feedback_wheel_angularVel", BUFF_SIZE, wheelfb_Callback);
        ros::spin();

        return NULL;
}

int main(int argc, char **argv)
{
        ros::init(argc, argv, "WheelFb_hz");
        pthread_t thread1_sub;
        pthread_t thread2_pub;

        pthread_create(&thread1_sub, NULL, &subscribe_WheelFb, NULL);
        pthread_create(&thread2_pub, NULL, &publish_WheelFb, NULL);

        pthread_join(thread1_sub, NULL);
        pthread_join(thread2_pub, NULL);

        return 0;
}

