#include <novation/launchcontrol.h>

namespace Novation
{	
	bool LaunchControl::MakePadValue(Pad *pad, uint8_t *value, uint8_t *message, uint16_t len)
	{
		if (len != 3) return false;
		*value = 0;

		switch (message[0] & 0xF0) 
		{
			case 0xB0:
				if (message[1] >= 0x15 && message[1] < 0x1D
				||  message[1] >= 0x29 && message[1] < 0x31)
					pad->SetType(PAD_TYPE_POTENTIOMETER);
				else	pad->SetType(PAD_TYPE_CONTROL);	
			case 0x90:
				*value = message[2];
			case 0x80:
				pad->SetChannel(message[0] & 0x0F);
				pad->SetId(message[1]);
				return true;
		}
		return false;
	}

	bool LaunchControl::ChangeTemplate(uint8_t templateId) const
	{
		uint8_t message[] = { 0xF0, 0x00, 0x20, 0x29, 0x02, 0x0A, 0x77, templateId, 0xF7 };
		return SendMessage(message, sizeof(message));
	}

	bool LaunchControl::SetTemplatePadColor(const Pad& pad, PadColor color) const
	{
		uint8_t message[] = { 0xF0, 0x00, 0x20, 0x29, 0x02, 0x0A, 0x78, pad.Channel(), pad.Id(), color.Value(), 0xF7 };
		return SendMessage(message, sizeof(message));
	}
}

