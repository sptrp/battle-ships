/**
 @file battleship.cpp
 @authors Ivan Ponomarev, Stefan Holzbauer
 @date 17.04.2021
 @brief Implementation-File
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
THWild_Game_Battleship::THWild_Game_Battleship(
  shared_ptr<ALBroker> broker,
  const string& name): ALModule(broker, name),
    fCallbackMutex(ALMutex::createALMutex())
{
    setModuleDescription("This module represents the game battleship and was created for Robotik2 T-18.");

    // Bindings for the moderator
    functionName("getGameName", getName(), "Method called when moderator calls it.");
    BIND_METHOD(THWild_Game_Battleship::getGameName);
    functionName("startGame", getName(), "Starts the connect four game");
    BIND_METHOD(THWild_Game_Battleship::startGame);
    functionName("onEndGame", getName(), "End procedure.");
    BIND_METHOD(THWild_Game_Battleship::onEndGame);


    // The callback methods to use when a bumper is pressed.
    functionName("headTouched", getName(), "Method called when the front head sensor is pressed. Starts a session.");
    BIND_METHOD(THWild_Game_Battleship::headTouched);
    functionName("onLeftBumperPressed", getName(), "Method called when the left bumper is pressed. Acts as a 'No' input from the player.");
    BIND_METHOD(THWild_Game_Battleship::onLeftBumperPressed);
    functionName("onRightBumperPressed", getName(), "Method called when the right bumper is pressed. Acts as a 'Yes' input from the player.");
    BIND_METHOD(THWild_Game_Battleship::onRightBumperPressed);
    functionName("barcodeRecognized", getName(), "Method called when QR Code is recognized.");
    BIND_METHOD(THWild_Game_Battleship::barcodeRecognized);
}

/**
* The destructor is being called when unloading the module.
*/
THWild_Game_Battleship::~THWild_Game_Battleship() {

    // unsubscribe the callback methods
    fMemoryProxy.unsubscribeToEvent("headTouched", "THWild_Game_Battleship");
    fMemoryProxy.unsubscribeToEvent("onLeftBumperPressed", "THWild_Game_Battleship");
    fMemoryProxy.unsubscribeToEvent("onRightBumperPressed", "THWild_Game_Battleship");
    fMemoryProxy.unsubscribeToEvent("barcodeRecognized", "THWild_Game_Battleship");

    // free memomory
    std::vector< std::vector<int> >().swap(shipList);
    delete computerBoard;
    delete enemyBoard;
}

/**
* This method is being called when loading the module, is empty because the moderator starts the game
*/
void THWild_Game_Battleship::init() {}


/**
* Method which the moderator uses to start the game
*/
void THWild_Game_Battleship::startGame() {
    try {
        // create a proxy to the memory module
        fMemoryProxy = ALMemoryProxy(getParentBroker());
        // create a proxy for barcode recognition
        qrCodeProxy = ALBarcodeReaderProxy(getParentBroker());
        // create proxy to go to postures
        postureProxy = ALRobotPostureProxy(getParentBroker());
        // create proxy for talking
        fTtsProxy = ALTextToSpeechProxy(getParentBroker());
        // set read language to German
        fTtsProxy.setLanguage("German");

        // Subscribes to the front head sensor and creates the callback to the function headTouched
        fMemoryProxy.subscribeToEvent("FrontTactilTouched", "THWild_Game_Battleship",
                                      "headTouched");
        // Subscribes to the onEndGame event from the moderator
        fMemoryProxy.subscribeToEvent("onEndGame", "THWild_Game_Battleship", "onEndGame");

        // NAO introduces the game and tells the player how to proceed
        NaoSpeak("Willkommen bei Schiffe versenken.");
        NaoSpeak("Ich werde mich jetzt erst einmal hinsetzen.");
        // going to sit position when module is initialized
        qiLogInfo(moduleName) << "Trying to sit" << endl;
        postureProxy.goToPosture("Sit", 0.3);

        NaoSpeak("Um eine Partie Schiffe versenken zu starten, beruehre bitte meinen Sensor auf der Stirn.");
        // initialize a few flags and variables
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
        // Logging information
        qiLogInfo(moduleName) <<  "Game started without error." << std::endl;

    } catch(const AL::ALError& e) {
        qiLogError(moduleName) <<  "Game started with error." << e.what() << std::endl;
    }
}

/**
 * method returns gamename for moderator application
 * @return gameName
 */
AL::ALValue THWild_Game_Battleship::getGameName(){
    std::string gameName = "Schiffe versenken";
    AL::ALValue value = AL::ALValue(gameName);
    return value;
}
/**
* Method which the moderator uses to end the game
*/
void THWild_Game_Battleship::onEndGame(){
    try {
        fMemoryProxy.unsubscribeToEvent("headTouched", "THWild_Game_Battleship");
        fMemoryProxy.unsubscribeToEvent("onLeftBumperPressed", "THWild_Game_Battleship");
        fMemoryProxy.unsubscribeToEvent("onRightBumperPressed", "THWild_Game_Battleship");
        fMemoryProxy.unsubscribeToEvent("barcodeRecognized", "THWild_Game_Battleship");
        NaoSpeak("Das Beispiel-Spiel wurde beendet!");
    } catch(const AL::ALError& e) {
        qiLogError("THWild_Game_Battleship::onEndGame: ") << e.what() << std::endl;
    }
}

/**
* This method initializes the QR code recognition
*/
void THWild_Game_Battleship::initBarcode() {

    if (getBarcodeReady() == 0) {
    fMemoryProxy.subscribeToEvent("BarcodeReader/BarcodeDetected", "THWild_Game_Battleship",
                                  "barcodeRecognized");
    setBarcodeReady(1);
    qiLogInfo(moduleName) <<  "Barcodeerkennung initialisiert." << std::endl;
    }
    else {
        qiLogInfo(moduleName) <<  "Barcoderecognition already initialized." << std::endl;
    }
}

/**
* This method terminates the QR code recognition
*/
void THWild_Game_Battleship::terminateBarcode() {
    if (getBarcodeReady() == 1) {
    fMemoryProxy.unsubscribeToEvent("BarcodeReader/BarcodeDetected", "THWild_Game_Battleship");
    setBarcodeReady(0);
    qiLogInfo(moduleName) <<  "Barcodeerkennung terminiert." << std::endl;
    }
    else {
        qiLogInfo(moduleName) <<  "Barcoderecognition already terminated." << std::endl;
    }
}

/**
 * Function to let NAO talk, not really necessary but for code clarity
 * @param speech is the string, which Nao will say
 */
void THWild_Game_Battleship::NaoSpeak(string speech) {

    try {
        fTtsProxy.say(speech);
    }
    catch (const AL::ALError& e) {
        qiLogError("module.example") << e.what() << std::endl;
    }

}

/**
* Initializes the foot bumper recognition
*/
void THWild_Game_Battleship::initBumperRecognition() {

    if (!getBumperInitialized()) {
    fMemoryProxy.subscribeToEvent("RightBumperPressed", "THWild_Game_Battleship",
                                  "onRightBumperPressed");
    fMemoryProxy.subscribeToEvent("LeftBumperPressed", "THWild_Game_Battleship",
                                  "onLeftBumperPressed");
    setBumperInitialized(true);
    qiLogInfo(moduleName) <<  "Bumpererkennung initialisiert." << std::endl;
    }
    else {
        qiLogInfo(moduleName) <<  "Bumper was already initialized." << std::endl;
    }
}

/**
* Terminates the foot bumper recognition
*/
void THWild_Game_Battleship::terminateBumperRecognition() {
    if (getBumperInitialized()) {
        fMemoryProxy.unsubscribeToEvent("onRightBumperPressed", "THWild_Game_Battleship");
        fMemoryProxy.unsubscribeToEvent("onLeftBumperPressed", "THWild_Game_Battleship");
        setBumperInitialized(false);
        qiLogInfo(moduleName) <<  "Bumper recognition terminated." << std::endl;
    }
    else {
        qiLogInfo(moduleName) <<  "Bumper recognition was already terminated." << std::endl;
    }
}

/**
* This method will be called every time the event RightBumperPressed is raised.
* @return NaoSpeak() on first call, computerAttack() when called twice after each other and returns void when NAO has won
*/
void THWild_Game_Battleship::onRightBumperPressed() {
    try {
        qiLogInfo(moduleName) << "Executing callback method on right foot." << std::endl;
        fState =  fMemoryProxy.getData("RightBumperPressed");
        if (fState  > 0.5f) {
            return;
        }

        // define this for thread safety
        ALCriticalSection section(fCallbackMutex);

        // vector which is pushed into shipList
        vector<int> coord;

        // if not waiting for shipSunk answer, do:
        if (!getWaitShipSunk()) {
            NaoSpeak("Ha! Ich habe getroffen!");
            setShipMet(1);
            setWaitShipSunk(true);
            coord.push_back(getNaoAttackCol());
            coord.push_back(getNaoAttackRow());
            shipList.push_back(coord);
            // if horizontal set shipIsHorizontal true
            if (horizontal) {
                shipIsHorizontal = true;
                qiLogInfo(moduleName) << "shipIsHorizontal true!" << std::endl;
            }
            // if vertical set shipIisVertical true
            if (vertical && !shipIsHorizontal) {
                shipIsVertical = true;
                qiLogInfo(moduleName) << "shipIsVertical true!" << std::endl;
            }
            // returns NaoSpeak() with question
            return NaoSpeak("Habe ich auch eins deiner Schiffe versenkt?");
        }
        // ship has been sunk
        else {
            // say something mean
            NaoSpeak("Du musst noch viel ueben.");
            // waiting for shipSunk to false, because new attack needs randomizer
            setWaitShipSunk(false);
            // shipDestroyed set to 1 for StartAttacking() method
            setDestroyed(1);
            // reset variables
            horizontal = false;
            shipIsHorizontal = false;
            shipIsVertical = false;
            // set shipDestroyed in enemyBoard to true
            enemyBoard->shipDestroyed = true;
            // add surrounding area to blacklist
            addShipAreaToBlacklist();
            // reset variables
            shipNotYetSunk = false;
            direction = 0;
            triggered = false;
            vertical = false;
            // start another attack
            enemyBoard->StartAttacking();

            // if all 6 ships have been sunk say Haha, Loser and return void
            if (getPlayerShipCounter() > 5) {
                NaoSpeak("Haaahaaa, du hast verloren!");
                return;
            }
            // else continue attack
            return computerAttack();
        }
    }
    catch (const AL::ALError& e) {
        qiLogError("module.example") << e.what() << std::endl;
    }
}

/**
* This method will be called every time the event LeftBumperPressed is raised.
*/
void THWild_Game_Battleship::onLeftBumperPressed() {
    try {
        qiLogInfo(moduleName) << "Executing callback method on left foot." << std::endl;
        fState =  fMemoryProxy.getData("LeftBumperPressed");
        if (fState  > 0.5f) {
            return;
        }

        // define this for thread safety
        ALCriticalSection section(fCallbackMutex);

        // if not waiting for sunk answer
        if (!getWaitShipSunk()) {
            NaoSpeak("Mist, daneben!");
            setShipMet(0);
            NaoSpeak("Du bist dran!");

            // if a special attack (more than 1 field to the hit) has been started, return to previous position
            if (!triggered) {
            switch (direction) {
                case(1):
                setNaoAttackCol(getNaoAttackCol()-1);
                qiLogInfo(moduleName) << "Attack Coor wurde zu" <<  getNaoAttackCol() << std::endl;
                break;
                case(2):
                setNaoAttackCol(getNaoAttackCol() +1);
                qiLogInfo(moduleName) << "Attack Coor wurde zu" <<  getNaoAttackCol() << std::endl;
                break;
                case(3):
                setNaoAttackRow(getNaoAttackRow() -1);
                qiLogInfo(moduleName) << "Attack Coor wurde zu" <<  getNaoAttackRow() << std::endl;
                break;
                case(4):
                setNaoAttackRow(getNaoAttackRow() + 1);
                qiLogInfo(moduleName) << "Attack Coor wurde zu" <<  getNaoAttackRow() << std::endl;
                break;
            }}
            // get ready to read qr code
            return initBarcode();

        }
        // answer to ship sunk?
        else {
            NaoSpeak("Dein Schiff wird nicht mehr lange schwimmen!");
            setWaitShipSunk(false);
            firstHit = true;
            // continue the attack
            return continueAttack();
        }
    }
    catch (const AL::ALError& e) {
        qiLogError("module.example") << e.what() << std::endl;
    }
}

/**
* This method will be called every time a QR code is regonized.
*/
void THWild_Game_Battleship::barcodeRecognized() {


    // define this for thread safety
    ALCriticalSection section(fCallbackMutex);

    qiLogInfo(moduleName) << "Callback: Barcode recognized" << endl;
    // stop qr code recognition
    terminateBarcode();

    //variables
    // int value of the row which is attacked (0 - 6)
    int row;
    // string value of the text read with the qr code
    string var;
    // int variable which stores the return value of ComputerTurn method
    int answer;

    // read and save the memory data from the event
    qState =  fMemoryProxy.getData("BarcodeReader/BarcodeDetected");
    //extract information
    field = (std::string) qState[0][0];
    field = field.substr(0,2);

    //Debug info of read qr code
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

    // set answer to return of ComputerTurn() method
    answer = ComputerTurn(computerBoard);
    // QR Code hit a ship
    if (answer == 1 ) {
        NaoSpeak("Treffer, du kannst nochmal!");
        return initBarcode();
    }
    // QR Code missed
    else if (answer == 2 ) {
        NaoSpeak("Leider daneben.");
        if (!shipNotYetSunk) {
            return computerAttack();
        }
        else {
            return continueAttack();
        }
    }
    // QR Code sank the last of NAO's ships
    else {
        NaoSpeak("Du hast gewonnen!");
        }
}

/**
* This method will be called every time the event front head sensor is raised.
*/
void THWild_Game_Battleship::headTouched() {
    try {
        qiLogInfo(moduleName) << "Executing callback method on head sensor." << std::endl;
        fState =  fMemoryProxy.getData("FrontTactilTouched");
        if (fState  > 0.5f) {
            return;
        }

        // define this for thread safety
        ALCriticalSection section(fCallbackMutex);

        // if the game has not been started before
        if (getGameStarted() == 0) {
            NaoSpeak("Hallo. Du hast meinen Kopf beruehrt, weshalb jetzt Schiffe versenken gestartet wird!");
            NaoSpeak("Bitte greife an, in dem du ein Plaettchen vor mein Gesicht haelst.");
            NaoSpeak("Meine Fragen kannst du mit Ja beantworten, in dem du meinen rechten Fuss drueckst. Fuer Nein, druecke meinen linken Fuss.");
            return startActualGame();
        }
        // game already in progress, repeat reading of attack coordinate
        else {
          qiLogInfo(moduleName) <<  "Read attack again!" << std::endl;
          return readAttack();
        }
    }
    catch (const AL::ALError& e) {
        qiLogError("module.example") << e.what() << std::endl;
    }
}
/**
* Once the game is started in the moderator, used to start a session
*/
void THWild_Game_Battleship::startActualGame() {
    // if game has not been started before.
    if (getGameStarted() == 0) {
        // set NAO's ships
        computerBoard->RandomizeShips();
        qiLogInfo(moduleName) <<  "Ships randomisiert!" << std::endl;
        // print ship coordinates in console log
        computerBoard->PrintBoard();
        qiLogInfo(moduleName) <<  "Ships printed!" << std::endl;
        // set gameStarted to 1
        setGameStarted(1);
        qiLogInfo(moduleName) << "Versuche, Funktion zu beenden." << std::endl;
        // initialize qr code scanning
        return initBarcode();
    }
    // game already in progress
    else {
        qiLogInfo(moduleName) << "Spiel laeuft bereits." << std::endl;
    }
}
/**
* This reads the attack from NAO to the player
*/
void THWild_Game_Battleship::readAttack() {
    // variable which is used to store the information which will be read
    string var;
    var = "Angriff auf Feld ";
    // translate coordinates to player coordinates (e.g. (2,2) to C3)
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
/**
*  The function which creates the attack of NAO against the player
*/
void THWild_Game_Battleship::computerAttack() {
    qiLogInfo(moduleName) << "Nao's turn to attack" << std::endl;
    // vector which is used to store the attack coordinates
    vector<int> coord;

    //creat attacking coordinates
    enemyBoard->RandomizeCoordinate();

    //add attack coordinates to blacklist
    coord.push_back(getNaoAttackCol());
    coord.push_back(getNaoAttackRow());
    enemyBoard->blacklist.push_back(coord);

    //read attack to player
    readAttack();

    NaoSpeak("Habe ich ein Schiff getroffen?");
    // initialized bumper recognition
    return initBumperRecognition();

}
/**
* This method handles the attack from the player to the NAO
* @param *myBoard the Board pointer, in which NAO saved its ships
* @return 1 for ship hit, 2 for attack missed, 4 for hit and all ships sunk (Player won)
*/
int THWild_Game_Battleship::ComputerTurn(Board *myBoard) {
    // int variable which stores if the attack was successful or not
    int attackEnemy;

    // 2 ship sunk, 1 ship hit, 0 missed
    attackEnemy = myBoard->AttackField();

    switch (attackEnemy) {
        case 2:
            NaoSpeak("Du hast ein Schiff versunken!");
            incrementNaoShipCounter();
            qiLogInfo(moduleName) << "Nao's ship counter: " << getNaoShipCounter() << std::endl;
            if (getNaoShipCounter() == 6) {
                return 4;
            }
            return 1;
        case 1:
            qiLogInfo(moduleName) << "Bulls eye!" << std::endl;
            initBarcode();
            return 1;
        case 0:
            qiLogInfo(moduleName) << "Sorry missed" << std::endl;
            return 2;
    }
}

/**
* This method is called when NAO has hit, but not yet sunk a ship. The first hit to the ship has been done already
*/
void THWild_Game_Battleship::continueAttack() {
    qiLogInfo(moduleName) << "ContinueAttack started." << std::endl;
    // vector which stores the attack
    vector<int> coord;
    shipNotYetSunk = true;
        // NAO tries to attack the player
        // it tries to go right, left, down or up from the previous hit
        // checks if coordinate is in blacklist before it can attack
        // if in blacklist, go to different direction

        //try to move one column right
        if (enemyBoard->IsWithinGrid(getNaoAttackCol() + 1, getNaoAttackRow()) && !enemyBoard->IsInBlacklist(getNaoAttackCol() + 1, getNaoAttackRow()) && !shipIsVertical) {
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
        else if (enemyBoard->IsWithinGrid(getNaoAttackCol() - 1, getNaoAttackRow()) && !enemyBoard->IsInBlacklist(getNaoAttackCol() - 1, getNaoAttackRow()) && !shipIsVertical) {
            qiLogInfo(moduleName) << "In Direction 2." << std::endl;
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

        //try to move one row down
        else if (enemyBoard->IsWithinGrid(getNaoAttackCol(), getNaoAttackRow() + 1) && !enemyBoard->IsInBlacklist(getNaoAttackCol(), getNaoAttackRow() + 1) && !shipIsHorizontal) {
            qiLogInfo(moduleName) << "In Direction 3." << std::endl;
            qiLogInfo(moduleName) << "before getRow: " << getNaoAttackRow() << std::endl;
            setNaoAttackRow(getNaoAttackRow() + 1);
            coord.push_back(getNaoAttackCol());
            coord.push_back(getNaoAttackRow());
            qiLogInfo(moduleName) << "after getRow: " << getNaoAttackRow() << std::endl;
            enemyBoard->blacklist.push_back(coord);
            readAttack();
            direction = 3;
            setWaitShipSunk(false);
            vertical = true;
            horizontal = false;
            return;
        }
        //try to move one row up
        else if (enemyBoard->IsWithinGrid(getNaoAttackCol(), getNaoAttackRow() - 1) && !enemyBoard->IsInBlacklist(getNaoAttackCol(), getNaoAttackRow() - 1) && !shipIsHorizontal) {
            qiLogInfo(moduleName) << "In Direction 4." << std::endl;
            setNaoAttackRow(getNaoAttackRow() - 1);
            coord.push_back(getNaoAttackCol());
            coord.push_back(getNaoAttackRow());
            enemyBoard->blacklist.push_back(coord);
            readAttack();
            direction = 4;
            setWaitShipSunk(false);
            vertical = true;
            horizontal = false;
            return;
        }
        // try to go two left
        else if (enemyBoard->IsWithinGrid(getNaoAttackCol() - 2, getNaoAttackRow()) && !enemyBoard->IsInBlacklist(getNaoAttackCol() - 2, getNaoAttackRow()) && !shipIsVertical) {
            qiLogInfo(moduleName) << "In Direction 5." << std::endl;
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
        // try to go three left
        else if (enemyBoard->IsWithinGrid(getNaoAttackCol() - 3, getNaoAttackRow()) && !enemyBoard->IsInBlacklist(getNaoAttackCol() - 3, getNaoAttackRow()) && !shipIsVertical) {
            qiLogInfo(moduleName) << "In Direction 6." << std::endl;
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
        // try to go two up
        else if (enemyBoard->IsWithinGrid(getNaoAttackCol(), getNaoAttackRow() - 2) && !enemyBoard->IsInBlacklist(getNaoAttackCol(), getNaoAttackRow() - 2) && !shipIsHorizontal) {
            qiLogInfo(moduleName) << "In Direction 7." << std::endl;
            setNaoAttackRow(getNaoAttackRow() - 2);
            coord.push_back(getNaoAttackCol());
            coord.push_back(getNaoAttackRow());
            enemyBoard->blacklist.push_back(coord);
            readAttack();
            setWaitShipSunk(false);
            triggered = true;
            firstHit = false;
            return;
        }
        // try to go three up
        else if (enemyBoard->IsWithinGrid(getNaoAttackCol(), getNaoAttackRow() - 3) && !enemyBoard->IsInBlacklist(getNaoAttackCol(), getNaoAttackRow() - 3) && !shipIsHorizontal) {
            qiLogInfo(moduleName) << "In Direction 8." << std::endl;
            setNaoAttackRow(getNaoAttackRow() - 3);
            coord.push_back(getNaoAttackCol());
            coord.push_back(getNaoAttackRow());
            enemyBoard->blacklist.push_back(coord);
            readAttack();
            setWaitShipSunk(false);
            triggered = true;
            firstHit = false;
            return;
        }
}

/**
* Adds the surrounding area of sunken ships to the blacklist
*/
void THWild_Game_Battleship::addShipAreaToBlacklist() {
    //vector stores the coordinates of the ship
    vector<int> coord;

    // in the shipList are all the coordinates of one ship
    // the algorithm then tries to add the surrounding 8 fields of the last coordinate in shipList to the blacklist
    // after that it pops the last coordinate until the shipList is empty
    while (!shipList.empty()) {
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                coord.clear();
                coord.push_back(shipList.back()[0]+(i-1));
                coord.push_back(shipList.back()[1]+(j-1));
                // checks if the surrounding coordinates are in the grid and not already in the blacklist
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
