#include "Robot.h"

class Arm
{
private:

    frc::Joystick controller{2};

	WPI_VictorSPX bendOne {6};
	WPI_VictorSPX bendTwo {1};
	WPI_VictorSPX intake1 {0};
	WPI_VictorSPX intake2 {7};


	int _leftTrigger = controller.GetRawAxis(2);
	int _rightTrigger = controller.GetRawAxis(3);

	int _xButton = controller.GetRawButton(3);
	int _bButton = controller.GetRawButton(2);
	int _yButton = controller.GetRawButton(4);

	int _lBumper = controller.GetRawButton(5);
	int _rBumper = controller.GetRawButton(6);

	double _leftJoy = -controller.GetRawAxis(1); 
	double _rightJoy = controller.GetRawAxis(5);

public:
    Arm(/* args */);
    void ArmBendOne(double maxSpeed);
    void ArmBendTwo(double maxSpeed);
    void Intake(double speed);

	// Raw Functions
	void RawIntake(double speed);
	void RawBendTwo(double speed);
	void ArmSafety(bool enabled);
};

