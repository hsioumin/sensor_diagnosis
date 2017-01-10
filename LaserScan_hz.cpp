#include "ros/ros.h"
#include "std_msgs/String.h"
#include "std_msgs/Float32.h"
#include "sensor_msgs/LaserScan.h"
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

void* publish_LaserScan(void* pVoid)
{
        struct timeval current_time, prev_time;
        int64_t nCurr, nPrev, nDiff;
        int64_t prev_Count = 0;

        ros::NodeHandle n;
        ros::Publisher laserscan_pub = n.advertise<std_msgs::Float32>("LaserScan_hz", BUFF_SIZE);

        while(ros::ok())
        {
                gettimeofday(&current_time,NULL);
                nCurr = ((int64_t)current_time.tv_sec)*MICRO_PERSEC + (int64_t)current_time.tv_usec;
                ::usleep(MICRO_PERSEC);
                gettimeofday(&prev_time,NULL);
                nPrev = ((int64_t)prev_time.tv_sec)*MICRO_PERSEC + (int64_t)prev_time.tv_usec;
                nDiff = nPrev - nCurr;
                std_msgs::Float32 laserscan_hz;
                laserscan_hz.data=(((float)(Count - prev_Count))/(float)nDiff)*MICRO_PERSEC;
                prev_Count = Count;
                laserscan_pub.publish(laserscan_hz);
                ros::spinOnce();
        }

        return NULL;
}

int main(int argc, char **argv)
{
        ros::init(argc, argv, "LaserScan_hz");
        pthread_t thread1_sub;
        pthread_t thread2_pub;

        pthread_create(&thread1_sub, NULL, &subscribe_LaserScan, NULL);
        pthread_create(&thread2_pub, NULL, &publish_LaserScan, NULL);

        pthread_join(thread1_sub, NULL);
        pthread_join(thread2_pub, NULL);

        return 0;
}


