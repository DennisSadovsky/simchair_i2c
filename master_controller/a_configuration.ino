// This file contains all user configurable parameters. This allows you to keep your settings when updating software.


// <GLOBAL>

  #define ADS1115_RESOLUTION 15 //bits, min 12, max 15

// <CYCLIC>

  #define FSGIMBAL_INVERT_X 1
  #define FSGIMBAL_INVERT_Y 0

  #define B8_POT_MODE "HAT_SW" // HAT_SW or ANALOG
  
  #define SENS_ADJ_METHOD  "LINEAR"
  
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
  
  #define XY_FILTERING_ENABLED 0 // this decreases jitter, but increases input lag; recommended setting is 0
  
  #define PTT_KEYBOARD_PRESS 0 // set to 1 to enable keyboard combination press instead of a joystick button
  #define PTT_BUTTON 4 //1st button is 0, 4 should be PTT trigger position
  // this should press CTRL + Q
  #define PTT_KEYBOARD_KEY 'q'
  char PTT_KEYBOARD_KEY_MOD = KEY_LEFT_CTRL;
  
// </CYCLIC>

// <PEDALS>

  #define INVERT_RUDDER 0
  #define RUDDER_FILTERING_ENABLED 0
  #define CUSTOM_RUDDER_SENS 80 // this value will be used if SENS_SWITCH is disabled

// </PEDALS>

// <COLLECTIVE>

  #define COLLECTIVE_HOLD_ENABLED 1 // this will hold your collective in place until you press the button again and return the lever to the initial position
  #define HUEY_HEAD_COLLECTIVE_HOLD_BUTTON 13
  #define AB412_HEAD_COLLECTIVE_HOLD_BUTTON 2
  
  //for sims that do not support axis movement below idle stop, sends the throttle up/down key when holding idle stop and rotating throttle simultaneously
  //not 100% realistic, yet better than nothing
  #define COLL_HEAD_IDLE_STOP_COMPAT_MODE 1 // ASSIGN IDLE STOP IN_GAME WHILE IN MODE 0 (MODE SWITCH CENTERED)!
  #define COLL_HEAD_IDLE_STOP_COMPAT_PROFILE "DCS_HUEY" //DCS_HUEY or XTRIDENT412
  #define COLL_HEAD_IDLE_STOP_COMPAT_PROFILE_XTRIDENT412 1

  byte coll_head_idle_stop_compat_throttle_up_keys[] = {KEY_PAGE_DOWN,'z'};
  byte coll_head_idle_stop_compat_throttle_down_keys[] = {KEY_PAGE_UP,'x'};

  #define COLL_HEAD_DCS_HUEY_COMPAT_MODE_BUTTON_HOLD 50 // how long to hold throttle up/down buttons, adjusted depending on idle stop axis range (smaller range- bigger hold time and vice versa)
  
// </COLLECTIVE>

// <AB412 COLL HEAD>

  #define AB412_COLL_HEAD_ROTARY_POTS  2 // 0, 1 or 2
  #define AB412_COLL_HEAD_LEFT_HAT_DIRECTIONS 4
  #define AB412_COLL_HEAD_RIGHT_HAT_DIRECTIONS 8
  #define AB412_IDLE1_SW_KEY  'y'
  #define AB412_IDLE2_SW_KEY  'u'
  #define AB412_STARTER1_SW_KEY  'i'
  #define AB412_STARTER2_SW_KEY  'o'
  #define COLL_HEAD_SW_HOLD_TIMEOUT 200
  #define AB412_COLL_HEAD_MODE_SWITCH 18 // 3-way fixed switch only; first (lower number) switch button must be defined there; remove it and next button from everywhere else; set to 0 to disable MODE SWITCH

  #define TWIN_ENGINE_COLLECTIVE_IDLE_STOP_THROTTLE1_VAL 50
  #define TWIN_ENGINE_COLLECTIVE_IDLE_STOP_THROTTLE2_VAL 50

  byte ab412_coll_head_idle_stop_buttons[] = {9,10}; // put 1 or 2 buttons here as seen in joy.cpl; these keys MUST be in the ab412_sw_mode_button_switches[] array below
  byte ab412_coll_head_starter_buttons[] = {11,12};

  // AB412 switch modes
  // write joystick button numbers here as they are displayed in joy.cpl in order of increment
  byte ab412_sw_mode_button_switches[] = {1,2,9,10,11,12,17};//e.g. {1,2,9,10,11,12,13,14,17};// active when being held
  byte ab412_sw_mode_toggle_switches[] = {3,4,5,6,7,8};//e.g. {3,4,5,6,7,8,15,16};// 2-way switch mode: single button press when switch is turned to "on", one more press when switch is turned to "off"; something you can assign to a single key press; e.g. gear extend/retract
  byte ab412_sw_mode_selector_button_switches[] = {13}; //e.g. {3,5,7,15}; //3-WAY SWITCHES ONLY, FIRST BUTTON (WITH LOWER NUMBER) MUST BE GIVEN HERE; REMOVE THE SECOND BUTTON FROM EVERYWHERE ELSE FOR CORRECT OPERATION; when switch is on, button is held; when off, another button is pressed and held;
  byte ab412_sw_mode_selector_switches[] = {15};//e.g. {3,5,7,15}; //same as above, but buttons are pressed and released - e.g. landing light extend / hold / retract
  
// </AB412 COLL HEAD>

// <HUEY COLL HEAD>

  #define HUEY_COLL_HEAD_HAT_DIRECTIONS 4
  #define HUEY_COLL_HEAD_MODE_SWITCH 3 // 3-way fixed switch only; first (lower number) switch button must be defined there; remove it and next button from everywhere else; set to 0 to disable MODE SWITCH
  
  byte huey_coll_head_idle_stop_buttons[] = {11};
   
  #define COLL_HEAD_IDLE_STOP_COMPAT_TRESHOLD 5 // should be larger than jitter of your throttle pot
  #define SINGLE_ENGINE_COLLECTIVE_IDLE_STOP_AXIS_VAL 137 //find it out with uncommenting Serial.print(throttle) in poll_single_engine_collective sub, see below
  
  byte huey_sw_mode_button_switches[] = {7,8,11,12,13};//{7,8,9,12,13};// active when being held
  byte huey_sw_mode_toggle_switches[] = {5,6};//{6};// 2-way switch mode: single button press when switch is turned to "on", one more press when switch is turned to "off"; something you can assign to a single key press; e.g. gear extend/retract
  byte huey_sw_mode_selector_button_switches[] = {1,9}; //3-WAY SWITCHES ONLY, FIRST BUTTON (WITH LOWER NUMBER) MUST BE GIVEN HERE; REMOVE THE SECOND BUTTON FROM EVERYWHERE ELSE FOR CORRECT OPERATION; when switch is on, button is held; when off, 
  //another button is pressed and held; THIS MODE SUPPORTS MODE SWITCH! for normal operation, return all switches to center when switching mode, otherwise they will hold button pressed until it will be pressed again in the same mode. This may or may not be
  //what you expect, so be warned.
  byte huey_sw_mode_selector_switches[] = {}; //same as above, but buttons are pressed and released - e.g. landing light extend / hold / retract

// </HUEY COLL HEAD>

//<THROTTLE QUADRANT>
  #define THROTTLE_QUADRANT_PHYSICAL_AXIS_MAX 1006
  #define THROTTLE_QUADRANT_PHYSICAL_AXIS_MIN 10

  #define THROTTLE_QUADRANT_MAIN_AXIS_TRESHOLD 718
  #define THROTTLE_QUADRANT_SECONDARY_AXIS_TRESHOLD 763

//</THROTTLE QUADRANT>


