#ifndef HMB__HALITE2__SHIP__H
#define HMB__HALITE2__SHIP__H

#include "hlt/ship.hpp"

namespace hmb { namespace halite2 {

class Ship
{
public:
	Ship(const hlt::Ship& ship);
	virtual ~Ship() { }

protected:
	unsigned int id_;
};
 
} }

#endif // HMB__HALITE2__SHIP__H
