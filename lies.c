#include <dlfcn.h>
#include <err.h>
#include <errno.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/un.h>

int setsockopt(int s, int level, int name, const void *val, socklen_t len) {
	int (*super)() = (int (*)())dlsym(RTLD_NEXT, "setsockopt");
	errno = 0;
	int res = super(s, level, name, val, len);

	/* silence complaints from trying to set TOS on dead socket */
	if (res == -1 && errno == EINVAL && level == IPPROTO_IP && name == IP_TOS)
		return 0;

	return res;
}

static const struct sockaddr_un sa_un_in = {
	.sun_family = AF_UNIX,
	.sun_path = "in",
};
int evhttp_accept_socket(void *http, int fd);
#define FAIL(...) do { warn("(lies)" __VA_ARGS__); goto fail; } while (0)
int evhttp_bind_socket(void *http, const char *address, uint16_t port) {
	/* only called for RPC server, so not going to bother parsing address */
	int fd = socket(AF_UNIX, SOCK_STREAM|SOCK_NONBLOCK, 0);
	if (fd == -1)
		FAIL("socket");
	if (bind(fd, (struct sockaddr *)&sa_un_in, sizeof sa_un_in) == -1)
		FAIL("bind");
	if (listen(fd, 128) == -1)
		FAIL("listen");

	return evhttp_accept_socket(http, fd);

fail:
	close(fd);
	return -1;
}
#undef FAIL
