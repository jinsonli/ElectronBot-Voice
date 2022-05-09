#include "common_inc.h"
#include "servo.h"
#include "UnitTest.h"

#define JOINT_ANY 			(0)
#define JOINT_HEAD 			(2)
#define JOINT_LEFT_ROLL 	(4)
#define JOINT_LEFT_PITCH 	(6)
#define JOINT_RIGHT_ROLL 	(8)
#define JOINT_RIGHT_PITCH 	(10)
#define JOINT_BODY 			(12)


 
void UtestServo(void)
{
	Servo servo(&hi2c1);
	
	servo.SetJointEnable(servo.joint[1], true);
    servo.SetJointEnable(servo.joint[2], true);
    servo.SetJointEnable(servo.joint[3], true);
    servo.SetJointEnable(servo.joint[4], true);
    servo.SetJointEnable(servo.joint[5], true);
	servo.SetJointEnable(servo.joint[6], true);
 
#if 0 // head
	servo.SetJointId(servo.joint[JOINT_ANY], JOINT_HEAD);
	servo.SetJointTorqueLimit(servo.joint[JOINT_ANY], 0.5f);
#endif	
	
#if 0 // left roll 
 	servo.SetJointId(servo.joint[JOINT_ANY], JOINT_LEFT_ROLL);
	servo.SetJointTorqueLimit(servo.joint[JOINT_ANY], 0.5f);
#endif	
	
#if 0 // left pitch
 	servo.SetJointId(servo.joint[JOINT_ANY], JOINT_LEFT_PITCH);
	servo.SetJointTorqueLimit(servo.joint[JOINT_ANY], 0.5f);
#endif	
	
#if 0 // right roll 
 	servo.SetJointId(servo.joint[JOINT_ANY], JOINT_RIGHT_ROLL);
	servo.SetJointTorqueLimit(servo.joint[JOINT_ANY], 0.5f);
#endif	
	
#if 0 // right pitch
 	servo.SetJointId(servo.joint[JOINT_ANY], JOINT_RIGHT_PITCH);
	servo.SetJointTorqueLimit(servo.joint[JOINT_ANY], 0.5f);
#endif	
	
#if 0 // body = 12
	servo.SetJointId(servo.joint[JOINT_ANY], JOINT_BODY);
	servo.SetJointKp(servo.joint[JOINT_ANY], 30);
	servo.SetJointKd(servo.joint[JOINT_ANY], 20);
	servo.SetJointTorqueLimit(servo.joint[JOINT_ANY], 0.8f);
#endif	
	
	servo.UpdateJointAngle(servo.joint[1], 0);
	servo.UpdateJointAngle(servo.joint[2], 0);
	servo.UpdateJointAngle(servo.joint[3], 0);
	servo.UpdateJointAngle(servo.joint[4], 0);
	servo.UpdateJointAngle(servo.joint[5], 0);
	servo.UpdateJointAngle(servo.joint[6], 0);
 
#if 0
	while(1)
	{
		for (uint32_t i = 0; i < 100; i++)
		{
			float angle = i;
			if (i >= 50) angle = 100 - i;
			
			float angle_head = i / 100.0f * 30 + -15.0f;
			
			servo.UpdateJointAngle(servo.joint[1], angle_head);
			servo.UpdateJointAngle(servo.joint[2], angle);HAL_Delay(1);
			servo.UpdateJointAngle(servo.joint[3], angle);HAL_Delay(1);
			servo.UpdateJointAngle(servo.joint[4], angle);HAL_Delay(1);
			servo.UpdateJointAngle(servo.joint[5], angle);HAL_Delay(1);
			servo.UpdateJointAngle(servo.joint[6], angle);HAL_Delay(1);
			HAL_Delay(10);
		}
	}	
#endif	
}




