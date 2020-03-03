// Header file for Pixy2 usage

#include <frc/I2C.h>
#include <frc/Timer.h>
#include <iostream>
#include <thread>
#include <chrono>

#define PIXY2_ADDR 0x54

uint8_t versionRequest[] = 
  {
    0xae,  // first byte of no_checksum_sync (little endian -> least-significant byte first)
    0xc1,  // second byte of no_checksum_sync
    0x0e,  // this is the version request type
    0x00   // data_length is 0
  };

uint8_t *recvBuf = new uint8_t[32];

class Pixy2
{
    public:
        frc::I2C i2c{frc::I2C::Port::kOnboard,0x54};
        void init();
        uint8_t* write_and_read(uint8_t* dataOut, uint8_t dataOut_len, uint8_t dataIn_len);
        uint8_t* getVersion();
        uint8_t* getResolution();
        uint8_t* setCameraBrightness(uint8_t brightness);
        uint8_t* setServos(uint8_t s0_lo, uint8_t s0_hi, uint8_t s1_lo, uint8_t s1_hi);
        uint8_t* setLED(uint8_t r, uint8_t g, uint8_t b);
        uint8_t* setLamp(uint8_t upper, uint8_t lower);
        uint8_t* getFPS();
        uint8_t* getBlocks(uint8_t sigmap, uint8_t maxBlocks);
        void getBallInfo(uint8_t sigmap);
        uint16_t* getBallData(uint8_t sigmap);
};