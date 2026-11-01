#ifndef SYSTEM_H
#define SYSTEM_H

#include <clownmdsdk.h>

namespace MD = ClownMDSDK::MainCPU;

#ifdef __CLOWNMDSDK_IP__
namespace MCD_RAM = MD::MegaCD::CDBoot;
#else
namespace MCD_RAM = MD::MegaCD::CartridgeBoot;
#endif

#endif // SYSTEM_H
