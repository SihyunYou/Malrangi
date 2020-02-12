#include "ipmanage.h"
#include "app.h"

int 
main(
	int argc,
	char* argv[])
{
	Sleep(2000);

	unique_ptr<Crcp::CircularPlay> Player = make_unique<Crcp::UrusRaid>();
	Player->Play();
}