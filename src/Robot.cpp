/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

//Dreadbot VIII - FIRST Power Up
//Authors: Christian Vaughan, Ethan Leonello, Clara Gauthier, Anna Robelen, Robert Lindskov, Justin McIntosh,

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
	Solenoid *armSol = new Solenoid(0); //arms up and down
	Solenoid *clawSol = new Solenoid(1); //open claw
	Solenoid *birdSol = new Solenoid(2); //deploy bird pole

//------------------------------------------------------
//Talon Variables
//------------------------------------------------------
	WPI_TalonSRX *lf = new WPI_TalonSRX(0); //left front
	WPI_TalonSRX *rf = new WPI_TalonSRX(1); //right front
	WPI_TalonSRX *lr = new WPI_TalonSRX(2); //left rear
	WPI_TalonSRX *rr = new WPI_TalonSRX(3); //right rear
	WPI_TalonSRX *sLift = new WPI_TalonSRX(4); //Sky lift
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
//	int targetAngle = 0;
//	double currentAngle = 0;
//	double angleRemain = targetAngle - currentAngle;
//	double angleSlop = 3;
//	int autonPosition = 0;
//	bool autonIsLeftSwitch = false;
//	bool autonIsBlueAlliance = false;
	double robotPos;
	bool encode;
	int timer;
	int state;
	std::string gameData;
	int currentAngle;
	double autonSpeed = 0.5;
	double autonTurnSpeed = 0.5;
	int FirstAction = 100;
	int SecondAction = 200;
	int ThirdAction = 350;
	int FourthAction = 550;
	int FifthAction = 600;
	int SixthAction = 625;
	double LeftDifference = 0;
	double RightDifference = 0;
	bool LiftComplete;
	frc::SendableChooser<int> posChooser;
	frc::SendableChooser<bool> encodeChooser;
	int oneSecond = 50;
	int halfSecond = 25;
	int twoSeconds = 100;


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
	bool isBirdOut = false;
	bool isYDown = false;




public:

	Joystick *js1; //primary controller
	Joystick *js2; //secondary controller
	MecanumDrive *m_robotDrive; //not currently in use
    AHRS *gyro;

	void RobotInit()
	{
		//m_chooser.AddDefault(kAutoNameDefault, kAutoNameDefault);
		//m_chooser.AddObject(kAutoNameCustom, kAutoNameCustom);
		//frc::SmartDashboard::PutData("Auto Modes", &m_chooser);

		gyro = new AHRS(SPI::Port::kMXP);

		gyro->ZeroYaw();
		ultra->SetAutomaticMode(true);

    	js1 = new Joystick(0);
    	js2 = new Joystick(1);
    	m_robotDrive = new MecanumDrive(*lf,*lr, *rf,*rr);
    	m_robotDrive->SetExpiration(0.5);
    	m_robotDrive->SetSafetyEnabled(false);
	//SmartDashboard::PutNumber("robotPos", robotPos);
    	posChooser.AddDefault("Auton Line", 0);
    	posChooser.AddObject("Left", 1);
    	posChooser.AddObject("Center", 2);
    	posChooser.AddObject("Right", 3);
    	encodeChooser.AddDefault("Yes", true);
    	encodeChooser.AddObject("No", false);
    	frc::SmartDashboard::PutData("robotPos", &posChooser);
    	frc::SmartDashboard::PutData("Using Encoders?", &encodeChooser);
    	lf->ConfigNominalOutputForward(0,0);
    	lr->ConfigNominalOutputForward(0,0);
    	rf->ConfigNominalOutputForward(0,0);
    	rr->ConfigNominalOutputForward(0,0);
    	sLift->ConfigNominalOutputForward(0,0);

    	lf->ConfigNominalOutputReverse(0,0);
    	lr->ConfigNominalOutputReverse(0,0);
    	rf->ConfigNominalOutputReverse(0,0);
    	rr->ConfigNominalOutputReverse(0,0);
    	sLift->ConfigNominalOutputReverse(0,0);

    	lf->ConfigPeakOutputForward(1,0);
    	lr->ConfigPeakOutputForward(1,0);
    	rf->ConfigPeakOutputForward(1,0);
    	rr->ConfigPeakOutputForward(1,0);
    	sLift->ConfigPeakOutputForward(1,0);

    	lf->ConfigPeakOutputReverse(-1,0);
    	lr->ConfigPeakOutputReverse(-1,0);
    	rf->ConfigPeakOutputReverse(-1,0);
    	rr->ConfigPeakOutputReverse(-1,0);
    	sLift->ConfigPeakOutputReverse(-1,0);
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
		int wait = 0;
		double timeLeft = t - wait;
		while (wait <= t)
		{
			distance = ultra->GetRangeInches();
			frc::SmartDashboard::PutNumber("distance", distance);
			timeLeft = t - wait;
			frc::SmartDashboard::PutNumber("Time Left", timeLeft);
			wait++;
		}

		return;
	}
	void Stop(){
		lf -> Set(ControlMode :: PercentOutput, 0);
		rf -> Set(ControlMode :: PercentOutput, 0);
		lr -> Set(ControlMode :: PercentOutput, 0);
		rr -> Set(ControlMode :: PercentOutput, 0);
	}
	void TurnRight(double speed){
		lf -> Set(ControlMode :: PercentOutput, -speed);
		lr -> Set(ControlMode :: PercentOutput, -speed);
		rf -> Set(ControlMode :: PercentOutput, -speed);
		rr -> Set(ControlMode :: PercentOutput, -speed);
	}
	void TurnLeft(double speed){
		lf -> Set(ControlMode :: PercentOutput, speed);
		lr -> Set(ControlMode :: PercentOutput, speed);
		rf -> Set(ControlMode :: PercentOutput, speed);
		rr -> Set(ControlMode :: PercentOutput, speed);
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
	
	void DriveStraight(double speed){
		float slop = 5;

		if(currentAngle < 0 - slop){
			lf -> Set(ControlMode :: PercentOutput, -speed);
			rf -> Set(ControlMode :: PercentOutput, speed+LeftDifference);
			lr -> Set(ControlMode :: PercentOutput, -speed);
			rr -> Set(ControlMode :: PercentOutput, speed+LeftDifference);
		}
		else if(currentAngle > 0 + slop){
			lf -> Set(ControlMode :: PercentOutput, -speed+RightDifference);
			rf -> Set(ControlMode :: PercentOutput, speed);
			lr -> Set(ControlMode :: PercentOutput, -speed+RightDifference);
			rr -> Set(ControlMode :: PercentOutput, speed);

		}

		else{
			rf -> Set(ControlMode :: PercentOutput, speed);
			lf -> Set(ControlMode :: PercentOutput, -speed);
			lr -> Set(ControlMode :: PercentOutput, -speed);
			rr -> Set(ControlMode :: PercentOutput, speed);
			}
		}

	void Strafe(std::string side)
	{
		if(side == "left"){
			lf ->Set(ControlMode :: PercentOutput, autonSpeed);
			lr ->Set(ControlMode :: PercentOutput, -autonSpeed);
			rf ->Set(ControlMode :: PercentOutput, autonSpeed);
			rr ->Set(ControlMode :: PercentOutput, -autonSpeed);
		}
		else if(side == "right"){
			lf ->Set(ControlMode :: PercentOutput, -autonSpeed);
			lr ->Set(ControlMode :: PercentOutput, autonSpeed);
			rf ->Set(ControlMode :: PercentOutput, -autonSpeed);
			rr ->Set(ControlMode :: PercentOutput, autonSpeed);
		}
	}

	void Lifter(int x)
	{
		//raise or lower the lifting device
		if (x == 1)
		{
			sLift->Set(-1);


		}
		else if(x==0){
			sLift->Set(-.2);
		}



	}
bool turnComplete = false;


	void Turn(int wantAngle)
	{
//		frc::SmartDashboard::PutNumber("Want Angle", wantAngle);
//		frc::SmartDashboard::PutNumber("Current Angle", CurrentAngle);
		int slop = 5;
		if(wantAngle < currentAngle - slop){
			TurnLeft(autonTurnSpeed);
		}
		else if(wantAngle > currentAngle + slop){
			TurnRight(autonTurnSpeed);
		}
		else{
			Stop();
			turnComplete = true;
			gyro->ZeroYaw();
			currentAngle = 0;
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


	void DeployBirdPole()
	{
		if(js1->GetRawButton(4) && isBirdOut == false && isYDown == false)
		{
			birdSol->Set(true);
			isBirdOut = true;
			isYDown = true;
		}
		else if(!js1->GetRawButton(4) && isBirdOut == true)
		{
			isYDown = false;
		}
		else if(js1->GetRawButton(4) && isBirdOut == true && isYDown == false)
		{
			birdSol->Set(false);
			isBirdOut = false;
			isYDown = true;
		}
		else if(!js1->GetRawButton(4) && isBirdOut == false)
		{
			isYDown = false;
		}
	}
	void winch()
	{
		if(js1->GetRawButton(1))
		{
			cWinch->Set(1);
		}
		else if(js1->GetRawButton(10))
		{
			cWinch->Set(-1);
		}
		else
		{
			cWinch->Set(0);
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
		 sLift ->Set(-1);
		}
		else if(goingDown){
			sLift ->Set(.5);
		}
	}

	void teleopArmControl() //Secondary controller controls to move grabber
	{
		if(js2->GetRawButton(pickupArmsUp))
		{
			armSol->Set(false);
		}
		if(js2->GetRawButton(pickupArmsDown))
		{
			armSol->Set(true);
		}
		if(js2->GetRawButton(openArms))
		{
			clawSol->Set(true);
		}
		if(js2->GetRawButton(closeArms))
		{
			clawSol->Set(false);
		}
	}

	void Drop()
	{
		armSol->Set(true);
		clawSol->Set(true);
		}


	void teleopGrabToggle()
	{
		if(js2->GetRawButton(4))
		{
			clawSol->Set(true);
		}
		else if(js2->GetRawButton(1))
		{
			clawSol->Set(false);
		}
		else
		{
			clawSol->Set(false);
		}

		if(js2->GetRawButton(5)){
			armSol->Set(true);
		}
		if(js2->GetRawButton(7)){
			armSol->Set(false);
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
		FirstAction = 40;
		SecondAction = 160;
		ThirdAction = 210;
		FourthAction = 220;

		if(timer < FirstAction){
			state = 1; //Lifting, from start to first action
		}
		if(timer < SecondAction && state == 1 && timer > FirstAction){
			state = 2; //Driving, between first and second action
		}
		if(timer == SecondAction && state == 2){
			state = 3;//Stopping at second action
		}
		if(state == 3){
			state = 4; //Turning Left, setting timer to third action when done
		}
		if(state == 4 && turnComplete){
			state = 5; //Driving, between third and fourth action
		}
		if(state == 5 && timer == FourthAction){
			state = 6;// Stopping, at fourth action
		}

		if(state == 1){
			Lifter(1);
		}
		else if(state == 2){
			Lifter(0);
			DriveStraight(autonSpeed);

		}
		else if(state == 3){
			Stop();
		}
		else if(state == 4){
			Turn(90);
			timer = ThirdAction;
		}
		else if(state == 5){
			DriveStraight(autonSpeed);
		}
		else if(state == 6){
			Stop();
			Drop();
		}
	}
	void LeftTwo()
	{
		FirstAction = 30;
		SecondAction = 180;
		ThirdAction = 250;

		if(timer < FirstAction){
			state = 1;//Lifting and driving from start to FirstAction

		}
		if(timer > FirstAction && timer < SecondAction){
			state = 2;//Strafing right from First to second action
		}
		if(timer == SecondAction) {
			state = 3;//stopping at second action
		}
		if(timer > SecondAction && timer < ThirdAction){
			state = 4;//Driving from second to third action
		}
		if(timer == ThirdAction){
			state = 5;//Stopping and dropping at third action
		}



		if(state == 1){
			Lifter(1);
			DriveStraight(autonSpeed);
			std::cout<<"lifting and driving"<<std::endl;
		}
		else if(state == 2){
			Lifter(0);
			Strafe("left");
			std::cout<<"strafing"<<std::endl;
		}
		else if(state == 3){
			Stop();
			std::cout<<"stop"<<std::endl;
		}
		else if(state == 4){
			DriveStraight(autonSpeed);
			std::cout<<"driving"<<std::endl;
		}
		else if(state == 5){
			Stop();
			std::cout<<"stop"<<std::endl;
			Drop();
			std::cout<<"drop"<<std::endl;
		}

	}
	void LeftThree()
	{
		if(timer < FirstAction){
			state = 1;
		}
		if(timer == FirstAction){
			state = 2;
		}
		if(timer > FirstAction && state == 2){
			state = 3;
		}
		if(state == 3 && turnComplete){
			state = 4;
		}
		if(state == 4 && timer < SecondAction){
			state = 5;
		}
		if(state == 5 && timer > SecondAction && timer < ThirdAction){
			state = 6;
		}
		if(state == 6 && timer == ThirdAction){
			state = 7;
		}
		if(state == 7 && timer > ThirdAction){
			state = 8;
		}
		if(state == 8 && turnComplete){
			state = 9;
		}
		if(state == 9 && timer >= FourthAction && timer < FifthAction){
			state = 10;
		}
		if(state == 10 && timer == FifthAction){
			state = 11;
		}
		if(state == 11 && timer > FifthAction && timer < SixthAction){
			state = 12;
		}
		if (state == 1){
			DriveStraight(autonSpeed);
		}
		else if (state == 2){
			Stop();
		}
		else if (state == 3){
			Turn(-90);
		}
		else if (state == 4){
			Stop();

			timer = FirstAction;

		}
		else if (state == 5){
			turnComplete = false;
			Lifter(1);
		}
		else if (state == 6){
			DriveStraight(autonSpeed);
		}
		else if(state == 7){
			Stop();
		}
		else if(state == 8){
			Turn(-90);
		}

		else if(state == 9){
			Stop();
			timer = FourthAction;
			turnComplete = false;
		}
		else if(state == 10){
			DriveStraight(autonSpeed);
		}
		else if (state == 11){
			Stop();
		}
		else if (state == 12){
			Drop();
		}
	}
	void LeftScale(){
		FirstAction = 80;
		SecondAction = 260;
		ThirdAction = 310;
		FourthAction = 320;

		if(timer < FirstAction){
			state = 1;
			timer = FirstAction;
		}
		if(timer < SecondAction && state == 1 && timer > FirstAction){
			state = 2;
		}
		if(timer == SecondAction && state == 2){
			state = 3;
		}
		if(state == 3){
			state = 4;
		}
		if(state == 4 && turnComplete){
			state = 5;
		}

		if(state == 1){
			Lifter(1);
		}
		else if(state == 2){
			DriveStraight(autonSpeed);
		}
		else if(state == 3){
			Stop();
		}
		else if(state == 4){
			Turn(90);
		}
		else if(state == 5){
			Drop();
		}
	}
	void RightOne()
	{
		if(timer < FirstAction){
					state = 1;

		}
		if(timer == FirstAction){
			state = 2;
		}
		if(timer > FirstAction && state == 2){
			state = 3;
		}
		if(state == 3 && turnComplete){
			state = 4;
		}
		if(state == 4 && timer < SecondAction){
			state = 5;
		}
		if(state == 5 && timer > SecondAction && timer < ThirdAction){
			state = 6;
		}
		if(state == 6 && timer == ThirdAction){
			state = 7;
		}
		if(state == 7 && timer > ThirdAction){
			state = 8;
		}
		if(state == 8 && turnComplete){
			state = 9;
		}
		if(state == 9 && timer >= FourthAction && timer < FifthAction){
			state = 10;
		}
		if(state == 10 && timer == FifthAction){
			state = 11;
		}
		if(state == 11 && timer > FifthAction && timer < SixthAction){
			state = 12;
		}

		if (state == 1){
			DriveStraight(autonSpeed);

		}
		else if (state == 2){
			Stop();
		}
		else if (state == 3){
			Turn(90);
		}
		else if (state == 4){
			Stop();

			timer = FirstAction;

		}
		else if (state == 5){
			turnComplete = false;
			Lifter(1);
		}
		else if (state == 6){
			DriveStraight(autonSpeed);
		}
		else if(state == 7){
			Stop();
		}
		else if(state == 8){
			Turn(90);
		}

		else if(state == 9){
			Stop();
			timer = FourthAction;
			turnComplete = false;
		}
		else if(state == 10){
			DriveStraight(autonSpeed);
		}
		else if (state == 11){
			Stop();
		}
		else if (state == 12){
			Drop();
		}
	}
	void RightTwo()
	{
		FirstAction = 30;
		SecondAction = 180;
		ThirdAction = 250;

		if(timer < FirstAction){
			state = 1;//Lifting and driving from start to FirstAction

		}
		if(timer > FirstAction && timer < SecondAction){
			state = 2;//Strafing right from First to second action
		}
		if(timer == SecondAction) {
			state = 3;//stopping at second action
		}
		if(timer > SecondAction && timer < ThirdAction){
			state = 4;//Driving from second to third action
		}
		if(timer == ThirdAction){
			state = 5;//Stopping and dropping at third action
		}



		if(state == 1){
			Lifter(1);
			DriveStraight(autonSpeed);
			std::cout<<"lifting and driving"<<std::endl;
		}
		else if(state == 2){
			Lifter(0);
			Strafe("right");
			std::cout<<"strafing"<<std::endl;
		}
		else if(state == 3){
			Stop();
			std::cout<<"stop"<<std::endl;
		}
		else if(state == 4){
			DriveStraight(autonSpeed);
			std::cout<<"driving"<<std::endl;
		}
		else if(state == 5){
			Stop();
			std::cout<<"stop"<<std::endl;
			Drop();
			std::cout<<"drop"<<std::endl;
		}

	}
	void RightThree()
	{
		FirstAction = 40;
		SecondAction = 160;
		ThirdAction = 210;
		FourthAction = 230;

		if(timer < FirstAction){
			state = 1; //Lifting, from start to first action
		}
		if(timer < SecondAction && state == 1 && timer > FirstAction){
			state = 2; //Driving, between first and second action
		}
		if(timer == SecondAction && state == 2){
			state = 3;//Stopping at second action
		}
		if(state == 3){
			state = 4; //Turning Left, setting timer to third action when done
		}
		if(state == 4 && turnComplete){
			state = 5; //Driving, between third and fourth action
		}
		if(state == 5 && timer == FourthAction){
			state = 6;// Stopping, at fourth action
		}

		if(state == 1){
			Lifter(1);
		}
		else if(state == 2){
			Lifter(0);
			DriveStraight(autonSpeed);

		}
		else if(state == 3){
			Stop();
		}
		else if(state == 4){
			Turn(-90);
			timer = ThirdAction;
		}
		else if(state == 5){
			DriveStraight(autonSpeed);
		}
		else if(state == 6){
			Stop();
			Drop();
		}
	}
	void RightScale(){
		FirstAction = 80;
		SecondAction = 260;
		ThirdAction = 310;
		FourthAction = 320;

		if(timer < FirstAction){
			state = 1;
			timer = FirstAction;
		}
		if(timer < SecondAction && state == 1 && timer > FirstAction){
			state = 2;
		 }
		if(timer == SecondAction && state == 2){
			state = 3;
		}
		if(state == 3){
			state = 4;
		}
		if(state == 4 && turnComplete){
			state = 5;
		}

		if(state == 1){
			Lifter(1);
		}
		else if(state == 2){
			DriveStraight(autonSpeed);
		}
		else if(state == 3){
			Stop();
		}
		else if(state == 4){
			Turn(-90);
		}
		else if(state == 5){
			Drop();
		}

	}
	void AutonLine(){
		FirstAction = 120;

		if(timer < FirstAction){
			state = 1;
		}
		if(timer == FirstAction){
			state = 2;
		}

		if(state == 1){
			DriveStraight(autonSpeed); //Driving, between start and first action
		}
		if(state == 2){
			Stop();
		}
	}
	void AutonTesting()
	{
		DriveForward(.5, 50);
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

	void TestAll()
	{
		lf -> Set(ControlMode::PercentOutput, 0.5);
		Wait(twoSeconds);
		lf -> Set(ControlMode::PercentOutput, 0);

		rf -> Set(ControlMode::PercentOutput, 0.5);
		Wait(twoSeconds);
		rf -> Set(ControlMode::PercentOutput, 0);

		lr -> Set(ControlMode::PercentOutput, 0.5);
		Wait(twoSeconds);
		lr -> Set(ControlMode::PercentOutput, 0);

		rr -> Set(ControlMode::PercentOutput, 0.5);
		Wait(twoSeconds);
		rr -> Set(ControlMode::PercentOutput, 0);

		cWinch -> Set(ControlMode::PercentOutput, .5);
		Wait(twoSeconds);
		cWinch -> Set(ControlMode::PercentOutput, -.5);
		Wait(twoSeconds);
		cWinch -> Set(ControlMode::PercentOutput, 0);

		sLift -> Set(ControlMode::PercentOutput, -1);
		Wait(twoSeconds);
		sLift -> Set(ControlMode::PercentOutput, .5);
		Wait(twoSeconds);
		sLift -> Set(ControlMode::PercentOutput, 0);

		cExtend -> Set(ControlMode::PercentOutput, .5);
		Wait(twoSeconds);
		cExtend -> Set(ControlMode::PercentOutput, -.5);
		Wait(twoSeconds);
		cExtend -> Set(ControlMode::PercentOutput, 0);

		pWheelL -> Set(ControlMode::PercentOutput, .5);
		Wait(twoSeconds);
		pWheelL -> Set(ControlMode::PercentOutput, 0);
		pWheelR -> Set(ControlMode::PercentOutput, .5);
		Wait(twoSeconds);
		pWheelR -> Set(ControlMode::PercentOutput, 0);

		armSol->Set(true);
		Wait(oneSecond);
		armSol->Set(false);
		Wait(oneSecond);

		clawSol->Set(true);
		Wait(oneSecond);
		clawSol->Set(false);
		Wait(oneSecond);

		birdSol->Set(true);
		Wait(oneSecond);
		birdSol->Set(false);
	}

	void ArcadeDrive(double yAxis, double rot)
	{
		double noMove = 0.2; //Dead area of the axes
		double maxSpeed = .5; //normal speed (not turbo)

		if (fabs(rot) < noMove)
			rot = 0.0;

		if (js1->GetRawButton(turboButton))
			maxSpeed = 1;

		else
			maxSpeed = 0.5;

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
		LiftComplete = false;
		timer = 0;
		robotPos = posChooser.GetSelected();
		encode = encodeChooser.GetSelected();
		gameData = frc::DriverStation::GetInstance().GetGameSpecificMessage();
		//m_autoSelected = m_chooser.GetSelected();
		// m_autoSelected = SmartDashboard::GetString(
		// 		"Auto Selector", kAutoNameDefault);
//		givenPos = robotPos+switchRL;
		//std::cout << "Auto selected: " << m_autoSelected << std::endl;
		gyro->ZeroYaw();
		turnComplete = false;
		state = 1;
		currentAngle = 0;
		//SmartDashboard::PutNumber("Auton Position", autonPosition);
		//SmartDashboard::PutBoolean("Auton Is Blue Alliance?", autonIsBlueAlliance);
		//SmartDashboard::PutBoolean("Auton Is Going to Left Switch?", autonIsLeftSwitch);

		//SmartDashboard::GetNumber("Auton Position", autonPosition);
		//SmartDashboard::GetBoolean("Auton Is Blue Alliance", autonIsBlueAlliance);

		//if (autonIsBlueAlliance == true)
		//{
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
		//}

		//else if(autonIsBlueAlliance == false)
		//{
//			if(/*left switch is red*/)
//			{
//				autonIsLeftSwitch = true;
//			}
		//}

		//SmartDashboard::GetBoolean("Auton Is Going to Left Switch?", autonIsLeftSwitch);

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

		//SmartDashboard::GetNumber("Auton Position", autonPosition);
		//SmartDashboard::GetBoolean("Auton Is Blue Alliance", autonIsBlueAlliance);
		//SmartDashboard::GetBoolean("Auton Is Going to Left Switch?", autonIsLeftSwitch);

		//autonSpeed = SmartDashboard::GetNumber("autonSpeed", autonSpeed);
		FirstAction = SmartDashboard::GetNumber("First Action Space", FirstAction);
		SecondAction = SmartDashboard::GetNumber("Second Action Space", SecondAction);
		ThirdAction = SmartDashboard::GetNumber("Third Action Space", ThirdAction);
		FourthAction = SmartDashboard::GetNumber("Fourth Action Space", FourthAction);
		FifthAction = SmartDashboard::GetNumber("Fifth Action Space", FifthAction);
		SixthAction = SmartDashboard::GetNumber("Sixth Action Space", SixthAction);

		frc::SmartDashboard::PutNumber("distance", distance);

		timer ++;
		LeftDifference = currentAngle*.0125;
		RightDifference = currentAngle*.0125;
		currentAngle = gyro->GetYaw(); //Getting what angle we are at
		SmartDashboard::PutNumber(  "CurrentAngle", currentAngle);
		frc::SmartDashboard::PutNumber("Timer", timer);
		frc::SmartDashboard::PutBoolean("TurnComplete?", turnComplete);
		frc::SmartDashboard::PutNumber("State", state);
		frc::SmartDashboard::PutNumber("Right Diff", RightDifference);
		frc::SmartDashboard::PutString("gameData", gameData);
if(encode){}
else if(!encode){
		if(gameData.length() > 0){
			if(robotPos == 0){
				AutonLine();
			}
			else if(gameData[1] == 'L' && robotPos == 1){
				LeftScale();
			}
			else if(gameData[1] == 'R' && robotPos == 3){
				RightScale();
			}
			else if(gameData[0] == 'L' && robotPos == 1){
				LeftOne();
			}
			else if(gameData[0] == 'R' && robotPos == 3){
				RightThree();
			}
			else if(gameData[0] == 'L' && robotPos == 3){
				LeftThree();
			}
			else if(gameData[0] == 'R' && robotPos == 1){
				RightThree();
			}
			else if(gameData[0] == 'L' && robotPos == 2){
				LeftTwo();
			}
			else if(gameData[0] == 'R' && robotPos == 2){
				RightTwo();
				std::cout << "Center Auton - Right Switch"<<std::endl;
			}
			else{}



		}
	}

}


	void TeleopInit()
	{
		gyro->Reset();
		isBirdOut = false;
		isYDown = false;
	}

	void TeleopPeriodic()
	{

		//teleDrive();

//		double jsY=js1->GetRawAxis(joystickY);
//		double jsRot=js1->GetRawAxis(joystickRot);
//		ArcadeDrive(-jsY,jsRot);

		double jsY=js1->GetRawAxis(joystickY);
		double jsX=js1->GetRawAxis(joystickX);
		double jsRot=js1->GetRawAxis(joystickRot);
		MecDrive(jsX,-jsY,jsRot);

		//teleopGrabToggle();

		teleopArmControl();
		//Drop();
		pickUpWheels();
		teleopSkyLift();
		DeployBirdPole();
		winch();

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
		//if(js1->GetRawButton(1){TestAll();})
		//testWinch();

	}

private:
	frc::LiveWindow& m_lw = *LiveWindow::GetInstance();
	frc::SendableChooser<std::string> m_chooser;
	//const std::string kAutoNameDefault = "Default";
	//const std::string kAutoNameCustom = "My Auto";
	std::string m_autoSelected;
};

START_ROBOT_CLASS(Robot)
