#ifndef CONTROL_PAD_H
#define CONTROL_PAD_H

#include "../common/control-pad-manager.h"

inline ControlPadManager<1> control_pad_manager;
inline const auto &control_pads = control_pad_manager.GetControlPads();

#endif // CONTROL_PAD_H
