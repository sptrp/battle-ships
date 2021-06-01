/**
 * Aus Einfuehrung in die Arbeit mit dem Nao Seite 52
 */

#include <boost/shared_ptr.hpp>

#include <alcommon/albroker.h>
#include <alcommon/albrokermanager.h>

#include "battleship.h"


# ifdef _WIN32
#  define ALCALL __declspec(dllexport)
# else
#  define ALCALL
# endif


extern "C"
{
  ALCALL int _createModule(boost::shared_ptr<AL::ALBroker> broker)
  {
    // init broker with the main broker instance
    // from the parent executable
    AL::ALBrokerManager::setInstance(broker->fBrokerManager.lock());
    AL::ALBrokerManager::getInstance()->addBroker(broker);
      AL::ALModule::createModule<Battleship>( broker, "Battleship" );

    return 0;
  }

  ALCALL int _closeModule()
  {
    return 0;
  }
}
