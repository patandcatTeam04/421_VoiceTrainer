# 421_VoiceTrainer
Rice BIOE 421 Final Project


Brainstormed Ideas, in order that we will choose them as our project:

1. Voice Trainer
	Hardware: Circuit Playground and GUI
	Motivation: improve Patricia's musical knowledge and Catherine's singing abilities; can apply to anyone without these skills!
	Function: Playground plays a note in an octave (CDEFGAB, including sharps and flats). The user must match this tone with their voice. The Playground's Sound Sensor picks up on the microphone input, which corresponds to a voltage. The input into the microphone is converted into the Frequency domain using an FFT (built-in function is CircuitPlayground.mic.fft). This frequency corresponds to a musical note that we code into the code's note database; we will also set thresholds according to the user's experience. For example, if someone is really bad at singing, the Playground will consider an A-flat response to an output of A as acceptable. (i.e. half steps, in musical terminology). When the user can repeat X amount of notes correctly within this threshold, the Playground modifies its frequency thresholds for accuracy to become more precise, until the user can match pitch well. Next-level prototypes will include the reverse process: Each note corresponds to a color on the Playground's LEDs, and using the manual, they will see "Oh, that's green, so I must sing a B flat." It is the difference between vocal recognition (first scenario) and vocal recall. Other next steps include having the response time of the Playground detecting notes in cutoffs being shorter. E.g. to begin with, we will prove the concept by having the Playground play a note and process the user's vocal response into the frequency domain. This will occur over the course of several seconds, with pauses in between input and output. To judge accuracy on the order of magnityude of songs, however, it is necessary to adjust the detection rate to be milliseconds.
2. Painting with Circuit Playground
	Circuit playground LEDs are the color palette. Using capacative touch to choose the color, the user places fingertip on that capacitive sensor; the playground senses this; then, the user directs the playground in space and MS Paint traces this pattern on the color on a monitor
3. What's for Dinner?
	hardware: Raspberry Pi camera
	function: user takes snapshots of different foods they have on-hand to make a meal; the camera processes the image and determines what food it is. The program then searches foodnetwork online using wget for recipes that use those food items
4. Hand-Steering Skateboard
	Using a small motor attached to a 3D-printed skateboard; pressure sensors in the rider's hands direct the direction of the skateboard



Abstract
