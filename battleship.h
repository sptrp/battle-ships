/**
 * The description of Battleship and its methods.
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

class THWild_Game_Battleship  : public AL::ALModule
{
  public:

   // Battleship();
    THWild_Game_Battleship(boost::shared_ptr<AL::ALBroker> broker, const std::string& name);

    bool waitingForCoord;
    int stepOfGame;

    virtual ~THWild_Game_Battleship();

    /** Overloading ALModule::init().
    * This is called right after the module has been loaded
    */
    AL::ALValue getGameName();
    void startGame();
    void onEndGame();
    virtual void init();
    void startActualGame();
    void NaoSpeak(std::string speech);
    void computerAttack();
    void initBumperRecognition();
    void terminateBumperRecognition();
    void onRightBumperPressed();
    int ComputerTurn(Board *myBoard);
    void initBarcode();
    void terminateBarcode();




  private:
    void continueAttack();
    void readAttack();
    void addShipAreaToBlacklist();
    bool firstHit;
    bool shipNotYetSunk;
    bool horizontal;
    bool shipIsHorizontal;
    bool vertical;
    bool shipIsVertical;
    bool triggered;
    int direction;
    /**
    * This method will be called every time a bar code is regonized.
    */
    void barcodeRecognized();
    std::vector< std::vector<int> > shipList;
    /**
    * This method will be called every time the event RightBumperPressed is raised.
    */
    void headTouched();

    /**
    * This method will be called every time the event LeftBumperPressed is raised.
    */
    void onLeftBumperPressed();

    AL::ALMemoryProxy fMemoryProxy;
    AL::ALBarcodeReaderProxy qrCodeProxy;
    AL::ALTextToSpeechProxy fTtsProxy;
    AL::ALRobotPostureProxy postureProxy;
    boost::shared_ptr<AL::ALMutex> fCallbackMutex;

    float fState;
    AL::ALValue qState;
    std::string field;

};

#endif  // THWILD_GAME_BATTLESHIP_H
