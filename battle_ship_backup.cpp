/**
 * The implementation of the module.
 */

#include "battleship.h"
#include "board.h"
#include "global.h"
#include "enemyboard.h"
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
using namespace glb;



Board *computerBoard = new Board(7);
EnemyBoard *enemyBoard = new EnemyBoard(7);


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



int ComputerTurn(Board *myBoard);



/**
 * Terminate Barcode recognition
 */
void Battleship::initBarcode() {

    if (getBarcodeReady() == 0) {
    fMemoryProxy.subscribeToEvent("BarcodeReader/BarcodeDetected", "Battleship",
                                  "barcodeRecognized");
    setBarcodeReady(1);
    qiLogInfo(__FUNCTION__) <<  "Barcodeerkennung initialisiert." << std::endl;
    }
    else {
        qiLogInfo(__FUNCTION__) <<  "Already initialized." << std::endl;
    }
}



/**
 * Stop Barcode recognition
 */
void Battleship::terminateBarcode() {
    if (getBarcodeReady() == 1) {
    fMemoryProxy.unsubscribeToEvent("BarcodeReader/BarcodeDetected", "Battleship");
    setBarcodeReady(0);
    qiLogInfo(__FUNCTION__) <<  "Barcodeerkennung terminiert." << std::endl;
    }
    else {
        qiLogInfo(__FUNCTION__) <<  "Already terminated." << std::endl;
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
    try {
      row = (int) field.at(1);

          qiLogInfo(moduleName) << "Row value is: " << row << endl;

  }
  catch (const ALError& e) {
    qiLogError(moduleName) << e.what() << std::endl;
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
  int col = 1;
  int row = 1;
  setPlayerAttackCol(col);
  setPlayerAttackRow(row);
  terminateBarcode();

  setEnemyTurn(ComputerTurn(computerBoard));

  if (getEnemyTurn() == 2) {

      enemyBoard->PrintBoard();
}

}

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
    fTtsProxy.say("Hallo. Du hast meinen rechten Fuß berührt, weshalb jetzt Schiffe versenken gestartet wird!");


  }
  catch (const ALError& e) {
    qiLogError(moduleName) << e.what() << std::endl;
  }

        qiLogInfo(moduleName) <<  "Initialisiere Board!" << std::endl;
                qiLogInfo(moduleName) <<  "Board initialisiert!" << std::endl;

  try {
          computerBoard->RandomizeShips();

      } catch (std::exception &e) {

          qiLogError(moduleName) << e.what() << std::endl;
      }
        qiLogInfo(moduleName) <<  "Ships randomisiert!" << std::endl;
  try {
          computerBoard->PrintBoard();

      } catch (std::exception &e) {

          qiLogError(moduleName) << e.what() << std::endl;
      }
        qiLogInfo(moduleName) <<  "Ships printed!" << std::endl;


        qiLogInfo(moduleName) <<  "Game started." << std::endl;
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


int ComputerTurn(Board *myBoard) {
    int attackEnemy;
    int shipCounter = 0;

    while (shipCounter < 6) {
        attackEnemy = myBoard->AttackField();

        switch (attackEnemy) {

            case 2:
                qiLogInfo(moduleName) << "Bulls eye!" << std::endl;
                qiLogInfo(moduleName) << "Ship sank!" << std::endl;
                shipCounter++;
                qiLogInfo(moduleName) << shipCounter << std::flush;
                qiLogInfo(moduleName) << " ships sank" << std::endl;
                if (shipCounter == 6)
                    return 2;

                break;
            case 1:
                qiLogInfo(moduleName) << "Bulls eye!" << std::endl;
                break;
            case 0:
                qiLogInfo(moduleName) << "Sorry missed" << std::endl;
                return 0;
        }
    }
}
