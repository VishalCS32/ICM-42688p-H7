#ifndef ICM42688P_H
#define ICM42688P_H

#include "stm32h7xx_hal.h"
#include <stdint.h>

// Register Definitions (only a few required ones for initialization)
#define ICM42688P_WHO_AM_I      0x75
#define ICM42688P_DEVICE_ID     0x47  // Expected WHO_AM_I value
#define ICM42688P_PWR_MGMT0     0x4E
#define ICM42688P_GYRO_CONFIG0  0x4F
#define ICM42688P_ACCEL_CONFIG0 0x50
#define ICM42688P_ACCEL_DATA_X1 0x1F
#define ICM42688P_GYRO_DATA_X1  0x25

// SPI Read/Write
#define ICM42688P_SPI_READ      0x80
#define ICM42688P_SPI_WRITE     0x00

// Chip Select Pin
#define ICM42688P_CS_PORT       GPIOA
#define ICM42688P_CS_PIN        GPIO_PIN_15

// Driver Struct
typedef struct {
    SPI_HandleTypeDef *hspi;
} ICM42688P_HandleTypeDef;

// Function Prototypes
void ICM42688P_Init(ICM42688P_HandleTypeDef *dev, SPI_HandleTypeDef *hspi);
uint8_t ICM42688P_ReadID(ICM42688P_HandleTypeDef *dev);
void ICM42688P_ReadAccelGyro(ICM42688P_HandleTypeDef *dev, int16_t *accel, int16_t *gyro);

#endif
