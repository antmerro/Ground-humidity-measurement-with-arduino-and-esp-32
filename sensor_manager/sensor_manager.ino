#include <dht_nonblocking.h>
#define DHT_SENSOR_TYPE DHT_TYPE_11

static const int DHT_SENSOR_PIN = 2;
static const int MOISTURE_SENSOR_PIN = A1;
static const int MOISTURE_SENSOR_MIN = 0;
static const int MOISTURE_SENSOR_MAX = 1023;

DHT_nonblocking dht_sensor( DHT_SENSOR_PIN, DHT_SENSOR_TYPE );



/*
 * Initialize the serial port.
 * Set Moisture pin as an input
 */
void setup( )
{
  Serial.begin( 9600);
  pinMode(MOISTURE_SENSOR_PIN, INPUT);
}



/*
 * Poll for a measurement, keeping the state machine alive.  Returns
 * true if a measurement is available.
 */
static bool measure_environment( float *temperature, float *humidity )
{
  static unsigned long measurement_timestamp = millis( );

  /* Measure once every four seconds. */
  if( millis( ) - measurement_timestamp > 3000ul )
  {
    if( dht_sensor.measure( temperature, humidity ) == true )
    {
      measurement_timestamp = millis( );
      return( true );
    }
  }

  return( false );
}

/*
 * Main program loop.
 */
void loop( )
{
  //Values of Air Measurement
  float temperature = 0.0;
  float airHumidity = 0.0;

  /* Measure temperature and humidity of air.  If the functions returns
     true, then a measurement is available. */
  if( measure_environment( &temperature, &airHumidity )){
    Serial.print( ":" );
    Serial.print( temperature, 1 );
    Serial.print( ":" );
    Serial.print( airHumidity, 1 );
    Serial.print( ":" );
    //Mesure ground humidity and displays it in a % value
    Serial.print( map(analogRead(MOISTURE_SENSOR_PIN),MOISTURE_SENSOR_MIN,MOISTURE_SENSOR_MAX,100,0), 1 );
    Serial.println( ":" );
  }
  //Delays next iteration 0.5 seconds
  delay(500);
}
