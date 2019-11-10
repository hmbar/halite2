#include "hmb/halite2/round_manager.h"

int main() 
{
	hmb::halite2::RoundManager roundManager(hlt::initialize("Hernan"));

	roundManager.init();

	for (;;)
	{			
		roundManager.round();
	}
}
