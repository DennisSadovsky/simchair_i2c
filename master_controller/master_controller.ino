//version 0.4
#include <Wire.h>
#include <Joystick.h>
#include <Adafruit_ADS1015.h>
#include <Keyboard.h>



//Joystick_ b8stick(0x20,JOYSTICK_TYPE_GAMEPAD,
//  6, 0,                  // Button Count, Hat Switch Count
//  true, true, false,     // X and Y, but no Z Axis
//  false, false, false,   // No Rx, Ry, or Rz
//  false, false,          // No rudder or throttle
//  false, false, false);  // No accelerator, brake, or steering

Joystick_ simchair(0x20, 0x04, 32, 1, true, true, true, true, true, false, true, true, false, false, false);
Joystick_ simchair_aux1(0x21, 0x04, 32, 1, true, true, true, true, true, true, false, true, false, false, false);
Joystick_ simchair_aux2(0x22, 0x04, 96, 1, false, false, false, true, true, false, false, false, false, false, false);

Adafruit_ADS1115 cyclic;
Adafruit_ADS1115 pedals(0x4A);

//<CONFIGURATION>
//----------------------------------------------------------------------------------------------

#define B8_POT_MODE "HAT_SW" // HAT_SW or ANALOG

#define SENS_ADJ_METHOD  "LINEAR"

#define ADS1115_RESOLUTION 15 //bits, min 12, max 15

#define PSEUDO_FORCE_TRIM 1
// this will freeze cyclic in-place until you press the button again.
// this is not exactly how it works in a real helicopter (force trim only provides centering forces, not disables cyclic input),
// but is useful in a sim and probably is the closest thing we can get on a non-FFB stick.
#define PSEUDO_FORCE_TRIM_BUTTON 1
// this will disable a choosen button and dedicate it to force trim. Top button in Huey, mid button in 407
#define PSEUDO_FORCE_TRIM_EFFECTS_PEDALS 1 //in a real heli force trim affects both cyclic and pedals, this is a default behavior
#define PSEUDO_FORCE_TRIM_RELEASE_MODE "ADAPTIVE" //INSTANT or ADAPTIVE
// ADAPTIVE will release trim when your lever will be close to the position it is held after the button press
// INSTANT will release instantly, on press
#define PSEUDO_FORCE_TRIM_RELEASE_DEVIATION 5  // how much deviation is allowed from trimmed position when releasing trim, percent
#define SENS_SWITCH_TRIM_EMERGENCY_RELEASE 1 // release the trim when you press SENS SWITCH

// use a button on cyclic to switch sensitivity in flight.
// this kind of functionality is not available in a real aircraft, however
// it may somewhat resemble simulated force trim.

#define SENS_DEVICE "b8_stick"
#define SENS_SWITCH_ENABLED 1
#define SENS_SWITCH_BUTTON 0 //0 is the first button

#define B8_HAT_SWITCH_MODE "ATT_TRIM" //"ATT_TRIM" or "HAT", it will adjust the position of the trimmed cyclic with hat(Bell 407-style)
#define ATT_TRIM_STEP_Y 0.05 //IN PERCENTS OF AXIS LENGHT
#define ATT_TRIM_STEP_X 0.05 //IN PERCENTS OF AXIS LENGHT
#define INVERT_HAT_TRIM_X 0
#define INVERT_HAT_TRIM_Y 1

// sens. switch behavior available options: "FORCE_TRIM" or "TOGGLE"
// FORCE_TRIM will reduce sensitivity to CYCLIC_SENS and RUDDER_SENS
// while SENSITIVITY_SWITCH_BUTTON is pressed, "TOGGLE" will act
// as a regular switch

#define SENS_SWITCH_MODE "TOGGLE"

// if SENS_SWITCH is disabled, these values will be used;
// set to 100 for full axis range
#define CUSTOM_CYCLIC_SENS 80
#define CUSTOM_RUDDER_SENS 80
#define INVERT_RUDDER 1

#define XY_FILTERING_ENABLED 0
#define RUDDER_FILTERING_ENABLED 0

#define PTT_KEYBOARD_PRESS 0 // set to 1 to enable keyboard combination press instead of a joystick button
#define PTT_BUTTON 4 //1st button is 0, 4 should be PTT trigger position

// this should press CTRL + Q
#define PTT_KEYBOARD_KEY 'q'
char PTT_KEYBOARD_KEY_MOD = KEY_LEFT_CTRL;

//AB412 collective head
#define AB412_COLL_HEAD_ROTARY_POTS  2 // 0, 1 or 2
#define AB412_COLL_HEAD_LEFT_HAT_DIRECTIONS 4
#define AB412_COLL_HEAD_RIGHT_HAT_DIRECTIONS 8
#define AB412_IDLE1_SW_KEY  'y'
#define AB412_IDLE2_SW_KEY  'u'
#define AB412_STARTER1_SW_KEY  'i'
#define AB412_STARTER2_SW_KEY  'o'

#define HUEY_COLL_HEAD_HAT_DIRECTIONS 4

#define COLL_HEAD_SW_HOLD_TIMEOUT 200
//#define HUEY_COLL_HEAD_SW_HOLD_TIMEOUT 200

#define COLLECTIVE_HOLD_ENABLED 1 // this will hold your collective in place until you press the button again and return the lever to the initial position
#define HUEY_HEAD_COLLECTIVE_HOLD_BUTTON 3
#define AB412_HEAD_COLLECTIVE_HOLD_BUTTON 2

#define AB412_COLL_HEAD_MODE_SWITCH 18 // 3-way fixed switch only; first (lower number) switch button must be defined there; remove it and next button from everywhere else; set to 0 to disable MODE SWITCH
#define HUEY_COLL_HEAD_MODE_SWITCH 4

//for sims that do not support axis movement below idle stop, sends the throttle up/down key when holding idle stop and rotating throttle simultaneously
//not 100% realistic, yet better than nothing
#define COLL_HEAD_IDLE_STOP_COMPAT_MODE 1 // ASSIGN IDLE STOP IN_GAME WHILE IN MODE 0 (MODE SWITCH CENTERED)!
#define COLL_HEAD_IDLE_STOP_COMPAT_PROFILE "DCS_HUEY" //DCS_HUEY or XTRIDENT412
#define COLL_HEAD_IDLE_STOP_COMPAT_PROFILE_XTRIDENT412 1

byte huey_coll_head_idle_stop_buttons[] = {12};
byte ab412_coll_head_idle_stop_buttons[] = {9,10}; // put 1 or 2 buttons here as seen in joy.cpl; these keys MUST be in the ab412_sw_mode_button_switches[] array below
byte ab412_coll_head_starter_buttons[] = {11,12};
byte coll_head_idle_stop_compat_throttle_up_keys[] = {KEY_PAGE_DOWN,'z'};
byte coll_head_idle_stop_compat_throttle_down_keys[] = {KEY_PAGE_UP,'x'};

#define COLL_HEAD_IDLE_STOP_COMPAT_TRESHOLD 5 //
#define SINGLE_ENGINE_COLLECTIVE_IDLE_STOP_AXIS_VAL 70 //find it out with uncommenting Serial.print(throttle) in poll_single_engine_collective sub, see below
#define TWIN_ENGINE_COLLECTIVE_IDLE_STOP_THROTTLE1_VAL 50
#define TWIN_ENGINE_COLLECTIVE_IDLE_STOP_THROTTLE2_VAL 50


// AB412 switch modes
// write joystick button numbers here as they are displayed in joy.cpl in order of increment
byte ab412_sw_mode_button_switches[] = {1,2,9,10,11,12,17};//e.g. {1,2,9,10,11,12,13,14,17};// active when being held
byte ab412_sw_mode_toggle_switches[] = {3,4,5,6,7,8};//e.g. {3,4,5,6,7,8,15,16};// 2-way switch mode: single button press when switch is turned to "on", one more press when switch is turned to "off"; something you can assign to a single key press; e.g. gear extend/retract
byte ab412_sw_mode_selector_button_switches[] = {13}; //e.g. {3,5,7,15}; //3-WAY SWITCHES ONLY, FIRST BUTTON (WITH LOWER NUMBER) MUST BE GIVEN HERE; REMOVE THE SECOND BUTTON FROM EVERYWHERE ELSE FOR CORRECT OPERATION; when switch is on, button is held; when off, another button is pressed and held;
byte ab412_sw_mode_selector_switches[] = {15};//e.g. {3,5,7,15}; //same as above, but buttons are pressed and released - e.g. landing light extend / hold / retract

byte huey_sw_mode_button_switches[] = {7,8,9,12,13};// active when being held
byte huey_sw_mode_toggle_switches[] = {6};// 2-way switch mode: single button press when switch is turned to "on", one more press when switch is turned to "off"; something you can assign to a single key press; e.g. gear extend/retract
byte huey_sw_mode_selector_button_switches[] = {1,10}; //3-WAY SWITCHES ONLY, FIRST BUTTON (WITH LOWER NUMBER) MUST BE GIVEN HERE; REMOVE THE SECOND BUTTON FROM EVERYWHERE ELSE FOR CORRECT OPERATION; when switch is on, button is held; when off, 
//another button is pressed and held; THIS MODE SUPPORTS MODE SWITCH! for normal operation, return all switches to center when switching mode, otherwise they will hold button pressed until it will be pressed again in the same mode. This may or may not be
//what you expect, so be warned.
byte huey_sw_mode_selector_switches[] = {}; //same as above, but buttons are pressed and released - e.g. landing light extend / hold / retract


//----------------------------------------------------------------------------------------------
//</CONFIGURATION>

bool force_trim_on = 0;
bool force_trim_position_set = 0;
bool force_trim_rudder_on = 0;
bool force_trim_rudder_position_set = 0;
uint16_t force_trim_x = 0;
uint16_t force_trim_y = 0 ;
uint16_t force_trim_rudder = 0;




int cyclic_sens = 100;
int rudder_sens = 100;

bool coll_head_lastButtonState[60];
bool coll_head_triggerState[60];
long coll_head_sw_ts[60];
byte coll_head_sw_mode[60];
bool coll_head_sw_mode_button_switches_parsed[40];
bool coll_head_sw_mode_toggle_switches_parsed[40];
bool coll_head_sw_mode_selector_button_switches_parsed[40];
bool coll_head_sw_mode_selector_switches_parsed[40];
byte coll_head_mode_sw_position = 0;
bool collective_hold_active = 0;
uint16_t collective_hold_position = 0;
bool collective_hold_position_set = 0;
bool collective_hold_button_was_pressed = 0;

uint16_t min_throttle_val[] = {0,0};
bool mode_sw_pos_0 = 0; // buffer that stores the current value of a 1st position in a mode switch
uint16_t last_throttle_setting[] = {0,0};
bool idle_stop_pressed[] = {0,0};
bool throttle_idle_cutoff[] = {1,1}; //we presume engines are off on startup


bool b8stick_lastButtonState[] = {0, 0, 0, 0, 0, 0};
bool dev_b8stick = 0;
bool dev_cyclic = 0;
bool dev_simple_collective = 0;
bool dev_single_engine_collective = 0;
bool dev_twin_engine_collective = 0;
bool dev_pedals = 0;
bool dev_cessna_engine_and_prop_controls = 0;
bool dev_ab412_coll_head = 0;
bool dev_huey_coll_head = 0;
bool SW_MODE_BUTTON = 0;
bool SW_MODE_TOGGLE = 1;
bool zero = 0;

//ADS1115 filter variables
const int xy_readings = 6;
const int rudder_readings = 8;
int buf_x[xy_readings];
int buf_y[xy_readings];
int buf_rudder[rudder_readings];
int xy_read_index = 0; // the index of the current reading
int rudder_read_index = 0;
long total_x = 0;  // the running total
long total_y = 0;
long total_rudder = 0;

int ADC_RANGE = 0.5 + pow(2, ADS1115_RESOLUTION);
int COLLECTIVE_ADC_RANGE = 0.5 + pow(2, 10);

void setup()
{
  delay (2000);
  Serial.begin(9600);
  simchair.begin();
  simchair_aux1.begin();
  simchair_aux2.begin();
  //detect and configure connected peripherals
  Wire.begin();
  setup_b8stick();
  setup_cyclic();
  setup_simple_collective();
  setup_single_engine_collective();
  setup_twin_engine_collective();
  setup_pedals();
  setup_cessna_engine_and_prop_controls();
  setup_ab412_coll_head();
  setup_huey_coll_head();
  if ((PTT_KEYBOARD_PRESS == 1) || (COLL_HEAD_IDLE_STOP_COMPAT_PROFILE_XTRIDENT412 == 1))
  {
    Keyboard.begin();
  }
}



void loop()
{
  //Serial.println("WHEEE");
  if (dev_b8stick == 1)
  {
    poll_b8stick();
  }
  if (dev_cyclic == 1)
  {
    poll_cyclic();
  }
  if (dev_simple_collective == 1)
  {
    poll_simple_collective();
  }
  if (dev_single_engine_collective == 1)
  {
    poll_single_engine_collective();
  }
  if (dev_twin_engine_collective == 1)
  {
    poll_twin_engine_collective();
  }
  if (dev_pedals == 1)
  {
    poll_pedals();
  }
  if (dev_cessna_engine_and_prop_controls == 1)
  {
    poll_cessna_engine_and_prop_controls();
  }
  if (dev_ab412_coll_head == 1)
  {
    poll_ab412_coll_head();
  }
  if (dev_huey_coll_head == 1)
  {
    poll_huey_coll_head();
  }
}


int adjust_sensitivity (int val, int param)
{
  if ((SENS_ADJ_METHOD == "LINEAR") && (param < 100))
  {
    int percent = param;
    // this will simply limit your controls throw range by the given percent,
    // and map full ADC range for it.
    // may be a good option for flying helicopters in x-plane
    int center = ADC_RANGE / 2;
    int adj_range = (ADC_RANGE / 100) * percent;
    val = map(val, 0, ADC_RANGE, center - (adj_range / 2), center + (adj_range / 2));
  }

  return val;
}

void setup_b8stick()
{
  //MKIII B8 stick
  Wire.beginTransmission(20);
  int error = Wire.endTransmission();
  if (error == 0)
  {
    simchair_aux1.setXAxisRange(0, 255);
    simchair_aux1.setYAxisRange(0, 255);
    dev_b8stick = 1;
  }
}

void setup_cyclic()
{
  Wire.beginTransmission(0x48);
  int error = Wire.endTransmission();
  if (error == 0)
  {
    //initialize ADS1115 filters
    for (int thisReading = 0; thisReading < xy_readings; thisReading++)
    {
      buf_x[thisReading] = 0;
      buf_y[thisReading] = 0;
    }
    simchair.setXAxisRange(0, ADC_RANGE);
    simchair.setYAxisRange(0, ADC_RANGE);
    dev_cyclic = 1;
    cyclic.begin();
    cyclic.setGain(GAIN_ONE);
  }
}

void setup_pedals()
{
  Wire.beginTransmission(0X4A);
  int error = Wire.endTransmission();
  if (error == 0)
  {
    //initialize ADS1115 filters
    for (int thisReading = 0; thisReading < xy_readings; thisReading++)
    {
      buf_rudder[thisReading] = 0;
    }
    if (INVERT_RUDDER == 1)
    {
      simchair.setRudderRange(ADC_RANGE, 0);
    }
    else
    {
      simchair.setRudderRange(0, ADC_RANGE);
    }
    dev_pedals = 1;
    pedals.begin();
    pedals.setGain(GAIN_ONE);
  }
}

void setup_simple_collective()
{
  Wire.beginTransmission(10);
  int error = Wire.endTransmission();
  if (error == 0)
  {
    simchair.setZAxisRange(0, 1023);
    simchair.setThrottleRange(0, 1023);
    dev_simple_collective = 1;
  }
}

void setup_single_engine_collective()
{
  Wire.beginTransmission(12);
  int error = Wire.endTransmission();
  if (error == 0)
  {
    simchair.setZAxisRange(0, 1023);
    simchair.setThrottleRange(0,1023);//SINGLE_ENGINE_COLLECTIVE_IDLE_STOP_AXIS_VAL, 1023);
    dev_single_engine_collective = 1;
  }
}
void setup_twin_engine_collective()
{
  Wire.beginTransmission(15);
  int error = Wire.endTransmission();
  if (error == 0)
  {
    simchair.setZAxisRange(1023, 0);
    simchair.setThrottleRange(0,1023);//SINGLE_ENGINE_COLLECTIVE_IDLE_STOP_AXIS_VAL, 1023);
    simchair.setRyAxisRange(0,1023);
    dev_twin_engine_collective = 1;
  }
}

void setup_cessna_engine_and_prop_controls()
{
  Wire.beginTransmission(11);
  int error = Wire.endTransmission();
  if (error == 0)
  {
    simchair_aux1.setRxAxisRange(0, 1023);
    simchair_aux1.setRyAxisRange(0, 1023);
    simchair_aux1.setThrottleRange(0, 1023);
    dev_cessna_engine_and_prop_controls = 1;
  }
}

void parse_sw_array (byte sw_arr[], byte sw_arr_size, bool parsed_sw_arr[])
{
  byte z = 0;
  for (int i = 0; i < sw_arr_size; i++)
  {
    for (int k = z; k <= sw_arr[i]; k++)
    {
      if (k == (sw_arr[i] - 1))
      {
        parsed_sw_arr[k] = 1;
      }
      else
      {
        parsed_sw_arr[k] = 0;
      }
    }
    z = sw_arr[i];
  }
}

void setup_ab412_coll_head()
{
  Wire.beginTransmission(13);
  int error = Wire.endTransmission();
  if (error == 0)
  {
    simchair_aux2.setXAxisRange(0, 255);
    simchair_aux2.setYAxisRange(0, 255);
    simchair_aux2.setRxAxisRange(0, 255);
    simchair_aux2.setRyAxisRange(0, 255);
    simchair_aux2.setZAxisRange(0, 255);
    simchair_aux2.setRzAxisRange(0, 255);
    dev_ab412_coll_head = 1;
    for (int i = 0; i < sizeof(coll_head_triggerState); i++)
    {
      coll_head_triggerState[i] = 0;
    }
    parse_sw_array(ab412_sw_mode_button_switches, sizeof(ab412_sw_mode_button_switches), coll_head_sw_mode_button_switches_parsed);
    parse_sw_array(ab412_sw_mode_toggle_switches, sizeof(ab412_sw_mode_toggle_switches), coll_head_sw_mode_toggle_switches_parsed);
    parse_sw_array(ab412_sw_mode_selector_button_switches, sizeof(ab412_sw_mode_selector_button_switches), coll_head_sw_mode_selector_button_switches_parsed);
    parse_sw_array(ab412_sw_mode_selector_switches, sizeof(ab412_sw_mode_selector_switches), coll_head_sw_mode_selector_switches_parsed);
    

    for (int i = 0; i < sizeof(ab412_sw_mode_selector_button_switches); i++)
    {
      coll_head_lastButtonState[ab412_sw_mode_selector_button_switches[i]] = 1;
    }

//        for (int p = 0; p < sizeof(coll_head_sw_mode_button_switches_parsed); p++)
//        {
//          Serial.print(p);
//          Serial.print(" ");
//          Serial.println(coll_head_sw_mode_button_switches_parsed[p]);
//        }

  }
}

void setup_huey_coll_head()
{

  Wire.beginTransmission(17);
  int error = Wire.endTransmission();
  if (error == 0)
  {
    simchair_aux2.setXAxisRange(0, 255);
    simchair_aux2.setYAxisRange(0, 255);
    dev_huey_coll_head = 1;
    
    for (int i = 0; i < sizeof(coll_head_triggerState); i++)
    {
      coll_head_triggerState[i] = 0;
    }
    parse_sw_array(huey_sw_mode_button_switches, sizeof(huey_sw_mode_button_switches), coll_head_sw_mode_button_switches_parsed);
    parse_sw_array(huey_sw_mode_toggle_switches, sizeof(huey_sw_mode_toggle_switches), coll_head_sw_mode_toggle_switches_parsed);
    parse_sw_array(huey_sw_mode_selector_button_switches, sizeof(huey_sw_mode_selector_button_switches), coll_head_sw_mode_selector_button_switches_parsed);
    parse_sw_array(huey_sw_mode_selector_switches, sizeof(huey_sw_mode_selector_switches), coll_head_sw_mode_selector_switches_parsed);

    for (int i = 0; i < sizeof(huey_sw_mode_selector_button_switches); i++)
    {
      coll_head_lastButtonState[huey_sw_mode_selector_button_switches[i]] = 1;
    }

//          for (int p = 0; p < sizeof(huey_sw_mode_selector_button_switches_parsed); p++)
//        {
//          Serial.print(p);
//          Serial.print(" ");
//          Serial.println(huey_sw_mode_selector_button_switches_parsed[p]);
//        }

  }
}

void poll_b8stick()
{
  //hot swap support for cyclic grip - a very useful feature
  Wire.beginTransmission(20);
  int error = Wire.endTransmission();
  if (error == 0)
  {
    uint8_t rx;
    uint8_t ry;
    uint8_t b;
    Wire.requestFrom(20, 3);
    while (Wire.available())
    {
      byte b1 = Wire.read();
      byte b2 = Wire.read();
      b = Wire.read();

      rx = b1;
      ry = b2;
    }
    if (B8_POT_MODE == "ANALOG")
    {
      simchair_aux1.setXAxis(rx);
      simchair_aux1.setYAxis(ry);
    }
    else
    {
      if (B8_HAT_SWITCH_MODE == "HAT")
      {
        int16_t hat_val = parse_hat_sw(rx, ry, 8);
        simchair.setHatSwitch(0, hat_val);
      }
      else
      {
        int16_t hat_val = parse_hat_trim(rx, ry, INVERT_HAT_TRIM_X, INVERT_HAT_TRIM_Y);
      }
      
    }
    for (byte i = 0; i < 6; i++)
    {
      bool v = (b >> i) & 1;
      if (v != b8stick_lastButtonState[i])
      {
        if (((SENS_SWITCH_ENABLED == 1) && (SENS_DEVICE == "b8_stick")) || (PTT_KEYBOARD_PRESS == 1) || (PSEUDO_FORCE_TRIM == 1))
        {
          if ((i != SENS_SWITCH_BUTTON) && (i != PTT_BUTTON) && (i != PSEUDO_FORCE_TRIM_BUTTON))
          {
            simchair.setButton(i, v);
          }
          else
          {
            if (((SENS_SWITCH_MODE == "FORCE_TRIM") || ((SENS_SWITCH_MODE == "TOGGLE") && (v == 1))) && (i != PTT_BUTTON) && (i != PSEUDO_FORCE_TRIM_BUTTON))
            {
              if (cyclic_sens == 100)
              {
                cyclic_sens = CUSTOM_CYCLIC_SENS;
                rudder_sens = CUSTOM_RUDDER_SENS;
                if (SENS_SWITCH_TRIM_EMERGENCY_RELEASE == 1)
                {
                  force_trim_on = 0;
                  force_trim_position_set = 0;
                  force_trim_rudder_on = 0;
                  force_trim_rudder_position_set = 0;
                }
              }
              else
              {
                cyclic_sens = 100;
                rudder_sens = 100;
                if (SENS_SWITCH_TRIM_EMERGENCY_RELEASE == 1)
                {
                  force_trim_on = 0;
                  force_trim_position_set = 0;
                  force_trim_rudder_on = 0;
                  force_trim_rudder_position_set = 0;
                }
              }
            }
            else if ((i == PTT_BUTTON) && (PTT_KEYBOARD_PRESS == 1))
            {
              if (v == 1)
              {
                Keyboard.press(PTT_KEYBOARD_KEY_MOD);
                Keyboard.press(PTT_KEYBOARD_KEY);
              }
              else
              {
                Keyboard.releaseAll();
              }

            }
            else if ((i == PSEUDO_FORCE_TRIM_BUTTON) && (PSEUDO_FORCE_TRIM == 1) && (v == 1))
            {
              if (force_trim_on == 0)
              {
                force_trim_on = 1;
                force_trim_rudder_on = 1;
              }
              else
              {
                force_trim_on = 0;
                force_trim_rudder_on = 0;
              }
            }
            else
            {
              simchair.setButton(i, v);
            }
          }
        }
        else
        {
          simchair.setButton(i, v);
        }
      }
      b8stick_lastButtonState[i] = v;
    }
  }
}

void poll_cyclic()
{
  uint16_t x, y;
  if (XY_FILTERING_ENABLED == 1)
  {
    total_x = total_x - buf_x[xy_read_index];
    total_y = total_y - buf_y[xy_read_index];
    buf_x[xy_read_index] = cyclic.readADC_SingleEnded(0) >> (15 - ADS1115_RESOLUTION);
    buf_y[xy_read_index] = cyclic.readADC_SingleEnded(1) >> (15 - ADS1115_RESOLUTION);

    total_x = total_x + buf_x[xy_read_index];
    total_y = total_y + buf_y[xy_read_index];
    xy_read_index = xy_read_index + 1;

    if (xy_read_index >= xy_readings)
    {
      // ...wrap around to the beginning:
      xy_read_index = 0;
    }

    x = total_x / xy_readings;
    y = total_y / xy_readings;


  }
  else
  {
    x = cyclic.readADC_SingleEnded(0) >> (15 - ADS1115_RESOLUTION);
    y = cyclic.readADC_SingleEnded(1) >> (15 - ADS1115_RESOLUTION);
    // uncomment next 3 lines for cyclic calibration
    //Serial.print(x);
    //Serial.print(" ");
    //Serial.println(y);
  }

  if (SENS_SWITCH_ENABLED == 1)
  {
    x = adjust_sensitivity(x, cyclic_sens);
    y = adjust_sensitivity(y, cyclic_sens);
  }
  else
  {
    x = adjust_sensitivity(x, CUSTOM_CYCLIC_SENS);
    y = adjust_sensitivity(y, CUSTOM_CYCLIC_SENS);
  }

  if (PSEUDO_FORCE_TRIM_RELEASE_MODE == "ADAPTIVE")
  {
    if ((force_trim_on == 0) && (force_trim_position_set == 0))
    {
      simchair.setXAxis(x);
      simchair.setYAxis(y);
    }
    else if ((force_trim_on == 1) && (force_trim_position_set == 0))
    {
      force_trim_x = x;
      force_trim_y = y;
      force_trim_position_set = 1;
    }
    else if ((force_trim_on == 0) && (force_trim_position_set == 1))
    {
      int one_percent_range = ADC_RANGE / 100;
      int diff_x = x - force_trim_x; // this is needed because of how the abs() works
      int diff_y = y - force_trim_y;

      if (((abs(diff_x)) < (PSEUDO_FORCE_TRIM_RELEASE_DEVIATION * one_percent_range)) && ((abs(diff_y)) < (PSEUDO_FORCE_TRIM_RELEASE_DEVIATION * one_percent_range)))
      {
        simchair.setXAxis(x);
        simchair.setYAxis(y);
        force_trim_position_set = 0;
      }
    }
  }
  else //INSTANT mode
  {
    if (force_trim_on == 0)
    {
      simchair.setXAxis(x);
      simchair.setYAxis(y);
    }
  }
}

void poll_pedals()
{
  uint16_t rudder;
  if (RUDDER_FILTERING_ENABLED == 1)
  {
    total_rudder = total_rudder - buf_rudder[rudder_read_index];
    buf_rudder[rudder_read_index] = pedals.readADC_SingleEnded(0) >> (15 - ADS1115_RESOLUTION);
    total_rudder = total_rudder + buf_rudder[rudder_read_index];
    rudder_read_index = rudder_read_index + 1;
    if (rudder_read_index >= rudder_readings)
    {
      rudder_read_index = 0;
    }
    rudder = total_rudder / rudder_readings;

  }
  else
  {
    rudder = pedals.readADC_SingleEnded(0) >> (15 - ADS1115_RESOLUTION);
    //Serial.println(rudder); //uncomment to calibrate pedals
  }
  if (SENS_SWITCH_ENABLED == 1)
  {
    rudder = adjust_sensitivity(rudder, rudder_sens);
  }
  else
  {
    rudder = adjust_sensitivity(rudder, CUSTOM_RUDDER_SENS);
  }
  if (PSEUDO_FORCE_TRIM_EFFECTS_PEDALS == 1)
  {
    if (PSEUDO_FORCE_TRIM_RELEASE_MODE == "ADAPTIVE")
    {
      if ((force_trim_rudder_on == 0) && (force_trim_rudder_position_set == 0))
      {
        simchair.setRudder(rudder);
      }
      else if ((force_trim_rudder_on == 1) && (force_trim_rudder_position_set == 0))
      {
        force_trim_rudder = rudder;
        force_trim_rudder_position_set = 1;
      }
      else if ((force_trim_rudder_on == 0) && (force_trim_rudder_position_set == 1))
      {
        int one_percent_range = ADC_RANGE / 100;
        int diff_rudder = rudder - force_trim_rudder; // this is needed because of how the abs() works

        if ((abs(diff_rudder)) < (PSEUDO_FORCE_TRIM_RELEASE_DEVIATION * one_percent_range))
        {
          simchair.setRudder(rudder);
          force_trim_rudder_position_set = 0;
        }
      }
    }
    else //INSTANT mode
    {
      if (force_trim_rudder_on == 0)
      {
        simchair.setRudder(rudder);
      }
    }
  }
  else
  {
    simchair.setRudder(rudder);
  }

}

void poll_simple_collective()
{
  uint16_t z;
  uint16_t throttle;

  Wire.requestFrom(10, 4);
  while (Wire.available())
  {
    byte b1 = Wire.read(); // receive a byte as character
    byte b2 = Wire.read();
    byte b3 = Wire.read(); // receive a byte as character
    byte b4 = Wire.read();

    z = b1;
    z = (z << 8) | b2;
    throttle = b3;
    throttle = (throttle << 8) | b4;

  }

  simchair.setZAxis(z);
  simchair.setThrottle(throttle);
}

void poll_single_engine_collective()
{
  uint16_t z;
  uint16_t throttle;

  Wire.requestFrom(12, 4);
  while (Wire.available())
  {
    byte b1 = Wire.read(); // receive a byte as character
    byte b2 = Wire.read();
    byte b3 = Wire.read(); // receive a byte as character
    byte b4 = Wire.read();

    z = b1;
    z = (z << 8) | b2;
    throttle = b3;
    throttle = (throttle << 8) | b4;
  }
  // uncomment the next line and turn your throttle to idle stop position to see SINGLE_ENGINE_COLLECTIVE_IDLE_STOP_AXIS_VAL value 
  //Serial.println(throttle);
  if ((COLL_HEAD_IDLE_STOP_COMPAT_MODE == 1) && (coll_head_mode_sw_position == 0))
  {
    if (COLL_HEAD_IDLE_STOP_COMPAT_PROFILE == "DCS_HUEY")
    {
      if (min_throttle_val[0] != SINGLE_ENGINE_COLLECTIVE_IDLE_STOP_AXIS_VAL)
      {
         simchair.setThrottleRange(SINGLE_ENGINE_COLLECTIVE_IDLE_STOP_AXIS_VAL, 1023);
         min_throttle_val[0] = SINGLE_ENGINE_COLLECTIVE_IDLE_STOP_AXIS_VAL;
      }
     
      coll_head_idle_stop_compat_dcs (throttle,0,SINGLE_ENGINE_COLLECTIVE_IDLE_STOP_AXIS_VAL,0);
      last_throttle_setting[0] = throttle;
      if (throttle < SINGLE_ENGINE_COLLECTIVE_IDLE_STOP_AXIS_VAL)
      {
        throttle = SINGLE_ENGINE_COLLECTIVE_IDLE_STOP_AXIS_VAL;
      }
      else
      {
        throttle_idle_cutoff[0] = 0; 
      }
      if ((idle_stop_pressed[0] != 1) && (throttle_idle_cutoff[0] == 0))
      {
        simchair.setThrottle(throttle);
      }
      else if (idle_stop_pressed[0] == 1)
      {
        throttle_idle_cutoff[0] = 1; 
      }   
    }
    else
    {
      //other profiles
      simchair.setThrottle(throttle);
    }
  }
  else
  {
    if (min_throttle_val[0] != 0)
    {
      simchair.setThrottleRange(0, 1023);
      min_throttle_val[0] = 0;
    }
    simchair.setThrottle(throttle);
  } 

  if ((collective_hold_active == 0) && (collective_hold_position_set == 0))
  {
    simchair.setZAxis(z);
  }
  else if ((collective_hold_active == 1) && (collective_hold_position_set == 0))
  {
    collective_hold_position = z;
    collective_hold_position_set = 1;
  }
  else if ((collective_hold_active == 0) && (collective_hold_position_set == 1))
  {
    int one_percent_range = COLLECTIVE_ADC_RANGE / 100;
    int diff_z = z - collective_hold_position; // this is needed because of how the abs() works

    if ((abs(diff_z)) < (PSEUDO_FORCE_TRIM_RELEASE_DEVIATION * one_percent_range))
    {
      simchair.setZAxis(z);
      collective_hold_position_set = 0;
    }
  }

}

void poll_twin_engine_collective()
{
  uint16_t z;
  uint16_t throttle;
  int16_t ry;

  Wire.requestFrom(15, 6);
  while (Wire.available())
  {
    byte b1 = Wire.read(); // receive a byte as character
    byte b2 = Wire.read();
    byte b3 = Wire.read(); // receive a byte as character
    byte b4 = Wire.read();
    byte b5 = Wire.read();
    byte b6 = Wire.read();

    z = b1;
    z = (z << 8) | b2;
    throttle = b3;
    throttle = (throttle << 8) | b4;
    ry = b5;
    ry = (ry << 8) | b6;
  }
  // uncomment the next line and turn your throttle to idle stop position to see SINGLE_ENGINE_COLLECTIVE_IDLE_STOP_AXIS_VAL value 
  //Serial.println(throttle);
  if ((COLL_HEAD_IDLE_STOP_COMPAT_MODE == 1) && (coll_head_mode_sw_position == 0))
  {
    if (COLL_HEAD_IDLE_STOP_COMPAT_PROFILE == "DCS_HUEY")
    {
      if (min_throttle_val[0] != TWIN_ENGINE_COLLECTIVE_IDLE_STOP_THROTTLE1_VAL)
      {
         simchair.setThrottleRange(TWIN_ENGINE_COLLECTIVE_IDLE_STOP_THROTTLE1_VAL, 1023);
         min_throttle_val[0] = TWIN_ENGINE_COLLECTIVE_IDLE_STOP_THROTTLE1_VAL;
      }
     
      coll_head_idle_stop_compat_dcs (throttle,0,TWIN_ENGINE_COLLECTIVE_IDLE_STOP_THROTTLE1_VAL,0);
      last_throttle_setting[0] = throttle;
      if (throttle < TWIN_ENGINE_COLLECTIVE_IDLE_STOP_THROTTLE1_VAL)
      {
        throttle = TWIN_ENGINE_COLLECTIVE_IDLE_STOP_THROTTLE1_VAL;
      }
      else
      {
        throttle_idle_cutoff[0] = 0; 
      }
      if ((idle_stop_pressed[0] != 1) && (throttle_idle_cutoff[0] == 0))
      {
        simchair.setThrottle(throttle);
      }
      else if (idle_stop_pressed[0] == 1)
      {
        throttle_idle_cutoff[0] = 1; 
      }   
    }
    else
    {
      //other profiles
      simchair.setThrottle(throttle);
      simchair.setRyAxis(ry);
    }
  }
  else
  {
    if (min_throttle_val[0] != 0)
    {
      simchair.setThrottleRange(0, 1023);
      simchair.setRyAxisRange(0,1023);
      min_throttle_val[0] = 0;
    }
    simchair.setThrottle(throttle);
    simchair.setRyAxis(ry);
  }
  simchair.setRyAxis(ry);
  
  if ((collective_hold_active == 0) && (collective_hold_position_set == 0))
  {
    simchair.setZAxis(z);
  }
  else if ((collective_hold_active == 1) && (collective_hold_position_set == 0))
  {
    collective_hold_position = z;
    collective_hold_position_set = 1;
  }
  else if ((collective_hold_active == 0) && (collective_hold_position_set == 1))
  {
    int one_percent_range = COLLECTIVE_ADC_RANGE / 100;
    int diff_z = z - collective_hold_position; // this is needed because of how the abs() works

    if ((abs(diff_z)) < (PSEUDO_FORCE_TRIM_RELEASE_DEVIATION * one_percent_range))
    {
      simchair.setZAxis(z);
      collective_hold_position_set = 0;
    }
  }
  
}


uint16_t coll_head_idle_stop_compat_dcs (uint16_t throttle0, uint16_t throttle1, uint16_t idle_val0, uint16_t idle_val1)
{
  uint16_t throttle[] = {throttle0,throttle1};
  uint16_t idle_val[] = {idle_val0,idle_val1};

  for (byte i = 0; i < sizeof(ab412_coll_head_idle_stop_buttons);i++)
    {
      if ((throttle[i] < idle_val[i]) && (throttle_idle_cutoff[i] == 1))
      {
        int diff = last_throttle_setting[i] - throttle[i];
        if (diff < - COLL_HEAD_IDLE_STOP_COMPAT_TRESHOLD)
        {
          Keyboard.press(coll_head_idle_stop_compat_throttle_down_keys[i]);
          delay(300);
          Keyboard.releaseAll();
        }
      }
      if (idle_stop_pressed[i] == 1)
      {
        int diff = last_throttle_setting[i] - throttle[i];
        if (diff > COLL_HEAD_IDLE_STOP_COMPAT_TRESHOLD)
        {
          Keyboard.press(coll_head_idle_stop_compat_throttle_up_keys[i]);
          delay(300);
          Keyboard.releaseAll();
        }
        last_throttle_setting[i] = throttle[i];
      }
    }
}
void poll_cessna_engine_and_prop_controls()
{
  uint16_t rx, ry, throttle;

  Wire.requestFrom(11, 6);
  while (Wire.available())
  {
    byte b1 = Wire.read(); // receive a byte as character
    byte b2 = Wire.read();
    byte b3 = Wire.read(); // receive a byte as character
    byte b4 = Wire.read();
    byte b5 = Wire.read(); // receive a byte as character
    byte b6 = Wire.read();
    throttle = b1;
    throttle = (throttle << 8) | b2;
    rx = b3;
    rx = (rx << 8) | b4;
    ry = b5;
    ry = (ry << 8) | b6;
  }
  simchair.setRxAxis(rx);
  simchair.setRyAxis(ry);
  simchair.setThrottle(throttle);
}

void poll_ab412_coll_head()
{
  uint8_t x, y, rx, ry, z, rz, b, s0, s1, s2;

  Wire.requestFrom(14, 7);
  while (Wire.available())
  {
    byte b1 = Wire.read(); // receive a byte as character
    byte b2 = Wire.read();
    byte b3 = Wire.read(); // receive a byte as character
    byte b4 = Wire.read();
    byte b5 = Wire.read(); // receive a byte as character
    byte b6 = Wire.read();
    byte b7 = Wire.read();

    x = b2;
    y = b1;
    rx = b4;
    ry = b3;
    z = b5;
    rz = b6;
    b = b7;
  }


  Wire.requestFrom(13, 3);
  while (Wire.available())
  {
    byte b1 = Wire.read(); // receive a byte as character
    byte b2 = Wire.read();
    byte b3 = Wire.read();

    s0 = b1;
    s1 = b2;
    s2 = b3;
  }

  if (AB412_COLL_HEAD_ROTARY_POTS == 0)
  {
    z = 0;
    rz = 0;
  }
  else if (AB412_COLL_HEAD_ROTARY_POTS == 1)
  {
    rz = 0;
  }


  int16_t hat0_val = parse_hat_sw(x, y,AB412_COLL_HEAD_LEFT_HAT_DIRECTIONS);

  simchair_aux2.setHatSwitch(0, hat0_val);

  int16_t hat1_val = parse_hat_sw(rx, ry, AB412_COLL_HEAD_RIGHT_HAT_DIRECTIONS);
  simchair_aux1.setHatSwitch(0, hat1_val);

  coll_head_parse_switches(b, 0, 2);
  coll_head_parse_switches(s0, 2, 0);
  coll_head_parse_switches(s1, 10, 0);
  coll_head_parse_switches(s2, 18, 0);



  simchair_aux2.setRxAxis(z);
  simchair_aux2.setRyAxis(rz);


}

void poll_huey_coll_head()
{
  
  uint8_t x, y, s0, s1;

  Wire.requestFrom(17, 4);
  while (Wire.available())
  {
    byte b1 = Wire.read(); // receive a byte as character
    byte b2 = Wire.read();
    byte b3 = Wire.read(); // receive a byte as character
    byte b4 = Wire.read();

    x = b2;
    y = b1;
    s0 = b3;
    s1 = b4;
  }


//  Serial.print(x);
//  Serial.print(" ");
//  Serial.print(y);
//  Serial.print(" ");
//  Serial.print(s0);
//  Serial.print(" ");
//  Serial.print(s1);
//  Serial.println();


  int16_t hat_val = parse_hat_sw(x, y,HUEY_COLL_HEAD_HAT_DIRECTIONS);
 // Serial.println(hat_val);

  simchair_aux2.setHatSwitch(0, hat_val);

  coll_head_parse_switches(s0, 0, 0);
  coll_head_parse_switches(s1, 8, 0);

}

void coll_head_parse_switches (int sw, int start_pos, int end_pos)
{
  if (end_pos == 0)
  {
    end_pos = 8 + start_pos;
  }
  for (byte i = start_pos; i < end_pos; i++)
  {
    bool v = (sw >> i - start_pos) & 1;
 //   if (AB412_COLL_HEAD_MODE_SWITCH_ENABLED == 1)
    //{
      if (dev_ab412_coll_head == 1)
      {
        if (i == AB412_COLL_HEAD_MODE_SWITCH - 1)
        {
         // Serial.println("BANG");
          if (v == 1)
          {
            coll_head_mode_sw_position = 1;
            mode_sw_pos_0 = 1;
          }
          else
          {
            mode_sw_pos_0 = 0;
          }
        }

          else if (i == (ab412_coll_head_starter_buttons[0]-1))
          {
            if ((COLL_HEAD_IDLE_STOP_COMPAT_PROFILE_XTRIDENT412 == 1) && ((coll_head_mode_sw_position == 0) || (AB412_COLL_HEAD_MODE_SWITCH == 0)))
            {
                if (v != coll_head_lastButtonState[i])
                {
                  Keyboard.write(AB412_STARTER2_SW_KEY);
                  coll_head_lastButtonState[i] = v;
                }
            }
          }
          else if (i == (ab412_coll_head_starter_buttons[1]-1))
          {
            if ((COLL_HEAD_IDLE_STOP_COMPAT_PROFILE_XTRIDENT412 == 1) && ((coll_head_mode_sw_position == 0) || (AB412_COLL_HEAD_MODE_SWITCH == 0)))
            {
                if (v != coll_head_lastButtonState[i])
                {
                  Keyboard.write(AB412_STARTER1_SW_KEY);
                  coll_head_lastButtonState[i] = v;
                }
            }
          }
          else if (i == (ab412_coll_head_idle_stop_buttons[0]-1))
          {
            if ((COLL_HEAD_IDLE_STOP_COMPAT_PROFILE_XTRIDENT412 == 1) && ((coll_head_mode_sw_position == 0) || (AB412_COLL_HEAD_MODE_SWITCH == 0)))
            {
                if (v != coll_head_lastButtonState[i])
                {
                  Keyboard.write(AB412_IDLE1_SW_KEY);
                  coll_head_lastButtonState[i] = v;
                }
            }
          }
          else if (i == (ab412_coll_head_idle_stop_buttons[1]-1))
          {
            //Serial.println("BOOM!");
            if ((COLL_HEAD_IDLE_STOP_COMPAT_PROFILE_XTRIDENT412 == 1) && ((coll_head_mode_sw_position == 0) || (AB412_COLL_HEAD_MODE_SWITCH == 0)))
            {
                if (v != coll_head_lastButtonState[i])
                {
                  //Serial.println("BOOM!");
                  Keyboard.write(AB412_IDLE2_SW_KEY);
                  coll_head_lastButtonState[i] = v;
                }
            }
          }
          
          else if (i == AB412_COLL_HEAD_MODE_SWITCH)
          {
            if (v == 1)
            {
              coll_head_mode_sw_position = 2;
            }
            else if (v == mode_sw_pos_0)
            {
              coll_head_mode_sw_position = 0;
            }
          }

         if ((COLLECTIVE_HOLD_ENABLED == 1) && ((coll_head_mode_sw_position == 0) || (AB412_COLL_HEAD_MODE_SWITCH == 0)))
         {                      
            if ((i == AB412_HEAD_COLLECTIVE_HOLD_BUTTON - 1) )
            {
              if (v != coll_head_lastButtonState[i])
              {
                if (v == 1)
                {
                  if (collective_hold_active == 0)
                  {
                    collective_hold_active = 1;
                  }
                  else if (collective_hold_active == 1)
                  {
                    collective_hold_active = 0;
                  }
                }
              }
            }
          }
        
      }
      else if (dev_huey_coll_head == 1)
      {
        if (i == HUEY_COLL_HEAD_MODE_SWITCH - 1)
        {
         // Serial.println("BANG");
          if (v == 1)
          {
            coll_head_mode_sw_position = 1;
            mode_sw_pos_0 = 1;
          }
          else
          {
            mode_sw_pos_0 = 0;
          }
        }
        else if (i == HUEY_COLL_HEAD_MODE_SWITCH)
        {
          if (v == 1)
          {
            coll_head_mode_sw_position = 2;
          }
          else if (v == mode_sw_pos_0)
          {
            coll_head_mode_sw_position = 0;
          }
        }
        if ((COLLECTIVE_HOLD_ENABLED == 1) && ((coll_head_mode_sw_position == 0) || (HUEY_COLL_HEAD_MODE_SWITCH == 0)))
        {                      
            if ((i == HUEY_HEAD_COLLECTIVE_HOLD_BUTTON - 1) )
            {
              if (v != coll_head_lastButtonState[i])
              {
                if (v == 1)
                {
                  if (collective_hold_active == 0)
                  {
                    collective_hold_active = 1;
                    coll_head_lastButtonState[i] = v;// added
                  }
                  else if (collective_hold_active == 1)
                  {
                    collective_hold_active = 0;
                    coll_head_lastButtonState[i] = v; //added
                  }
                }
              }
            }
        }
      }

    if (coll_head_sw_mode_toggle_switches_parsed[i] == 1)
    {
      if (v != coll_head_lastButtonState[i])
      { 
        if (COLL_HEAD_IDLE_STOP_COMPAT_MODE == 1)
        {
          if (dev_ab412_coll_head == 1)
          {
            for (byte j = 0; j < sizeof(ab412_coll_head_idle_stop_buttons); j++)
            {
              if (i == ab412_coll_head_idle_stop_buttons[j] - 1)
              {
                idle_stop_pressed[j] = v;           
              }
            }
          }
          else if (dev_huey_coll_head == 1)
          {
            for (byte j = 0; j < sizeof(huey_coll_head_idle_stop_buttons); j++)
            {
              if (i == huey_coll_head_idle_stop_buttons[j] - 1)
              {
                idle_stop_pressed[j] = v;           
              }
            }
          }
        }    
        long now = millis();
        if ((now - coll_head_sw_ts[i]) > COLL_HEAD_SW_HOLD_TIMEOUT)
        {
          simchair_aux2.setButton(i, v);
          //set_button_mode_aware(i,v);
          coll_head_lastButtonState[i] = v;
          coll_head_sw_ts[i] = millis();
        }        
      }
      else if ((v == 1) && (coll_head_triggerState[i] == 0))
      {
        long now = millis();
        if ((now - coll_head_sw_ts[i]) > COLL_HEAD_SW_HOLD_TIMEOUT)
        {
          simchair_aux2.setButton(i, !v);
          //set_button_mode_aware(i,!v);
          coll_head_triggerState[i] = 1;
        }
      }
      else if ((v == 0) && (coll_head_triggerState[i] == 1))
      {
        long now = millis();
        simchair_aux2.setButton(i, !v);
        //set_button_mode_aware(i,!v);
        coll_head_triggerState[i] = 0;
        coll_head_sw_ts[i] = millis();
        coll_head_lastButtonState[i] = !v;
      }
    }
    else if (coll_head_sw_mode_button_switches_parsed[i] == 1)
    {
      if (v != coll_head_lastButtonState[i])
      {
        if (COLL_HEAD_IDLE_STOP_COMPAT_MODE == 1)
        {
//          for (byte j = 0; j < sizeof(ab412_coll_head_idle_stop_buttons); j++)
//          {
//            if (i == ab412_coll_head_idle_stop_buttons[j] - 1)
//            {
//              idle_stop_pressed[j] = v;  
//            }
//          }
          if (dev_ab412_coll_head == 1)
          {
            for (byte j = 0; j < sizeof(ab412_coll_head_idle_stop_buttons); j++)
            {
              if (i == ab412_coll_head_idle_stop_buttons[j] - 1)
              {
                idle_stop_pressed[j] = v;           
              }
            }
          }
          else if (dev_huey_coll_head == 1)
          {
            for (byte j = 0; j < sizeof(huey_coll_head_idle_stop_buttons); j++)
            {
              if (i == huey_coll_head_idle_stop_buttons[j] - 1)
              {
                idle_stop_pressed[j] = v;           
              }
            }
          }
        }  
        //}
        //simchair_aux2.setButton(i, v);
        set_button_mode_aware(i,v);
        coll_head_lastButtonState[i] = v;
      }
    }
//    if (coll_head_mode_sw_position == 0)
//    {
    else if (coll_head_sw_mode_selector_button_switches_parsed[i] == 1)
      //if (coll_head_sw_mode_selector_button_switches_parsed[i] == 1)
      {          
        if (v != coll_head_lastButtonState[i])
        {
          //simchair_aux2.setButton(i, v);
          set_button_mode_aware(i,v);
          coll_head_lastButtonState[i] = v;
          //simchair_aux2.setButton(33 - i, !v);
          set_button_mode_aware(31 - i, !v);
          //coll_head_lastButtonState[33 - i] = !v;
          coll_head_triggerState[31 - i] = 0;
        }
      }
      else if (coll_head_sw_mode_selector_button_switches_parsed[i - 1] == 1)
      {      
        if (v != coll_head_lastButtonState[i])
        {
          //simchair_aux2.setButton(i, v);
          set_button_mode_aware(i,v);
          coll_head_lastButtonState[i] = v;
          //simchair_aux2.setButton(32 - i + 1, !v);
          set_button_mode_aware(31 - i + 1, !v);
          coll_head_lastButtonState[31 - i + 1] = !v;
          coll_head_triggerState[31 - i + 1] = 0;
        }
      }
      else if (coll_head_sw_mode_selector_switches_parsed[i] == 1)
      {
        if (v != coll_head_lastButtonState[i])
        {
          //Serial.println("BANG");
          simchair_aux2.setButton(i, v);
          //set_button_mode_aware(i, v);
          coll_head_lastButtonState[i] = v;
          simchair_aux2.setButton(31 - i, !v);
          //set_button_mode_aware(32 - i, !v);
          coll_head_lastButtonState[31 - i] = !v;
          //ab412_coll_head_triggerState[32 - i] = 0;
          coll_head_sw_ts[i] = millis();
          coll_head_sw_ts[31 - i] = millis();
        }
        else if ((v == 1) && (coll_head_triggerState[i] == 0))
        {
          long now = millis();
          if ((now - coll_head_sw_ts[i]) > COLL_HEAD_SW_HOLD_TIMEOUT)
          {
            simchair_aux2.setButton(i, !v);
            //set_button_mode_aware(i, !v);
            coll_head_triggerState[i] = 1;
          }
        }
        else if ((v == 0) && (coll_head_triggerState[i] == 1))
        {
          long now = millis();
          if ((now - coll_head_sw_ts[i]) > COLL_HEAD_SW_HOLD_TIMEOUT)
          {
            simchair_aux2.setButton(i, v);
            //set_button_mode_aware(i,v);
            coll_head_lastButtonState[i] = v;
            coll_head_triggerState[i] = 0;
  
            simchair_aux2.setButton(31 - i, v);
            //set_button_mode_aware(32 - i, v);
            coll_head_lastButtonState[31 - i] = v;
            coll_head_triggerState[31 - i] = 0;
          }
        }
  
      }
      else if (coll_head_sw_mode_selector_switches_parsed[i - 1] == 1)
      {
        if (v != coll_head_lastButtonState[i])
        {
          simchair_aux2.setButton(i, v);
          //set_button_mode_aware(i, v);
          coll_head_lastButtonState[i] = v;
          simchair_aux2.setButton(31 - i + 1, !v);
          //set_button_mode_aware(32 - i + 1,!v);
          coll_head_lastButtonState[31 - i + 1] = !v;
          coll_head_sw_ts[i] = millis();
          coll_head_sw_ts[31 - i + 1] = millis();
        }
        else if ((v == 1) && (coll_head_triggerState[i] == 0))
        {
          long now = millis();
          if ((now - coll_head_sw_ts[i]) > COLL_HEAD_SW_HOLD_TIMEOUT)
          {
            simchair_aux2.setButton(i, !v);
            //set_button_mode_aware(i, !v);
            coll_head_triggerState[i] = 1;
          }
        }
        else if ((v == 0) && (coll_head_triggerState[i] == 1))
        {
          long now = millis();
          if ((now - coll_head_sw_ts[i]) > COLL_HEAD_SW_HOLD_TIMEOUT)
          {
            simchair_aux2.setButton(i, v);
            //set_button_mode_aware(i, v);
            coll_head_lastButtonState[i] = v;
            coll_head_triggerState[i] = 0;
  
            simchair_aux2.setButton(31 - i + 1, v);
            //set_button_mode_aware(32 - i + 1, v);
            coll_head_lastButtonState[31 - i + 1] = v;
            coll_head_triggerState[31 - i + 1] = 0;
          }
        }
      }
 //   }
  }
}

void set_button_mode_aware (int i, int v)
{
  if (coll_head_mode_sw_position == 0)
  {
    simchair_aux2.setButton(i, v);
  }
  else if (coll_head_mode_sw_position == 1)
  {
    simchair_aux2.setButton(32 + i, v);
  }
  else if (coll_head_mode_sw_position == 2)
  {
    simchair_aux2.setButton(64 + i, v);
  }

}

int parse_hat_sw (int x, int y, byte dirs)
{
  int hat_val;
  if (dirs == 8)
  {
    if (y > 145)
    {
      if (x > 145)
      {
        hat_val = 45;
      }
      else if (x < 105)
      {
        hat_val = 315;
      }
      else
      {
        hat_val = 0;
      }
    }
    else if (y < 105)
    {
      if (x > 160)
      {
        hat_val = 135;
      }
      else if (x < 105)
      {
        hat_val = 225;
      }
      else
      {
        hat_val = 180;
      }
    }
    else if (x > 145)
    {
      hat_val = 90;
    }
    else if (x < 105)
    {
      hat_val = 270;
    }
    else
    {
      hat_val = JOYSTICK_HATSWITCH_RELEASE;
    }
  }
  else
  {
    if (y > 145)
    {
      hat_val = 0;
    }
    else if (y < 105)
    {
       hat_val = 180;
    }
    else if (x > 145)
    {
      hat_val = 90;
    }
    else if (x < 105)
    {
      hat_val = 270;
    }
    else
    {
      hat_val = JOYSTICK_HATSWITCH_RELEASE;
    }
  }
  return hat_val;
}

int parse_hat_trim (int x, int y, bool invert_x, bool invert_y)
{
  int hat_val;

  if (force_trim_position_set == 1)
  {
    int one_percent_range = ADC_RANGE / 100;
    int adj_step_x = one_percent_range * ATT_TRIM_STEP_X;
    int adj_step_y = one_percent_range * ATT_TRIM_STEP_Y;
    if (y > 145)
    {
      //hat up
      if (invert_y == 0)
      {
        force_trim_y = force_trim_y + adj_step_y;
      }
      else
      {
        force_trim_y = force_trim_y - adj_step_y;
      }
    }
    else if (y < 105)
    {
      //hat down
      if (invert_y == 0)
      {
        force_trim_y = force_trim_y - adj_step_y;
      }
      else
      {
        force_trim_y = force_trim_y + adj_step_y;
      }
    }
    else if (x > 145)
    {
      //hat right
      if (invert_x == 0)
      {
        force_trim_x = force_trim_x + adj_step_x;
      }
      else
      {
        force_trim_x = force_trim_x - adj_step_x;
      }
    }
    else if (x < 105)
    {
      //hat left
      if (invert_x == 0)
      {
        force_trim_x = force_trim_x - adj_step_x;
      }
      else
      {
        force_trim_x = force_trim_x + adj_step_x;
      }
    }
//
//          Serial.print(force_trim_x);
//      Serial.print(" ");
//      Serial.println(force_trim_y);
      simchair.setXAxis(force_trim_x);
      simchair.setYAxis(force_trim_y);
  }

    

 
}

