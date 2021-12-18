#ifndef OTA_H
#define OTA_H

#ifdef __cplusplus
    extern "C"{
#endif

#include <stdint.h>


/*
    General Frame Data
     ------------------------------------------------------------------------------
    |   SOF    |  PACKET TYPE |  DATA LENGTH |    DATA     |   CRC32    |   EOF    |
    | (1 byte) |   (1 byte)   |   (2 bytes)  |  (n bytes)  |  (4 bytes) | (1 byte) |
     -------------------------------------------------------------------------------



    OTA START COMMAND
     ------------------------------------------------------------------------------
    |    SOF   |  CMD         |       1      |  OTA START  |    CRC32   |   EOF    |
    | (1 byte) |   (1 byte)   |   (2 bytes)  |  (1 byte)   |  (4 bytes) | (1 byte) |
     ------------------------------------------------------------------------------
    OTA END COMMAND
     ------------------------------------------------------------------------------
    |   SOF    |     CMD      |      1       |   OTA END   |   CRC32    |    EOF   |
    | (1 byte) |   (1 byte)   |   (2 bytes)  |  (1 byte)   |  (4 bytes) | (1 byte) |
     ------------------------------------------------------------------------------
    OTA HEADER
     ------------------------------------------------------------------------------
    |   SOF    |    HEADER    |      16      | HEADER DATA |   CRC32    |    EOF   |
    | (1 byte) |   (1 byte)   |  (2 bytes)   |  (16 bytes) |  (4 bytes) | (1 byte) |
     ------------------------------------------------------------------------------
    OTA DATA CHUNK
     ------------------------------------------------------------------------------
    |   SOF    |     DATA     | DATA SIZE(n) |    DATA     |   CRC32    |    EOF   |
    | (1 byte) |   (1 byte)   |   (2 bytes)  |  (n bytes)  |  (4 bytes) | (1 byte) |
     ------------------------------------------------------------------------------
    OTA RESPONSE
     ------------------------------------------------------------------------------
    |   SOF    |      RESP    |     1        |  ACK/NACK   |   CRC32    |    EOF   |
    | (1 byte) |   (1 byte)   |   (2 bytes)  |  (1 byte1)  |  (4 bytes) | (1 byte) |
     ------------------------------------------------------------------------------
*/


#define START_OF_FRAME              0xAA    /* Start of Frame */
#define END_OF_FRAME                0xBB    /* End of Frame */
#define MAX_SIZE_PACK               1024U   /* Maximum size number of bytes data a Frame */
#define DATA_LEN_CMD                1U      /* Number of bytes data a Command Frame */
#define DATA_LEN_HDR                16U     /* Number of bytes data a Header Frame */
#define DATA_LEN_RESP               1U      /* Number of bytes data a Data Frame */

#define RESPONSE_COMMAND_PACK_LEN   10U     /* Number of bytes Response/Command Frame */
#define HEADER_PACK_LEN             25U     /* Number of bytes Header Frame */

#define SOF_FIELD_LEN               1U      /* Number of bytes SOF field */      
#define PACKET_TYPE_FIELD_LEN       1U      /* Number of bytes PACKET TYPE field */
#define DATA_LENGTH_FIELD_LEN       2U      /* Number of bytes DATA LENGTH field */
#define CRC32_FIELD_LEN             4U      /* Number of bytes CRC32 field */
#define EOF_FIELD_LEN               1U      /* Number of bytes EOF field */


typedef enum
{
    OTA_IDLE_STATE          = 0U,
    OTA_START_STATE         = 1U,
    OTA_HEADER_STATE        = 2U,
    OTA_RECEIVE_DATA_STATE  = 3U,
    OTA_END_STATE           = 4U
} OTA_States;

typedef enum
{
    OTA_START           = 0U,
    OTA_RESPONSE_ACK    = 1U,
    OTA_RESPONSE_NACK   = 2U,
    OTA_END             = 3U
} DATA_TYPE_Typedef;

typedef enum
{
    OTA_OK      = 0U,
    OTA_BUSY    = 1U,
    OTA_TIMEOUT = 2U,
    OTA_ERROR   = 3U
} OTA_Status;

typedef enum
{
    OTA_COMMAND     = 0U,
    OTA_HEADER      = 1U,
    OTA_DATA        = 2U,
    OTA_RESPONSE    = 3U
} PACKET_TYPE_Typedef;


typedef struct
{
    uint8_t sof;
    uint8_t packtype;
    uint16_t datalen;
    uint8_t *data;
    uint32_t crc;
    uint8_t eof;
} FRAME_Typedef;

void help(void);
OTA_Status ota_start(int fd, FRAME_Typedef *start);
OTA_Status ota_header(int fd, FRAME_Typedef *header);
OTA_Status ota_data(int fd, FRAME_Typedef *data);
OTA_Status ota_response(int fd);
OTA_Status ota_end(int fd, FRAME_Typedef *end);
OTA_Status filt_data(const char *buffer, OTA_States * nextstate);


#ifdef __cplusplus
    }
#endif

#endif /* OTA_H */
