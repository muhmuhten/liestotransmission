#include <err.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>

int main(int argc, char **argv) {
	struct sockaddr_un sa = {.sun_family = AF_UNIX};

	if (argc < 2)
		errx(2, "usage: socketbinder path command...");

	close(0);
	if (socket(PF_UNIX, SOCK_STREAM|SOCK_NONBLOCK, 0) != 0)
		err(1, "socket");

	*stpncpy(sa.sun_path, argv[1], sizeof sa.sun_path) = 0;
	unlink(sa.sun_path);
	if (bind(0, (struct sockaddr *)&sa, sizeof sa) == -1)
		err(1, "bind");

	if (listen(0, SOMAXCONN) == -1)
		err(1, "listen");

	execvp(argv[2], argv+2);
	err(1, "exec");
}
