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
    vertical = false;
    firstHit = false;
    shipNotYetSunk = false;
    direction = 0;
    horizontal = false;
    shipIsHorizontal = false;
    vertical = false;
    shipIsVertical = false;
    triggered = false;
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

    if (!getBumperInitialized()) {
    fMemoryProxy.subscribeToEvent("RightBumperPressed", "Battleship",
                                  "onRightBumperPressed");
    fMemoryProxy.subscribeToEvent("LeftBumperPressed", "Battleship",
                                  "onLeftBumperPressed");
    setBumperInitialized(true);
    qiLogInfo(moduleName) <<  "Bumpererkennung initialisiert." << std::endl;
    }
    else {
        qiLogInfo(moduleName) <<  "Bumper was already initialized." << std::endl;
    }
}

/**
 * Stop foot bumper recognition
 */
void Battleship::terminateBumperRecognition() {
    if (getBumperInitialized()) {
        fMemoryProxy.unsubscribeToEvent("onRightBumperPressed", "Battleship");
        fMemoryProxy.unsubscribeToEvent("onLeftBumperPressed", "Battleship");
        setBumperInitialized(false);
        qiLogInfo(moduleName) <<  "Bumper recognition terminated." << std::endl;
    }
    else {
        qiLogInfo(moduleName) <<  "Bumper recognition was already terminated." << std::endl;
    }
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

        // vector which pushes back into shipList
        vector<int> coord;



        if (!getWaitShipSunk()) {
            NaoSpeak("Ha! Ich habe getroffen!");
            setShipMet(1);
            setWaitShipSunk(true);
            coord.push_back(getNaoAttackCol());
            coord.push_back(getNaoAttackRow());
            shipList.push_back(coord);
            if (horizontal) {
                shipIsHorizontal = true;
            }
            return NaoSpeak("Habe ich auch eins deiner Schiffe versenkt?");
        }
        else {
            NaoSpeak("Du musst noch viel ueben.");
            setWaitShipSunk(false);
            setDestroyed(1);
            horizontal = false;
            shipIsHorizontal = false;
            shipIsVertical = false;
            enemyBoard->shipDestroyed = true;
            addShipAreaToBlacklist();
            shipNotYetSunk = false;
            direction = 0;
            triggered = false;
            vertical = false;
            enemyBoard->StartAttacking(false);

            // Escape attacking program if ship limit reached TODO: change to 6
            if (getPlayerShipCounter() > 5) {
                NaoSpeak("Haaahaaa, du hast verloren!");
                return;
            }
            return computerAttack();
        }
    }
    catch (const AL::ALError& e) {
        qiLogError("module.example") << e.what() << std::endl;
    }
}

/**
 * The callback method being called when the LeftBumperPressed event occured.
 */
void Battleship::onLeftBumperPressed() {
    try {
        qiLogInfo(moduleName) << "Executing callback method on left foot." << std::endl;
        fState =  fMemoryProxy.getData("LeftBumperPressed");
        if (fState  > 0.5f) {
            return;
        }

        // define this for thread safety
        ALCriticalSection section(fCallbackMutex);

        if (!getWaitShipSunk()) {
            NaoSpeak("Mist, daneben!");
            setShipMet(0);
            NaoSpeak("Du bist dran!");
            if (!triggered) {
            switch (direction) {
                case(1):
                setNaoAttackCol(getNaoAttackCol()-1);
                break;
                case(2):
                setNaoAttackCol(getNaoAttackCol()+1);
                break;
                case(3):
                //setNaoAttackRow(getNaoAttackRow());
                break;
                case(4):
                //setNaoAttackRow(getNaoAttackRow()+1);
                break;
            }}
            return initBarcode();

        }
        else {
            NaoSpeak("Dein Schiff wird nicht mehr lange schwimmen!");
            setWaitShipSunk(false);
            setDestroyed(0);
            firstHit = true;

            return continueAttack();
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
        if (!shipNotYetSunk) {
            return computerAttack();
        }
        else {
            return continueAttack();
        }
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

        if (getGameStarted() == 0) {
            NaoSpeak("Hallo. Du hast meinen Kopf beruehrt, weshalb jetzt Schiffe versenken gestartet wird!");
            NaoSpeak("Bitte greife an, in dem du ein Plaettchen in mein Gesicht haelst.");
            return startGame();
        }
        else {
          qiLogInfo(moduleName) <<  "Read attack again!" << std::endl;
          return readAttack();
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
        qiLogInfo(moduleName) << "Spiel laeuft bereits." << std::endl;
        //Nao's turn to attack
        //computerAttack();
    }
}

void Battleship::readAttack() {
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
}

void Battleship::computerAttack() {
    qiLogInfo(moduleName) << "Nao's turn to attack" << std::endl;

    vector<int> coord;

    //created attacking coordinates
    enemyBoard->RandomizeCoordinate();

    //add coordinates to blacklist
    coord.push_back(getNaoAttackCol());
    coord.push_back(getNaoAttackRow());
    enemyBoard->blacklist.push_back(coord);

    //read attack to player
    readAttack();


    //NaoSpeak("Habe ich ein Schiff getroffen, dann druecke meinen rechten Fuss. Habe ich nichts getroffen, dann druecke den linken.");
    NaoSpeak("Habe ich ein Schiff getroffen?");
    return initBumperRecognition();

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
/**
 * Function which is called if a ship was hit but not destroyed
 */
void Battleship::continueAttack() {
    qiLogInfo(moduleName) << "ContinueAttack started." << std::endl;
    vector<int> coord;
    shipNotYetSunk = true;
    if (!vertical) {
        //try to move one column right
        if (enemyBoard->IsWithinGrid(getNaoAttackCol() + 1, getNaoAttackRow()) && !enemyBoard->IsInBlacklist(getNaoAttackCol() + 1, getNaoAttackRow())) {
            setNaoAttackCol(getNaoAttackCol() + 1);
            coord.push_back(getNaoAttackCol());
            coord.push_back(getNaoAttackRow());
            enemyBoard->blacklist.push_back(coord);
            readAttack();
            setWaitShipSunk(false);
            direction = 1;
            horizontal = true;
            return;
        }
        //try to move one column left
        else if (enemyBoard->IsWithinGrid(getNaoAttackCol() - 1, getNaoAttackRow()) && !enemyBoard->IsInBlacklist(getNaoAttackCol() - 1, getNaoAttackRow())) {
            setNaoAttackCol(getNaoAttackCol() - 1);
            coord.push_back(getNaoAttackCol());
            coord.push_back(getNaoAttackRow());
            enemyBoard->blacklist.push_back(coord);
            readAttack();
            setWaitShipSunk(false);
            direction = 2;
            horizontal = true;
            return;
        }
        else {
            vertical = true;
            return continueAttack();
        }
    }
    else {
        //try to move one row down
        if (enemyBoard->IsWithinGrid(getNaoAttackCol(), getNaoAttackRow() + 1) && !enemyBoard->IsInBlacklist(getNaoAttackCol(), getNaoAttackRow() + 1) && !shipIsHorizontal) {
            setNaoAttackRow(getNaoAttackRow() + 1);
            coord.push_back(getNaoAttackCol());
            coord.push_back(getNaoAttackRow());
            enemyBoard->blacklist.push_back(coord);
            readAttack();
            direction = 3;
            setWaitShipSunk(false);
            shipIsVertical = true;
            return;
        }
        //try to move one row up
        else if (enemyBoard->IsWithinGrid(getNaoAttackCol(), getNaoAttackRow() - 1) && !enemyBoard->IsInBlacklist(getNaoAttackCol(), getNaoAttackRow() - 1) && !shipIsHorizontal) {
            setNaoAttackRow(getNaoAttackRow() - 1);
            coord.push_back(getNaoAttackCol());
            coord.push_back(getNaoAttackRow());
            enemyBoard->blacklist.push_back(coord);
            readAttack();
            direction = 4;
            setWaitShipSunk(false);
            shipIsVertical = true;
            return;
        }
        else if (enemyBoard->IsWithinGrid(getNaoAttackCol() - 2, getNaoAttackRow()) && !enemyBoard->IsInBlacklist(getNaoAttackCol() - 2, getNaoAttackRow()) && !shipIsVertical) {
            //Schiff ist horizontal, aber der Angriff muss weiter links.
            setNaoAttackCol(getNaoAttackCol() - 2);
            coord.push_back(getNaoAttackCol());
            coord.push_back(getNaoAttackRow());
            enemyBoard->blacklist.push_back(coord);
            readAttack();
            setWaitShipSunk(false);
            triggered = true;
            firstHit = false;
            return;
        }
        else if (enemyBoard->IsWithinGrid(getNaoAttackCol() - 3, getNaoAttackRow()) && !enemyBoard->IsInBlacklist(getNaoAttackCol() - 3, getNaoAttackRow()) && !shipIsVertical) {
            setNaoAttackCol(getNaoAttackCol() - 3);
            coord.push_back(getNaoAttackCol());
            coord.push_back(getNaoAttackRow());
            enemyBoard->blacklist.push_back(coord);
            readAttack();
            setWaitShipSunk(false);
            triggered = true;
            firstHit = false;
            return;
        }
        else if (enemyBoard->IsWithinGrid(getNaoAttackCol(), getNaoAttackRow() - 2) && !enemyBoard->IsInBlacklist(getNaoAttackCol(), getNaoAttackRow() - 2)) {
            setNaoAttackCol(getNaoAttackCol());
            coord.push_back(getNaoAttackCol() - 2);
            coord.push_back(getNaoAttackRow());
            enemyBoard->blacklist.push_back(coord);
            readAttack();
            setWaitShipSunk(false);
            triggered = true;
            firstHit = false;
            return;
        }
        else if (enemyBoard->IsWithinGrid(getNaoAttackCol(), getNaoAttackRow() - 3) && !enemyBoard->IsInBlacklist(getNaoAttackCol(), getNaoAttackRow() - 3)) {
            setNaoAttackCol(getNaoAttackCol());
            coord.push_back(getNaoAttackCol() - 3);
            coord.push_back(getNaoAttackRow());
            enemyBoard->blacklist.push_back(coord);
            readAttack();
            setWaitShipSunk(false);
            triggered = true;
            firstHit = false;
            return;
        }
    }
}

/**
 * Function which adds 1 area buffer to blacklist for sunken ships
 */
void Battleship::addShipAreaToBlacklist() {
    vector<int> coord;

    while (!shipList.empty()) {
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                coord.clear();
                coord.push_back(shipList.back()[0]+(i-1));
                coord.push_back(shipList.back()[1]+(j-1));
                if (!enemyBoard->IsInBlacklist(coord[0], coord[1]) && enemyBoard->IsWithinGrid(coord[0], coord[1])) {
                    enemyBoard->blacklist.push_back(coord);
                    qiLogInfo(moduleName) << "Added col: " << coord[0] << " : " << coord[1] << std::endl;
                }

            }

        }
        shipList.pop_back();
    }
    qiLogInfo(moduleName) << "addShipAreaToBlacklist stops" << std::endl;
}
