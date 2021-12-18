#ifndef RS232_H
#define RS232_H

#include <stdio.h>
#include <termios.h>

#ifdef __cplusplus
    extern "C"{
#endif


void open_device(const char* dev, const char* img);
// int set_interface_attributes(struct Atributes_Typedef *attributes);
void set_blocking(int fd, int should_block);
int open_serial_port(const char* dev);
void configure_attributes(int fd, speed_t baud);
off_t read_file(const char * file, const char *buffer);
int serial_read(int fd, const char *buffer, size_t len);
int serial_write(int fd, const char *buffer, size_t len);
void serial_close(int fd);
void lock_serial(int fd);


#ifdef __cplusplus
    }
#endif

#endif /* RS232_H */