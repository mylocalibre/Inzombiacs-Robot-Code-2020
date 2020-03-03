// Pixy2 Functions

#include "Pixy2.h"

void Pixy2::init()
{
    int waiting = 0;
    // Initialize the Pixy2
    std::cout << "Initilizing Pixy2..." << std::endl;
    // Write to the Pixy2 to begin
    i2c.WriteBulk(versionRequest,4);
    while(waiting < 100)
    {
        waiting += 1;
    }
    // Read from the Pixy2
    i2c.ReadOnly(32,recvBuf);
    std::cout << "Pixy2 ready!" << std::endl;
}

uint8_t* Pixy2::write_and_read(uint8_t* dataOut, uint8_t dataOut_len, uint8_t dataIn_len)
{
    // Every write to the Pixy2 results in returned
    // data, whether it is desired or not
    // Every byte of data is required, defined at 
    // https://docs.pixycam.com/wiki/doku.php?id=wiki:v2:porting_guide
    i2c.WriteBulk(dataOut,dataOut_len);
    // May need to wait between the two, no idea though
    uint8_t *dataIn = new uint8_t[dataIn_len];
    i2c.ReadOnly(dataIn_len,dataIn);
    //std::this_thread::sleep_for(std::chrono::milliseconds(10));
    return dataIn;
}

uint8_t* Pixy2::getVersion()
{
    // Returns the version information for the Pixy2
    //uint8_t *dataIn;
    uint8_t dataOut[4] = {174,193,14,0};
    //std::memcpy(dataIn, Pixy2::write_and_read(dataOut,4,13),13);
    //return dataIn;
    return Pixy2::write_and_read(dataOut,4,13);
}

uint8_t* Pixy2::getResolution()
{
    //Returns the resolution information for the Pixy2
    //uint8_t *dataIn;
    uint8_t dataOut[5] = {174,193,12,1,0};
    //std::memcpy(dataIn, Pixy2::write_and_read(dataOut,5,10),10);
    //return dataIn;
    return Pixy2::write_and_read(dataOut,5,10);
}

uint8_t* Pixy2::setCameraBrightness(uint8_t brightness)
{
    // Sets the brightness to a value between 0 and 255
    // Returns 32-nit result value
    //uint8_t *dataIn;
    uint8_t dataOut[5] = {174,193,16,1,brightness};
    //std::memcpy(dataIn, Pixy2::write_and_read(dataOut,5,10),10);
    //return dataIn;
    return Pixy2::write_and_read(dataOut,5,10);
}

uint8_t* Pixy2::setServos(uint8_t s0_lo, uint8_t s0_hi, uint8_t s1_lo, uint8_t s1_hi)
{
    // Sets the servos to values for pan and tilt, respectively,
    // between 0 and 511
    uint8_t dataOut[8] = {174,193,18,4,s0_lo, s0_hi, s1_lo, s1_hi};
    return Pixy2::write_and_read(dataOut,8,10);
}

uint8_t* Pixy2::setLED(uint8_t r, uint8_t g, uint8_t b)
{
    // Sets the LED to a value between 0 and 255 for r, g, & b
    uint8_t dataOut[7] = {174,193,20,3,r, g, b};
    return Pixy2::write_and_read(dataOut,7,10);
}

uint8_t* Pixy2::setLamp(uint8_t upper, uint8_t lower)
{
    // Upper turns on/off the two white LEDs along the top edge of Pixy2
    // Upper turns on/off the RGB LED along the bottom edge of Pixy2
    uint8_t dataOut[6] = {174,193,22,2,upper, lower};
    return Pixy2::write_and_read(dataOut,6,10);
}

uint8_t* Pixy2::getFPS()
{
    //Returns the FPS as the highest 4 bytes
    uint8_t dataOut[4] = {174,193,24,0};
    return Pixy2::write_and_read(dataOut,4,10);
}

// This function is specific to Color Connected Components
uint8_t* Pixy2::getBlocks(uint8_t sigmap, uint8_t maxBlocks)
{
    // Each bit of sigmap represents a signiture
    // Eg. bit 0 for sig. 1, bit 1 for sig. 2, etc.
    // Can also do bitwise or for multiple signatures
    // maxBlocks is the number of blocks for each signature
    // to return. The data is output as one large array
    uint8_t dataOut[6] = {174,193,32,2,sigmap, maxBlocks};
    uint8_t* dataTemp;
    dataTemp = new uint8_t[20];
    uint8_t* dataIn;
    dataIn = new uint8_t [20*maxBlocks];
    dataTemp = Pixy2::write_and_read(dataOut,6,20);
    for(uint8_t j = 0; j < 20; j++)
    {
        dataIn[j] = dataTemp[j];
    }

    for(uint8_t i = 1; i < maxBlocks; i++)
    {
        for(uint8_t j = 0; j < 20; j++)
        {
            dataIn[j + i*20] = dataTemp[j];
        }
        delete [] dataTemp;
        i2c.ReadOnly(20,dataTemp);
    }
    return dataIn;
}

void Pixy2::getBallInfo(uint8_t sigmap)
{
    // Returns the relative size of the ball with respect
    // to the size of the screen
    // Only doing it for one block
    uint8_t *pixy2In;
    pixy2In = new uint8_t[20];
    pixy2In = Pixy2::getBlocks(sigmap,1);
    uint16_t b_height = 0;
    uint16_t b_width = 0;
    uint16_t b_xpos = 0;
    uint16_t b_ypos = 0;
    if (((pixy2In[7] << 8)|(pixy2In[6])) == sigmap)
    {
        b_height = ((pixy2In[15] << 8)|(pixy2In[14]));
        b_width = ((pixy2In[13] << 8)|(pixy2In[12]));
        b_xpos = ((pixy2In[9] << 8)|(pixy2In[8]));
        b_ypos = ((pixy2In[11] << 8)|(pixy2In[10]));
         
        std::cout << "Height: " << std::to_string(b_height) << std::endl;
        std::cout << "Width: " << std::to_string(b_width) << std::endl;
        std::cout << "X Position: " << std::to_string(b_xpos) << std::endl;
        std::cout << "Y Position: " << std::to_string(b_ypos) << std::endl << std::endl;
    }
    else
    {
        std::cout << "Cannot see the ball!" << std::endl << std::endl;
    }
}

uint16_t* Pixy2::getBallData(uint8_t sigmap)
{
    // Returns pixy2Data, an array that contains information about
    // the proximity to the ball & the x position
    uint8_t *pixy2In;
    pixy2In = new uint8_t[20];
    uint16_t *pixy2Return;
    pixy2Return = new uint16_t[2];  
    pixy2In = Pixy2::getBlocks(sigmap,1);
    //uint16_t b_height = 0;
    //uint16_t b_width = 0;
    uint16_t b_dist = 0;
    uint16_t b_xpos = 0;
    //uint16_t b_ypos = 0;
    if (((pixy2In[7] << 8)|(pixy2In[6])) == sigmap)
    {
        //b_height = ((pixy2In[15] << 8)|(pixy2In[14]));
        //b_width = ((pixy2In[13] << 8)|(pixy2In[12]));
        b_xpos = ((pixy2In[9] << 8)|(pixy2In[8]));
        //b_ypos = ((pixy2In[11] << 8)|(pixy2In[10]));
        b_dist = 0; // FILL THIS LINE IN
    }
    pixy2Return[0] = b_dist;
    pixy2Return[1] = b_xpos;
    return pixy2Return;
}