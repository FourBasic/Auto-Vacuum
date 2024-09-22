WORK IN PROGRESS

PlatformIO Source Code for the 3D-printed Auto-Vacuum
Arduino Uno R4 Wifi

CAD - https://cad.onshape.com/documents/eea0d15e9e3c86644c471f6e/w/5db2f025dccb5d182f8a3442/e/d2089aca2580d8d13c74baf3?renderMode=0&uiState=66f05397396eef09f7064db2

Attempting to design a battery powered vacuum that can:
1) Build a floor map
   - Navigate its way through an unknown area.
   - Generate a complete map of the area's boundaries using feedback from:
         1) Compass (HMC5883L)
         2) Low resolution encoder mounted to one of the drive wheels
         3) Ultrasonic range finder mounted to a servo
   - Save this map to non-volatile memory.
2) Vacuum the entire area of a built floor map
   - Use a completed map to "plan" vacuuming the area.
   - Use the encoder & compass as the primary means of tracking its rough position on the map.
   - Use the ultrasonic range finder to deduce corrections to its position and perform more precise short term navigation around non-mapped obstacles.

![image](https://github.com/user-attachments/assets/8adf70b6-359c-40b2-999a-280c45062bb5)

![image](https://github.com/user-attachments/assets/8dfb0526-38e5-4259-90d3-f3a3dc445531)

![Screenshot from 2024-09-22 12-56-42](https://github.com/user-attachments/assets/a9852cd0-272e-4199-a86c-699a4be4a330)
