#include <novation/launchpad.h>

namespace Novation
{
	bool LaunchPad::MakePadValue(Pad *pad, uint8_t *value, uint8_t *message, uint16_t len)
	{
		if (len != 3) return false;
		*value = 0;

		switch (message[0] & 0xF0)
		{
			case 0xB0:
				pad->SetType(PAD_TYPE_CONTROL);
			case 0x90:
				*value = message[2];
			case 0x80:
				pad->SetChannel(message[0] & 0x0F);
				pad->SetId(message[1]);
				return true;
		}
		return false;			
	}

	bool LaunchPad::SetLayout(LaunchPadLayout layout) const
	{
		uint8_t midi[] = { 0xb0, 0x00, (uint8_t)layout };
		return SendMessage(midi, 3);	
	}

	bool LaunchPad::ScrollText(const char *text, PadColor color, bool loop) const
	{
		bool result = false;

		uint8_t c = color.Value() | (loop ? 64 : 0);
		uint8_t len = strlen(text);
		uint8_t *midi = (uint8_t*)malloc(7+len);
		uint8_t header[] = { 0xF0, 0x00, 0x20, 0x29, 0x09, c };

		if (!midi) throw NovationException("Out of memory");

		memcpy(midi, header, 6);
		memcpy(midi+6, text, len);
		midi[len+6] = 0xF7;

		try
		{
			result = SendMessage(midi, len+7);
		} 
		catch (Exception &e)
		{
			free(midi);
			throw e;
		}
		free(midi);
		return result;	
	}

	bool LaunchPad::RapidLedUpdate(const PadColor *colors, uint16_t len) const
	{
		uint8_t msg = 0x92;
		bool result = false;

		_lock->Lock();
		try 
		{
			result |= SendMessage(&msg, 1);
			result |= SendMessage((const uint8_t*)colors, len);
		}
		catch (Exception &e)
		{
			_lock->Unlock();
			throw e;
		}	
		_lock->Unlock();
		return result;
	}

}
