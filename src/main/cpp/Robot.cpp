// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <math.h>
#include "Robot.h"
#include <unistd.h>
#include <iostream>
#include <math.h>
#include <chrono>
#include <thread>
#include "Drive.h"
#include "Auto.h"
#include "Arm.h"
#include <frc/filter/SlewRateLimiter.h>
#include <thread>
#include <chrono>
#include "cameraserver/CameraServer.h"
#include <frc/AnalogGyro.h>
#include <frc/TimedRobot.h>


#include <frc/smartdashboard/SmartDashboard.h>

void Robot::RobotInit() {
	std::cout << "-- LTBT Robot Program Start --" << std::endl;

	cs::UsbCamera camera = frc::CameraServer::StartAutomaticCapture();
	camera.SetResolution(640, 480);
	cs::CvSink cvSink = frc::CameraServer::GetVideo();
	cs::CvSource outputStream = frc::CameraServer::PutVideo("Video", 640, 480);
	
	ahrs->Reset();
	
	// frc::CameraServer::StartAutomaticCapture();

	// cs::CvSink cvSink = frc::CameraServer::GetVideo();

	// cs::CvSource outputStream = frc::CameraServer::PutVideo("Blur", 640, 480);
}

void Robot::RobotPeriodic() {}

void Robot::AutonomousInit() 
{
	hasMovedX = false;
	hasMovedY = false;
	hasRotated = false;


	// Auto newAuto;

	//newAuto.TimedAutoArmBendTwo(2000, -0.4);

	//newAuto.TimedAutoIntake(2000, 0.5);

	//newAuto.ArmStop();

	//newAuto.TimedAutoMecDrive(10000, 0.0, -0.25, 0.0);
    // using namespace std::this_thread;
	// using namespace std::chrono;
	// // set motors fo move backwards
	// backL.Set(autoSpeed);
	// backR.Set(autoSpeed);
	// frontL.Set(autoSpeed);
	// frontR.Set(autoSpeed);
	// // sleep for 5 seconds
	// sleep_for(milliseconds(5000));
	// // set motors to stop
	// backL.Set(0);
	// backR.Set(0);
	// frontL.Set(0);
	// frontR.Set(0);
}

void Robot::AutonomousPeriodic() 
{
    using namespace std::this_thread;
	using namespace std::chrono;

	double displacementX = ahrs->GetDisplacementX();
	double displacementY = ahrs->GetDisplacementY();
	double rotation = ahrs->GetAngle() * (M_PI / 180);


	double motors [4] = {0,0,0,0};
	
	if (hasMovedY == false) {
		double moveSpeed = moveYPID.Calculate(displacementY, 1) * autoSpeed;

		// left
		motors[0] += (moveSpeed);
		motors[1] += (moveSpeed);

		// right
		motors[2] += (-moveSpeed);
		motors[3] += (-moveSpeed);

		if (moveYPID.AtSetpoint()) {
			hasMovedY = true;
		}
	}

	if (hasRotated == false && hasMovedY == true) {
		double rotateSpeed = rotatePID.Calculate(rotation, 0.5) * autoSpeed;

		// left
		motors[0] -= (rotateSpeed);
		motors[1] -= (rotateSpeed);

		// right
		motors[2] -= (rotateSpeed);
		motors[3] -= (rotateSpeed);

		if (rotatePID.AtSetpoint()) {
			hasRotated = true;
		}

	}

	if (hasMovedX == false && hasRotated == true && hasMovedY == true) {
		double moveSpeed = moveXPID.Calculate(displacementX, -1) * autoSpeed;

		// left
		motors[0] += (moveSpeed);
		motors[1] += (moveSpeed);

		// right
		motors[2] += (-moveSpeed);
		motors[3] += (-moveSpeed);

		if (moveXPID.AtSetpoint()) {
			hasMovedX = true;
		}

	}	

	if (hasMovedX == true) {
		// set motors to stop
		backL.Set(0);
		backR.Set(0);
		frontL.Set(0);
		frontR.Set(0);

		intake.Set(-0.6);
		sleep_for(milliseconds(3000));
		intake.Set(0);
	} 

 


	frontL.Set(motors[0]);
	backL.Set(motors[1]);

	backR.Set(motors[2]);
	frontR.Set(motors[3]);

	// std::cout << "GetDisX:" << ahrs->GetDisplacementX() << "\n";
	// std::cout << "GetDisY:" << ahrs->GetDisplacementY() << "\n";
	// std::cout << "GetRads:" << ahrs->GetAngle() * (M_PI / 180) << "\n"; 
}

void Robot::TeleopInit() 
{
}

void Robot::TeleopPeriodic() 
{
	// Note for Teleop, DONT USE FOR LOOPS OR WHILE LOOPS! (unless it is really fast / no sleep_for() in the loop) 
	// it will stay at the loop until it is completed (essentially freezing your robot).
	// Teleop is already called every cycle, so use this to your advantage.

	using namespace frc;
	
	// std::cout << "GetAngle:" << ahrs->GetAngle() << "\n";
	// std::cout << "GetRads:" << ahrs->GetAngle() * (M_PI / 180) << "\n"; 
	// std::cout << "GetRadsSin:" << sin(ahrs->GetAngle() * (M_PI / 180)) << "\n"; 
	// std::cout << "GetRadsCos:" << cos(ahrs->GetAngle() * (M_PI / 180)) << "\n"; 
	// std::cout << "GetRadsTan:" << tan(ahrs->GetAngle() * (M_PI / 180)) << "\n"; 

	double joyXPower = controller2.GetRawAxis(1) * fabs(controller2.GetRawAxis(1)); 
	double joyYPower = controller2.GetRawAxis(0) * fabs(controller2.GetRawAxis(0)) * -1;
	double joyZPower = controller2.GetRawAxis(4);
	
	double YawRads = ahrs->GetAngle() * (M_PI / 180);

	double theta = atan2(joyYPower, joyXPower) - YawRads;
	double power = hypot(joyXPower, joyYPower);

	double SinComponent = sin(theta - M_PI/4);
	double CosComponent = cos(theta - M_PI/4);
	double MaxComponent = std::max(fabs(SinComponent), fabs(CosComponent));


	double motors [4] = {0,0,0,0};

	motors[0] = power * CosComponent/MaxComponent + joyZPower;
	motors[1] = power * SinComponent/MaxComponent + joyZPower;

	motors[2] = power * CosComponent/MaxComponent - joyZPower;
	motors[3] = power * SinComponent/MaxComponent - joyZPower;

	if ((power + fabs(joyZPower)) > 1) {
		motors[0] /= power + fabs(joyZPower);
		motors[1] /= power + fabs(joyZPower);

		motors[2] /= power + fabs(joyZPower);
		motors[3] /= power + fabs(joyZPower);
	}


	frontL.Set(motors[0]);
	backL.Set(motors[1]);

	backR.Set(motors[2]);
	frontR.Set(motors[3]);

	// Create new arm object
	double _leftJoy = -controller.GetRawAxis(1); 

	double _rightJoy = controller.GetRawAxis(5);
	int _bButton = controller.GetRawButton(2);


    double rightPower = _rightJoy * fabs(_rightJoy);

	if (_rightJoy >= 0.1){
		arm.Set(_rightJoy*_rightJoy*0.1);
	}
	else if (_rightJoy <= -0.1){
		arm.Set(_rightJoy*_rightJoy*-0.4);
	}
	else {
		arm.Set(0);
	}

	

	int _yButton = controller.GetRawButton(4);
	int _aButton = controller.GetRawButton(1);

	if (_yButton)
	{
		climber.Set(0.4);
	}
	else if (_aButton)
	{
		climber.Set(-0.25);
	}
	else
	{
		climber.Set(0);
	}
    

    if (_leftJoy >= 0.1)
    {
        armSpeed = _leftJoy*_leftJoy*1;
		intake.Set(armSpeed);
     
    }
    else if (_leftJoy <= -0.1)
    {
        armSpeed = _leftJoy*_leftJoy*0.6;
		intake.Set(-armSpeed);
       
    }
    else
    {
        intake.Set(0);
     
    }

}


void Robot::DisabledInit() {}

void Robot::DisabledPeriodic() {}

void Robot::TestInit() {}

void Robot::TestPeriodic() {}

#ifndef RUNNING_FRC_TESTS
int main() 
{
  return frc::StartRobot<Robot>();
}
#endif
