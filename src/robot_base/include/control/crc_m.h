// #ifndef __CRC_M_H__
// #define __CRC_M_H__

// // #include <sys.h>
// #include <stdint.h>
// #include <stddef.h>

// uint8_t verifyCRC8Checksum(uint8_t* pchMessage, uint16_t dwLength);
// uint8_t verifyCRC16Checksum(uint8_t* pchMessage, uint32_t dwLength);

// void appendCRC8Checksum(uint8_t* pchMessage, uint16_t dwLength);
// void appendCRC16Checksum(uint8_t* pchMessage, uint32_t dwLength);

// uint8_t verifyCRC8Checksum(std::vector<uint8_t>  pchMessage, uint16_t dwLength);
// uint8_t verifyCRC16Checksum(std::vector<uint8_t>  pchMessage, uint32_t dwLength);

// void appendCRC8Checksum(std::vector<uint8_t>  pchMessage, uint16_t dwLength);
// void appendCRC16Checksum(std::vector<uint8_t>  pchMessage, uint32_t dwLength);


// #endif

#ifndef __CRC_H__
#define __CRC_H__

#include <serial/serial.h>
#include <vector>

#pragma pack(1)
typedef struct{
  uint8_t markHead; //帧头
  float setSpeedX; //垂直云台
  float setSpeedY; //平行云台
  float setAngle; //逆时针转向
  uint8_t markTail; //帧尾
}ctrlFromPC_t;
#pragma pack()

typedef union{
  uint8_t usbDataBuffer[14];
  ctrlFromPC_t ctrlFromPC;
}dataFromPC_t;


#endif  // __CRC_H__

