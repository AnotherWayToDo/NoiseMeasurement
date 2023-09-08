/*
  This example reads audio data from the on-board PDM microphones, and prints
  out the samples to the Serial console. The Serial Plotter built into the
  Arduino IDE can be used to plot the audio data (Tools -> Serial Plotter)

  Circuit:
  - Arduino Nano 33 BLE Sense board

See : https://docs.arduino.cc/learn/built-in-libraries/pdm
*/

#include <PDM.h>
#include <math.h>

// buffer to read samples into, each sample is 16-bits
short sampleBuffer[256];
char buffer[50];

// number of samples read
volatile int samplesRead;
volatile int minVal, maxVal, avgVal, dbValue;
long sumVal=0;

unsigned int level1 = 30;
unsigned int level2 = 40;
unsigned int level3 = 50;
unsigned int level4 = 60;


// Board initialization
void setup()
{
  Serial.begin(9600);
  while (!Serial);

  // configure the data receive callback
  PDM.onReceive(onPDMdata);

  // optionally set the gain, defaults to 20
  // PDM.setGain(30);

  // initialize PDM with:
  // - one channel (mono mode)
  // - a 16 kHz sample rate
  if (!PDM.begin(1, 16000)) 
  {
    Serial.println("Failed to start PDM!");
    while (1);
  }

}


// Main loop
void loop() 
{
  // wait for samples to be read
  if (samplesRead) 
  {
    
    minVal = sampleBuffer[0];
    maxVal = sampleBuffer[0];
    sumVal = 0;

    for (int i=0; i<samplesRead; i++)
    {
      minVal = min(sampleBuffer[i],minVal);
      maxVal = max(sampleBuffer[i],maxVal);
      sumVal += sampleBuffer[i];
    }
    
    avgVal = sumVal/samplesRead;


    dbValue = max(abs(minVal),abs(maxVal));
    dbValue = 20*log10(dbValue);

    sprintf(buffer,"%d, %d, %d, %d, %d, 0, 100", dbValue, level1, level2, level3, level4);
    Serial.println(buffer);

    
    // clear the read count
    samplesRead = 0;
  }
}



// Callback used when PDM data are received
void onPDMdata() 
{
  // query the number of bytes available
  int bytesAvailable = PDM.available();

  // read into the sample buffer
  PDM.read(sampleBuffer, bytesAvailable);

  // 16-bit, 2 bytes per sample
  samplesRead = bytesAvailable / 2;
}


