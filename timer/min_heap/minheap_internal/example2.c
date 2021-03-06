#include <stdio.h>
#include <sys/epoll.h>

#include "min_heap.h"

time_heap_t *heap;
minheap_node_t *n1 = NULL;
minheap_node_t *n2 = NULL;

void n2_timer(void *args);

void n3_timer(void *args)
{
	static int count = 0;
	printf("n3_timer %u\n", count++);
}


void n1_timer(void *args)
{
	printf("n1_timer: %u\n", time(NULL));


	//MINHEAP_TIMER_OFF(heap, n2);
	
	MINHEAP_TIMER_ON(heap, n2, n2_timer, NULL, 5000);

	return;
}

void n2_timer(void *args)
{
	printf("n2_timer: %u\n", time(NULL));

	return;
}


int main()
{
	int n;
	int epollfd = 0;
	struct epoll_event ev[10];
	int expire = 0;
	minheap_node_t node[1000];
	minheap_node_t *nd;
	int timeout = 0;
	
	heap = time_heap_init();

	//MINHEAP_TIMER_ON(heap, n1, n1_timer, NULL, 1000);
	//MINHEAP_TIMER_ON(heap, n2, n2_timer, NULL, 3000);

	for(n = 0; n < 1000; n++) {
		nd = &node[n];
		MINHEAP_TIMER_ON(heap, nd, n3_timer, NULL, timeout);
		timeout += 100;
	}



	epollfd = epoll_create(1);


	while(1) {
		expire = 1;

		time_heap_tick(heap);

		time_heap_time_wait(heap, &expire);

		//printf("expire: %u\n", expire);
		int n = epoll_wait(epollfd, ev, 10, expire);
	}

	close(epollfd);
	
	return 0;
}
