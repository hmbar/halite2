#include "hlt/hlt.hpp"
#include "hlt/navigation.hpp"
#include <sstream>
#include <tuple>
#include <cstdint>
#include <iomanip>
#include <utility>

const hlt::Metadata metadata = hlt::initialize("Hernan");
const hlt::PlayerId player_id = metadata.player_id;

namespace utils
{
	namespace planets {

		uint32_t getPriority(const hlt::Planet& planet, double distance = 0.0)
		{
			static const uint32_t STATE_MASK = 0x07;
			static const uint8_t  STATE_MASK_SHIFT = 29;
			static const uint32_t RADIUS_MASK = 0xFFFF;
			static const uint8_t  RADIUS_MASK_SHIFT = 10;
			static const uint32_t DISTANCE_MASK = 0x3FF;
			static const uint8_t  DISTANCE_MASK_SHIFT = 0;

			typedef enum
			{
				UNKNOWN = 0
				, MINE_AND_FULL
				, OTHER
				, EMPTY
				, MINE_AND_NOT_FULL
			} state_t;

			state_t state = state_t::UNKNOWN;

			uint32_t priority = 0, ui32;

			if (planet.owned && planet.owner_id == player_id)
			{
				state = planet.is_full() ? MINE_AND_FULL : MINE_AND_NOT_FULL;
			}
			else
			{
				state = planet.owned ? OTHER : EMPTY;
			}

			ui32 = ((STATE_MASK & state) << STATE_MASK_SHIFT);
			priority |= ui32;

			double radius = planet.radius * 1E3;
			ui32 = (((radius < RADIUS_MASK) ? uint32_t(radius) : RADIUS_MASK) << RADIUS_MASK_SHIFT);
			priority |= ui32;

			if (distance > 0.0)
			{			
				ui32 = uint32_t(distance);
				ui32 = ((ui32 < DISTANCE_MASK) ? ui32 : DISTANCE_MASK);
				ui32 = ~ui32;
				ui32 &= DISTANCE_MASK;
				ui32 <<= DISTANCE_MASK_SHIFT;
				priority |= ui32;
			}
			return priority;
		}


		struct less_than
		{
			// If planet lhs is less interesting to conqueror than rhs, this method must return false
			inline bool operator() (const hlt::Planet& lhs, const hlt::Planet& rhs)
			{
				return getPriority(lhs) > getPriority(rhs);
			}

			inline bool operator() (const std::pair<const hlt::Planet&, const hlt::Ship&>& lhs, const std::pair<const hlt::Planet&, const hlt::Ship&>& rhs)
			{
				return getPriority(lhs.first, lhs.first.location.get_distance_to(lhs.second.location)) > getPriority(rhs.first, rhs.first.location.get_distance_to(rhs.second.location));
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

		ship_cnt = 0;
		for (const hlt::Ship& ship : map.ships.at(player_id))
		{
			if (ship.docking_status != hlt::ShipDockingStatus::Undocked)
				continue;

			++ship_cnt;

			std::vector<std::pair<hlt::Planet, hlt::Ship> > planetsPerShip;
			for (const hlt::Planet& planet : planets)
			{
				planetsPerShip.push_back(std::make_pair(planet, ship));
			}
			
			std::sort(planetsPerShip.begin(), planetsPerShip.end(), utils::planets::less_than());
			std::vector<std::pair<hlt::Planet, hlt::Ship> >::iterator it = planetsPerShip.begin();

			ss << "Planets from Ship:" << std::endl;

			for (; it != planetsPerShip.end(); ++it)
			{
				ss << std::dec
					<< " id planet/ship: " << (*it).first.entity_id << "/" << (*it).second.entity_id
					<< "; owned: " << (*it).first.owned
					<< "; owner_id: " << (*it).first.owner_id
					<< "; is_full: " << (*it).first.is_full()
					<< "; radius: " << (*it).first.radius
					<< "; spots/ships: " << (*it).first.docking_spots << "/" << (*it).first.docked_ships.size()
					<< "; production current/remaining: " << (*it).first.current_production << "/" << (*it).first.remaining_production
					<< "; distance: " << (*it).first.location.get_distance_to((*it).second.location)
					<< "; priority: " << std::hex << std::setiosflags(std::ios::showbase) << utils::planets::getPriority((*it).first, (*it).first.location.get_distance_to((*it).second.location))
					<< std::endl;
			}

			ss << std::endl
				<< "Movements: " << std::endl;

			it = planetsPerShip.begin();

			if (ship.can_dock((*it).first) && (!(*it).first.owned||(*it).first.owner_id==player_id && !(*it).first.is_full()) )
			{
				moves.push_back(hlt::Move::dock(ship.entity_id, (*it).first.entity_id));
				ss << " docking ship " << ship.entity_id << " to planet " << (*it).first.entity_id << std::endl;
			}
			else
			{
				hlt::possibly<hlt::Move> move;
				if((!(*it).first.owned || (*it).first.owner_id == player_id && !(*it).first.is_full()))
				{
					move = hlt::navigation::navigate_ship_to_dock(map, ship, (*it).first, hlt::constants::MAX_SPEED);
				}
				else
				{
					move = hlt::navigation::navigate_ship_towards_target(map, ship, (*it).first.location, hlt::constants::MAX_SPEED, true, 10, 90);
				}
				

				if (move.second) 
				{
					moves.push_back(move.first);
					ss << " moving ship " << ship.entity_id << " to planet " << (*it).first.entity_id << std::endl;
				}
			}
		}

		hlt::Log::log(ss.str());

		if (!hlt::out::send_moves(moves)) {
			hlt::Log::log("send_moves failed; exiting");
			break;
		}
	}
}
