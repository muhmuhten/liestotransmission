#include <dlfcn.h>
#include <err.h>
#include <errno.h>
#include <netinet/in.h>

int setsockopt(int s, int level, int name, const void *val, socklen_t len) {
	int (*super)() = (int (*)())dlsym(RTLD_NEXT, "setsockopt");
	int res = super(s, level, name, val, len);

	/* silence complaints from trying to set TOS on dead socket */
	if (res == -1 && errno == EINVAL && level == IPPROTO_IP && name == IP_TOS)
		return 0;

	return res;
}

int evhttp_accept_socket(void *http, int fd);
int evhttp_bind_socket(void *http, const char *address, uint16_t port) {
	return evhttp_accept_socket(http, 0);
}
