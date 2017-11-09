
// Voice Trainer 
// The purpose of this program is to train the human voice to match and recall pitch.
// The CircuitPlayground uses its speakers to play a musical note, then records the user's voice, performs an fft and determines if the notes match.
// As of 11/09 this code gives user feedback after 2s of recording the voice (not in real time), and outputs feedback only in Serial
// Next steps include: incorporating more real time feedback as the user is singing, fixing a bug with -16,000 Hz sounds randomly playing, and attempting to increase resolution


#include <Adafruit_CircuitPlayground.h>

// Define variables to keep track of rounds and sequence
int seqNumber; //keeps track of which round youre in the game
int randNumber;

// Define variables to keep track of time to accept user input
int maxtime = 2000; // the game will only wait 3 seconds on the first round; This will be increased with ea. advancing round to account for longer sequences

// Define a variable to track errors
boolean noerror = 1; // this is a round by round error tracker. This will let you advance even if you have gotten an error before
int ecount = 0; // this is an overall error counter

// define constants needed for FFT
#define BINS  32          // The number of FFT frequency bins
#define FRAMES 4           // This many FFT cycles are averaged 

// Define frequency vector that stores each note as a frequency
//double freq[] = {293.66, 329.63, 349.23, 392, 440, 493.88, 523.25, 587.33, 659.25, 698.96, 783.99, 880, 987.77, 1046.5, 1174.66, 1318.51, 1396.91, 1567.98, 1760, 1975.53, 2093, 2349.32, 2637.02, 2793.83, 3135.96};
//int freq[] = {293, 329, 349, 392, 440, 493, 523, 587, 659, 698, 783, 880, 987, 1046, 1174, 1318, 1396, 1567, 1760, 1975, 2093, 2349, 2637, 2793, 3135};
int freq[] = {293, 329, 349, 392, 440, 493, 523, 587, 659, 698, 783, 880, 987};

//Map bins to frequencies of the human vocal range

/*
  bin2[] = {0,200}
  bin3[] = {200,400};
  bin4[] = {400,600};
  bin5[] = {600,700};
  bin7[] = {700,1000};
  bin10[]= {1000,1500};
  bin13[] = {1500,2000};
  bin20[] = {2000,3000};
*/

int bin2freq[12][2] = {
  {2, 0},
  {3, 390},
  {4, 530},
  {5, 670},
  {6, 820},
  {7, 970}, //iffy
  {8, 1150},
  {9, 1260},
  {10, 1410},
  {11, 1580},
  {12, 1720},
  {13, 1900},
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


  // FIRST, generate a random number. This will be your new musical note
  // however only generate a new random number in the sequence, if it is a new round (i.e. you have zero errors)
  if (ecount == 0) {
    randNumber = random(1, 14); //chooses a random number between 1 and 4 and append that value to randNumber vector (our memory)
  }
  delay(2000);

  Serial.print("RandNumber =  ");
  Serial.println(randNumber);
  Serial.print("Frequency =  ");
  Serial.println(freq[randNumber]);



  // SECOND, play the note
  CircuitPlayground.setPixelColor(0, 255, 0, 0);
  CircuitPlayground.playTone(freq[randNumber], 2000);
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
      avg[i] = avg[i] / FRAMES;            //  divide about the number of values aaveraged
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
  
  while (freq[randNumber] > bin2freq[i][1]) {
    i = i + 1;
    //Serial.print("our i =  ");
    //Serial.println(i);
  }
  Serial.print("The Cp tone's bin: ");
  Serial.println(bin2freq[i-1][0]);
  
  //determine if what you sang is within the frequency range played by the CP
  if (maxIndex_avg != bin2freq[i-1][0]) {
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
