#ifndef LMTZ_NOVATION_LAUNCHPAD
#define LMTZ_NOVATION_LAUNCHPAD

#include <novation/common.h>

namespace Novation
{
	enum LaunchPadLayout
	{
		LAUNCHPAD_LAYOUT_XY   = 1,
		LAUNCHPAD_LAYOUT_DRUM = 2
	};

	class LaunchPad : public NovationMidiDevice
	{

		public:
		LaunchPad(const char *id) : NovationMidiDevice(id) {}

		bool SetLayout(LaunchPadLayout layout) const;
		bool ScrollText(const char *text, PadColor color, bool loop) const;
		bool RapidLedUpdate(const PadColor *colors, uint16_t len) const;

		protected:
		bool MakePadValue(Pad *pad, uint8_t *value, uint8_t *message, uint16_t len);
	};
}

#endif
