/**
 * 
 * 
 * MAIN DRIVE TRAIN CODE 
 * 
 * 
**/

#include "DriveTrain.h"

uint8_t *pixy2In;
int timed = 0;

// This is called whenever the robot turns on
void Robot::RobotInit() 
{ 
  m_compressor.Start();
  m_compressor.SetClosedLoopControl(false);
  pixy2.init();
  pixy2.setLamp(0,0);
  pixy2.setLED(0,0,0);

  imu.Calibrate();
}

// This is called every robot packet, regardless of state of the robot
void Robot::RobotPeriodic(){}
// This is called whenever the robot is disabled, and is run once
void Robot::DisabledInit(){}
void Robot::DisabledPeriodic(){}
void Robot::AutonomousInit(){}
void Robot::AutonomousPeriodic(){}
void Robot::TeleopInit(){}

//main loop
void Robot::TeleopPeriodic() 
{
  // Get button values
  m_XButton = m_stick.GetRawButton(kJoystickButtonX);
  m_AButton = m_stick.GetRawButton(kJoystickButtonA);
  m_BButton = m_stick.GetRawButton(kJoystickButtonB);
  m_YButton = m_stick.GetRawButton(kJoystickButtonY);
  m_LBButton = m_stick.GetRawButton(kJoystickButtonLB);
  m_RBButton = m_stick.GetRawButton(kJoystickButtonRB);
  m_LTButton = m_stick.GetRawButton(kJoystickButtonLT);
  m_RTButton = m_stick.GetRawButton(kJoystickButtonRT);
  m_BackButton = m_stick.GetRawButton(kJoystickButtonBack);
  m_StartButton = m_stick.GetRawButton(kJoystickButtonStart);
  m_LStickButton = m_stick.GetRawButton(kJoystickButtonLStick);
  m_RStickButton = m_stick.GetRawButton(kJoystickButtonRStick);
  m_altEnableA = m_alt.GetRawButton(kAltJoystickEnableA);
  m_altEnableB = m_alt.GetRawButton(kAltJoystickEnableB);

  /*
    --------------------------
    MAIN CONTROllER SUBSYSTEMS
    --------------------------
    BACK DOOR SOLENOIDS, BELT MOTOR, AND COMPRESSOR
 */
  //Back Door Control
  backState = 0;
  if(m_AButton == true && m_BButton == false)
  {
    backState = 1;
    m_backDoorDown.Set(1);
  }
  if(m_BButton == true && m_AButton == false)
  {
    backState = 1;
    m_backDoorUp.Set(1);
  }
  if(backState == 0)
  {
    m_backDoorDown.Set(0);
    m_backDoorUp.Set(0);
  }
  //Belt Control Code (on main controller) 
  //button state more like button florida xd
  int florida = 0;
  if(m_LTButton == true){florida = 1;}
  if(m_RTButton == true){florida = -1;}
  //belt speed is a constant set in the DriveTrain.h file
  m_belt.Set(belt_speed * florida);

  //Compressor Control 
  cReading = m_XButton;
  if(cReading == true && cPrevious == false)
   {
    if(cState == true)
    {
      m_compressor.Stop();
      cState = false;
    }
    else
    {
      m_compressor.Start();
      cState = true;
    }
  }
  cPrevious = cReading;
 




  /* 
    ------------------
    ALT STICK CONTROLS
    ------------------
  */
  //Sets mode of operation on the alternate stick
  //(Two control planes)
  enableState = 0; 
  if(m_altEnableB == false && m_altEnableA == false){enableState = 0;}
  if(m_altEnableA == true && m_altEnableB == false){enableState = 1;}
  if(m_altEnableB == true && m_altEnableA == false){enableState = -1;}
  if(m_altEnableB == true && m_altEnableA == true){enableState = 0;}
 
 /*
    ENABLE STATE = 1
    PISTON SOLENOIDS AND WINCH MOTOR
 */
  //Solenoid Control
  armState = 0;  
  if(m_alt.GetY() > 0.75 && enableState == 1)
  {
    armState = 1;
    m_mainArmsDown.Set(1);
  }
  if(m_alt.GetY() < -0.75 && enableState == 1)
  {
    armState = 1;
    m_mainArmsUp.Set(1);
  }
  if(armState == 0)
  {
    m_mainArmsUp.Set(0);
    m_mainArmsDown.Set(0);
  }
  
  //Winch Control
  winchState = 0;
  if(m_alt.GetX() > 0.25 && enableState == 1)
  {
    winchState = 1;
    m_winch.Set(1.33*(m_alt.GetX() - 0.25));
  }
  if(m_alt.GetX() < -0.25 && enableState == 1)
  {
    winchState = 1;
    m_winch.Set(1.33*(m_alt.GetX() + 0.25));
  }
  if(winchState == 0)
  {
    m_winch.Set(0);
  }



 /*
    ENABLE STATE = -1
    CONTROL ARM PISTON SOLENOIDS AND SPINNER MOTOR
 */
  //Control Arm Solenoid
  controlState = 0;  
  if(m_alt.GetY() > 0.75 && enableState == -1)
  {
    controlState = 1;
    m_controlArmDown.Set(1);
  }
  if(m_alt.GetY() < -0.75 && enableState == -1)
  {
    controlState = 1;
    m_controlArmUp.Set(1);
  }
  if(controlState == 0)
  {
    m_controlArmUp.Set(0);
    m_controlArmDown.Set(0);
  }

  //Control Arm Spinner
  spinnerState = 0;
  if(m_alt.GetX() > 0.25 && enableState == -1)
  {
    spinnerState = 1;
    m_controlArmSpinner.Set(1.33*(m_alt.GetX() - 0.25));
  }
  if(m_alt.GetX() < -0.25 && enableState == -1)
  {
    spinnerState = 1;
    m_controlArmSpinner.Set(1.33*(m_alt.GetX() + 0.25));
  }
  if(spinnerState == 0)
  {
    m_controlArmSpinner.Set(0);
  }


  
  /*
    DRIVING
    PIXY CAM PROCESSING, IMU PROCESSING, AND DRIVE TRAIN MOVEMENT
  */

  // Output the position of one iteration of the first
  // signature to the driver station
  if (timed == 10)
  {
    //pixy2.getBallInfo(1);
    timed = 0;
  } else {
    timed += 1;
  }

  // Mecanum Drive
  if ((m_BackButton_old == 0) && (m_BackButton == 1))
  {
    driveState = !driveState;
    if (driveState == false)
    {
      std::cout << "Switching to robot-orinted drive!" << std::endl;
    }
    else
    {
      std::cout << "Switching to field-orinted drive!" << std::endl;
    }
  }

  if (driveState == false)
  {
    m_robotDrive.DriveCartesian(0.5*m_stick.GetX(), -0.5*m_stick.GetY(), m_stick.GetZ());
  }
  else
  {
    m_robotDrive.DriveCartesian(0.5*m_stick.GetX(), -0.5*m_stick.GetY(), m_stick.GetZ(), imu.GetGyroInstantZ());
  }

  // Ball tracking
  if ((m_StartButton_old == 0) && (m_StartButton == 1))
  {
    trackState = !trackState;
    if (trackState == false)
    {
      std::cout << "Starting ball tracking!" << std::endl;
      TrackBall();
    }
    else
    {
      std::cout << "Stopping ball tracking!" << std::endl;
    }
  }
  
  m_BackButton_old = m_BackButton;
  m_StartButton_old = m_StartButton;

}

void Robot::TestPeriodic(){}

void Robot::TrackBall()
{
  int ballPos = 0;
  int ballPos_old = 0;
  uint16_t *pixy2Data;
  pixy2Data = pixy2.getBallData(1);
  ballPos = pixy2Data[1] - pixyMid;

  // If a ball is present and outside of the threshhold
  std::cout << "Ball: " << ballPos << std::endl;
  if ((ballPos != noBall) && (std::abs(ballPos) > ballOffset))
  {
    int trackTimer = 0;
    // If there is a ball, align the robot so that the ball is in the middle
    while (std::abs(ballPos) > ballOffset)
    {
      if ((ballPos < 0) && ballPos != noBall)
      {
        // Ball is to the left of center
        // Move robot to the left
        //std::cout << "Speed: " << ((0.9 * (ballPos/pixyMid)) - 0.1) << std::endl;
        m_robotDrive.DriveCartesian((0.5 * ((double)ballPos/(double)pixyMid)) - 0.1, 0, 0);
      }
      else if ((ballPos > 0) && ballPos != noBall)
      {
        // Ball is to the right of center
        // Move robot to the right
        //std::cout << "Speed: " << ((0.9 * (ballPos/pixyMid)) + 0.1) << std::endl;
        m_robotDrive.DriveCartesian((0.5 * ((double)ballPos/(double)pixyMid)) + 0.1, 0, 0);
      }
      trackTimer += 1;
      if (trackTimer == 100)
      {
        pixy2Data = pixy2.getBallData(1);
        ballPos_old = ballPos;
        ballPos = pixy2Data[1] - pixyMid;
        if (ballPos == noBall)
        {
          ballPos = ballPos_old;
        }
        std::cout << "Ball: " << ballPos << std::endl;
        trackTimer = 0;
      }
    }
    trackState = true;
    std::cout << "Robot lined up to ball!" << std::endl;
  }
  else
  {
    trackState = true;
    std::cout << "No ball to track!" << std::endl;
  }
  
}

#ifndef RUNNING_FRC_TESTS
int main() { return frc::StartRobot<Robot>(); }
#endif