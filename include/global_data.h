//defined some datastructure to store global data

#ifndef __GLOBAL_DATA_H__
#define __GLOBAL_DATA_H_
#endif

struct move_cmd
{
	int cmd_type;
	int angle;
	int vel;
};
struct move_info
{	
	int vel_info;
	int temperature;
	int dist;
};
struct socket_flag
{
	unsigned char *aimed_ip;
	int port;
        int socket_con_status;
	int data_trans_status;
};

struct udp_flag
{
	unsigned char *aimed_ip;
	int port;
        int video_send_status;
};
struct video_data
{
        unsigned char *start_data;
        int length;
	int send_status;
};
