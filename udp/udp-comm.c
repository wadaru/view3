#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>

int view3Send[9], view3Recv[9];

int
main(int argc, char *argv[])
{
	int sockSend, sockRecv;
	int portSend, portRecv;
	char IPSend[16], IPRecv[16];
	int i;
	int yes = 1;
	struct sockaddr_in addrSend, addrRecv;

	for(i = 1; i < 9; i++) view3Send[i] = i + 10;
	unsigned char buf[256];
	unsigned char checkSum;

	sockSend = socket(AF_INET, SOCK_DGRAM, 0);

	if (argc != 5) {
		portSend = 9180; portRecv = 9182;
		strcpy(IPSend, "127.0.1.1");
	        strcpy(IPRecv, "127.0.1.1");
	} else {
		strcpy(IPSend, argv[1]);
		portSend = atoi(argv[2]);
		strcpy(IPRecv, argv[3]);
		portRecv = atoi(argv[4]);
	}

	printf("UDP connection: send %s:%d, recv %s:%d\n", IPSend, portSend, IPRecv, portRecv);

	addrSend.sin_family = AF_INET;
	addrSend.sin_port = htons(portSend);
	addrSend.sin_addr.s_addr = inet_addr(IPSend);
	// addr.sin_addr.s_addr = inet_addr("255.255.255.255");
	setsockopt(sockSend, SOL_SOCKET, SO_BROADCAST, (char *)&yes, sizeof(yes));

	sockRecv = socket(AF_INET, SOCK_DGRAM, 0);
	addrRecv.sin_family = AF_INET;
	addrRecv.sin_port = htons(portRecv);
	addrRecv.sin_addr.s_addr = inet_addr(IPRecv); // INADDR_ANY;
	bind(sockRecv, (struct sockaddr *)&addrRecv, sizeof(addrRecv));

	memset(buf, 0, sizeof(buf));


for(;;){

	printf("start\n");
	if (recv(sockRecv, buf, sizeof(buf), MSG_WAITALL) > 0) {
		if (buf[0] == 0) { // we can receive only Message 0.
			for (i = 1; i < 9; i++) {
				view3Recv[i] = (buf[i * 4 + 3] << 24) + (buf[i * 4 + 2] << 16) + (buf[i * 4 + 1] << 8) + (buf[i * 4]);
				printf("INT%d = %x, ", i, view3Recv[i]);
			}
			printf("\n");
			view3Send[1] = view3Recv[1] + 1;	// seq number
		}
		buf[0] = 0;
		buf[1] = 36;
		buf[2] = 0;
		buf[3] = 0; // checkSum;
		for (i = 1; i < 9; i++) {
			buf[i * 4    ] =  view3Send[i]        & 0xff;
			buf[i * 4 + 1] = (view3Send[i] >>  8) & 0xff;
			buf[i * 4 + 2] = (view3Send[i] >> 16) & 0xff;
			buf[i * 4 + 3] = (view3Send[i] >> 24) & 0xff;
		}
		checkSum = 0;
		for (i = 0; i < 36; i++) checkSum += buf[i];
		buf[3] = 0xff - checkSum;
		printf("checkSum = %d\n", checkSum);
		printf("sendto: %d\n", sendto(sockSend, buf, 36, 0, (struct sockaddr *)&addrSend, sizeof(addrSend)));

	}
	// sleep(0.1);
}
	close(sockSend);
	close(sockRecv);

	return 0;
}
