#include "hmb/halite2/round_manager.h"

int main() 
{
	const hlt::Metadata metadata = hlt::initialize("Hernan");
	hmb::halite2::RoundManager roundManager(metadata.player_id);

	roundManager.init(metadata.initial_map);

	for (;;)
	{			
		hlt::Map map = hlt::in::get_map();
		roundManager.round(map);
	}
}
