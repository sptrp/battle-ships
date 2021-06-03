/**
 * The description of the module.
 */

#ifndef BATTLESHIP_BATTLESHIP_H
#define BATTLESHIP_BATTLESHIP_H

#include <boost/shared_ptr.hpp>
#include <alcommon/almodule.h>
#include <string>
#include "board.h"
//#include "enemyboard.h"
#include <vector>
#include <alproxies/almemoryproxy.h>
#include <alproxies/albarcodereaderproxy.h>
#include <alproxies/altexttospeechproxy.h>
#include <alproxies/alrobotpostureproxy.h>
#include <althread/almutex.h>


namespace AL

{
  class ALBroker;
}

class Battleship : public AL::ALModule
{
  public:

   // Battleship();
    Battleship(boost::shared_ptr<AL::ALBroker> broker, const std::string& name);

    bool waitingForCoord;
    int stepOfGame;
    virtual ~Battleship();

    /** Overloading ALModule::init().
    * This is called right after the module has been loaded
    */
    virtual void init();
    void startGame();
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
    int shipLength;
    void addShipAreaToBlacklist();
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
    bool vertical;

};

#endif  // BATTLESHIP_BATTLESHIP_H
