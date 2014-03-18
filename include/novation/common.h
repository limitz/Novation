#ifndef LMTZ_NOVATION_COMMON
#define LMTZ_NOVATION_COMMON

#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <inttypes.h>
#include <alsa/asoundlib.h>
#include <exception.h>
#include <threading.h>

namespace Novation
{
	GENERATE_EXCEPTION(NovationException, Exception);

	class NovationMidiDevice;

	enum PadType
	{
		PAD_TYPE_NORMAL,
		PAD_TYPE_CONTROL,
		PAD_TYPE_POTENTIOMETER
	};

	struct Pad
	{
		private:
		PadType _type;
		uint8_t _channel;
		uint8_t _id;
		NovationMidiDevice *_origin;

		public:
		Pad(uint8_t id = 0, PadType type = PAD_TYPE_NORMAL, uint8_t channel = 0) { _id = id; _type = type; _channel = channel; _origin = NULL; }

		NovationMidiDevice *Origin() const { return _origin; }
		PadType Type() const { return _type; }
		uint8_t Channel() const { return _channel; }
		uint8_t Id() const { return _id; }

		void SetOrigin(NovationMidiDevice *device) { _origin = device; }
		void SetType(PadType type) { _type = type; }
		void SetChannel(uint8_t channel) { _channel = channel; }
		void SetId(uint8_t id) { _id = id; }
	};

	struct PadColor
	{
		private:
		uint8_t _v;

		public:
		PadColor(uint8_t red=0, uint8_t green=0);

		uint8_t Value() const { return _v; }
		uint8_t RedValue() const;
		uint8_t GreenValue() const;

		void SetValue(uint8_t v) { _v = v; }
		void SetRedValue(uint8_t r);
		void SetGreenValue(uint8_t g);

		static const PadColor Off;
		static const PadColor FullGreen;
		static const PadColor Green;
		static const PadColor DimGreen;
		static const PadColor FullRed;
		static const PadColor Red;
		static const PadColor DimRed;
		static const PadColor FullAmber;
		static const PadColor Amber;
		static const PadColor DimAmber;
		static const PadColor Yellow;
		static const PadColor Orange;
		static const PadColor Lime;
		static const PadColor Auburn;
	};

	struct PadBrightness
	{
		private:
		uint8_t _controller;
		uint8_t _v;
	
		public:
		PadBrightness(uint8_t numerator, uint8_t denominator);

		uint8_t Controller() const { return _controller; }
		uint8_t Value() const { return _v; }
		uint8_t Numerator() const;
		uint8_t Denominator() const; 
		void SetController(uint8_t controller) { _controller = controller; }
		void SetValue(uint8_t value) { _v = value; }
		void SetNumerator(uint8_t numerator);
		void SetDenominator(uint8_t denominator); 
	};

	enum DoubleBufferWriteMode
	{
		NOV_DB_WRITE_MODE_DEFAULT = 0x00,
		NOV_DB_WRITE_MODE_COPY    = 0x04,
		NOV_DB_WRITE_MODE_CLEAR   = 0x08
	};

	enum DoubleBufferMode
	{
		NOV_DB_MODE_COPY    = 0x10,
		NOV_DB_MODE_FLASH   = 0x08,
		NOV_DB_MODE_UPDATE  = 0x04,
		NOV_DB_MODE_DISPLAY = 0x01
	};

	class NovationMidiMessageHandler
	{
		public:
		friend class NovationMidiDevice;

		virtual void OnPad(const Pad& pad, uint8_t value)
		{
			if (pad.Type() == PAD_TYPE_POTENTIOMETER) OnPotentiometer(pad, value);
			else if (value) OnPadDown(pad);
			else OnPadUp(pad);
		}

		virtual void OnPadDown(const Pad& pad) {}
		virtual void OnPadUp(const Pad& pad) {}
		virtual void OnPotentiometer(const Pad& pad, uint8_t value) {}
	};

	class NovationMidiDevice : public Thread
	{
		private:
		char 		*_id;
		snd_rawmidi_t 	*_midiIn;
		snd_rawmidi_t 	*_midiOut;
		mutable uint8_t _runningStatus;
		bool 		_isOpen;
		NovationMidiMessageHandler *_handler;

		protected:	
		Mutex		*_lock;

		public:
		NovationMidiDevice(const char *id);
		~NovationMidiDevice();

		const char* Id() const { return _id; }
		const char* Name() const {};

		bool Reset(uint8_t ledBrightness = 0) const;
		bool SetBrightness(PadBrightness brightness) const;
		bool SetBufferMode(DoubleBufferMode bufferMode) const;
		bool SetPadColor(const Pad& pad, PadColor color, DoubleBufferWriteMode writeMode = NOV_DB_WRITE_MODE_DEFAULT) const;

		NovationMidiMessageHandler* MessageHandler() const { return _handler; }
		void SetMessageHandler(NovationMidiMessageHandler *handler) { _handler = handler; }
		
		bool GetMessage(uint8_t *buffer, uint16_t *len) const;
		bool SendMessage(const uint8_t *buffer, uint16_t len) const;
			
		protected:
		void OnStart();
		virtual bool MakePadValue(Pad *pad, uint8_t *value, uint8_t *message, uint16_t len) { return false; };

		private:
		void Open();
		void Close();
		bool IsOpen() const;
	};
}

#endif
