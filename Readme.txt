								---Braille Bot---

Problem Statement:
Limitations in Access to Literacy for Visually Impaired Individuals.
Specific Pain Points:
1. Lack of access to information: Visually impaired individuals face difficulties in accessing written materials, including books, articles, 				  and online content.
2. Limited employment opportunities: The inability to read and write can limit job prospects and career advancement opportunities.
3. Social isolation: Limited access to written communication and social interactions can lead to feelings of loneliness and disconnection.

__________________________________________________________________________________________________________________________________________

Our Objective:
To design and develop a mechanical bot that can efficiently and effectively assist visually impaired individuals with reading tasks, thereby improving access to literacy and promoting greater independence and inclusion.

__________________________________________________________________________________________________________________________________________

Our Solution:
A robotic bot that captures text using ESP32-CAM, processes images with Optical Character Recognition, and converts to Braille using machine learning algorithms. The bot then simulates raised dots using SG90 Servo Motors, providing tactile feedback for visually impaired individuals, enabling independent reading experience.

__________________________________________________________________________________________________________________________________________

HOW  DOES  IT  WORK ?
1. Text Capture (ESP32 CAM)
	* Component: ESP32-CAM module
	* Function: Reads text from physical sources such as signs, labels, or documents.
	* Workflow:
		+ The camera module captures frames of the input material.
		+ The ESP32 CAM process image processing to enhance and optimize image quality.
		+ Image is then sent to Optical Recognition Technology for extraction.

2. Image Processing (Optical Recognition)
	* Component: Computer Vision Algorithm (Optical Character Recognition, OCR)
	* Function: Processes captured images to extract text data.
	* Workflow:
		+ The processed image is fed into the computer vision algorithm.
		+ Algorithm uses machine learning models to identify and recognize characters in the image.
		+ Extracted text is converted into a string.

3. Text Conversion (Machine Learning-based Model)
	* Component: Text-to-Braille Conversion Model
	* Function: Converts extracted text into Braille format using machine learning algorithms.
	* Workflow:
		+ The extracted text is fed into the text-to-Braille conversion model.
		+ Model uses pattern recognition and synthesis to generate Braille characters from the input text.

4. Braille Generation (Servo Motor)
	* Component: SG90 Servo Motor
	* Function: Controls movement of servos connected to physical pins for tactile feedback.
	* Workflow:
		+ The converted Braille text is sent to the servo motor.
		+ Servo motor moves to simulate raised dots, enabling tactile feedback.

5. Servo Motor Control (Motor Driver)
	* Component: Motor Driver
	* Function: Regulates and synchronizes movement of servo motors.
	* Workflow:
		+ The motor driver receives signals from the ESP32 CAM to control Braille character sequence.
		+ Motor driver sends signals to the SG90 Servo Motor to execute movement.

6. Power Supply (Li-Po Battery)
	* Component: 3S Li-Po Battery
	* Function: Provides power supply to the entire system for continuous operation.
	* Workflow:
		+ Power is drawn from the Li-Po battery through the power management module.
		+ Power management ensures efficient use of battery life.

7. Physical Display (3D Printed Body)
	* Component: 3D Printed Body
	* Function: Houses and physically interacts with servos to convey Braille output.
	* Workflow:
		+ The physical body receives control signals from the servo motors.
		+ Servo movements trigger tactile feedback, allowing users to recognize written content.


