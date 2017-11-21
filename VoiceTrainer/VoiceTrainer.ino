// Voice Trainer
// The purpose of this program is to train the human voice to match and recall pitch.
// The CircuitPlayground uses its speakers to play a musical note, then records the user's voice, performs an fft and determines if the notes match.
// As of 11/09 this code gives user feedback after 2s of recording the voice (not in real time), and outputs feedback only in Serial
// Next steps include: incorporating more real time feedback as the user is singing, fixing a bug with -16,000 Hz sounds randomly playing, and attempting to increase resolution


#include <Adafruit_CircuitPlayground.h>

// Define variables to keep track of rounds and sequence
int seqNumber; //keeps track of which round youre in the game
int randNumber; // this is the random number to generate a random note
bool slideSwitch; // this is a boolean to use the slide switch to indicate whether the user would want recognition or recall mode

// Define variables to keep track of time to accept user input
int maxtime = 2000; // the game will only wait 3 seconds on the first round; This will be increased with ea. advancing round to account for longer sequences

// Define a variable to track errors
boolean noerror = 1; // this is a round by round error tracker. This will let you advance even if you have gotten an error before
int ecount = 0; // this is an overall error counter

// define constants needed for FFT
#define BINS  32          // The number of FFT frequency bins
#define FRAMES 4           // This many FFT cycles are averaged 

// Define frequency matrix that stores each frequency that will be played by the CP as well as the row index of the corresponding LED found in the note2LED matrix
//double freq[] = {293.66, 329.63, 349.23, 392, 440, 493.88, 523.25, 587.33, 659.25, 698.96, 783.99, 880, 987.77, 1046.5, 1174.66, 1318.51, 1396.91, 1567.98, 1760, 1975.53, 2093, 2349.32, 2637.02, 2793.83, 3135.96};
//int freq[] = {293, 329, 349, 392, 440, 493, 523, 587, 659, 698, 783, 880, 987, 1046, 1174, 1318, 1396, 1567, 1760, 1975, 2093, 2349, 2637, 2793, 3135};
int freq[13][2] = {
  {293, 0}, //D4
  {329, 1}, //E4
  {349, 2},  //F4
  {392, 3},  //G4
  {440, 4},  //A4
  {493, 5},  //B4
  {523, 6},  //C5
  {587, 7},  //D5
  {659, 8},  //E5
  {698, 9},  //F5
  {783, 10},  //G5
  {880, 11},  //A5
  {987, 12},  //B5
};

// maps LED numbers and their RGB values to notes; this matrix is in order of increasing frequency
int note2LED[13][4] = {
  {9, 30, 30, 30}, // LED#: 9, Color: White, note: D4
  {8, 75, 0, 130}, // LED#: 8, Color: Violet, note: E4
  {7, 25, 25, 112}, // LED#: 7, Color: Blue light, note: F4
  {5, 60, 185, 113}, // LED#: 5, Color: Green light, note: G4
  {3, 224, 102, 0}, // LED#: 3, Color: Orange light, note: A4
  {2, 255, 99, 80}, // LED#: 2, Color: Maroon, note: B4
  {0, 255, 0, 255}, // LED#: 0, Color: Pink, note: C5
  {9, 255, 255, 255}, // LED#: 9, Color: White, note: D5
  {8, 200, 0, 200}, // LED#: 8, Color: Violet, note: E5
  {7, 0, 0, 255}, // LED#: 7, Color: Blue, note: F5
  {5, 0, 255, 0 }, // LED#: 5, Color: Green, note: G5
  {3, 255, 69, 0}, // LED#: 3, Color: Orange strong, note: A5
  {2, 255, 0, 0}, // LED#: 2, Color: Red, note: B5
};


//Map bins to frequencies of the human vocal range as well as the note2LED rox indices corresponding to notes encompassed in the bin
int bin2freq[12][4] = {
  {2, 0, 0, 2}, // D4 - F4 (3 notes)
  {3, 390, 3, 6}, // G4 - C5 (6 notes)
  {4, 530, 7, 8}, // C#5 - E5 (4 notes)
  {5, 670, 9, 10}, // F5 - G5 (3 notes)
  {6, 820, 11, 11}, // G#5 - A#5 (3 notes)
  {7, 970, 12, 12}, // B5 - C#6 (3 notes)
  {8, 1150, 12, 12}, // D6 - D#6 (2 notes)
  {9, 1260, 12, 12}, // E6 - F6 (2 notes)
  {10, 1410, 12, 12}, // F#6 - G6 (2 notes)
  {11, 1580, 12, 12}, // G#6 ( 1 note)
  {12, 1720, 12, 12}, // A6 - A#6 (2 notes)
  {13, 1900, 12, 12}, // anything higher than B6 (? notes)
};



void setup() {
  Serial.begin(9600); //setup serial monitor
  CircuitPlayground.begin(); // set up CircuitPlayground
  randomSeed(analogRead(0)); //initializes the random number generator

  seqNumber = 0; //initialize the round index (0 = 1st round bc of matrix indexing)
}

//ENTER THE TRAINER!!!!
void loop() {

  uint8_t i, j;
  uint16_t spectrum[BINS];     // FFT spectrum output buffer
  uint16_t avg[BINS];          // The average of FRAME "listens"

  slideSwitch = CircuitPlayground.slideSwitch(); //read the switch position; This allows user to indicate whether they would like Recognition or Recall mode


  // FIRST, generate a random number. This will be your new musical note
  // however only generate a new note, if it is a new round (i.e. you have zero errors OR you messed up 3 times and are trying out a new note)
  if (ecount == 0) {
    randNumber = random(0, 12); //chooses a random number between 0 and 12; This will correspond to the frequency played by the CP
  }
  delay(2000);

  // for testing purposes: print the random number and its corresponding frequency value
  Serial.print("RandNumber =  ");
  Serial.println(randNumber);
  Serial.print("Frequency =  ");
  Serial.println(freq[randNumber][0]);



  // SECOND, play OR show the note depending on the mode (recognition will play and show; recall will only show)



  //if slideSwitch is positive, play the tone! this is recognition mode
  if (slideSwitch) {
    // to call each variable needed for pixel color setting your main source of info is the note2LED mapping matrix; the row within the note2LED matrix depends on the frequency played
    CircuitPlayground.setPixelColor(note2LED[freq[randNumber][1]][0], note2LED[freq[randNumber][1]][1], note2LED[freq[randNumber][1]][2], note2LED[freq[randNumber][1]][3] );
    CircuitPlayground.playTone(freq[randNumber][0], 2000);
  }
  else {
    // to call each variable needed for pixel color setting your main source of info is the note2LED mapping matrix; the row within the note2LED matrix depends on the frequency played
    CircuitPlayground.setPixelColor(note2LED[freq[randNumber][1]][0], note2LED[freq[randNumber][1]][1], note2LED[freq[randNumber][1]][2], note2LED[freq[randNumber][1]][3] );
  }

  delay(500);
  CircuitPlayground.clearPixels();



  // THIRD, within a set amount of time, wait for user feedback
  // for each round we want to start fresh. Initialize both variables with the current time
  unsigned long previousMillis = millis();
  unsigned long currentMillis = millis();

  noerror = 1; //reset the boolean to TRUE! You start "errorless"
  int whilecnt = 0;
  int maxIndex_avg = 0;

  //while the time elapsed since the tones stop playing is less than the maxtime, accept user input!
  while ((unsigned long)(currentMillis - previousMillis) < maxtime) {

    // turn on all LEDs so user knows when their voice is being recorded
    CircuitPlayground.setPixelColor(0, 255, 255, 255);
    CircuitPlayground.setPixelColor(1, 255, 255, 255);
    CircuitPlayground.setPixelColor(2, 255, 255, 255);
    CircuitPlayground.setPixelColor(3, 255, 255, 255);
    CircuitPlayground.setPixelColor(4, 255, 255, 255);
    CircuitPlayground.setPixelColor(5, 255, 255, 255);
    CircuitPlayground.setPixelColor(6, 255, 255, 255);
    CircuitPlayground.setPixelColor(7, 255, 255, 255);
    CircuitPlayground.setPixelColor(8, 255, 255, 255);
    CircuitPlayground.setPixelColor(9, 255, 255, 255);
    CircuitPlayground.setPixelColor(10, 255, 255, 255);


    // add to whileloop count
    whilecnt = whilecnt + 1;

    // perform FFT
    for (j = 1; j <= FRAMES; j++) {          // We gather data FRAMES times and average it
      CircuitPlayground.mic.fft(spectrum);  // Here is the CP listen and FFT the data routine
      for (i = 0; i < BINS; i++) {          // Add for an average
        if (spectrum[i] > 255) spectrum[i] = 255; // limit outlier data
        if (i == 0)
          avg[i] = spectrum[i];
        else
          avg[i] = avg[i] + spectrum[i];
      }
    }

    // Complete calculation of averages for FFT spectra
    for (i = 0; i < BINS; i++) {            // For each output bin average
      avg[i] = avg[i] / FRAMES;             //  divide about the number of values aaveraged
    }

    // determine dominant frequency
    int maxVal = 0, maxIndex = 0;
    for (i = 0; i < BINS; i++) {            // For each output bin average
      if (avg[i] >= maxVal) {              //  find the peak value
        maxVal = avg[i];
        maxIndex = i;                      //  and the bin that max value is in
      }
    }


    // compile sum for average (average is taken outside the loop... a few lines down)
    maxIndex_avg = maxIndex_avg + maxIndex;
    // take a NEW snapshot of what time is it so you can run a comparison to the time you started the WHILE loop
    currentMillis = millis();
  } // end of the fft while loop



  // turns off LED so user knows CP is done recording voice
  CircuitPlayground.clearPixels();

  // this actually calculates the average index value and therefore the average frequency for the sampling period
  maxIndex_avg = maxIndex_avg / whilecnt;
  Serial.print("You voice's bin =  ");
  Serial.println(maxIndex_avg);

  // converting frequency played by CP to a bin number
  i = 0;

  while (freq[randNumber][0] > bin2freq[i][1]) {
    i = i + 1;
  }
  int cp_bin = bin2freq[i - 1][0];
  Serial.print("The Cp tone's bin: ");
  Serial.println(bin2freq[i - 1][0]);



  // NEXT, give visual feedback on how your voice compared to the tone played by the CP

  if (maxIndex_avg < cp_bin) { //if you sang too low, you need to sing higher

    // set starting pixel as LED that corresponds to the note that corresponds to the bin you sang (use mapping matrices!)
    int pixel1 = note2LED[ bin2freq[maxIndex_avg - 2][2] ][0];
    int pixel2 = pixel1 - 1;

    //set color to that corresponding to the target note
    int color [1][3] = {
      {note2LED[freq[randNumber][1]][1], note2LED[freq[randNumber][1]][2], note2LED[freq[randNumber][1]][3]}
    };

    // calculate steps to get to target note (note the cp played); the "steps" corresponds to the number of LEDs that need to light up
    int diff_LED = pixel1 - note2LED[ freq[randNumber][1] ][0];

    if (diff_LED < 0) {
      diff_LED = 10 + diff_LED;
    }
    Serial.println("pixel1 = ");
    Serial.println(pixel1);
    Serial.println("diff_LED = ");
    Serial.println(diff_LED);

    // create a loop to display the LEDs in clockwise fashion from your lowest note to the target note
    for (i = 0; i < diff_LED; i++) {

      // Turn off all the NeoPixels
      CircuitPlayground.clearPixels();

      // Turn on two pixels to COLOR
      CircuitPlayground.setPixelColor(pixel1, color[0][0], color[0][1], color[0][2]);
      CircuitPlayground.setPixelColor(pixel2, color[0][0], color[0][1], color[0][2]);

      // Increment pixels to move them around the board
      pixel1 = pixel1 - 1;
      pixel2 = pixel2 - 1;

      // Check pixel values
      if (pixel1 < 0) pixel1 = 9;
      if (pixel2 < 0) pixel2 = 9;

      delay(400);
    }
    CircuitPlayground.clearPixels();
    CircuitPlayground.setPixelColor(pixel1, color[0][0], color[0][1], color[0][2]);

  }


  if (maxIndex_avg > cp_bin) { //if you sang too high, you need to sing lower

    // set starting pixel as LED that corresponds to the note that corresponds to the bin you sang (use mapping matrices!)
    int pixel1 = note2LED[ bin2freq[maxIndex_avg - 2][2] ][0];
    int pixel2 = pixel1 + 1; // set pixel two as one LED in the direction you will travel (counterclockwise, increasing LED #)

    //set color to that corresponding to the target note
    int color [1][3] = {
      {note2LED[freq[randNumber][1]][1], note2LED[freq[randNumber][1]][2], note2LED[freq[randNumber][1]][3]}
    };

    // calculate steps to get to target note (note the cp played); the "steps" corresponds to the number of LEDs that need to light up
    int diff_LED = note2LED[ freq[randNumber][1] ][0] - pixel1;

    if (diff_LED < 0) {
      diff_LED = 10 + diff_LED;
    }
    Serial.println("pixel1 = ");
    Serial.println(pixel1);
    Serial.println("diff_LED = ");
    Serial.println(diff_LED);

    // create a loop to display the LEDs in counterclockwise fashion from your lowest note to the target note
    for (i = 0; i < diff_LED; i++) {

      // Turn off all the NeoPixels
      CircuitPlayground.clearPixels();

      // Turn on two pixels to COLOR
      CircuitPlayground.setPixelColor(pixel1, color[0][0], color[0][1], color[0][2]);
      CircuitPlayground.setPixelColor(pixel2, color[0][0], color[0][1], color[0][2]);

      // Increment pixels to move them around the board
      pixel1 = pixel1 + 1;
      pixel2 = pixel2 + 1;

      // Check pixel values
      if (pixel1 > 9) pixel1 = 0;
      if (pixel2 > 9) pixel2 = 0;

      delay(400);
    }
    CircuitPlayground.clearPixels();
    CircuitPlayground.setPixelColor(pixel1, color[0][0], color[0][1], color[0][2]);

  }




  //determine if what you sang is within the frequency range played by the CP
  if (maxIndex_avg != bin2freq[i - 1][0]) {
    ecount = ecount + 1;
    noerror = 0;
    Serial.println("you bad");
    Serial.println(" ");
  }


  // LASTLY, advance the round index if you have no errors
  if (noerror == 1) {
    ecount = 0;
    maxtime = maxtime + 650; // give the user a bit more time for each new round so they can press more buttons
    Serial.println("You're a star!");
    Serial.println(" ");

    //reset the game!
    noerror = 1;
    ecount = 0;
  }


  // once you get 3 errors, RESET
  if (ecount >= 3) {
    ecount = 0;
    noerror = 1;
    maxtime = 3000;
  }


}

