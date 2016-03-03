#include <sys/socket.h>
#include <sys/un.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

char *socket_path = "/tmp/librpip-socket";

int main(int argc, char *argv[]) {
	struct sockaddr_un addr;
	char buf[350];
	int fd,rc;

	if ( (fd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
		perror("socket error");
		exit(-1);
	}

	memset(&addr, 0, sizeof(addr));
	addr.sun_family = AF_UNIX;
	strncpy(addr.sun_path, socket_path, sizeof(addr.sun_path)-1);

	if (connect(fd, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
		perror("connect error");
		exit(-1);
	}

	rc=read(STDIN_FILENO, buf, sizeof(buf));
	
	if (write(fd, buf, rc) != rc) {
		if (rc > 0) 
			printf("partial write\n");
		else {
			perror("write error");
			exit(-1);
		}
	}
	
	rc=read(fd,buf,sizeof(buf)-1);
	if(rc > 0) {
		buf[rc]='\0';
		printf("%s",buf);
	}
	return 0;
}
