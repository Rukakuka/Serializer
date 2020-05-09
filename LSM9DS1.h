#include "I2C.h"

class LSM9DS1
{
public:
    enum class GyroODR
    {
        PowerDown = 0b000,
        ODR_14_9 = 0b001,
        ODR_59_5 = 0b010,
        ODR_119 = 0b011,
        ODR_238 = 0b100,
        ODR_476 = 0b101,
        ODR_952 = 0b110,
    };
    enum class GyroFullscale
    {
        FS_245 = 0b00,
        FS_500 = 0b01,
        FS_2000 = 0b11,
    };
    enum class GyroInterruptGeneration
    {
        GenAfterLPF1 = 0b00,
        GenAfterHPF = 0b01,
        GenAfterLPF2 = 0b10,
        None = 0b11
    };
    enum class GyroOutDataConfiguration
    {
        OutAfterLPF1 = 0b00,
        OutAfterHPF = 0b01,
        OutAfterLPF2 = 0b10,
        None = 0b11,
    };
    enum class GyroLowPowerMode
    {
        Enabled = 0b1,
        Disabled = 0b0,
    };
    enum class GyroAxisSign
    {
        Positive = 0b1,
        Negative = 0b0,
    };
    enum class GyroAxisOrder
    {
        XYZ = 0b000,
        XZY = 0b001,
        YXZ = 0b010,
        YZX = 0b011,
        ZXY = 0b100,
        ZYX = 0b101,
    };
    enum class AccelDecimation
    {
        NoDecimation = 0b00,
        EveryTwoSamples = 0b01,
        EveryFourSamples = 0b10,
        EveryEightSamples = 0b11,
    };
    enum class AccelODR
    {
        PowerDown = 0b000,
        ODR_10 = 0b001,
        ODR_50 = 0b010,
        ODR_119 = 0b011,
        ODR_238 = 0b100,
        ODR_476 = 0b101,
        ODR_952 = 0b110,
    };
    enum class AccelFullscale
    {
        G_2 = 0b00,
        G_4 = 0b01,
        G_8 = 0b10,
        G_16 = 0b11,
    };
    enum class AccelAntiAliasingBandwidth
    {
        BW_408 = 0b00,
        BW_211 = 0b01,
        BW_105 = 0b10,
        BW_50 = 0b11,
    };
    enum class AccelHighResolutionMode
    {
        Disabled = 0b0,
        Enabled = 0b1,
    };
    enum class AccelFilterCutoff
    {
        ODR_TO_50 = 0b00,
        ODR_TO_100 = 0b01,
        ODR_TO_9 = 0b10,
        ODR_TO_400 = 0b11,
    };
    enum class AccelDataSelection
    {
        Bypassed = 0b0,
        Filtered = 0b1,
    };
    enum class MagnetTempCompensation
    {
        Enabled = 0b1,
        Disabled = 0b0,
    };
    enum class MagnetAxisOperativeMode
    {
        LowPower = 0b00,
        MediumPerfomance = 0b01,
        HighPerfomance = 0b10,
        UltraHighPerfomance = 0b11,
    };
    enum class MagnetODR
    {
        ODR_0_625 = 0b000,
        ODR_1_25 = 0b001,
        ODR_2_5 = 0b010,
        ODR_5 = 0b011,
        ODR_10 = 0b100,
        ODR_20 = 0b101,
        ODR_40 = 0b110,
        ODR_80 = 0b111,
    };
    enum class MagnetFastODRMode
    {
        Enabled = 0b1,
        Disabled = 0b0,
    };
    enum class MagnetFullscale
    {
        GAUSS_4 = 0b00,
        GAUSS_8 = 0b01,
        GAUSS_12 = 0b10,
        GAUSS_16 = 0b11,
    };
    enum class MagnetLowPowerMode
    {
        Enabled = 0b1,
        Disabled = 0b0,
    };
    enum class MagnetOperatingMode
    {
        ContinuousConversion = 0b00,
        SingleConversion = 0b01,
        PowerDown = 0b10,
    };
    enum class MagnetDataUpdate
    {
        Continuous = 0b0,
        NotUpdatedUntilRead = 0b1,
    };
    enum class Error
    {
        None = 0,
        AccelerometerFail = 1,
        GyroscopeFail = 2,
        MagnetometerFail = 3,
        GeneralFail = 4,
    };
    /* GYRO CONFIG */
    uint8_t Set_CTRL_REG1_G(GyroODR gyroODR, GyroFullscale fullscale);
    uint8_t Set_CTRL_REG2_G(GyroInterruptGeneration intgen, GyroOutDataConfiguration outdata);
    uint8_t Set_CTRL_REG3_G(GyroLowPowerMode powermode);
    uint8_t Set_ORIENT_CFG_G(GyroAxisSign xsign, GyroAxisSign ysign, GyroAxisSign zsign, GyroAxisOrder order);
    /* ACCEL CONFIG */
    uint8_t Set_REG5_XL(AccelDecimation decimation);
    uint8_t Set_REG6_XL(AccelODR odr, AccelFullscale fullscale, AccelAntiAliasingBandwidth bandwidth);
    uint8_t Set_REG7_XL(AccelHighResolutionMode mode, AccelFilterCutoff cutoff, AccelDataSelection selection);
    /* MAGNET CONFIG */
    uint8_t Set_REG1_M(MagnetTempCompensation compensation, MagnetAxisOperativeMode XYAxisMode, MagnetODR odr, MagnetFastODRMode fastodr);
    uint8_t Set_REG2_M(MagnetFullscale fullscale);
    uint8_t Set_REG3_M(MagnetLowPowerMode powermode,MagnetOperatingMode operatingmode);
    uint8_t Set_REG4_M(MagnetAxisOperativeMode ZAxisMode);
    uint8_t Set_REG5_M(MagnetDataUpdate update);
    /* COMMON */
    LSM9DS1(I2C Bus);
    Error Update();

    struct ThreeAxisData {
        int16_t X;
        int16_t Y;
        int16_t Z;
    };

    ThreeAxisData Accelerometer;
    ThreeAxisData Gyroscope;
    ThreeAxisData Magnetometer;

private:
    I2C Bus;
    Error Err;
    /*****       DEVICE       *****/
    const byte LSM9DS1_AG_ADDR = 0x6B;
    const byte LSM9DS1_M_ADDR = 0x1E;
    /*****   GYRO REGISTERS   *****/
    const byte LSM9DS1_CTRL_REG1_G = 0x10;
    const byte LSM9DS1_CTRL_REG2_G = 0x11;
    const byte LSM9DS1_CTRL_REG3_G = 0x12;
    const byte LSM9DS1_ORIENT_CFG_G = 0x13;
    const byte LSM9DS1_XL_G = 0x18;
    const byte LSM9DS1_XH_G = 0x19;
    const byte LSM9DS1_YL_G = 0x1A;
    const byte LSM9DS1_YH_G = 0x1B;
    const byte LSM9DS1_ZL_G = 0x1C;
    const byte LSM9DS1_ZH_G = 0x1D;
    /*****   ACCEL REGISTERS   *****/
    const byte LSM9DS1_REG5_XL = 0x1F;
    const byte LSM9DS1_REG6_XL = 0x20;
    const byte LSM9DS1_REG7_XL = 0x21;
    const byte LSM9DS1_XL_A = 0x28;
    const byte LSM9DS1_XH_A = 0x29;
    const byte LSM9DS1_YL_A = 0x2A;
    const byte LSM9DS1_YH_A = 0x2B;
    const byte LSM9DS1_ZL_A = 0x2C;
    const byte LSM9DS1_ZH_A = 0x2D;
    /*****   MAGNET REGISTERS   *****/
    const byte LSM9DS1_CTRL_REG1_M = 0x20;
    const byte LSM9DS1_CTRL_REG2_M = 0x21;
    const byte LSM9DS1_CTRL_REG3_M = 0x22;
    const byte LSM9DS1_CTRL_REG4_M = 0x23;
    const byte LSM9DS1_CTRL_REG5_M = 0x24;
    const byte LSM9DS1_XL_M = 0x28;
    const byte LSM9DS1_XH_M = 0x29;
    const byte LSM9DS1_YL_M = 0x2A;
    const byte LSM9DS1_YH_M = 0x2B;
    const byte LSM9DS1_ZL_M = 0x2C;
    const byte LSM9DS1_ZH_M = 0x2D;
};
