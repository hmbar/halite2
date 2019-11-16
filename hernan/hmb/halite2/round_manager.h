#ifndef HMB__HALITE2__ROUND_MANAGER
#define HMB__HALITE2__ROUND_MANAGER

#include <climits>
#include <sstream>
#include <vector>
#include <map>

#include "hlt/hlt.hpp"
#include "hmb/halite2/planet.h"

namespace hmb { namespace halite2 {

class RoundManager
{
public:
	RoundManager(const hlt::PlayerId& player_id);
	virtual ~RoundManager() { }

	bool init(const hlt::Map& initial_map);
	bool round(hlt::Map& map);

protected:
	const hlt::PlayerId player_id;

	hmb::halite2::Planet& getPlanet(hlt::EntityId planetId);

	std::vector<hlt::Move> moves;
	std::stringstream ss;
	std::map< unsigned int, hmb::halite2::Planet> planets;
};

} }

#endif // HMB__HALITE2__ROUND_MANAGER
