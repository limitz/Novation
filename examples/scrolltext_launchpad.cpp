#include <novation/launchpad.h>

using namespace Novation;

int main(int argc, char *argv[])
{
	LaunchPad *launchpad = 0;

	try
	{
		launchpad = new LaunchPad(argc > 1 ? argv[1] : "hw:1,0,0");
		const char *text = argc > 2 ? argv[2] : "Limitz Launchpad Library";
		
		launchpad->ScrollText(text, PadColor::Lime, false);
		printf("Press enter to continue...");
	
		getchar();
	}
	catch (const char* &e)
	{
		fprintf(stderr, "%s\n", e);
	}
	delete launchpad;
	return 0;
}
