#include "LSM9DS1.h"

LSM9DS1::LSM9DS1(I2C Bus)
{
	this->Bus = Bus;
	
}

uint8_t LSM9DS1::Set_CTRL_REG1_G(LSM9DS1::GyroODR gyroODR, LSM9DS1::GyroFullscale fullscale)
{
	byte config = ((byte)gyroODR << 5) + ((byte)fullscale << 3);
	return Bus.write(uint8_t(LSM9DS1_AG_ADDR), uint8_t(LSM9DS1_CTRL_REG1_G), uint8_t(config));
}

uint8_t LSM9DS1::Set_CTRL_REG2_G(LSM9DS1::GyroInterruptGeneration intgen, LSM9DS1::GyroOutDataConfiguration outdata)
{
	byte config = ((byte)intgen << 2) + ((byte)outdata);
	return Bus.write(uint8_t(LSM9DS1_AG_ADDR), uint8_t(LSM9DS1_CTRL_REG2_G), uint8_t(config));
}

uint8_t LSM9DS1::Set_CTRL_REG3_G(LSM9DS1::GyroLowPowerMode powermode)
{
	byte config = ((byte)powermode << 7);
	return Bus.write(uint8_t(LSM9DS1_AG_ADDR), uint8_t(LSM9DS1_CTRL_REG3_G), uint8_t(config));
}

uint8_t LSM9DS1::Set_ORIENT_CFG_G(LSM9DS1::GyroAxisSign xsign, LSM9DS1::GyroAxisSign ysign, LSM9DS1::GyroAxisSign zsign, LSM9DS1::GyroAxisOrder order)
{
	byte config = ((byte)xsign << 5) + ((byte)ysign << 4) + ((byte)zsign << 3) + ((byte)order);
	return Bus.write(uint8_t(LSM9DS1_AG_ADDR), uint8_t(LSM9DS1_ORIENT_CFG_G), uint8_t(config));
}

uint8_t LSM9DS1::Set_REG5_XL(AccelDecimation decimation)
{
	byte config = ((byte)decimation << 6) + ((byte)0b111000);
	return Bus.write(uint8_t(LSM9DS1_AG_ADDR), uint8_t(LSM9DS1_REG5_XL), uint8_t(config));
}

uint8_t LSM9DS1::Set_REG6_XL(AccelODR odr, AccelFullscale fullscale, AccelAntiAliasingBandwidth bandwidth)
{
	byte config = ((byte)odr << 5) + ((byte)fullscale << 3) + ((byte)bandwidth);
	return Bus.write(uint8_t(LSM9DS1_AG_ADDR), uint8_t(LSM9DS1_REG6_XL), uint8_t(config));
}

uint8_t LSM9DS1::Set_REG7_XL(AccelHighResolutionMode mode, AccelFilterCutoff cutoff, AccelDataSelection selection)
{
	byte config = ((byte)mode << 7) + ((byte)cutoff << 5) + ((byte)selection << 2);
	return Bus.write(uint8_t(LSM9DS1_AG_ADDR), uint8_t(LSM9DS1_REG7_XL), uint8_t(config));
}

uint8_t LSM9DS1::Set_REG1_M(MagnetTempCompensation compensation, MagnetAxisOperativeMode XYAxisMode, MagnetODR odr, MagnetFastODRMode fastodr)
{
	byte config = ((byte)compensation << 7) + ((byte)XYAxisMode << 5) + ((byte)odr << 2) + ((byte)fastodr << 1);
	return Bus.write(uint8_t(LSM9DS1_M_ADDR), uint8_t(LSM9DS1_CTRL_REG1_M), uint8_t(config));
}

uint8_t LSM9DS1::Set_REG2_M(MagnetFullscale fullscale)
{
	byte config = (byte)fullscale << 5;
	return Bus.write(uint8_t(LSM9DS1_M_ADDR), uint8_t(LSM9DS1_CTRL_REG2_M), uint8_t(config));
}

uint8_t LSM9DS1::Set_REG3_M(MagnetLowPowerMode powermode, MagnetOperatingMode operatingmode)
{
	byte config = ((byte)powermode << 5) + (byte)operatingmode;
	return Bus.write(uint8_t(LSM9DS1_M_ADDR), uint8_t(LSM9DS1_CTRL_REG3_M), uint8_t(config));
}

uint8_t LSM9DS1::Set_REG4_M(MagnetAxisOperativeMode ZAxisMode)
{
	byte config = (byte)ZAxisMode << 2;
	return Bus.write(uint8_t(LSM9DS1_M_ADDR), uint8_t(LSM9DS1_CTRL_REG4_M), uint8_t(config));
}

uint8_t LSM9DS1::Set_REG5_M(MagnetDataUpdate update)
{
	byte config = (byte)update << 6;
	return Bus.write(uint8_t(LSM9DS1_M_ADDR), uint8_t(LSM9DS1_CTRL_REG5_M), uint8_t(config));
}

void LSM9DS1::UpdateMeasurements()
{
	auto Receive = [this]()
	{
		return int16_t(Bus.receive() | Bus.receive() << 8);
	};

	Bus.read(uint8_t(LSM9DS1_AG_ADDR), uint8_t(LSM9DS1_XL_A), uint8_t(6));

	Accelerometer.X = Receive();
	Accelerometer.Y = Receive();
	Accelerometer.Z = Receive();

	Bus.read(uint8_t(LSM9DS1_AG_ADDR), uint8_t(LSM9DS1_XL_G), uint8_t(6));
	
	Gyroscope.X = Receive();
	Gyroscope.Y = Receive();
	Gyroscope.Z = Receive();
	
	Bus.read(uint8_t(LSM9DS1_M_ADDR), uint8_t(LSM9DS1_XL_M), uint8_t(6));

	Magnetometer.X = Receive();
	Magnetometer.Y = Receive();
	Magnetometer.Z = Receive();
}


void LSM9DS1::UpdateBuffer()
{
	Bus.read(uint8_t(LSM9DS1_AG_ADDR), uint8_t(LSM9DS1_XL_A), uint8_t(6), &Buf[0]);
	Bus.read(uint8_t(LSM9DS1_AG_ADDR), uint8_t(LSM9DS1_XL_G), uint8_t(6), &Buf[6]);
	Bus.read(uint8_t(LSM9DS1_M_ADDR), uint8_t(LSM9DS1_XL_M), uint8_t(6), &Buf[12]);
}