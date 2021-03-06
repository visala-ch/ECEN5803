#include "mbed.h"
#include "MMA8451Q.h"
#include "TSISensor.h"

#if   defined (TARGET_KL25Z) || defined (TARGET_KL46Z)
  PinName const SDA = PTE25;
  PinName const SCL = PTE24;
#elif defined (TARGET_KL05Z)
  PinName const SDA = PTB4;
  PinName const SCL = PTB3;
#elif defined (TARGET_K20D50M)
  PinName const SDA = PTB1;
  PinName const SCL = PTB0;
#else
  #error TARGET NOT DEFINED
#endif

#define MMA8451_I2C_ADDRESS (0x1d<<1)

int main(void)
{
    MMA8451Q acc(SDA, SCL, MMA8451_I2C_ADDRESS);
    PwmOut rled(LED1);
    PwmOut gled(LED2);
    PwmOut bled(LED3);
    TSISensor tsi;

    printf("MMA8451 ID: %d\n", acc.getWhoAmI());

    while (true) {
        float x, y, z;
        x = abs(acc.getAccX());
        y = abs(acc.getAccY());
        z = abs(acc.getAccZ());
        rled = 1.0f - x - tsi.readPercentage();;
        gled = 1.0f - y - tsi.readPercentage();;
        bled = 1.0f - z - tsi.readPercentage();;
        wait(0.1f);
        printf("X: %1.2f, Y: %1.2f, Z: %1.2f\n", x, y, z);
    }
}
