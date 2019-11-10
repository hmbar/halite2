#ifndef HMB__HALITE2__PLANET
#define HMB__HALITE2__PLANET

#include <climits>
#include <string>
#include "hlt/planet.hpp"
#include "hlt/map.hpp"

namespace hmb {	namespace halite2 {

class Planet
{
public:
	Planet();
	Planet(const ::hlt::Planet& planet);
	virtual ~Planet() { }

	bool operator==(const ::hlt::Planet& other);

	bool isFull();
	bool isEmpty();
	bool isMine();

	std::string operator()(const ::hlt::Map& map);

	static uint32_t getPriority(const hlt::PlayerId player_id, const hlt::Planet& planet, double distance = 0.0);

	struct less_than
	{
		less_than(const hlt::PlayerId player_id) : player_id(player_id) { }

		// If planet lhs is less interesting to conqueror than rhs, this method must return false
		inline bool operator() (const hlt::Planet& lhs, const hlt::Planet& rhs)
		{
			return getPriority(player_id, lhs) > getPriority(player_id, rhs);
		}

		inline bool operator() (const std::pair<const hlt::Planet&, const hlt::Ship&>& lhs, const std::pair<const hlt::Planet&, const hlt::Ship&>& rhs)
		{
			return getPriority(player_id, lhs.first, lhs.first.location.get_distance_to(lhs.second.location)) > getPriority(player_id, rhs.first, rhs.first.location.get_distance_to(rhs.second.location));
		}

	protected:
		const hlt::PlayerId player_id;
	};

protected:
	unsigned int id_;
};

} }


#endif

