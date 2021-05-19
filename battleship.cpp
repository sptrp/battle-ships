/**
 * The implementation of the module.
 */

#include "battleship.h"
#include <string>
#include <iostream>
#include <alvalue/alvalue.h>
#include <alcommon/alproxy.h>
#include <alcommon/albroker.h>
#include <qi/log.hpp>
#include <althread/alcriticalsection.h>
#include <alproxies/albarcodereaderproxy.h>
#include <alproxies/alrobotpostureproxy.h>

using namespace AL;
using namespace boost;
using namespace std;

const char* moduleName = "Battleship";

/**
 * The constructor which sets up the module with its description and methods.
 */
Battleship::Battleship(
  shared_ptr<ALBroker> broker,
  const string& name): ALModule(broker, name),
    fCallbackMutex(ALMutex::createALMutex())
{
  setModuleDescription("This module presents how to subscribe to a simple event (here RightBumperPressed and LeftBumperPressed) and use a callback method.");

  // The callback methods to use when a bumper is pressed.
  functionName("onRightBumperPressed", getName(), "Method called when the right bumper is pressed. Makes a LED animation.");
  BIND_METHOD(Battleship::onRightBumperPressed);

  functionName("onLeftBumperPressed", getName(), "Method called when the left bumper is pressed. Makes a LED animation.");
  BIND_METHOD(Battleship::onLeftBumperPressed);

  functionName("barcodeRecognized", getName(), "Method called when QR Code is recognized.");
  BIND_METHOD(Battleship::barcodeRecognized);
}

/**
 * The method being called when unloading the module.
 */
Battleship::~Battleship() {
    // unsubscribe the callback methods
  fMemoryProxy.unsubscribeToEvent("onRightBumperPressed", "Battleship");
  fMemoryProxy.unsubscribeToEvent("onLeftBumperPressed", "Battleship");
  fMemoryProxy.unsubscribeToEvent("BarcodeReader/BarcodeDetected", "Battleship");
}

/**
  * The method being called when loading the module.
  */
void Battleship::init() {
  try {

    // create a proxy to the memory module
    fMemoryProxy = ALMemoryProxy(getParentBroker());
    qrCodeProxy = ALBarcodeReaderProxy(getParentBroker());
    postureProxy = ALRobotPostureProxy(getParentBroker());

    /** Subscribe to events
    * Arguments:
    * - name of the event
    * - name of the module to be called for the callback
    * - name of the bound method to be called on event
    */
    fMemoryProxy.subscribeToEvent("RightBumperPressed", "Battleship",
                                  "onRightBumperPressed");
    fMemoryProxy.subscribeToEvent("LeftBumperPressed", "Battleship",
                                  "onLeftBumperPressed");
    fMemoryProxy.subscribeToEvent("BarcodeReader/BarcodeDetected", "Battleship",
                                  "barcodeRecognized");

    qiLogInfo(moduleName) << "Trying to sit" << endl;
    qiLogInfo(moduleName) << postureProxy.getPostureList() << endl;
    postureProxy.goToPosture("Sit", 0.3);

  }
  catch (const AL::ALError& e) {
    qiLogError(moduleName) << e.what() << std::endl;
  }
}


/**
 * The callback method being called when a QR code is recognized.
 */
void Battleship::barcodeRecognized() {
  qiLogInfo(moduleName) << "Executing callback method on barcode recognized event" << endl;

  // define this for thread safety
  ALCriticalSection section(fCallbackMutex);

  // read the memory data from the event and check if the bumper was pressed
  qState =  fMemoryProxy.getData("BarcodeReader/BarcodeDetected");
  field = (std::string) qState[0][0];
  field = field.substr(0,2);

  qiLogInfo(moduleName) << "QR Code scanned. Feld: " << field << endl;

  for (unsigned i=0; i<field.length(); ++i)
    {
      qiLogInfo(moduleName) << "Field str length: "  << field.at(i) << endl;
    }

  if (field == "A1") {
      fTtsProxy = ALTextToSpeechProxy(getParentBroker());
      fTtsProxy.setLanguage("German");

      // make the robot say that the bumper was pressed
      fTtsProxy.say("A1 erkannt. A1 erkannt.");
  }
  else {

      try {

        // create a proxy to text to speech
        fTtsProxy = ALTextToSpeechProxy(getParentBroker());
        fTtsProxy.setLanguage("German");

        // make the robot say that the bumper was pressed
        fTtsProxy.say("Hallo. Barcode erkannt.");
      }
      catch (const ALError& e) {
        qiLogError(moduleName) << e.what() << std::endl;
      }
  }
}

/**
 * The callback method being called when the RightBumperPressed event occured.
 */
void Battleship::onRightBumperPressed() {
  qiLogInfo(moduleName) << "Executing callback method on right bumper event" << endl;

  // define this for thread safety
  ALCriticalSection section(fCallbackMutex);

  // read the memory data from the event and check if the bumper was pressed
  fState =  fMemoryProxy.getData("RightBumperPressed");
  if (fState  > 0.5f) {
      qiLogInfo(moduleName) << "Right-Bumper value is: " << fState << endl;
      return;
  }

  try {

    // create a proxy to text to speech
    fTtsProxy = ALTextToSpeechProxy(getParentBroker());
    fTtsProxy.setLanguage("German");

    // make the robot say that the bumper was pressed
    fTtsProxy.say("Hallo. Du hast meinen rechten Fuß berührt");
  }
  catch (const ALError& e) {
    qiLogError(moduleName) << e.what() << std::endl;
  }
}


/**
 * The callback method being called when the RightBumperPressed event occured.
 */
void Battleship::onLeftBumperPressed() {
    qiLogInfo(moduleName) << "Executing callback method on left bumper event" << std::endl;

  // define this for thread safety
  ALCriticalSection section(fCallbackMutex);

  // read the memory data from the event and check if the bumper was pressed
  fState =  fMemoryProxy.getData("LeftBumperPressed");
  if (fState  > 0.5f) {
      qiLogInfo(moduleName) << "Left-Bumper value is: " << fState << endl;
      return;
  }
  try {
    // create a proxy to text to speech
    fTtsProxy = ALTextToSpeechProxy(getParentBroker());

    // make the robot say that the bumper was pressed
    fTtsProxy.say("Hallo. Du hast meinen linken Fuß berührt");
  }
  catch (const ALError& e) {
    qiLogError(moduleName) << e.what() << std::endl;
  }
}
