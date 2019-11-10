#ifndef HMB__HALITE2__ROUND_MANAGER
#define HMB__HALITE2__ROUND_MANAGER

#include <climits>
#include <strstream>
#include <vector>
#include <map>

#include "hlt/hlt.hpp"
#include "hmb/halite2/planet.h"

namespace hmb { namespace halite2 {

class RoundManager
{
public:
	RoundManager(const hlt::Metadata& metadata);
	virtual ~RoundManager() { }

	bool init();
	bool round();

protected:
	const hlt::Metadata metadata;

	hmb::halite2::Planet& getPlanet(hlt::EntityId planetId);

	std::vector<hlt::Move> moves;
	std::stringstream ss;
	std::map< unsigned int, hmb::halite2::Planet> planets;
};

} }

#endif // HMB__HALITE2__ROUND_MANAGER
