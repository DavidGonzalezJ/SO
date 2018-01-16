/*
 * Extracted from fftest.c developed by Johann Deneux
 */

/*
 * Tests the force feedback driver
 */

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <linux/input.h>

int main(int argc, char** argv)
{
	struct ff_effect effect;
	struct input_event gain, play, stop;
	int fd;
	const char * device_file_name = "/dev/input/event0";

	int i;

	printf("HOLD FIRMLY YOUR WHEEL OR JOYSTICK TO PREVENT DAMAGES\n\n");

	for (i=1; i<argc; ++i) {
		if (strncmp(argv[i], "--help", 64) == 0) {
			printf("Usage: %s /dev/input/eventXX\n", argv[0]);
			printf("Tests the force feedback driver\n");
			exit(1);
		}
		else {
			device_file_name = argv[i];
		}
	}

	/* Open device */
	fd = open(device_file_name, O_RDWR);
	if (fd == -1) {
		perror("Open device file");
		exit(1);
	}
	printf("Device %s opened\n", device_file_name);


	/***************************************/
	/* Set master gain to 75% if supported */
	/***************************************/
	memset(&gain, 0, sizeof(gain));
	gain.type = EV_FF;
	gain.code = FF_GAIN;
	gain.value = 0x9FFF; /* [0, 0xFFFF]) */

	printf("Setting master gain to 75%% ... ");
	fflush(stdout);
	if (write(fd, &gain, sizeof(gain)) != sizeof(gain)) {
	  perror("Error:");
	} else {
	  printf("OK\n");
	}

	/*******************************/
	/*  periodic sinusoidal effect */
	/*******************************/
	memset(&effect,0,sizeof(effect));
	effect.type = FF_PERIODIC;
	effect.id = -1;
	effect.u.periodic.waveform = FF_SINE;
	effect.u.periodic.period = 100;	/* 0.1 second */
	effect.u.periodic.magnitude = 0xffff;	/* Maximum magnitude */
	effect.u.periodic.offset = 0;
	effect.u.periodic.phase = 0;
	effect.direction = 0x4000;	/* Along X axis */
	effect.u.periodic.envelope.attack_length = 1000;
	effect.u.periodic.envelope.attack_level = 0x7fff;
	effect.u.periodic.envelope.fade_length = 1000;
	effect.u.periodic.envelope.fade_level = 0x7fff;
	effect.trigger.button = 0;
	effect.trigger.interval = 0;
	effect.replay.length = 4000;  /* 4 seconds */
	effect.replay.delay = 1000;

	fflush(stdout);
	if (ioctl(fd, EVIOCSFF, &effect) == -1) {
		perror("Error:");
	} else {
		printf("OK (id %d)\n", effect.id);
	}

	/*************/
	/* Main loop */
	/*************/
	printf("\n\n");
	for (i=0; i<16; i+=2)
	{
		/* Set master gain to x% if supported */
		memset(&gain, 0, sizeof(gain));
		gain.type = EV_FF;
		gain.code = FF_GAIN;
		gain.value = i<<12;

		printf("Setting master gain to %i%% ... ", i*100/16);
		fflush(stdout);
		if (write(fd, &gain, sizeof(gain)) != sizeof(gain)) {
		  perror("Error:");
		} else {
		  printf("OK\n");
		}

		/* Start the effect */
		memset(&play,0,sizeof(play));
		play.type = EV_FF;
		play.code = effect.id;
		play.value = 1;

		if (write(fd, (const void*) &play, sizeof(play)) == -1) {
			perror("Play effect");
			exit(1);
		}
		printf("Now Playing: Periodic sinusoidal\n\n");
		sleep(5);
	}
	/* Stop the effect */
	printf("Stopping effects\n");
	memset(&stop,0,sizeof(stop));
	stop.type = EV_FF;
	stop.code =  effect.id;
	stop.value = 0;

	if (write(fd, (const void*) &stop, sizeof(stop)) == -1) {
		perror("");
		exit(1);
	}
	

	exit(0);
}
