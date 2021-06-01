/**
 * The description of the module.
 */

#ifndef BATTLESHIP_BATTLESHIP_H
#define BATTLESHIP_BATTLESHIP_H

#include <boost/shared_ptr.hpp>
#include <alcommon/almodule.h>
#include <string>

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

    Battleship(boost::shared_ptr<AL::ALBroker> broker, const std::string& name);
    int row;
    int col;
    int enemyTurn;

    virtual ~Battleship();

    /** Overloading ALModule::init().
    * This is called right after the module has been loaded
    */
    virtual void init();


    void initBarcode();
    void terminateBarcode();
    /**
    * This method will be called every time the event RightBumperPressed is raised.
    */
    void onRightBumperPressed();

    /**
    * This method will be called every time the event LeftBumperPressed is raised.
    */
    void onLeftBumperPressed();

    /**
    * This method will be called every time a bar code is regonized.
    */
    void barcodeRecognized();

  private:
    AL::ALMemoryProxy fMemoryProxy;
    AL::ALBarcodeReaderProxy qrCodeProxy;
    AL::ALTextToSpeechProxy fTtsProxy;
    AL::ALRobotPostureProxy postureProxy;
    AL::ALMemoryProxy initMemoryProxy;
    boost::shared_ptr<AL::ALMutex> fCallbackMutex;

    float fState;
    AL::ALValue qState;
    std::string field;

};

#endif  // BATTLESHIP_BATTLESHIP_H
