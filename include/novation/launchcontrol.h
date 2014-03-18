#ifndef LMTZ_NOVATION_LAUNCHCONTROL
#define LMTZ_NOVATION_LAUNCHCONTROL

#include <novation/common.h>

namespace Novation
{
	class LaunchControl : public NovationMidiDevice
	{
		public:
		LaunchControl(const char *id) : NovationMidiDevice(id) {}

		bool ChangeTemplate(uint8_t templateId) const;
		bool SetTemplatePadColor(const Pad& pad, PadColor color) const;

		protected:
		bool MakePadValue(Pad *pad, uint8_t *value, uint8_t *message, uint16_t len);
	};
}

#endif
