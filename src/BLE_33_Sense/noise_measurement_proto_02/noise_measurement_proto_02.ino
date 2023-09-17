/*
  This example reads audio data from the on-board PDM microphones, compute
  dbValue and update dbHistory. To show the result it print min, max and 
  avg dbValues to the Serial console.
  The Serial Plotter built into the Arduino IDE can be used to plot the
  dbValues data (Tools -> Serial Plotter)

  Circuit:
  - Arduino Nano 33 BLE Sense board
*/

#include <PDM.h>
#include <math.h>

#define NB_HIST_VALUE 50
#define level1 30
#define level2 40
#define level3 50
#define level4 60

// buffer to read samples into, each sample is 16-bits
#define NB_SAMPLES 256
short sampleBuffer[NB_SAMPLES];
char str_buffer[150];

unsigned int counter = 0;

// number of samples read
volatile unsigned int samplesRead;

short minVal, maxVal, avgVal;
unsigned int sumVal = 0;

//unsigned int mi=0;
//unsigned int mx=0;

//float mean_dbValue=0.0f;
short dbValue=0;
short min_dbValue=0;
short max_dbValue=0;
unsigned int sum_dbValue=0;
short avg_dbValue=0;


unsigned int nb = NB_HIST_VALUE;


short dbValue_history[NB_HIST_VALUE];



unsigned int last_display_ms = 0;
unsigned int display_update_ms = 50;
unsigned int mic_gain = 50;


// ============================================================================
// Function used to return the min, max and the avg of an array
void extended_avg_array(short* ptr_array, unsigned int size_array, short* min_res, short* max_res, unsigned int* sum_res, short* avg_res)
{
  short min = ptr_array[0];
  short max = ptr_array[0];
  float avg = 0.0f;
  unsigned int sum=0;

  // Calculate the avg of the array
  for (unsigned int i=0; i<size_array; i++)
  {
    if (ptr_array[i] < min)
      min = ptr_array[i];

    if (ptr_array[i] > max)
      max = ptr_array[i];

    sum += ptr_array[i];
  }
    
  avg = sum/size_array;


  *min_res = min;
  *max_res=max;
  *sum_res=sum;
  *avg_res=avg;
}

// ============================================================================
// Board initialization
void setup()
{
  Serial.begin(9600);
  while (!Serial);

  // configure the data receive callback
  PDM.onReceive(onPDMdata);

  // optionally set the gain, defaults to 20
  // have to change it to match approx the value spoted on my iPhone
  PDM.setGain(mic_gain);

  // initialize PDM with:
  // - one channel (mono mode)
  // - a 16 kHz sample rate
  if (!PDM.begin(1, 16000)) 
  {
    Serial.println("Failed to start PDM!");
    while (1);
  }

  // Initialize the array of dbValue (historic of dbValue for the avg)
  for (int i=0;i<NB_HIST_VALUE;i++)
    dbValue_history[i]=0;
}

// ============================================================================
// Main loop
void loop() 
{
  // wait for samples to be read
  if (samplesRead) 
  {
    // One a sample is available, update our data ...
    // ... initialize default values
    minVal = sampleBuffer[0];
    maxVal = sampleBuffer[0];
    sumVal = 0;

    /*
    // ... parse the sample
    for (int i=0; i<samplesRead; i++)
    {
      minVal = min(sampleBuffer[i],minVal);
      maxVal = max(sampleBuffer[i],maxVal);
      sumVal += sampleBuffer[i];
    }
    
    // ... update avg 
    avgVal = sumVal/samplesRead;
  */
  extended_avg_array(sampleBuffer, samplesRead, &minVal, &maxVal, &sumVal, &avgVal);

    // ... update dbValue
    short tmp = max(abs(minVal),abs(maxVal));
    dbValue = 20*log10(tmp);

    dbValue_history[counter] = dbValue;
    counter = (counter+1)%NB_HIST_VALUE;
    
    // clear the read count
    samplesRead = 0;
  }


  // Every display_update_ms, update the plotter output
  if (millis() - last_display_ms > display_update_ms)
  {
    last_display_ms = millis();

    extended_avg_array(dbValue_history, nb, &min_dbValue, &max_dbValue, &sum_dbValue, &avg_dbValue);
  
    sprintf(str_buffer,"%d,%d,%d,%d,%d,%d,%d", level1, level2, level3, level4, min_dbValue, max_dbValue, avg_dbValue);
    Serial.println(str_buffer);
  }


}


// ============================================================================
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
