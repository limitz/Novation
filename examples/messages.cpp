#include <novation/novation.h>
#include <threading.h>

using namespace Novation;

class MessageThread : public Thread
{
	private:
	NovationMidiDevice *_device;

	public:
	MessageThread(NovationMidiDevice *device) { _device = device; }

	protected:
	void OnStart()
	{
		uint8_t message[64];
		uint16_t len;
		try
		{	
			while (_device->GetMessage(message, &len))
			{
				for (int i=0;i<len;i++)
				{
					printf("%02X ", message[i]);
				}
				printf("\n");
				_device->SendMessage(message, len);	
			}
		}
		catch (Exception &e)
		{
			fprintf(stderr, "%s\n", e.Message());
		}
	}
};

int main(int argc, char *argv[])
{
	const char* device = "hw:1,0,0";
	if (argc > 1) device = argv[1];

	NovationMidiDevice *novation = 0;
	Thread *t = 0;

	try
	{
		NovationMidiDevice *novation = new NovationMidiDevice(device);

		Thread *t = new MessageThread(novation);
		t->Start();
	
		printf("Press buttons on your launchpad to light them up.\n");
		printf("Press enter to continue...\n");
		getchar();
	}
	catch (Exception &e)
	{
		fprintf(stderr, "%s\n", e.Message());
	}

	if (novation) delete novation;
	if (t) 
	{
		t->Join();
		delete t;
	}
	return 0;
}
