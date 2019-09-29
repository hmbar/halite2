#include "hlt/hlt.hpp"
#include "hlt/navigation.hpp"
#include <strstream>
#include <tuple>
#include <cstdint>
#include <iomanip>

const hlt::Metadata metadata = hlt::initialize("Hernan");
const hlt::PlayerId player_id = metadata.player_id;

namespace utils
{
	namespace planets {

		uint32_t getPriority(const hlt::Planet& planet)
		{
			uint32_t priority = 0;

//			if (planet.is_alive())
//				priority |= 0x8000000U;

			if (planet.owned && planet.owner_id == player_id)
				priority |= 0x4000000U;

			if (!planet.owned)
				priority |= 0x2000000U;
			
			if(!planet.is_full())
				priority |= 0x8000000U;

			double radius = planet.radius * 1E3;
			priority |= (radius < 65535.0) ? uint16_t(radius) : 0xFFFFU;

			return priority;
		}


		struct less_than
		{
			// If planet lhs is less interesting to conqueror than rhs, this method must return false
			inline bool operator() (const hlt::Planet& lhs, const hlt::Planet& rhs)
			{
				return getPriority(lhs) > getPriority(rhs);
			}
		};
	}
}

int main() {

	const hlt::Map& initial_map = metadata.initial_map;

	// We now have 1 full minute to analyse the initial map.
	std::ostringstream initial_map_intelligence;
	initial_map_intelligence
		<< "width: " << initial_map.map_width
		<< "; height: " << initial_map.map_height
		<< "; players: " << initial_map.ship_map.size()
		<< "; my ships: " << initial_map.ship_map.at(player_id).size()
		<< "; planets: " << initial_map.planets.size();
	hlt::Log::log(initial_map_intelligence.str());

	std::vector<hlt::Move> moves;

	std::stringstream ss;

	ss << "my player_id: " << player_id << std::endl;
	hlt::Log::log(ss.str());

	for (;;)
	{			
		size_t planets_cnt;
		moves.clear();
		const hlt::Map map = hlt::in::get_map();

		std::vector<hlt::Planet> planets = map.planets;
		std::sort(planets.begin(), planets.end(), utils::planets::less_than());

		std::vector<hlt::Planet>::iterator it = planets.begin();

		std::stringstream ss;
		ss << "Planets:" << std::endl;
		for (planets_cnt=0; it != planets.end(); ++it)
		{
			ss << std::dec
				<< " planet_id: "  << (*it).entity_id 
				<< "; owned: "    << (*it).owned 
				<< "; owner_id: " << (*it).owner_id
				<< "; is_full: " << (*it).is_full()
				<< "; radius: " << (*it).radius
				<< "; spots/ships: " << (*it).docking_spots << "/" << (*it).docked_ships.size() 
				<< "; production current/remaining: " << (*it).current_production << "/" << (*it).remaining_production
				<< "; priority: " << std::hex << std::setiosflags(std::ios::showbase) << utils::planets::getPriority(*it)
				<< std::endl;

			if ((*it).is_alive())
			{
				++planets_cnt;
			}
		}
		
		
		size_t ship_cnt = 0;
		for (const hlt::Ship& ship : map.ships.at(player_id))
		{
			if (ship.docking_status != hlt::ShipDockingStatus::Undocked)
				continue;

			++ship_cnt;
		}

		size_t delta = 1 + (ship_cnt / planets_cnt);
		it = planets.begin();
		

		ss << std::dec << std::endl 
			<< "We have " << planets_cnt << " alive planets and " << ship_cnt << " undocked ships." << std::endl;

		ss << std::endl 
			<< "Movements: " << std::endl;
		
		ship_cnt = 0;
		for (const hlt::Ship& ship : map.ships.at(player_id))
		{
			if (ship.docking_status != hlt::ShipDockingStatus::Undocked)
				continue;

			++ship_cnt;

			if (ship.can_dock(*it) && !(*it).is_full() )
			{
				moves.push_back(hlt::Move::dock(ship.entity_id, (*it).entity_id));
				ss << " docking ship " << ship.entity_id << " to planet " << (*it).entity_id << std::endl;
			}
			else
			{
				const hlt::possibly<hlt::Move> move = hlt::navigation::navigate_ship_to_dock(map, ship, *it, hlt::constants::MAX_SPEED);

				if (move.second) 
				{
					moves.push_back(move.first);
					ss << " moving ship " << ship.entity_id << " to planet " << (*it).entity_id << std::endl;
				}
			}

			if ((ship_cnt % delta) == 0)
			{
//				++it;
			}
		}

		hlt::Log::log(ss.str());

		if (!hlt::out::send_moves(moves)) {
			hlt::Log::log("send_moves failed; exiting");
			break;
		}
	}
}
