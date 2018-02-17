/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

//Dreadbot VIII - FIRST Power Up
//Authors: Christian Vaughan, Ethan Leonello, Clara Gauthier, Anna Robelen

#include <iostream>
#include <string>

#include <time.h>
#include "Timer.h"

#include <IterativeRobot.h>
#include <LiveWindow/LiveWindow.h>
#include <SmartDashboard/SendableChooser.h>
#include <SmartDashboard/SmartDashboard.h>

#include <WPILib.h>
#include <ctre/Phoenix.h>

#include <AHRS.h>

class Robot : public frc::IterativeRobot
{


//------------------------------------------------------
//Solenoid Variables
//------------------------------------------------------
	Compressor *grabComp = new Compressor(0);
	Solenoid *gSol1 = new Solenoid(0); //arms up and down
	Solenoid *gSol2 = new Solenoid(1); //open claw


//------------------------------------------------------
//Talon Variables
//------------------------------------------------------
	WPI_TalonSRX *lf = new WPI_TalonSRX(0); //left front
	WPI_TalonSRX *rf = new WPI_TalonSRX(1); //right front
	WPI_TalonSRX *lr = new WPI_TalonSRX(2); //left rear
	WPI_TalonSRX *rr = new WPI_TalonSRX(3); //right rear
	WPI_TalonSRX *sLift = new WPI_TalonSRX(4); //Skylift
	WPI_TalonSRX *cWinch = new WPI_TalonSRX(5); //climb winch
	WPI_TalonSRX *cExtend = new WPI_TalonSRX(6); //climb extend
	WPI_TalonSRX *pWheelL = new WPI_TalonSRX(7); //pickup wheels left
	WPI_TalonSRX *pWheelR = new WPI_TalonSRX(8); //pickup wheels right


//------------------------------------------------------
//Ultrasonic Variables
//------------------------------------------------------
	Ultrasonic *ultra = new Ultrasonic(0, 1); //ultra sonic sensor
	double distance = 0;
	double driveSpeed = 0;


//------------------------------------------------------
//Controller Variables
//------------------------------------------------------

	//joystick variables
	int joystickX=0;
	int joystickY=1;
	int joystickRot=2;

	//controller 1 buttons
	int climbTheScale = 1;
	int retractBird = 2;
	int extendBird = 3;
	int deployBirdArm = 4;
	int skyLiftDown = 5;
	int cubeDrop = 6;
	int skyLiftUp = 7;
	int turboButton = 8;

	//controller 2 buttons
	int closeArms = 1;
	int openArms = 4;
	int pickupArmsUp = 5;
	int throwCube = 6;
	int pickupArmsDown = 7;
	int captureCube = 8;


//------------------------------------------------------
//Auton Variables
//------------------------------------------------------
	int targetAngle = 0;
	double currentAngle = 0;
	double angleRemain = targetAngle - currentAngle;
	double angleSlop = 3;
	int autonPosition = 0;
	bool autonIsLeftSwitch = false;
	bool autonIsBlueAlliance = false;

//------------------------------------------------------
//Vision Variables
//------------------------------------------------------
	double centerX = 0.0;
	double centerY = 0.0;
	double area = 0.0;

//------------------------------------------------------
//Drive Variables
//------------------------------------------------------
	double teleMaxSpeed = 0.5;




public:

	Joystick *js1; //primary controller
	Joystick *js2; //secondary controller
	MecanumDrive *m_robotDrive;
    AHRS *gyro;

	void RobotInit()
	{
		//m_chooser.AddDefault(kAutoNameDefault, kAutoNameDefault);
		//m_chooser.AddObject(kAutoNameCustom, kAutoNameCustom);
		frc::SmartDashboard::PutData("Auto Modes", &m_chooser);

		gyro = new AHRS(SPI::Port::kMXP);

		gyro->ZeroYaw();
		ultra->SetAutomaticMode(true);

    	js1 = new Joystick(0);
    	js2 = new Joystick(1);
    	m_robotDrive = new MecanumDrive(*lf,*lr, *rf,*rr);
    	m_robotDrive->SetExpiration(0.5);
    	m_robotDrive->SetSafetyEnabled(false);

	}


	/*
	 * This autonomous (along with the chooser code above) shows how to
	 * select
	 * between different autonomous modes using the dashboard. The sendable
	 * chooser code works with the Java SmartDashboard. If you prefer the
	 * LabVIEW Dashboard, remove all of the chooser code and uncomment the
	 * GetString line to get the auto name from the text box below the Gyro.
	 *
	 * You can add additional auto modes by adding additional comparisons to
	 * the
	 * if-else structure below with additional strings. If using the
	 * SendableChooser make sure to add them to the chooser code above as
	 * well.
	 */




	void Wait(double t)
	{
		clock_t wait;
		wait = clock();
		double start = wait;
		double timeLeft = (start + t) - wait;
		while (wait <= start + t)
		{
			wait = clock();
			distance = ultra->GetRangeInches();
			frc::SmartDashboard::PutNumber("distance", distance);
			timeLeft = (start + t) - wait;
			frc::SmartDashboard::PutNumber("Time Left", timeLeft);
		}

		return;
	}

	void DriveForward(double x, double t)
	{
		lf -> Set(ControlMode::PercentOutput, x);
		lr -> Set(ControlMode::PercentOutput, x);
		rf -> Set(ControlMode::PercentOutput, x);
		rr -> Set(ControlMode::PercentOutput, x);
		Wait(t); //Motors set for t time
		lf -> Set(ControlMode::PercentOutput, 0);
		lr -> Set(ControlMode::PercentOutput, 0);
		rf -> Set(ControlMode::PercentOutput, 0);
		rr -> Set(ControlMode::PercentOutput, 0);
		x = 0;
		t = 0;
	}

	void DriveSideways(double x, double t)
	{
		lf -> Set(ControlMode::PercentOutput, -x);
		lr -> Set(ControlMode::PercentOutput, x);
		rf -> Set(ControlMode::PercentOutput, -x);
		rr -> Set(ControlMode::PercentOutput, x);
		Wait(t);
		lf -> Set(ControlMode::PercentOutput, 0);
		lr -> Set(ControlMode::PercentOutput, 0);
		rf -> Set(ControlMode::PercentOutput, 0);
		rr -> Set(ControlMode::PercentOutput, 0);
	}

	void Lifter(int x)
	{
		//raise or lower the lifting device
		if (x == 0)
		{

		}

		else if(x == 1)
		{

		}

		else
		{

		}
	}



	void TurnToAngle(int targetAngle)
	{
			//Turn setting motor speed to  currentAngle / targetAngle and have a minimum of .1
		currentAngle = gyro->GetYaw();
		double x = targetAngle / fabs(currentAngle);

		while (fabs(currentAngle) < fabs(targetAngle) - angleSlop)
		{
			if (x > 1)
				x = 1;
			lf -> Set(ControlMode::PercentOutput, x);
			lr -> Set(ControlMode::PercentOutput, x);
			rf -> Set(ControlMode::PercentOutput, -x);
			rr -> Set(ControlMode::PercentOutput, -x);
			currentAngle = gyro->GetYaw();
			x = targetAngle / currentAngle;

			}


	}
	void DriveToBlock(){
	double distance = ultra->GetRangeInches();
	SmartDashboard::PutNumber("DriveSpeed", driveSpeed);
	//if (driveMode != DriveMode::Driving){
	//return;
	//	}
			if (distance >= 44){
			lf ->Set(ControlMode::PercentOutput, -driveSpeed);
			lr ->Set(ControlMode::PercentOutput, -driveSpeed);
			rf ->Set(ControlMode::PercentOutput, driveSpeed);
			rr ->Set(ControlMode::PercentOutput, driveSpeed);

			distance = ultra->GetRangeInches();
			SmartDashboard::PutNumber("Distance", distance);
				}
		if (distance < 44 && distance >= 8){

		driveSpeed = driveSpeed * 0.9;

		}
		if (distance < 8) {
		//driveMode = DriveMode::Pickup;
		lf ->Set(ControlMode::PercentOutput, 0);
		lr ->Set(ControlMode::PercentOutput, 0);
		rf ->Set(ControlMode::PercentOutput, 0);
	    rr ->Set(ControlMode::PercentOutput, 0);
		}
	//here's where we need to grab the block
	}

	//This extends and retracts the pole. It also sets the motor to 0
	void ActuateBirdPole(int extend) //this function 1. makes the motor go forward to extend the pole
									 //2. rewinds the pole by making the motor negative 3. stops the motor
		{
			if (extend > 0)
			{
				cExtend->Set(0.5);
			}

			else if (extend < 0)
			{
				cExtend->Set(-0.5);
			}

			else if (extend == 0)
			{
				cExtend->Set(0.0);
			}
		}

	void teleopSkyLift()
	{
		bool goingUp = js1->GetRawButton(skyLiftUp);
		bool goingDown = js1->GetRawButton(skyLiftDown);

		if(!goingUp && !goingDown){
			sLift ->Set(-0.2);
		}
		else if(goingUp){
		 sLift ->Set(-.6);
		}
		else if(goingDown){
			sLift ->Set(.1);
		}
	}
	void teleopArmControl() //Secondary controller controls to move grabber
	{
		if(js2->GetRawButton(pickupArmsUp))
		{
			gSol1->Set(false);
		}
		if(js2->GetRawButton(pickupArmsDown))
		{
			gSol1->Set(true);
		}
		if(js2->GetRawButton(openArms))
		{
			gSol2->Set(true);
		}
		if(js2->GetRawButton(closeArms))
		{
			gSol2->Set(false);
		}
	}

	void Drop()
	{
		if(js1->GetRawButton(cubeDrop))
		{
			gSol2->Set(true);
		}
		else
		{
			gSol2->Set(false);
		}
	}

	void teleopGrabToggle()
	{
		if(js2->GetRawButton(4))
		{
			gSol2->Set(true);
		}
		else if(js2->GetRawButton(1))
		{
			gSol2->Set(false);
		}
		else
		{
			gSol2->Set(false);
		}

		if(js2->GetRawButton(5)){
			gSol1->Set(true);
		}
		if(js2->GetRawButton(7)){
			gSol1->Set(false);
		}
	}

	void pickUpWheels() //Secondary controller controls to turn on wheels
	{
		if(js2->GetRawButton(captureCube)){
			pWheelL->Set(.75);
			pWheelR->Set(-.75);
		}
		else if (js2->GetRawButton(throwCube)){
			pWheelL->Set(-.6);
			pWheelR->Set(.6);
		}
		else{
			pWheelL->Set(0);
			pWheelR->Set(0);
		}



	}

	void LeftOne()
	{
		DriveForward(1, 1000);
		TurnToAngle(-90);
		Lifter(1);
		DriveForward(1, 1000);
		TurnToAngle(-90);
		Drop();
	}


	void LeftTwo()
	{
		Wait(5000);
		DriveSideways(-1, 1000);
		Lifter(1);
		DriveForward(1, 1000);
		Drop();
	}

	void LeftThree()
	{
		DriveForward(1, 1000);
		Lifter(1);
		TurnToAngle(90);
		DriveForward(1, 1000);
		Drop();
	}

	void RightOne()
	{
		DriveForward(1, 1000);
		Lifter(1);
		TurnToAngle(-90);
		DriveForward(1, 1000);
		Drop();
	}

	void RightTwo()
	{
		Wait(5);
		DriveSideways(1, 1000);
		Lifter(1);
		DriveForward(1, 1000);
		Drop();
	}

	void RightThree()
	{
		DriveForward(1, 1000);
		TurnToAngle(90);
		Lifter(1);
		DriveForward(1, 1000);
		TurnToAngle(90);
		Drop();
	}

	void MecDrive(double xAxis, double yAxis, double rot) //homemade mecanum drive!
	{
		double noMove = 0.2; //Dead area of the axes
		double maxSpeed = .5; //normal speed (not turbo)

		if (fabs(xAxis) < noMove)
			xAxis = 0.0;

		if (fabs(yAxis) < noMove)
			yAxis = 0.0;

		if (fabs(rot) < noMove)
			rot = 0.0;

		if (js1->GetRawButton(turboButton))
			maxSpeed = 1;

		else
			maxSpeed = .5;

		double lfSpeed = -yAxis - xAxis - rot;
		double rfSpeed = +yAxis - xAxis - rot;
		double rrSpeed = +yAxis + xAxis - rot;
		double lrSpeed = -yAxis + xAxis - rot;

		if (fabs(lfSpeed) > 1)
			lfSpeed = fabs(lfSpeed) / lfSpeed;

		if (fabs(lrSpeed) > 1)
			lrSpeed = fabs(lrSpeed) / lrSpeed;

		if (fabs(rfSpeed) > 1)
			rfSpeed = fabs(rfSpeed) / rfSpeed;

		if (fabs(rrSpeed) > 1)
			rrSpeed = fabs(rrSpeed) / rrSpeed;

		lf -> Set(ControlMode::PercentOutput, lfSpeed*maxSpeed);
		lr -> Set(ControlMode::PercentOutput, lrSpeed*maxSpeed);
		rf -> Set(ControlMode::PercentOutput, rfSpeed*maxSpeed);
		rr -> Set(ControlMode::PercentOutput, rrSpeed*maxSpeed);
	}
//	void TankDrive(double xAxis, double yAxis, double zAxis)
//	{
//		double noMove = 0.2; //Dead area of the axes
//		double maxSpeed = .5;
//
//		double lfSpeed = 0;
//		double rfSpeed = 0;
//		double lrSpeed = 0;
//		double rrSpeed = 0;
//
//
//		lf -> Set(ControlMode::PercentOutput, lfSpeed*maxSpeed);
//		lr -> Set(ControlMode::PercentOutput, -lrSpeed*maxSpeed);
//		rf -> Set(ControlMode::PercentOutput, rfSpeed*maxSpeed);
//		rr -> Set(ControlMode::PercentOutput, -rrSpeed*maxSpeed);
//
//	}

	void ArcadeDrive(double yAxis, double rot)
	{
		double noMove = 0.2; //Dead area of the axes
		double maxSpeed = .5; //normal speed (not turbo)

		if (fabs(rot) < noMove)
			rot = 0.0;

		if (js1->GetRawButton(turboButton))
			maxSpeed = 1;

		else
			maxSpeed = .5;

		double lfSpeed = -yAxis - rot;
		double rfSpeed = +yAxis - rot;
		double rrSpeed = +yAxis - rot;
		double lrSpeed = -yAxis - rot;

		if (fabs(lfSpeed) > 1)
			lfSpeed = fabs(lfSpeed) / lfSpeed;

		if (fabs(lrSpeed) > 1)
			lrSpeed = fabs(lrSpeed) / lrSpeed;

		if (fabs(rfSpeed) > 1)
			rfSpeed = fabs(rfSpeed) / rfSpeed;

		if (fabs(rrSpeed) > 1)
			rrSpeed = fabs(rrSpeed) / rrSpeed;

		lf -> Set(ControlMode::PercentOutput, lfSpeed*maxSpeed);
		lr -> Set(ControlMode::PercentOutput, lrSpeed*maxSpeed);
		rf -> Set(ControlMode::PercentOutput, rfSpeed*maxSpeed);
		rr -> Set(ControlMode::PercentOutput, rrSpeed*maxSpeed);
	}

    double Dz(double axisVal) //deadzone value
    {
    	if(axisVal < -0.20)
    		return axisVal;
    	if(axisVal > 0.20)
    		return axisVal;
    	return 0;
    }



	char givenPos = ' '; //Where we start against the alliance wall
	char switchRL = ' '; //Where our side if the switch is

	void AutonomousInit() override
	{
		//SmartDashboard::PutNumber("Auton Position", autonPosition);
		//SmartDashboard::PutBoolean("Auton Is Blue Alliance?", autonIsBlueAlliance);
		//SmartDashboard::PutBoolean("Auton Is Going to Left Switch?", autonIsLeftSwitch);

		SmartDashboard::GetNumber("Auton Position", autonPosition);
		SmartDashboard::GetBoolean("Auton Is Blue Alliance", autonIsBlueAlliance);

		if (autonIsBlueAlliance == true)
		{
//			if (/*left switch is blue*/)
//			{
//				autonIsLeftSwitch = true;
//			}
//			if ()/*left switch is blue*/
//			{
//				autonIsLeftSwitch = true;
//			}
//			if ()/*left switch is blue*/
//			{
//				autonIsLeftSwitch = true;
//			}
//			if ()/*left switch is blue*/
//			{
//				autonIsLeftSwitch = true;
//			}
//			if ()/*left switch is blue*/
//			{
//				autonIsLeftSwitch = true;
//			}
//			if ()/*left switch is blue*/
//			{
//				autonIsLeftSwitch = true;
//			}
//			if ()/*left switch is blue*/
//			{
//				autonIsLeftSwitch = true;
//			}
//			if ()/*left switch is blue*/
//			{
//				autonIsLeftSwitch = true;
//			}
//			if ()/*left switch is blue*/
//			{
//				autonIsLeftSwitch = true;
//			}
//			if ()/*left switch is blue*/
//			{
//				autonIsLeftSwitch = true;
//			}
		}

		else if(autonIsBlueAlliance == false)
		{
//			if(/*left switch is red*/)
//			{
//				autonIsLeftSwitch = true;
//			}
		}

		SmartDashboard::GetBoolean("Auton Is Going to Left Switch?", autonIsLeftSwitch);

		currentAngle = 0;
		gyro->ZeroYaw();
		}

	void AutonomousPeriodic()
	{
		//These are the numbers coming from the vision camera
		centerX = SmartDashboard::GetNumber("yellowbox.contour_1.cx", -1);
		centerY = SmartDashboard::GetNumber("yellowbox.contour_1.cy", -1);
		area = SmartDashboard::GetNumber("yellowbox.contour_1.area", -1);

		//Putting the same values as X and Y values for easy identification
		SmartDashboard::PutNumber("Center X ", centerX);
		SmartDashboard::PutNumber("Center Y ", centerY);
		SmartDashboard::PutNumber("Area ", area);

		distance = ultra->GetRangeInches();
		frc::SmartDashboard::PutNumber("distance", distance);

		currentAngle = gyro->GetYaw();
		frc::SmartDashboard::PutNumber("Current Angle", currentAngle);

		SmartDashboard::GetNumber("Auton Position", autonPosition);
		SmartDashboard::GetBoolean("Auton Is Blue Alliance", autonIsBlueAlliance);
		SmartDashboard::GetBoolean("Auton Is Going to Left Switch?", autonIsLeftSwitch);

		if(autonPosition == 1)
		{
			if (autonIsLeftSwitch == true)
				LeftOne();

			else
				RightOne();
		}

		if(autonPosition == 2)
		{
			if (autonIsLeftSwitch == true)
				LeftTwo();

			else
				RightTwo();
		}

		if(autonPosition == 3)
		{
			if (autonIsLeftSwitch == true)
				LeftThree();

			else
				RightThree();
		}

	}



	void TeleopInit()
	{
		gyro->Reset();
	}

	void TeleopPeriodic()
	{


		//teleDrive();


		double jsX=js1->GetRawAxis(joystickX);
		double jsY=js1->GetRawAxis(joystickY);
		double jsRot=js1->GetRawAxis(joystickRot);
		ArcadeDrive(-jsY,jsRot);


		//teleopGrabToggle();

		teleopArmControl();
		//Drop();
		pickUpWheels();
		teleopSkyLift();

		int PoleExtend=0;
		if (js1->GetRawButton(extendBird)){ //Button B extends the pole
			PoleExtend=1;
		}
		if (js1->GetRawButton(retractBird)){ //Button A retracts the pole
			PoleExtend=-1;
		}

		ActuateBirdPole(PoleExtend);

	}

	void TestPeriodic()
	{

	}

private:
	frc::LiveWindow& m_lw = *LiveWindow::GetInstance();
	frc::SendableChooser<std::string> m_chooser;
	//const std::string kAutoNameDefault = "Default";
	//const std::string kAutoNameCustom = "My Auto";
	std::string m_autoSelected;
};

START_ROBOT_CLASS(Robot)