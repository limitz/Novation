#include <novation/common.h>

namespace Novation
{
	static inline bool IsValidMessage(const uint8_t *buffer, uint16_t len)
	{
		if (!len) return false;
		switch (buffer[0] & 0xF0)
		{
			case 0x80:
			case 0x90:
			case 0xB0:
				return len == 3;
			case 0xF0:
				return buffer[len-1] == 0xF7;
			default:
				return false;
		}
	}


	NovationMidiDevice::NovationMidiDevice(const char *id)
	{
		_id = strdup(id);

		_lock = new Mutex();
		_midiIn = 0;
		_midiOut = 0;
		_isOpen = false;
		_runningStatus = 0x00;
		_handler = 0;

		Open();
		Reset();
	}

	NovationMidiDevice::~NovationMidiDevice()
	{
		Reset();
		Close();
		if (_id) free(_id);
		delete _lock;
	}

	void NovationMidiDevice::Open()
	{	
		try
		{
			_lock->Lock("Opening");
	
			int error = snd_rawmidi_open(&_midiIn, &_midiOut, _id, 0);
			if (error < 0) throw NovationException("Unable to open device [%s]: %s", _id, snd_strerror(error));
			if (!_midiIn)  throw NovationException("No MIDI input found for [%s]: %s", _id, snd_strerror(error));
			if (!_midiOut) throw NovationException("No MIDI output found for [%s]: %s", _id, snd_strerror(error));
	
			snd_rawmidi_nonblock(_midiOut, 1);
			_isOpen = true;
		}
		catch (Exception &e)
		{
			_lock->Unlock();
			throw e;
		}
		_lock->Unlock();
	}

	void NovationMidiDevice::Close()
	{
		_lock->Lock("Closing");
	
		if (_midiOut)
		{
			snd_rawmidi_drain(_midiOut);
			snd_rawmidi_close(_midiOut);
			_midiOut = 0;
		}
		if (_midiIn)
		{
			snd_rawmidi_close(_midiIn);
			_midiIn = 0;
		}

		_isOpen = false;
		_lock->Unlock();
	}

	bool NovationMidiDevice::IsOpen() const
	{
		_lock->Lock();
		bool result = _isOpen;
		_lock->Unlock();
		return result;
	}

	void NovationMidiDevice::OnStart()
	{
		uint8_t buffer[64];
		uint16_t len = 0;

		while (IsRunning())
		{
			if (!IsValidMessage(buffer, len))
			{
				uint8_t byte;
				int result = snd_rawmidi_read(_midiIn, &byte, 1);
				if (result == -EAGAIN) 
				{
					usleep(1000);
					continue;
				}
				if (result < 0) 
				{
					if (!IsOpen())
					{
						return;
					}
					throw NovationException("Unable to read from [%s]: %s", _id, snd_strerror(result));
				}
				if (byte&0x80) _runningStatus = byte;
				else if (!len) buffer[len++] = _runningStatus;

				buffer[len++] = byte;
			}
			else
			{	
				Pad p;
				uint8_t value;
				if (_handler && MakePadValue(&p, &value, buffer, len))
				{
					p.SetOrigin(this);
					_handler->OnPad(p, value);	
				}
				len = 0;
			}
		}
	}

	bool NovationMidiDevice::Reset(uint8_t ledBrightness) const
	{
		uint8_t brightness = ledBrightness ? 0x7c + ledBrightness : 0;
		uint8_t midi[] = { 0xb0, 0x00, brightness };
		return SendMessage(midi, 3);
	}


	bool NovationMidiDevice::SetBrightness(PadBrightness b) const
	{
		uint8_t midi[] = { 0xb0, b.Controller(), b.Value() };
		return SendMessage(midi, 3);	
	}

	bool NovationMidiDevice::SetBufferMode(DoubleBufferMode mode) const
	{
		uint8_t midi[] = { 0xb0, 0x00, (uint8_t)mode };
		return SendMessage(midi, 3);
	}

	bool NovationMidiDevice::SetPadColor(const Pad& pad, PadColor color, DoubleBufferWriteMode mode) const
	{
		uint8_t c = color.Value() | (uint8_t) mode;
		uint8_t midi[] = { (pad.Type() == PAD_TYPE_NORMAL ? 0x90 : 0xB0) + pad.Channel(), pad.Id(), c };
		return SendMessage(midi, 3);
	}

	bool NovationMidiDevice::SendMessage(const uint8_t *buffer, uint16_t len) const
	{
		_lock->Lock("SendMessage");
		if (!IsOpen()) 
		{
			_lock->Unlock();
			return false;
		}
	
		int result = snd_rawmidi_write(_midiOut, buffer, len);
		if (result < 0)
		{
			if (!IsOpen()) 
			{
				_lock->Unlock(); 
				return false;
			}
			_lock->Unlock();
			throw NovationException("Unable to write to [%s]: %s", _id, snd_strerror(result));
		}
		else 
		{
			_lock->Unlock();
			return true;
		}
	}

	bool NovationMidiDevice::GetMessage(uint8_t *buffer, uint16_t *len) const
	{
		*len = 0;
		while (!IsValidMessage(buffer, *len))
		{
			uint8_t byte;
		
			int result = snd_rawmidi_read(_midiIn, &byte, 1);
		
			if (result == -EAGAIN) 
			{
				usleep(1000);
				continue;
			}
			if (result < 0) 
			{
				if (!IsOpen())
				{
					return false;
				}
				throw NovationException("Unable to read from [%s]: %s", _id, snd_strerror(result));
			}
			if (byte&0x80) _runningStatus = byte;
			else if (!*len) buffer[(*len)++] = _runningStatus;

			buffer[(*len)++] = byte;
		}
		return true;
	}

	const PadColor PadColor::Off        = PadColor(0,0);
	const PadColor PadColor::FullGreen  = PadColor(0,3);
	const PadColor PadColor::Green      = PadColor(0,2);
	const PadColor PadColor::DimGreen   = PadColor(0,1);
	const PadColor PadColor::FullRed    = PadColor(3,0);
	const PadColor PadColor::Red        = PadColor(2,0); 
	const PadColor PadColor::DimRed     = PadColor(1,0);
	const PadColor PadColor::FullAmber  = PadColor(3,3);
	const PadColor PadColor::Amber      = PadColor(2,2);
	const PadColor PadColor::DimAmber   = PadColor(1,1);
	const PadColor PadColor::Yellow     = PadColor(2,3);
	const PadColor PadColor::Orange     = PadColor(3,2);
	const PadColor PadColor::Lime       = PadColor(1,3);
	const PadColor PadColor::Auburn     = PadColor(3,1);

	PadColor::PadColor(uint8_t red, uint8_t green)
	{
		_v = ((green&3)<<4)|(red&3); 
	}

	uint8_t PadColor::RedValue() const
	{
		return _v&3;
	}

	uint8_t PadColor::GreenValue() const
	{
		return (_v>>4)&3;
	}

	void PadColor::SetRedValue(uint8_t red)
	{
		_v = (_v&~0x03)|(red&3);
	}

	void PadColor::SetGreenValue(uint8_t green)
	{
		_v = (_v&~0x30)|((green&3)<<4);
	}

	PadBrightness::PadBrightness(uint8_t numerator, uint8_t denominator)
	{
		SetNumerator(numerator);
		SetDenominator(denominator);
	}

	uint8_t PadBrightness::Numerator() const 
	{
		return _controller == 0x1E ? (_v>>4)+1 : (_v>>4)+9;
	}

	uint8_t PadBrightness::Denominator() const 
	{
		return (_v&0xF)+3;
	}

	void PadBrightness::SetNumerator(uint8_t numerator)
	{
		if (numerator < 9) 
		{
			_v = ((numerator-1)<<4)|(_v&0xF);
			_controller = 0x1E;
		} else {
			_v = ((numerator-9)<<4)|(_v&0xF);
			_controller = 0x1F;
		}
	}

	void PadBrightness::SetDenominator(uint8_t denominator)
	{
		_v = (_v&0xF0)|(denominator-3);
	}
}
