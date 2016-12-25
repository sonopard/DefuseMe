#include "pins_arduino.h"
#include "DefuseMe.h"
#include "PinArray.h"
#include "LED.h"

DefuseMeModule module;
PinArray pins(8, 9, 99/*GND*/);
LED armedLED(0);

///////////////////////////////////////////////////////////////////////////////

void setup() {
  Serial.begin (115200);
  Serial.println(F("Edison"));

  // init the module engine with SPI and random seed
  module.begin();

  // the Values we want to read from our neighbours
  IntTaggedValue snoTag = IntTaggedValue(F("SNO"));
  TaggedValue* interestingTags[1] =  {&snoTag};

  // the Values we want to send out to our neighbours
  tag *ourtags = new tag[1];
  ourtags[0] = {.name = F("ACTIVE"), .data = "true"}; // passive module =>no user interaction possible

  // creates the module description and waits for the bomb controller to send the broadcasts of the other members and start the game
  module.waitForInit(interestingTags,  1, F("ID:6911\n"
                     "VERSION:0.1\n"
                     "URL:https://defuseme.org/\n"
                     "AUTHOR:sonopard\n"
                     "DESC:Edison\n"
                     "REPO:https://github.com/defuseme/DefuseMe\n"),
                     ourtags, 1);

  // those are not needed anymore
  delete ourtags;
}

void loop() {
  pins.DoProcess(); // pins.CheckConnections()
  if (module.updateState())
  {
    // Demo...
    long countdown = module.getGameState().time;
    byte state = module.getGameState().state;
    byte strikes = module.getGameState().strikes;
    
    if (pins.IsJustChanged()) // pins.ChangeDetected()
    {
      if (pins.IsAnyGrounded())
      {
        module.trigger();
      }
      if(pins.IsConnected(0,1))
      {
        if(countdown > 1000*20) {
          module.trigger();
        } else {
          module.setMyState(0);
        }
      }
    }
  }
}

///////////////////////////////////////////////////////////////////////////////

