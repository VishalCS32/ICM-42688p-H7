#include "icm42688p.h"
#include <stdio.h>   // For printf

static void CS_Low(void) {
    HAL_GPIO_WritePin(ICM42688P_CS_PORT, ICM42688P_CS_PIN, GPIO_PIN_RESET);
}

static void CS_High(void) {
    HAL_GPIO_WritePin(ICM42688P_CS_PORT, ICM42688P_CS_PIN, GPIO_PIN_SET);
}

static uint8_t SPI_ReadWrite(ICM42688P_HandleTypeDef *dev, uint8_t data) {
    uint8_t rx;
    HAL_SPI_TransmitReceive(dev->hspi, &data, &rx, 1, HAL_MAX_DELAY);
    return rx;
}

static void ICM42688P_WriteReg(ICM42688P_HandleTypeDef *dev, uint8_t reg, uint8_t data) {
    CS_Low();
    SPI_ReadWrite(dev, reg | ICM42688P_SPI_WRITE);
    SPI_ReadWrite(dev, data);
    CS_High();
}

static uint8_t ICM42688P_ReadReg(ICM42688P_HandleTypeDef *dev, uint8_t reg) {
    CS_Low();
    SPI_ReadWrite(dev, reg | ICM42688P_SPI_READ);
    uint8_t val = SPI_ReadWrite(dev, 0xFF);
    CS_High();
    return val;
}

static void ICM42688P_ReadMulti(ICM42688P_HandleTypeDef *dev, uint8_t reg, uint8_t *buf, uint16_t len) {
    CS_Low();
    SPI_ReadWrite(dev, reg | ICM42688P_SPI_READ);
    for (uint16_t i = 0; i < len; i++) {
        buf[i] = SPI_ReadWrite(dev, 0xFF);
    }
    CS_High();
}

void ICM42688P_Init(ICM42688P_HandleTypeDef *dev, SPI_HandleTypeDef *hspi) {
    dev->hspi = hspi;
    HAL_Delay(50);

    uint8_t whoami = ICM42688P_ReadReg(dev, ICM42688P_WHO_AM_I);
    printf("ICM42688P WHO_AM_I: 0x%02X\r\n", whoami);

    if (whoami != ICM42688P_DEVICE_ID) {
        printf("❌ ICM42688P not detected! Check wiring.\r\n");
        while (1);
    }

    // Enable accelerometer + gyroscope
    ICM42688P_WriteReg(dev, ICM42688P_PWR_MGMT0, 0x0F);
    HAL_Delay(10);

    // Configure gyro: ±2000 dps, ODR = 1 kHz
    ICM42688P_WriteReg(dev, ICM42688P_GYRO_CONFIG0, 0x06);

    // Configure accel: ±16g, ODR = 1 kHz
    ICM42688P_WriteReg(dev, ICM42688P_ACCEL_CONFIG0, 0x06);

    printf("✅ ICM42688P Initialized Successfully.\r\n");
}

uint8_t ICM42688P_ReadID(ICM42688P_HandleTypeDef *dev) {
    return ICM42688P_ReadReg(dev, ICM42688P_WHO_AM_I);
}

void ICM42688P_ReadAccelGyro(ICM42688P_HandleTypeDef *dev, int16_t *accel, int16_t *gyro) {
    uint8_t buf[12];
    ICM42688P_ReadMulti(dev, ICM42688P_ACCEL_DATA_X1, buf, 12);

    // Read accelerometer data
    accel[0] = (int16_t)(buf[0] << 8 | buf[1]);
    accel[1] = (int16_t)(buf[2] << 8 | buf[3]);
    accel[2] = (int16_t)(buf[4] << 8 | buf[5]);

    // Read gyroscope data
    gyro[0] = (int16_t)(buf[6] << 8 | buf[7]);
    gyro[1] = (int16_t)(buf[8] << 8 | buf[9]);
    gyro[2] = (int16_t)(buf[10] << 8 | buf[11]);

    // Convert raw accelerometer values to g
    float accel_g[3];
    accel_g[0] = (float)accel[0] / 2048.0f;
    accel_g[1] = (float)accel[1] / 2048.0f;
    accel_g[2] = (float)accel[2] / 2048.0f;

    // Convert raw gyro values to degrees per second
    float gyro_dps[3];
    gyro_dps[0] = (float)gyro[0] / 16.384f;
    gyro_dps[1] = (float)gyro[1] / 16.384f;
    gyro_dps[2] = (float)gyro[2] / 16.384f;

    // Print readings
    printf("Accel (g): X=%.2f Y=%.2f Z=%.2f |\r\n"
           "Gyro (DPS): X=%.2f Y=%.2f Z=%.2f\r\n"
           "\r\n",
           accel_g[0], accel_g[1], accel_g[2],
           gyro_dps[0], gyro_dps[1], gyro_dps[2]);
}
