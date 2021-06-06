/**
 @file battleship.h
 @authors Ivan Ponomarev, Stefan Holzbauer
 @date 17.04.2021
 @brief Header-File
*/

#ifndef THWILD_GAME_BATTLESHIP_H
#define THWILD_GAME_BATTLESHIP_H

#include <boost/shared_ptr.hpp>
#include <alcommon/almodule.h>
#include <string>

#include "board.h"
#include <vector>
#include <alproxies/almemoryproxy.h>
#include <alproxies/albarcodereaderproxy.h>
#include <alproxies/altexttospeechproxy.h>
#include <alproxies/alrobotpostureproxy.h>
#include <althread/almutex.h>
#include <alproxies/alledsproxy.h>


namespace AL

{
  class ALBroker;
}

/**
* This class handles the battleship game
*/
class THWild_Game_Battleship  : public AL::ALModule {
  public:
    /**
    * The constructor which sets up the module with its description and methods.
    */
    THWild_Game_Battleship(boost::shared_ptr<AL::ALBroker> broker, const std::string& name);
    /**
    * The destructor is being called when unloading the module.
    */
    virtual ~THWild_Game_Battleship();
    /**
    * This method is being called when loading the module.
    */
    virtual void init();
    /**
    * Set game name for moderator
    */
    AL::ALValue getGameName();
    /**
    * Method which the moderator uses to start the game
    */
    void startGame();
    /**
    * Method which the moderator uses to end the game
    */
    void onEndGame();

  private:
    // Methods
    /**
    * Once the game is started in the moderator, used to start a session
    */
    void startActualGame();
    /**
    * Function which handles the TexttoSpeech
    */
    void NaoSpeak(std::string speech);
    /**
    *  The function which creates the attack of NAO against the player
    */
    void computerAttack();
    /**
    * Initializes the foot bumper recognition
    */
    void initBumperRecognition();
    /**
    * Terminates the foot bumper recognition
    */
    void terminateBumperRecognition();
    /**
    * This method will be called every time the event RightBumperPressed is raised.
    */
    void onRightBumperPressed();
    /**
    * This method will be called every time the event LeftBumperPressed is raised.
    */
    void onLeftBumperPressed();
    /**
    * This method handles the attack from the player to the NAO
    * @param *myBoard the Board, in which NAO saved its ships
    */
    int ComputerTurn(Board *myBoard);
    /**
    * This method initializes the QR code recognition
    */
    void initBarcode();
    /**
    * This method terminates the QR code recognition
    */
    void terminateBarcode();
    /**
    * This method is called when NAO has hit, but not yet sunk a ship. The first hit to the ship has been done already
    */
    void continueAttack();
    /**
    * This reads the attack from NAO to the player
    */
    void readAttack();
    /**
    * Adds the surrounding area of sunken ships to the blacklist
    */
    void addShipAreaToBlacklist();
    /**
    * This method will be called every time the event front head sensor is raised.
    */
    void headTouched();
    /**
    * This method will be called every time a QR code is regonized.
    */
    void barcodeRecognized();

    // Variables
    /**
    * boolean variable which turn true if the first hit to a ship has been set and turns false, once a ship is sunk
    */
    bool firstHit;
    /**
    * boolean variable which has the state, if the ship is still floating (if true, ship still on water)
    */
    bool shipNotYetSunk;
    /**
    * boolean variable which turn true when a horizontal attack has been started
    */
    bool horizontal;
    /**
    * boolean variable which turns true if the horizontal attack actually hit a ship
    */
    bool shipIsHorizontal;
    /**
    * boolean variable which turn true when a vertical attack has been started
    */
    bool vertical;
    /**
    * boolean variable which turns true if the vertical attack actually hit a ship
    */
    bool shipIsVertical;
    /**
    * boolean variable which is true, when a special attack (more than 1 field to the hit) has been started
    */
    bool triggered;
    /**
    * int variable which is the direction of the attack (1 for right, 2 for left, 3 for down and 4 for up)
    */
    int direction;
    /**
    * vector within vector which saves the coordinates of the attack ship and whill be popped once the ship is sunk
    */
    std::vector< std::vector<int> > shipList;

    /**
    * Memory proxy for event subscription and data access.
    */
    AL::ALMemoryProxy fMemoryProxy;
    /**
    * QR proxy for recognizing QR codes with the camera
    */
    AL::ALBarcodeReaderProxy qrCodeProxy;
    /**
    * TextToSpeech proxy to read text out loud
    */
    AL::ALTextToSpeechProxy fTtsProxy;
    /**
    * posture proxy to go into predefined positions
    */
    AL::ALRobotPostureProxy postureProxy;
    /**
    * ALMUtex
    */
    boost::shared_ptr<AL::ALMutex> fCallbackMutex;

    /**
    * float which saves the information of the sensors if pressed (between 0 and 1)
    */
    float fState;
    /**
    * ALValue variable which saves the information of the recognized QR Code
    */
    AL::ALValue qState;
    /**
    * String variable which has the QR code text of the attacked coordinate
    */
    std::string field;

};

#endif  // THWILD_GAME_BATTLESHIP_H
