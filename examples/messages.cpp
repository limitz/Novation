#include <novation/novation.h>

using namespace Novation;

int main(int argc, char *argv[])
{
	const char* device = "hw:1,0,0";
	if (argc > 1) device = argv[1];

	NovationMidiDevice *novation = 0;
	uint8_t message[64];
	uint16_t len;

	try
	{
		NovationMidiDevice *novation = new NovationMidiDevice(device);

		while (novation->IsOpen())
		{
			novation->Process();
			if (novation->GetMessage(message, &len))
			{
				for (int i=0;i<len;i++)
				{
					printf("%02X ", message[i]);
				}
				printf("\n");
				novation->SendMessage(message, len);	
			}
		}

	}
	catch (const char* &e)
	{
		fprintf(stderr, "%s\n", e);
	}

	if (novation) delete novation;
	return 0;
}
