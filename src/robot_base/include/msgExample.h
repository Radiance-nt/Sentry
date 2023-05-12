#include <string>


#pragma pack(1)
typedef struct {
    uint8_t head;
    uint16_t remainTime;
    uint8_t thisHPRatio;
    uint8_t thisBaseHPRatio;
    uint8_t anotherBaseHPRatio;
    uint8_t sentryHPDiffer;
    uint8_t allAlive;
    uint16_t bulletCost;
    uint16_t sentryHPReal;
    uint8_t tail;
}sentryStrategy_t;
#pragma pack()


#pragma pack(1)
typedef struct {
    char aim[14];
    bool allow_rotate;
    bool allow_hitBack;
}aimData;
#pragma pack()

