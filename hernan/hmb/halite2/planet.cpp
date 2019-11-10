#include <sstream>
#include "hmb/halite2/planet.h"


namespace hmb { namespace halite2 {
	
Planet::Planet() 
:	id_(0xFFFFFFFFU) 
{ 
}

Planet::Planet(const::hlt::Planet& planet) 
:	id_(planet.entity_id) 
{ 
}

bool Planet::operator==(const ::hlt::Planet& other)
{
	return false;
}

bool Planet::isFull()
{
	return false;
}

bool Planet::isEmpty()
{
	return false;
}

bool Planet::isMine()
{
	return false;
}

std::string Planet::operator()(const ::hlt::Map& map)
{
	std::stringstream ss;
	try
	{
		::hlt::Planet p(map.get_planet(id_));
		ss << ::std::dec
			<< " planet_id: " << p.entity_id
			<< "; owned: " << p.owned
			<< "; owner_id: " << p.owner_id
			<< "; is_full: " << p.is_full()
			<< "; radius: " << p.radius
			<< "; spots/ships: " << p.docking_spots << "/" << p.docked_ships.size()
			<< "; production current/remaining: " << p.current_production << "/" << p.remaining_production;
	} catch (...)
	{
	}

	return ss.str();
}

uint32_t Planet::getPriority(const hlt::PlayerId player_id, const hlt::Planet& planet, double distance)
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

	state_t state = UNKNOWN;

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


} }
