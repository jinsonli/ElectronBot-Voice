#ifndef ELECTRONBOT_FW_SERVO_H
#define ELECTRONBOT_FW_SERVO_H

#include "stm32f4xx.h"
#include "i2c.h"

class Servo
{
public:
    Servo(I2C_HandleTypeDef* _motorI2c) :
        motorI2c(_motorI2c)
    {
        /********* Need to adjust parameters for specific hardware *********/
        joint[0].id = 0;
        joint[0].angleMin = 0;
        joint[0].angleMax= 180;
        joint[0].angle = 90;
        joint[0].modelAngelMin = -90;
        joint[0].modelAngelMax = 90;
        joint[0].inverted = false;
#if 1	
		// head
        joint[1].id = 2;
        joint[1].angleMin = 60;
        joint[1].angleMax= 100;	
        joint[1].angle = 90;
        joint[1].modelAngelMin = -15;
        joint[1].modelAngelMax = 15;
        joint[1].inverted = true;
		
		// Left arm roll
        joint[2].id = 4;
        joint[2].angleMin = 90;
        joint[2].angleMax= 120;
        joint[2].angle = 0;
        joint[2].modelAngelMin = 0;
        joint[2].modelAngelMax = 30;
        joint[2].inverted = false;
		
		// Left arm pitch
        joint[3].id = 6;
        joint[3].angleMin = 0;
        joint[3].angleMax= 180;
        joint[3].angle = 0;
        joint[3].modelAngelMin = -20;
        joint[3].modelAngelMax = 180;
        joint[3].inverted = false;
		
		// Right arm roll
        joint[4].id = 8;
        joint[4].angleMin = 80;
        joint[4].angleMax= 110;	
        joint[4].angle = 0;
        joint[4].modelAngelMin = 0;
        joint[4].modelAngelMax = 30;
        joint[4].inverted = true;
		
		// Right arm pitch
        joint[5].id = 10;
        joint[5].angleMin = 0;
        joint[5].angleMax= 180;
        joint[5].angle = 0;
        joint[5].modelAngelMin = -20;
        joint[5].modelAngelMax = 180;
        joint[5].inverted = true;

		// Body
        joint[6].id = 12;
        joint[6].angleMin = 0;
        joint[6].angleMax= 180;
        joint[6].angle = 0;
        joint[6].modelAngelMin = -90;
        joint[6].modelAngelMax = 90;
        joint[6].inverted = true;
        /********* Need to adjust parameters for specific hardware *********/

#else
		// head
        joint[1].id = 2;
        joint[1].angleMin = 0;
        joint[1].angleMax= 180;		
        joint[1].angle = 90;
        joint[1].modelAngelMin = 0;
        joint[1].modelAngelMax = 180;
        joint[1].inverted = true;
		
		// Left arm roll
        joint[2].id = 4;
		joint[2].angleMin = 0;
        joint[2].angleMax= 180;	
        joint[2].angle = 0;
        joint[2].modelAngelMin = 0;
        joint[2].modelAngelMax = 180;
        joint[2].inverted = false;
		
		// Left arm pitch
        joint[3].id = 6;
        joint[3].angleMin = 0;
        joint[3].angleMax= 180;
        joint[3].angle = 0;
        joint[3].modelAngelMin = 0;
        joint[3].modelAngelMax = 180;
        joint[3].inverted = false;
		
		// Right arm roll
        joint[4].id = 8;
        joint[4].angleMin = 0;
        joint[4].angleMax= 180;			
        joint[4].angle = 0;
        joint[4].modelAngelMin = 0;
        joint[4].modelAngelMax = 180;
        joint[4].inverted = true;
		
		// Right arm pitch
        joint[5].id = 10;
        joint[5].angleMin = 0;
        joint[5].angleMax= 180;
        joint[5].angle = 0;
        joint[5].modelAngelMin = 0;
        joint[5].modelAngelMax = 180;
        joint[5].inverted = true;

		// Body
        joint[6].id = 12;
        joint[6].angleMin = 0;
        joint[6].angleMax= 180;
        joint[6].angle = 0;
        joint[6].modelAngelMin = 0;
        joint[6].modelAngelMax = 180;
        joint[6].inverted = true;
#endif

    }	

    struct JointStatus_t
    {
        uint8_t id;
        float angleMin;
        float angleMax;
        float angle;
        float modelAngelMin;
        float modelAngelMax;
        bool inverted;
    };
    JointStatus_t joint[7];
	JointStatus_t joint_dbg;

	void Init(void);
	
    void SetJointId(JointStatus_t &_joint, uint8_t _id);
    void SetJointKp(JointStatus_t &_joint, float _value);
    void SetJointKi(JointStatus_t &_joint, float _value);
    void SetJointKv(JointStatus_t &_joint, float _value);
    void SetJointKd(JointStatus_t &_joint, float _value);
    void SetJointEnable(JointStatus_t &_joint, bool _enable);
    void SetJointInitAngle(JointStatus_t &_joint, float _angle);
    void SetJointTorqueLimit(JointStatus_t &_joint, float _percent);

    void UpdateServoAngle(JointStatus_t &_joint);
    void UpdateServoAngle(JointStatus_t &_joint, float _angleSetPoint);
    void UpdateJointAngle(JointStatus_t &_joint);
    void UpdateJointAngle(JointStatus_t &_joint, float _angleSetPoint);
	
	void TransmitAndReceiveI2cPacket(uint8_t _id);
private:
    I2C_HandleTypeDef* motorI2c;

    uint8_t i2cRxData[8];
    uint8_t i2cTxData[8];

    //void TransmitAndReceiveI2cPacket(uint8_t _id);
};

#endif //ELECTRONBOT_FW_ROBOT_H
