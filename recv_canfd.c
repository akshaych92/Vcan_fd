#include <stdio.h>
#include <stdlib.h> // For exit()
#include <string.h>
#include <unistd.h>

#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/socket.h>

#include <linux/can.h>
#include <linux/can/raw.h>

//#define CANFD_MTU (sizeof(struct canfd_frame))
int main()
{

    struct sockaddr_can addr;
    struct ifreq ifr;
    struct canfd_frame frame;
    int s, nbytes;
    int enable_canfd = 1;

    printf("CAN Sockets Receive Demo\r\n");

    if ((s = socket(PF_CAN, SOCK_RAW, CAN_RAW)) < 0)
    {
        perror("Socket");
        return 1;
    }

    strcpy(ifr.ifr_name, "vcan0");
    ioctl(s, SIOCGIFINDEX, &ifr);

    memset(&addr, 0, sizeof(addr));
    addr.can_family = AF_CAN;    
    addr.can_ifindex = ifr.ifr_ifindex;
  
    if (setsockopt(s, SOL_CAN_RAW, CAN_RAW_FD_FRAMES, &enable_canfd, sizeof(enable_canfd))){
			printf("error when enabling CAN FD support\n");
			return 1;
	}

    if (bind(s, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
        perror("Bind");
        return 1;
    }

    struct can_filter rfilter[2];

    rfilter[0].can_id = 0x555;
    setsockopt(s, SOL_CAN_RAW, CAN_RAW_FD_FILTER, &rfilter, sizeof(rfilter));

    nbytes = read(s, &frame, CANFD_MTU);

    if (nbytes < 0)
    {
        perror("Read");
        return 1;
    }

    printf("0x%03X [%d] ", frame.can_id, frame.len);

    return 0;
}