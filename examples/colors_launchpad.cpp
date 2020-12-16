#include <novation/launchpad.h>

using namespace Novation;

int main(int argc, char *argv[])
{
	LaunchPad *launchpad = 0;

	try
	{
		launchpad = new LaunchPad(argc > 1 ? argv[1] : "hw:1,0,0");

		// MAIN MATRIX
		for (int row=0; row<8; row++)
		{
			for (int col=0; col<8; col++)
			{
				Pad pad = Pad(row<<4|col);
				PadColor color = PadColor(row%4, col%4);
				launchpad->SetPadColor(pad, color);
			}
		}

		// SIDE COLUMN
		for (int row=0; row<8; row++)
		{
			int col = 8;
			Pad pad = Pad(row<<4|col);
			launchpad->SetPadColor(pad, PadColor::Green);
		}

		// TOP ROW
		for (int i=0; i<8; i++)
		{
			Pad pad = Pad(0x68+i, PAD_TYPE_CONTROL);
			launchpad->SetPadColor(pad, PadColor::Red);
		}

		while (launchpad->IsOpen())
		{
			launchpad->Process();
			usleep(1000);
		}
	}
	catch (const char* &e)
	{
		fprintf(stderr, "%s\n", e);
	}
	delete launchpad;
	return 0;
}
