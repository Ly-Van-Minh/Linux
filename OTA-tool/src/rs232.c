#include <stdio.h>
#include <stdarg.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#include "log.h"
#include "rs232.h"
#include <string.h>
#include <sys/file.h>

int open_serial_port(const char* dev)
{
    LOGI("Opening serial port...\r\n");
    LOGI("dev = %s\r\n", dev);
    int fd = open(dev, O_RDWR | O_NOCTTY | O_NDELAY);
    LOGI("fd value = %i\r\n", fd);
    if(fd < 0)
    {
        LOGE("Error %i from open: %s\r\n", errno, strerror(errno));
    }
    return fd;
}

void configure_attributes(int fd, speed_t baud)
{
    struct termios tty;
    if(tcgetattr(fd, &tty))
    {
        LOGE("Error %i from tcgetattr: %s\r\n", errno, strerror(errno));
    }
    /* Configure parity and stop field */
    tty.c_cflag &= ~PARENB;        /* Clear parity bit, disable parity */
    tty.c_cflag &= ~CSTOP;         /* Clear stop field, only one stop bit used */
    /* Configure number of bits per byte */
    tty.c_cflag &= ~CSIZE;         /* Clear all the size bits */
    tty.c_cflag |= CS8;            /* Set 8 bits per byte */
    /* Configure flow control CRTSCTS */
    tty.c_cflag &= ~CRTSCTS;       /* Disable RTS/CTS hardware flow control */
    tty.c_cflag |= CREAD | CLOCAL; /* Turn on READ and ignore ctrl lines */
    /* Configure canonical mode */
    tty.c_lflag &= ~ICANON;        /* Set canonical mode */
    /* Configure echo */
    tty.c_lflag &= ~ECHO;          /* Disable echo */
    tty.c_lflag &= ~ECHOE;         /* Disable erasure */
    tty.c_lflag &= ~ECHONL;        /* Disable new line echo */
    /* Configure signal chars */
    tty.c_lflag &= ~ISIG;          /* Disable interpretation of INTR, QUIT and SUSP */ 
    /* Configure flow control */
    tty.c_iflag &= ~(IXON | IXOFF | IXANY);    /* Turn off SW flow control */
    /* Configure special handling of bytes on receive */
    tty.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | IGNCR | ICRNL); /* Disable any special handling of received bytes */
    /* Configure output mode */
    tty.c_oflag &= ~(OPOST);   /* Prevent special interpertation of output bytes */
    tty.c_oflag &= ~(ONLCR);   /* Prevent conversion of newline to carriage return/line feed */
    /* Configure VTIME and VMIN */
    tty.c_cc[VTIME] = 10;  /* wait for up to 1s, returning as soon as any data is received */
    tty.c_cc[VMIN] = 0;
    /* Configure baud rate */
    cfsetispeed(&tty, baud);
    cfsetospeed(&tty, baud);
    if(tcsetattr(fd, TCSANOW, &tty))
    {
        LOGE("Error %i from tcsetattr: %s\r\n", errno, strerror(errno));
    }
}

off_t read_file(const char * file, const char *buffer)
{
    int fd;
    off_t size;
    fd = open(file, O_RDONLY);
    size = lseek(fd, 0, SEEK_END);
    lseek(fd, 0, SEEK_SET);
    LOGI("size of file = %ld\r\n", size);
    if(size == -1)
    {
        LOGE("Error %i from lseek: %s\r\n", errno, strerror(errno));
    }
    if(read(fd, (void*)buffer, size) == -1)
    {
        LOGE("Error %i from read: %s\r\n", errno, strerror(errno));
    }
    if(close(fd) == -1)
    {
        LOGE("Error %i from close:%s\r\n", errno, strerror(errno));
    }
    return size;
}

int serial_read(int fd, const char *buffer, size_t len)
{
    if(read(fd, (void*)buffer, len) == -1)
    {
        LOGE("Error %i from read: %s\r\n", errno, strerror(errno));
    }
    return 0;
}

int serial_write(int fd, const char *buffer, size_t len)
{
    if(write(fd, buffer, len) == -1)
    {
        LOGE("Error %i from write: %s\r\n", errno, strerror(errno));
    }
    LOGI("String of buffer: %s", buffer);
    return 0;
}

void serial_close(int fd)
{
    if(close(fd) == -1)
    {
        LOGE("Error %i from close: %s", errno, strerror(errno));
    }
}

void lock_serial(int fd)
{
    if(flock(fd, LOCK_EX | LOCK_NB) == -1)
    {
        LOGE("Serial port with file descriptor %d is already locked by another process\r\n", fd);
    }
}