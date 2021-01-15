#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <asm/types.h>
#include <linux/videodev2.h>

#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

extern "C" {
#include <jpeglib.h>
}

typedef struct {
  uint8_t* start;
  size_t length;
} buffer_t;

typedef struct {
  int fd;
  uint32_t width;
  uint32_t height;
  size_t buffer_count;
  buffer_t* buffers;
  buffer_t head;
} camera_t;


void quit(const char * msg);
int xioctl(int fd, int request, void* arg);
camera_t* camera_open(const char * device, uint32_t width, uint32_t height);
void camera_init(camera_t* camera);
void camera_start(camera_t* camera);
void camera_stop(camera_t* camera);
void camera_finish(camera_t* camera);
void camera_close(camera_t* camera);
int camera_capture(camera_t* camera);
int camera_frame(camera_t* camera, struct timeval timeout);
void jpeg(FILE* dest, uint8_t* rgb, uint32_t width, uint32_t height, int quality);
int minmax(int min, int v, int max);
uint8_t* yuyv2rgb(uint8_t* yuyv, uint32_t width, uint32_t height);
void camera_pic(int num);
char *itoa_my(int value,char *string,int radix);




#endif // CAMERA_HPP
