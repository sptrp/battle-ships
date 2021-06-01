/**
 * The implementation of the module.
 */

#include "battleship.h"
#include "board.h"
#include "global.h"
#include "enemyboard.h"
#include <string>
#include <iostream>
#include <unistd.h>
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
using namespace glb;

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
  functionName("headTouched", getName(), "Method called when the right bumper is pressed. Makes a LED animation.");
  BIND_METHOD(Battleship::headTouched);

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
  fMemoryProxy.unsubscribeToEvent("headTouched", "Battleship");
  fMemoryProxy.unsubscribeToEvent("onLeftBumperPressed", "Battleship");
  fMemoryProxy.unsubscribeToEvent("barcodeRecognized", "Battleship");
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
    fMemoryProxy.subscribeToEvent("FrontTactilTouched", "Battleship",
                                  "headTouched");
    fMemoryProxy.subscribeToEvent("LeftBumperPressed", "Battleship",
                                  "onLeftBumperPressed");
   // fMemoryProxy.subscribeToEvent("BarcodeReader/BarcodeDetected", "Battleship",
   //                              "barcodeRecognized");

    qiLogInfo(moduleName) << "Trying to sit" << endl;
    postureProxy.goToPosture("Sit", 0.3);

    qiLogInfo(moduleName) <<  "Willkommen bei Schiffe Versenken!" << std::endl;
    qiLogInfo(moduleName) <<  "Versuche Board zu initialisieren." << std::endl;

  }
  catch (const AL::ALError& e) {
    qiLogError(moduleName) << e.what() << std::endl;
  }
}



/**
 * Terminate Barcode recognition
 */
void Battleship::initBarcode() {

    if (getBarcodeReady() == 0) {
    fMemoryProxy.subscribeToEvent("BarcodeReader/BarcodeDetected", "Battleship",
                                  "barcodeRecognized");
    setBarcodeReady(1);
    qiLogInfo(moduleName) <<  "Barcodeerkennung initialisiert." << std::endl;
    }
    else {
        qiLogInfo(moduleName) <<  "Already initialized." << std::endl;
    }
}



/**
 * Stop Barcode recognition
 */
void Battleship::terminateBarcode() {
    if (getBarcodeReady() == 1) {
    fMemoryProxy.unsubscribeToEvent("BarcodeReader/BarcodeDetected", "Battleship");
    setBarcodeReady(0);
    qiLogInfo(moduleName) <<  "Barcodeerkennung terminiert." << std::endl;
    }
    else {
        qiLogInfo(moduleName) <<  "Already terminated." << std::endl;
    }
}





/**
 * The callback method being called when a QR code is recognized.
 */
void Battleship::barcodeRecognized() {
  qiLogInfo(moduleName) << "Callback: Barcode recognized" << endl;
  terminateBarcode();
  int row;

  // define this for thread safety
  ALCriticalSection section(fCallbackMutex);

  // read the memory data from the event
  qState =  fMemoryProxy.getData("BarcodeReader/BarcodeDetected");
  //extract information
  field = (std::string) qState[0][0];
  field = field.substr(0,2);

  try {
    // create a proxy to text to speech
    fTtsProxy = ALTextToSpeechProxy(getParentBroker());
    fTtsProxy.setLanguage("German");

    // make the robot say that an attack was started and on which field
    string var;
    var = "Angriff auf Feld ";
    var += field;
    var += " erkannt!";
    fTtsProxy.say(var);
    //usleep(1500);
  }
  catch (const ALError& e) {
    qiLogError(moduleName) << e.what() << std::endl;
  }
  //Debug info
  qiLogInfo(moduleName) << "QR Code scanned. Feld: " << field << endl;

  //Translate qr code to coordinates for attacking a ship
  switch (field.at(0)) {
  case 'A':
      setPlayerAttackCol(0);
      break;
  case 'B':
      setPlayerAttackCol(1);
      break;
  case 'C':
      setPlayerAttackCol(2);
      break;
  case 'D':
      setPlayerAttackCol(3);
      break;
  case 'E':
      setPlayerAttackCol(4);
      break;
  case 'F':
      setPlayerAttackCol(5);
      break;
  case 'G':
      setPlayerAttackCol(6);
      break;
  default:
      qiLogInfo(moduleName) << "Default case in barcode recognition, so something must have gone terribly wrong. Recognized field: " << field.at(1) << endl;
    }
  //convert recognized number as char to int
  row = (int) field.at(1) - '0';
  //array starts a 0 but children count from 1
  row = row -1;
  //set global attack coordinate variable
  setPlayerAttackRow(row);
  //actually attack the coordinate (enemy = player, who is playing against Nao)
  setEnemyTurn(ComputerTurn(computerBoard));

  if (getEnemyTurn() == 2) {

      enemyBoard->PrintBoard();
}

}

void Battleship::headTouched() {
  qiLogInfo(moduleName) << "Executing callback method on right bumper event" << endl;

  // define this for thread safety
  ALCriticalSection section(fCallbackMutex);

  // read the memory data from the event and check if the bumper was pressed
  fState =  fMemoryProxy.getData("FrontTactilTouched");
  if (fState  > 0.5f) {
      qiLogInfo(moduleName) << "Right-Bumper value is: " << fState << endl;
      return;
  }

  try {

    // create a proxy to text to speech
    fTtsProxy = ALTextToSpeechProxy(getParentBroker());
    fTtsProxy.setLanguage("German");

    // make the robot say that the bumper was pressed
    fTtsProxy.say("Hallo. Du hast meinen Kopf beruehrt, weshalb jetzt Schiffe versenken gestartet wird!");


  }
  catch (const ALError& e) {
    qiLogError(moduleName) << e.what() << std::endl;
  }

  if (getGameStarted() != 1) {
      startGame();
  }
  else {
      qiLogInfo(moduleName) <<  "Game was already started!" << std::endl;
  }

}
void Battleship::startGame() {

    computerBoard->RandomizeShips();
    qiLogInfo(moduleName) <<  "Ships randomisiert!" << std::endl;
    computerBoard->PrintBoard();
    qiLogInfo(moduleName) <<  "Ships printed!" << std::endl;
    initBarcode();

        /**
        while (enemyBoard->shipCounter < 6) {
                // Enemy turn
                while (getEnemyTurn() != 0) {

                    initBarcode();


                       //delete enemyBoard;
                       // delete computerBoard;

                        //return 0;
                    }
                }

                // Computer turn
                qiLogInfo(moduleName) << "My turn" << std::endl;

                if (!enemyBoard->goForth) {

                    enemyBoard->StartAttacking(false);
                } else {

                    enemyBoard->StartAttacking(true);
                }

                // Reset enemy turn
                setEnemyTurn(-1);
                **/
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


int Battleship::ComputerTurn(Board *myBoard) {
    int attackEnemy;

    attackEnemy = myBoard->AttackField();

    switch (attackEnemy) {

        case 2:
            qiLogInfo(moduleName) << "Bulls eye!" << std::endl;
            qiLogInfo(moduleName) << "Ship sank!" << std::endl;
            incrementNaoShipCounter();
            qiLogInfo(moduleName) << getNaoShipCounter() << std::flush;
            qiLogInfo(moduleName) << " ships sank" << std::endl;
            if (getNaoShipCounter() == 6) {
                return 2;
            }
            initBarcode();
        case 1:
            qiLogInfo(moduleName) << "Bulls eye!" << std::endl;
            initBarcode();
            break;
        case 0:
            qiLogInfo(moduleName) << "Sorry missed" << std::endl;
            return 0;
    }
}
