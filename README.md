# robotic-arm
Project proposal to bring new students into the world of robotics. 

# Schematics
Hardware schematics for power supply cable of supplying up to three robots. 

![hardware schematics](RobotArm_v3_schematics.png?raw=true)

## Schematics Notes
- The 5VEN (5 Volt Enable) on the L298N2 motor driver jumper (represented by green wire) needs to be populated so that the 12V motor power will be used for the 5V logic.
- Arduino power to be supplied by labtop USB. IT IS NOT RECOMMENDED TO HAVE LAPTOP CHARGING WHILE PLUGGED INTO ARDUINO. This will create a ground loop with the power supply which may affect the performance.

# Robot Arm
![hardware schematics](robotarm_v1_assembled.png?raw=true)
