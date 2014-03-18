#include <novation/launchcontrol.h>

using namespace Novation;

int main(int argc, char *argv[])
{
	LaunchControl *launchcontrol = 0;

	try
	{
		launchcontrol = new LaunchControl(argc > 1 ? argv[1] : "hw:1,0,0");
		launchcontrol->ChangeTemplate(0);

		// TEMPLATES
		for (int t = 1; t<16; t++)
		{
			for (int button=0; button<12; button++)
			{
				Pad pad = Pad(button, PAD_TYPE_NORMAL, t);
				PadColor color = PadColor(t%4, t/4);
				launchcontrol->SetTemplatePadColor(pad, color);
			}
		}

		// MAIN ROW
		for (int row=0; row<2; row++)
		{
			for (int col=0; col<4; col++)
			{
				Pad pad = Pad(row<<4|(col+9));
				PadColor color = PadColor(1+(col%4)*2, col/2+row*2);
				launchcontrol->SetPadColor(pad, color);
			}
		}

		// CURSOR
		for (int i=0; i<4; i++)
		{
			Pad pad = Pad(0x72+i, PAD_TYPE_CONTROL);
			launchcontrol->SetPadColor(pad, PadColor(i,0));
		}

		printf("Press enter to continue...");
		getchar();
	}
	catch (Exception &e)
	{
		fprintf(stderr, "%s\n", e.Message());
	}
	delete launchcontrol;
	return 0;
}
