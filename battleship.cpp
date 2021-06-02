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
#include <boost/thread/thread.hpp>

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
    functionName("onRightBumperPressed", getName(), "Method called when the left bumper is pressed. Makes a LED animation.");
    BIND_METHOD(Battleship::onRightBumperPressed);
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
  fMemoryProxy.unsubscribeToEvent("onRightBumperPressed", "Battleship");
  fMemoryProxy.unsubscribeToEvent("barcodeRecognized", "Battleship");
}

/**
  * The method being called when loading the module.
  */
void Battleship::init() {
  try {

    // create a proxy to the memory module
    fMemoryProxy = ALMemoryProxy(getParentBroker());
    // create a proxy for barcode recognition
    qrCodeProxy = ALBarcodeReaderProxy(getParentBroker());
    // create proxy to go to postures
    postureProxy = ALRobotPostureProxy(getParentBroker());
    // create proxy for talking
    fTtsProxy = ALTextToSpeechProxy(getParentBroker());
    fTtsProxy.setLanguage("German");


    /** Subscribe to events
    * Arguments:
    * - name of the event
    * - name of the module to be called for the callback
    * - name of the bound method to be called on event
    */
    fMemoryProxy.subscribeToEvent("FrontTactilTouched", "Battleship",
                                  "headTouched");

    // going to sit position when module is initialized
    qiLogInfo(moduleName) << "Trying to sit" << endl;
    postureProxy.goToPosture("Sit", 0.3);
    setGameStarted(0);
    qiLogInfo(moduleName) <<  "GameStarted: "<< getGameStarted() << std::endl;
    qiLogInfo(moduleName) <<  "Willkommen bei Schiffe Versenken!" << std::endl;
    qiLogInfo(moduleName) <<  "Versuche Board zu initialisieren." << std::endl;

  }
  catch (const AL::ALError& e) {
    qiLogError(moduleName) << e.what() << std::endl;
  }
}

/**
 * Initiate Barcode recognition
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
        qiLogInfo(moduleName) <<  "Barcodeerkennung already terminated." << std::endl;
    }
}

/**
 * Function to let Nao talk
 * @param speech is the string, which Nao will say
 */
void Battleship::NaoSpeak(string speech) {

    qiLogInfo(moduleName) << "NaoSpeak started.." << endl;

    try {
        fTtsProxy.say(speech);
    }
    catch (const AL::ALError& e) {
        qiLogError("module.example") << e.what() << std::endl;
    }

}

/**
 * Initiate foot bumper recognition
 */
void Battleship::initBumperRecognition() {

    if (!bumperInitialized) {
    fMemoryProxy.subscribeToEvent("RightBumperPressed", "Battleship",
                                  "onRightBumperPressed");
    fMemoryProxy.subscribeToEvent("LeftBumperPressed", "Battleship",
                                  "onLeftBumperPressed");
    bumperInitialized = true;
    qiLogInfo(moduleName) <<  "Bumpererkennung initialisiert." << std::endl;
    }
    else {
        qiLogInfo(moduleName) <<  "Bumper already initialized." << std::endl;
    }
}

/**
 * Stop foot bumper recognition
 */
void Battleship::terminateBumperRecognition() {
    fMemoryProxy.unsubscribeToEvent("onRightBumperPressed", "Battleship");
    fMemoryProxy.unsubscribeToEvent("onLeftBumperPressed", "Battleship");
}


/**
 * The callback method being called when a right foot bumper is pressed
 */
void Battleship::onRightBumperPressed() {
    try {
        qiLogInfo(moduleName) << "Executing callback method on right foot." << std::endl;
        fState =  fMemoryProxy.getData("RightBumperPressed");
        if (fState  > 0.5f) {
            return;
        }

        // define this for thread safety
        ALCriticalSection section(fCallbackMutex);

        if (!getWaitShipSunk()) {
        NaoSpeak("Du hast meinen rechten Fuss beruehrt, dass heisst, dass ich eins deiner Schiffe getroffen habe");
        setShipMet(1);
        setWaitShipSunk(true);
        return NaoSpeak("Habe ich auch eins deiner Schiffe versenkt?");
        }
        else {
            NaoSpeak("Ich habe also eines deiner Schiffe versenkt.");
            setWaitShipSunk(false);
            setDestroyed(1);
            qiLogInfo(moduleName) << "goForth: " << enemyBoard->goForth << std::endl;
                try {
                enemyBoard->StartAttacking(false);
                }     catch (const AL::ALError& e) {
                    qiLogError("module.example") << e.what() << std::endl;
                }
                return computerAttack();
            }
        }
    catch (const AL::ALError& e) {
        qiLogError("module.example") << e.what() << std::endl;
    }

}


/**
 * The callback method being called when a QR code is recognized.
 */
void Battleship::barcodeRecognized() {

    // define this for thread safety
    ALCriticalSection section(fCallbackMutex);

    qiLogInfo(moduleName) << "Callback: Barcode recognized" << endl;
    terminateBarcode();

    //variables
    int row;
    string var;

    // read the memory data from the event
    qState =  fMemoryProxy.getData("BarcodeReader/BarcodeDetected");
    //extract information
    field = (std::string) qState[0][0];
    field = field.substr(0,2);

    //Debug info
    qiLogInfo(moduleName) << "QR Code: " << field << endl;

    // make the robot say that an attack was started and on which field
    var = "Angriff auf Feld ";
    var += field;
    var += " erkannt!";
    NaoSpeak(var);

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
    }
    //convert recognized number as char to int
    row = (int) field.at(1) - '0';
    //array starts at 0 but children count from 1
    row = row -1;
    //set global attack coordinate variable
    setPlayerAttackRow(row);

    if (ComputerTurn(computerBoard) != 0 ) {
        NaoSpeak("Treffer, du kannst nochmal!");
        return initBarcode();
    }
    else {
        NaoSpeak("Leider daneben.");
        return startGame();
    }


}

void Battleship::headTouched() {
    try {
        qiLogInfo(moduleName) << "Executing callback method on head sensor." << std::endl;
        fState =  fMemoryProxy.getData("FrontTactilTouched");
        if (fState  > 0.5f) {
            return;
        }

        // define this for thread safety
        ALCriticalSection section(fCallbackMutex);


        NaoSpeak("Hallo. Du hast meinen Kopf beruehrt, weshalb jetzt Schiffe versenken gestartet wird!");
        NaoSpeak("Bitte greife an, in dem du ein Plaettchen in mein Gesicht haelst.");

        if (getGameStarted() == 0) {
            startGame();
        }
        else {
          qiLogInfo(moduleName) <<  "Game was already started!" << std::endl;
        }
    }
    catch (const AL::ALError& e) {
        qiLogError("module.example") << e.what() << std::endl;
    }
}

void Battleship::startGame() {
    //Game wird gestartet und initialisiert
    if (getGameStarted() == 0) {
    int oldRow;
    int oldCol;
    bool waitingForCoord = true;
    int stepOfGame = 1;
    computerBoard->RandomizeShips();
    qiLogInfo(moduleName) <<  "Ships randomisiert!" << std::endl;
    computerBoard->PrintBoard();
    qiLogInfo(moduleName) <<  "Ships printed!" << std::endl;
    setGameStarted(1);
    qiLogInfo(moduleName) << "Versuche, Funktion zu beenden." << std::endl;
    return initBarcode();
    }
    //Spiel laeuft bereits
    else {
        qiLogInfo(moduleName) << "Nao startet seinen Zug." << std::endl;
        //Nao's turn to attack
        computerAttack();
    }
}


void Battleship::computerAttack() {
    qiLogInfo(moduleName) << "Nao's turn to attack" << std::endl;
    //created attacking coordinates
    enemyBoard->RandomizeCoordinate();

    //read attacking coordinates
    qiLogInfo(moduleName) <<  "Versuche Angriff vorzulesen." << std::endl;
    qiLogInfo(moduleName) <<  "Col: " << getNaoAttackCol() << " Row: " << getNaoAttackRow() << std::endl;
    string var;
    var = "Angriff auf Feld ";
    switch (getNaoAttackCol()) {
        case 0:
            var += "A";
            break;
        case 1:
            var += "B";
            break;
        case 2:
            var += "C";
            break;
        case 3:
            var += "D";
            break;
        case 4:
            var += "E";
            break;
        case 5:
            var += "F";
            break;
        case 6:
            var += "G";
            break;
    }

    var += " ";
    var += getNaoAttackRow() + '1';
    NaoSpeak(var);

    NaoSpeak("Habe ich ein Schiff getroffen, dann druecke meinen rechten Fuss. Habe ich nichts getroffen, dann druecke den linken.");
    return initBumperRecognition();

}


/**
 * Function to ask player if Nao hit a ship

void Battleship::hitShip() {

}
 */


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
            qiLogInfo(moduleName) << "Get NaoShipCounter before: " << getNaoShipCounter() << std::endl;
            incrementNaoShipCounter();
            qiLogInfo(moduleName) << "Get NaoShipCounter after: " << getNaoShipCounter() << std::endl;
            qiLogInfo(moduleName) << " ships sank" << std::endl;
            if (getNaoShipCounter() == 6) {
                return 2;
            }
            initBarcode();
        case 1:
            qiLogInfo(moduleName) << "Bulls eye!" << std::endl;
            initBarcode();
            return 1;
        case 0:
            qiLogInfo(moduleName) << "Sorry missed" << std::endl;
            //computerAttack();
            return 0;
    }
}
