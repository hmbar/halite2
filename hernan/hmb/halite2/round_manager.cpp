#include "hmb/halite2/round_manager.h"

#include "hlt/hlt.hpp"
#include "hlt/navigation.hpp"
#include <tuple>
#include <cstdint>
#include <iomanip>
#include <utility>
#include <cassert>

namespace hmb { namespace halite2 {

RoundManager::RoundManager(const hlt::Metadata& metadata)
	: metadata(metadata)
{
}

bool RoundManager::init()
{
	const hlt::PlayerId player_id = metadata.player_id;
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

	std::stringstream ss;

	ss << "my player_id: " << player_id << std::endl;

	ss << std::endl 
		<< "Initial Planets:" << std::endl;
	for (auto it = initial_map.planets.begin(); it != initial_map.planets.end(); ++it)
	{
		hmb::halite2::Planet p;
		if (planets.find((*it).entity_id) == planets.end())
		{
			p = planets[(*it).entity_id] = hmb::halite2::Planet(*it);
		}
		else
		{
			p = planets[(*it).entity_id];
		}

		ss << p(initial_map)
			<< "; priority: " << std::hex << std::setiosflags(std::ios::showbase) << hmb::halite2::Planet::getPriority(metadata.player_id, *it)
			<< std::dec << std::endl;
	}

	hlt::Log::log(ss.str());
	return true;
}

bool RoundManager::round()
{
	size_t planets_cnt;
	moves.clear();
	hlt::Map map = hlt::in::get_map();
	std::vector<hlt::Planet> planets = map.planets;

	std::sort(planets.begin(), planets.end(), hmb::halite2::Planet::less_than(metadata.player_id));

	std::vector<hlt::Planet>::iterator it = planets.begin();

	std::stringstream ss;
	ss << "Planets:" << std::endl;
	for (planets_cnt = 0; it != planets.end(); ++it)
	{
		hmb::halite2::Planet p = getPlanet((*it).entity_id);

		ss << p(map)
			<< "; priority: " << std::hex << std::setiosflags(std::ios::showbase) << hmb::halite2::Planet::getPriority(metadata.player_id, *it)
			<< std::dec << std::endl;

		if ((*it).is_alive())
		{
			++planets_cnt;
		}
	}


	size_t ship_cnt = 0;
	for (const hlt::Ship& ship : map.ships.at(metadata.player_id))
	{
		if (ship.docking_status != hlt::ShipDockingStatus::Undocked)
			continue;

		++ship_cnt;
	}

	size_t delta = 1 + (ship_cnt / planets_cnt);
	it = planets.begin();


	ss << std::dec << std::endl
		<< "We have " << planets_cnt << " alive planets and " << ship_cnt << " undocked ships." << std::endl
		<< std::endl;

	ship_cnt = 0;
	for (const hlt::Ship& ship : map.ships.at(metadata.player_id))
	{
		if (ship.docking_status != hlt::ShipDockingStatus::Undocked)
			continue;

		++ship_cnt;

		std::vector<std::pair<hlt::Planet, hlt::Ship> > planetsPerShip;
		for (const hlt::Planet& planet : planets)
		{
			planetsPerShip.push_back(std::make_pair(planet, ship));
		}

		std::sort(planetsPerShip.begin(), planetsPerShip.end(), hmb::halite2::Planet::less_than(metadata.player_id));
		std::vector<std::pair<hlt::Planet, hlt::Ship> >::iterator it = planetsPerShip.begin();

		ss << std::endl 
			<< "Planets from Ship: " << ship.entity_id << std::endl;

		for (; it != planetsPerShip.end(); ++it)
		{
			hmb::halite2::Planet p = getPlanet((*it).first.entity_id);

			ss << p(map)
				<< "; distance: " << (*it).first.location.get_distance_to((*it).second.location)
				<< "; priority: " << std::hex << std::setiosflags(std::ios::showbase) << hmb::halite2::Planet::getPriority(metadata.player_id, (*it).first, (*it).first.location.get_distance_to((*it).second.location))
				<< std::dec << std::endl;
		}

		ss << std::endl
			<< "Movements: " << std::endl;

		it = planetsPerShip.begin();

		if (ship.can_dock((*it).first) && (!(*it).first.owned || (*it).first.owner_id == metadata.player_id && !(*it).first.is_full()))
		{
			moves.push_back(hlt::Move::dock(ship.entity_id, (*it).first.entity_id));
			ss << " docking ship " << ship.entity_id << " to planet " << (*it).first.entity_id << std::endl;
		}
		else
		{
			hlt::possibly<hlt::Move> move;
			if ((!(*it).first.owned || (*it).first.owner_id == metadata.player_id && !(*it).first.is_full()))
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
		return false;
	}

	return true;
}

hmb::halite2::Planet& RoundManager::getPlanet(hlt::EntityId planetId)
{
	if (planets.find(planetId) != planets.end())
	{
		return planets[planetId];
	}
	else
	{
		assert(false);
	}
}

} }