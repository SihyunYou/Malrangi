#include "app.h"

int 
main(
	int argc,
	char* argv[])
{
	Sleep(2000);

	unique_ptr<Crcp::CircularPlay> Player;
	CHAR Flag = 'U';

	switch (Flag)
	{
	case 'U':
		Player = make_unique<Crcp::UrusRaid>();
		break;
	
	case 'Z':
		Player = make_unique<Crcp::ZacumRaid>();
		break;

	case 'z':
		Player = make_unique<Crcp::ZacumCalc>();
		break;
	}

	Player->Play();
}