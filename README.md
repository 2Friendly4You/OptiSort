# OptiSort

## Overview
OptiSort is a fully automated, AI-based sorting machine designed to identify and separate objects based on visual characteristics. Initially developed for sorting apples by distinguishing between good and spoiled ones, OptiSort's technology has proven to be universally applicable across various sorting tasks. This project was realized as part of a youth research initiative, showcasing the integration of artificial intelligence, mechanical engineering, and software development to solve real-world problems efficiently.

## Technology Stack
- **AI and Machine Learning:** Utilizes TensorFlow for image processing and classification tasks.
- **Hardware:** Employs standard cameras for optical scanning, a conveyor belt system for object transportation, pneumatic separators, and electronic sliders for physical sorting.
- **Software:** Developed using Python, with Flask for web interface management. The system's coordination is handled by a microcontroller for mechanical operations and an industrial computer for AI processing.

## Features
- **High Accuracy:** Achieves up to 95% accuracy in correctly categorizing objects.
- **User-Friendly Interface:** Equipped with a web interface for easy operation and training of the AI on new sorting tasks.
- **Flexibility:** Designed to be adaptable for various sorting applications beyond its initial focus on apples.

## Setup
### Requirements
- Raspberry Pi or equivalent industrial computer
- Microcontroller (e.g., Arduino, XMC1100)
- Standard cameras (5x) with manual focus
- Conveyor belt with pneumatic separator and electronic slider
- Additional hardware components (LEDs, lasers, photodiodes, relays)

### Installation
1. Clone the repository to your local machine or industrial computer.
2. Install the required Python libraries including Flask and TensorFlow.
3. Assemble the hardware components according to the provided schematics.
4. Upload the microcontroller firmware for managing the mechanical parts.

## Usage
1. Start the Flask server on the industrial computer to launch the web interface.
2. Place objects on the conveyor belt to begin the sorting process.
3. Monitor the sorting process and results via the web interface.
4. Train the AI on new objects by uploading images through the web interface.

## Contributing
Contributions to OptiSort are welcome! Please follow the standard fork-and-pull request workflow. Make sure your code adheres to the existing style and all tests pass before submitting a pull request.

## Acknowledgments
- Mikro Makro Mint Foundation, Baden-Württemberg for financial support.
- Trumpf Ditzingen for manufacturing support.
- Our mentors and teachers for their guidance and support throughout the project development.
