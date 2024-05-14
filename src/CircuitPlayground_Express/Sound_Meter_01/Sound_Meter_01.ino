// ============================================================================
// Basique prototype of a sound meter based on CircuitPlayground Express.
// The target is simple: measure the ambiant sound level and - depending - of 
// db value, the color of the leds change.
//
// Note: the mic on the CircuitPlayground is not of hi-quality so it's hard to 
// have real low level values.
// ============================================================================

#include <Adafruit_CircuitPlayground.h>

#define NB_SAMPLE 25
float db_values_frame[NB_SAMPLE];

#define NB_LED 9
unsigned int r_value[NB_LED];
unsigned int g_value[NB_LED];
unsigned int b_value[NB_LED];

#define LOW_VALUE 60
#define HIGH_VALUE 67


unsigned int db_index;
unsigned int led_index;
unsigned long last_millis_graph = 0;
unsigned long last_millis_led = 0;


//#define _DEBUG_



void display_array()
{
  for (int i=0;i<NB_SAMPLE;i++)
  {
    Serial.print(db_values_frame[i]);
    Serial.print(" ");
    }
}




float parse_db_frame(float &mean, float &min, float &max)
{
  float mean_res = 0.0f;
  float min_res = 120.0f;
  float max_res = 0.0f;

  float total = 0.0f;
  for (int i=0;i<NB_SAMPLE;i++)
  {
    if (db_values_frame[i] < min_res)
      min_res = db_values_frame[i];

    if (db_values_frame[i] > max_res)
      max_res = db_values_frame[i];

    total += db_values_frame[i];
  }
  mean_res = (total/NB_SAMPLE);

  mean = mean_res;
  min = min_res;
  max = max_res;
}






void setup() 
{
  // Initialize data 
  memset(db_values_frame,0x00,sizeof(float)*NB_SAMPLE);
  db_index=0;
  led_index=0;

  memset(r_value,0x00,sizeof(unsigned int)*NB_LED);
  memset(g_value,0x00,sizeof(unsigned int)*NB_LED);
  memset(b_value,0x00,sizeof(unsigned int)*NB_LED);


  Serial.begin(115200);
  //while(!Serial) {;}

  CircuitPlayground.begin();
}



void loop() 
{

  // Read the db value
  float db = CircuitPlayground.mic.soundPressureLevel(10);

  db_values_frame[db_index%NB_SAMPLE] = db;
  db_index = (db_index+1)%NB_SAMPLE;

  #ifdef _DEBUG_:
  // Update the graph every 250ms
  if (millis()-last_millis_graph > 250)
  {
    last_millis_graph = millis();
    float mean_db, min_db, max_db;
    parse_db_frame(mean_db, min_db, max_db);

    Serial.print("40,");
    Serial.print(LOW_VALUE);
    Serial.print(",");
    Serial.print(mean_db);
    Serial.print(",");
    Serial.print(HIGH_VALUE);
    Serial.println(",100");
  }
  #endif

  // Update LED value array
  if (millis()-last_millis_led > 50)
  {
    last_millis_led = millis();
    float mean_db, min_db, max_db;
    parse_db_frame(mean_db, min_db, max_db);
    unsigned int r_val, g_val, b_val;

    if (mean_db<=LOW_VALUE)
    {
      r_val=0;
      g_val=255;
      b_val=0;
    }
    if (mean_db>LOW_VALUE && mean_db<HIGH_VALUE)
    {
      r_val=255;
      g_val=165;
      b_val=0;
    }
    if (mean_db>=HIGH_VALUE)
    {
      r_val=255;
      g_val=0;
      b_val=0;
    }

    r_value[led_index] = r_val;
    g_value[led_index] = g_val;
    b_value[led_index] = b_val;

    led_index = (led_index+1)%NB_LED;

    
    for (int i=0;i<NB_LED;i++)
      CircuitPlayground.setPixelColor(i,r_value[i],g_value[i],b_value[i]);
    

      
  } 
}
