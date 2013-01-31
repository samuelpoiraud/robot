/*
fichier nécessaire au test
*/

#include "main.h"

int main (int argc, char *argv[])
{
	pthread_t ihm_thread_display;
	pthread_t ihm_thread_receiver;

	pthread_create(&ihm_thread_display, NULL,IHM_thread_display, NULL);
	pthread_create(&ihm_thread_receiver, NULL,IHM_thread_receiver, NULL);

	printf("thread lancé \n");
	while(1);
	return 0;
}

