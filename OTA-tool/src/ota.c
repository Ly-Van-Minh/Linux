#include "rs232.h"
#include "ota.h"
#include "log.h"
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <sys/stat.h>
#include <string.h>

int main(int argc, char *argv[])
{
    int fd;
    off_t size;
    char buffer[1024];
    LOGI("Starting OTA ...\r\n");
    if(argc != 4)
    {
        help();
    }
    else
    {
        LOGI("Serial name: %s\r\n", argv[1]);
        fd = open_serial_port(argv[1]);
        configure_attributes(fd, atoi(argv[2]));
        lock_serial(fd);
        size = read_file(argv[3], buffer);
        serial_write(fd, buffer, size);
        serial_close(fd);
    }
    // FindDevice();
    return 0;
}

void help(void)
{
    LOGW("Usage: ./build/OTA <device> <baudrate> <file>\r\n");      
    LOGW("      device:     serial port in /dev/tty*\r\n");
    LOGW("      baudrate:   baud rate of serial port\r\n");
    LOGW("      file:       file need to transmit\r\n");
}

OTA_Status ota_start(int fd, FRAME_Typedef *start)
{
    OTA_Status ret = OTA_ERROR;
 
    start->packtype   = OTA_COMMAND;
    start->datalen    = DATA_LEN_CMD;
    // start->data       = OTA_START;

    if(!serial_write(fd, (const char *)start, sizeof(start)))
    {
        LOGE("serial_write error!\r\n");
        ret = OTA_OK;
    }
    return ret;
}

OTA_Status ota_header(int fd, FRAME_Typedef *header)
{

    OTA_Status ret = OTA_ERROR; 
    header->packtype   = OTA_HEADER;
    header->datalen    = DATA_LEN_RESP;

    if(!serial_write(fd, (const char *)header, sizeof(header)))
    {
        LOGE("serial_write error!\r\n");
        ret = OTA_OK;
    }
    return ret;
}

OTA_Status ota_data(int fd, FRAME_Typedef *data)
{
    OTA_Status ret = OTA_ERROR;
 
    data->packtype   = OTA_DATA;

    if(!serial_write(fd, (const char *)data, sizeof(data)))
    {
        LOGE("serial_write error!\r\n");
        ret = OTA_OK;
    }
    return ret;
}

OTA_Status ota_end(int fd, FRAME_Typedef *end)
{
    OTA_Status ret = OTA_ERROR;
 
    end->packtype   = OTA_COMMAND;
    end->datalen    = DATA_LEN_CMD;
    // end->data       = OTA_END;

    if(!serial_write(fd, (const char *)end, sizeof(end)))
    {
        LOGE("serial_write error!\r\n");
        ret = OTA_OK;
    }
    return ret;
}

OTA_Status ota_response(int fd)
{
    OTA_Status ret = OTA_ERROR;
    uint8_t count = 0;
    uint16_t datalen = 0;
    char *buffer = NULL;

    if(!serial_read(fd, buffer, RESPONSE_COMMAND_PACK_LEN))
    {
        LOGE("serial_read error!\r\n");
    }
    else
    {
        while(count != RESPONSE_COMMAND_PACK_LEN)
        {
            if((buffer[count++] == START_OF_FRAME) && (buffer[count++] == OTA_RESPONSE))
            {
                datalen = (*(uint16_t *)(buffer + count));
                count += DATA_LENGTH_FIELD_LEN;
                if(datalen == DATA_LEN_RESP)
                {
                    if(buffer[count++] == OTA_RESPONSE_ACK)
                    {
                        count += CRC32_FIELD_LEN;
                        if(buffer[count] == END_OF_FRAME)
                        {
                            LOGI("Response ACK\r\n");
                            ret = OTA_OK;
                        }
                        else
                        {
                            LOGE("Not encounter EOF\r\n");
                            ret = OTA_ERROR;
                        }
                    }
                    else
                    {
                        LOGE("Response NACK\r\n");
                        ret = OTA_ERROR;
                    }
                }
                else
                {
                    LOGE("Data length of response != 1\r\n");
                    ret = OTA_ERROR;
                }
            }
        }
    }
    return ret;
}

OTA_Status filt_data(const char *buffer, OTA_States *nextstate)
{
    uint16_t count = 0;
    uint16_t datalen = 0;
    OTA_Status ret = OTA_ERROR;
    while(*(buffer + count) != END_OF_FRAME)
    {
        if(buffer[count] == START_OF_FRAME)
        {
            count++;
        }
        else
        {
            break;
        }
        switch (buffer[count++])
        {
        case OTA_COMMAND:
            datalen = (*(uint16_t *)(buffer + count));
            count += DATA_LENGTH_FIELD_LEN;
            if(datalen == DATA_LEN_CMD)
            {
                if(buffer[count] == OTA_START)
                { 
                    count += (CRC32_FIELD_LEN + DATA_LEN_CMD);
                    if(buffer[count] == END_OF_FRAME)
                    {
                        LOGI("Received OTA START Command Packet!\r\n");
                        *nextstate = OTA_HEADER_STATE;
                        ret = OTA_OK;
                    }
                }
                else if (buffer[count] == OTA_END)
                {
                    count += (CRC32_FIELD_LEN + DATA_LEN_CMD);
                    if(buffer[count] == END_OF_FRAME)
                    {
                        LOGI("Received OTA END Command Packet!\r\n");
                        *nextstate = OTA_IDLE_STATE;
                        ret = OTA_OK;
                    }
                }
                else
                {
                    LOGE("Received OTA CMD Error!\r\n");
                    ret = OTA_ERROR;
                }
            }            
            break;
        case OTA_HEADER:
            datalen = (*(uint16_t*)(buffer + count));
            count += DATA_LENGTH_FIELD_LEN;
            if((datalen == DATA_LEN_CMD) && (buffer[count] == OTA_START))
            break;
        case OTA_DATA:
        
            break;
        case OTA_RESPONSE:
        
            break;
        default:
            break;
        }
    }
    return ret;
}