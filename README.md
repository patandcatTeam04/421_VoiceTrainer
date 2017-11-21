# 421_VoiceTrainer
Rice BIOE 421 Final Project


## Brainstormed Ideas, in order that we will choose them as our project:

1. Voice Trainer
	Hardware: Circuit Playground and GUI
	Motivation: improve Patricia's musical knowledge and Catherine's singing abilities; can apply to anyone without these skills!
	Function: Playground plays a note in an octave (CDEFGAB, including sharps and flats). The user must match this tone with their voice. The Playground's Sound Sensor picks up on the microphone input, which corresponds to a voltage. The input into the microphone is converted into the Frequency domain using an FFT (built-in function is CircuitPlayground.mic.fft). This frequency corresponds to a musical note that we code into the code's note database; we will also set thresholds according to the user's experience. For example, if someone is really bad at singing, the Playground will consider an A-flat response to an output of A as acceptable. (i.e. half steps, in musical terminology). When the user can repeat X amount of notes correctly within this threshold, the Playground modifies its frequency thresholds for accuracy to become more precise, until the user can match pitch well. 
2. Painting with Circuit Playground
	Circuit playground LEDs are the color palette. Using capacative touch to choose the color, the user places fingertip on that capacitive sensor; the playground senses this; then, the user directs the playground in space and MS Paint traces this pattern on the color on a monitor
3. What's for Dinner?
	hardware: Raspberry Pi camera
	function: user takes snapshots of different foods they have on-hand to make a meal; the camera processes the image and determines what food it is. The program then searches foodnetwork online using wget for recipes that use those food items
4. Hand-Steering Skateboard
	Using a small motor attached to a 3D-printed skateboard; pressure sensors in the rider's hands direct the direction of the skateboard



## Abstract

### Voice Trainer

### Hardware:
Circuit Playground (CP), color coded legend for notes

### Motivation:
To improve musical knowledge and singing abilities

### Function:
This project exists to improve user’s musical knowledge and singing abilities via an interactive singing feedback device. The CircuitPlayground (CP) will play a note within a the average human vocal range. The user must match this tone with their voice. The CP’s Sound Sensor receives the audio input. The human voice will then be converted from the time domain to the frequency domain through the built-in FFT function. If the person did not match the tone, which typically corresponds to a tone between 20 and 40 Hz plus or minus the note the CP played, the CP repeats the same note. If the user gets it wrong 3 times, the CP moves to a different tone.


### Sophistication:
Next-level prototypes will include the reverse process. Each note corresponds to a color on the CP’s LEDs, and using the manual, the user will see the color and sing the corresponding note without a sound cue. For example, if a G is green according to the color-coded note legend, the user sings a B upon seeing the green LED light up. This sophistication captures the difference between vocal recognition (first scenario) and vocal recall. 
Other next steps include shortening the response time of the CP’s note detection.  This will allow the CP to judge singing accuracy on a timing scale similar to that of songs. 

The voice trainer is built to develop singing skills. Therefore, we can also set thresholds according to the user's experience, and increase the difficulty of those thresholds as the user progresses. For example, at a beginner level, the CP will consider an A-flat response to an output of A as acceptable (i.e. half steps are acceptable). When the user can repeat a certain amount of notes correctly within this level, the CP modifies its frequency thresholds for accuracy to become more precise, until the user can match pitch well.

