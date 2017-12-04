#include <arpa/inet.h>
#include <sys/socket.h>
#include <netdb.h>
#include <ifaddrs.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <net/if.h>

static int make_local_listeners_list(void)
{
	int ret = 0;
	struct ifaddrs * ifs = NULL;
	struct ifaddrs * ifa = NULL;
	char saddr[INET6_ADDRSTRLEN] = "";
    int family;

	if((getifaddrs(&ifs) == 0) && ifs) {

		for (ifa = ifs; ifa != NULL; ifa = ifa->ifa_next) {

            family = ifa->ifa_addr->sa_family;

			if(!(ifa->ifa_flags & IFF_UP))
				continue;

			if(!(ifa->ifa_addr))
				continue;

			if (ifa ->ifa_addr->sa_family == AF_INET) {
				if(!inet_ntop(AF_INET, &((struct sockaddr_in *) ifa->ifa_addr)->sin_addr, saddr, INET_ADDRSTRLEN))
					continue;

				if(strstr(saddr,"169.254.") == saddr)
					continue;
				
				if(!strcmp(saddr,"0.0.0.0"))
					continue;
			} else if (ifa->ifa_addr->sa_family == AF_INET6) {
				if(!inet_ntop(AF_INET6, &((struct sockaddr_in6 *) ifa->ifa_addr)->sin6_addr, saddr, INET6_ADDRSTRLEN))
					continue;
		
				if(strstr(saddr,"fe80") == saddr)
					continue;
				
				if(!strcmp(saddr,"::"))
			  		continue;
			} else {
				continue;
			}


		if(!(ifa->ifa_flags & IFF_LOOPBACK))
			ret++;

            printf("%s  address family: %d%s\n",
                       ifa->ifa_name, family,
                       (family == AF_PACKET) ? " (AF_PACKET)" :
                       (family == AF_INET) ?   " (AF_INET)" :
                       (family == AF_INET6) ?  " (AF_INET6)" : "");
    
			printf("address is:%s\n", saddr);
		}

		freeifaddrs(ifs);
	}
	
	return ret;
}

int main() 
{
	make_local_listeners_list();
	return 0;
}
