#include <string.h>
#include "servo.h"

//#define USE_OS_TOOLS

#ifdef USE_OS_TOOLS
#include "FreeRTOS.h"
#include "semphr.h"
#endif

#ifdef USE_OS_TOOLS

static SemaphoreHandle_t sema_i2c_tx;
static SemaphoreHandle_t sema_i2c_rx;

static void semp_i2c_init()
{
	sema_i2c_tx = xSemaphoreCreateBinary();
	sema_i2c_rx = xSemaphoreCreateBinary();
}

static void semp_i2c_tx_wait()
{
	if (sema_i2c_tx != NULL)
	{
		xSemaphoreTake(sema_i2c_tx, portMAX_DELAY);
	}
}
static void semp_i2c_rx_wait()
{
	if (sema_i2c_rx != NULL)
	{
		xSemaphoreTake(sema_i2c_rx, portMAX_DELAY);
	}
}

static void semp_i2c_tx_give()
{
	if (sema_i2c_tx != NULL)
	{
		BaseType_t xHigherPriorityTaskWoken = pdFALSE;
		xSemaphoreGiveFromISR(sema_i2c_tx, &xHigherPriorityTaskWoken);
		portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
	}
}
static void semp_i2c_rx_give()
{
	if (sema_i2c_rx != NULL)
	{
		BaseType_t xHigherPriorityTaskWoken = pdFALSE;
		xSemaphoreGiveFromISR(sema_i2c_rx, &xHigherPriorityTaskWoken);
		portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
	}
}

#endif

extern "C" void servo_i2c_tx_done()
{
#ifdef USE_OS_TOOLS	
	semp_i2c_tx_give();
#endif		
}
extern "C" void servo_i2c_rx_done()
{
#ifdef USE_OS_TOOLS	
	semp_i2c_rx_give();
#endif		
}

void Servo::Init(void)
{
#ifdef USE_OS_TOOLS
	semp_i2c_init();
#endif		
}

void Servo::UpdateServoAngle(Servo::JointStatus_t &_joint, float _angleSetPoint)
{
    if (_angleSetPoint >= _joint.angleMin && _angleSetPoint <= _joint.angleMax)
    {
        unsigned char* b = (unsigned char*) (&_angleSetPoint);

        i2cTxData[0] = 0x01;
        for (int i = 0; i < 4; i++)
            i2cTxData[i + 1] = *(b + i);

        TransmitAndReceiveI2cPacket(_joint.id);

        //_joint.angle = *(float*) (i2cRxData + 1);
		memcpy(&_joint.angle, i2cRxData + 1, sizeof(float));		 
    }
}


void Servo::UpdateServoAngle(Servo::JointStatus_t &_joint)
{
    //unsigned char* b = (unsigned char*) &(_joint.angle);
    i2cTxData[0] = 0x11;

    TransmitAndReceiveI2cPacket(_joint.id);

    //_joint.angle = *(float*) (i2cRxData + 1);
	memcpy(&_joint.angle, i2cRxData + 1, sizeof(float));
}


void Servo::SetJointEnable(Servo::JointStatus_t &_joint, bool _enable)
{
    i2cTxData[0] = 0xff;
    i2cTxData[1] = _enable ? 1 : 0;

    TransmitAndReceiveI2cPacket(_joint.id);

    //_joint.angle = *(float*) (i2cRxData + 1);
	memcpy(&_joint.angle, i2cRxData + 1, sizeof(float));
}

void Servo::TransmitAndReceiveI2cPacket(uint8_t _id)
{
    HAL_StatusTypeDef state = HAL_ERROR;
	
#ifdef USE_OS_TOOLS
	state = HAL_I2C_Master_Transmit_DMA(motorI2c, _id, i2cTxData, 5);
	semp_i2c_tx_wait();
	state = HAL_I2C_Master_Receive_DMA(motorI2c, _id, i2cRxData, 5);
	semp_i2c_rx_wait();
#else
	uint32_t try_cnt = 0;
    do
    {		
        state = HAL_I2C_Master_Transmit(motorI2c, _id, i2cTxData, 5, 5);
		try_cnt ++;
    } while ((state != HAL_OK) && (try_cnt < 5));
	
	try_cnt  = 0;
    do
    {
        state = HAL_I2C_Master_Receive(motorI2c, _id, i2cRxData, 5, 5);
		try_cnt ++;
    } while ((state != HAL_OK) && (try_cnt < 5)); 
	HAL_Delay(5);
#endif		
}


void Servo::SetJointTorqueLimit(Servo::JointStatus_t &_joint, float _percent)
{
    if (_percent >= 0 && _percent <= 1)
    {
        unsigned char* b = (unsigned char*) (&_percent);

        i2cTxData[0] = 0x26;
        for (int i = 0; i < 4; i++)
            i2cTxData[i + 1] = *(b + i);

        TransmitAndReceiveI2cPacket(_joint.id);

        //_joint.angle = *(float*) (i2cRxData + 1);
		memcpy(&_joint.angle, i2cRxData + 1, sizeof(float));

        HAL_Delay(500); // wait servo reset
    }
}


void Servo::SetJointId(Servo::JointStatus_t &_joint, uint8_t _id)
{
    i2cTxData[0] = 0x21;
    i2cTxData[1] = _id;

    TransmitAndReceiveI2cPacket(_joint.id);

    //_joint.angle = *(float*) (i2cRxData + 1);
	memcpy(&_joint.angle, i2cRxData + 1, sizeof(float));

    HAL_Delay(500); // wait servo reset
}


void Servo::SetJointInitAngle(Servo::JointStatus_t &_joint, float _angle)
{
    float sAngle = _joint.inverted ?
                   (_angle - _joint.modelAngelMin) /
                   (_joint.modelAngelMax - _joint.modelAngelMin) *
                   (_joint.angleMin - _joint.angleMax) + _joint.angleMax :
                   (_angle - _joint.modelAngelMin) /
                   (_joint.modelAngelMax - _joint.modelAngelMin) *
                   (_joint.angleMax - _joint.angleMin) + _joint.angleMin;


    if (sAngle >= _joint.angleMin && sAngle <= _joint.angleMax)
    {
        unsigned char* b = (unsigned char*) (&_angle);

        i2cTxData[0] = 0x27;
        for (int i = 0; i < 4; i++)
            i2cTxData[i + 1] = *(b + i);

        TransmitAndReceiveI2cPacket(_joint.id);

        //_joint.angle = *(float*) (i2cRxData + 1);
		memcpy(&_joint.angle, i2cRxData + 1, sizeof(float));

        HAL_Delay(500); // wait servo reset
    }
}


void Servo::SetJointKp(Servo::JointStatus_t &_joint, float _value)
{
    unsigned char* b = (unsigned char*) (&_value);

    i2cTxData[0] = 0x22;
    for (int i = 0; i < 4; i++)
        i2cTxData[i + 1] = *(b + i);

    TransmitAndReceiveI2cPacket(_joint.id);

    //_joint.angle = *(float*) (i2cRxData + 1);
	memcpy(&_joint.angle, i2cRxData + 1, sizeof(float));

    HAL_Delay(500); // wait servo reset
}


void Servo::SetJointKi(Servo::JointStatus_t &_joint, float _value)
{
    unsigned char* b = (unsigned char*) (&_value);

    i2cTxData[0] = 0x23;
    for (int i = 0; i < 4; i++)
        i2cTxData[i + 1] = *(b + i);

    TransmitAndReceiveI2cPacket(_joint.id);

    //_joint.angle = *(float*) (i2cRxData + 1);
	memcpy(&_joint.angle, i2cRxData + 1, sizeof(float));

    HAL_Delay(500); // wait servo reset
}


void Servo::SetJointKv(Servo::JointStatus_t &_joint, float _value)
{
    unsigned char* b = (unsigned char*) (&_value);

    i2cTxData[0] = 0x24;
    for (int i = 0; i < 4; i++)
        i2cTxData[i + 1] = *(b + i);

    TransmitAndReceiveI2cPacket(_joint.id);

    //_joint.angle = *(float*) (i2cRxData + 1);
	memcpy(&_joint.angle, i2cRxData + 1, sizeof(float));

    HAL_Delay(500); // wait servo reset
}


void Servo::SetJointKd(Servo::JointStatus_t &_joint, float _value)
{
    unsigned char* b = (unsigned char*) (&_value);

    i2cTxData[0] = 0x25;
    for (int i = 0; i < 4; i++)
        i2cTxData[i + 1] = *(b + i);

    TransmitAndReceiveI2cPacket(_joint.id);

    //_joint.angle = *(float*) (i2cRxData + 1);
	memcpy(&_joint.angle, i2cRxData + 1, sizeof(float));

    HAL_Delay(500); // wait servo reset
}

 
void Servo::UpdateJointAngle(Servo::JointStatus_t &_joint)
{
    UpdateServoAngle(_joint);

    float jAngle = _joint.inverted ?
                   (_joint.angleMax - _joint.angle) /
                   (_joint.angleMax - _joint.angleMin) *
                   (_joint.modelAngelMax - _joint.modelAngelMin) + _joint.modelAngelMin :
                   (_joint.angle - _joint.angleMin) /
                   (_joint.angleMax - _joint.angleMin) *
                   (_joint.modelAngelMax - _joint.modelAngelMin) + _joint.modelAngelMin;

    _joint.angle = jAngle;
}


void Servo::UpdateJointAngle(Servo::JointStatus_t &_joint, float _angleSetPoint)
{
    float sAngle = _joint.inverted ?
                   (_angleSetPoint - _joint.modelAngelMin) /
                   (_joint.modelAngelMax - _joint.modelAngelMin) *
                   (_joint.angleMin - _joint.angleMax) + _joint.angleMax :
                   (_angleSetPoint - _joint.modelAngelMin) /
                   (_joint.modelAngelMax - _joint.modelAngelMin) *
                   (_joint.angleMax - _joint.angleMin) + _joint.angleMin;

    UpdateServoAngle(_joint, sAngle);

    float jAngle = _joint.inverted ?
                   (_joint.angleMax - _joint.angle) /
                   (_joint.angleMax - _joint.angleMin) *
                   (_joint.modelAngelMax - _joint.modelAngelMin) + _joint.modelAngelMin :
                   (_joint.angle - _joint.angleMin) /
                   (_joint.angleMax - _joint.angleMin) *
                   (_joint.modelAngelMax - _joint.modelAngelMin) + _joint.modelAngelMin;

    _joint.angle = jAngle;
}
