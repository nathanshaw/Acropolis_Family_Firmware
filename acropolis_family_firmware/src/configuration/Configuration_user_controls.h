
#ifndef __CONFIGURATION_USER_CONTROLS_H__
#define __CONFIGURATION_USER_CONTROLS_H__

// to get some definitions such as SPECULATOR
#include <Macros.h>

//////////////////// Will the artefact read the user controls? ////////
#if ARTEFACT_SPECIES == SPEC_MINOR
#define USER_CONTROLS_ACTIVE     false
#else
#define USER_CONTROLS_ACTIVE     true
#endif // species

#endif // __CONFIGURATION_USER_CONTROLS_H__