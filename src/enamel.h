/**
 * This file was generated with Enamel : http://gregoiresage.github.io/enamel
 */

#ifndef ENAMEL_H
#define ENAMEL_H

#include <pebble.h>

// -----------------------------------------------------
// Getter for 'AppTotalHugsNum'
#define APPTOTALHUGSNUM_PRECISION 1
int32_t enamel_get_AppTotalHugsNum();
// -----------------------------------------------------

// -----------------------------------------------------
// Getter for 'AppResetOnSave'
bool enamel_get_AppResetOnSave();
// -----------------------------------------------------

// -----------------------------------------------------
// Getter for 'AppShowSeconds'
bool enamel_get_AppShowSeconds();
// -----------------------------------------------------

void enamel_init();

void enamel_deinit();

typedef void(EnamelSettingsReceivedCallback)(void);

void enamel_register_settings_received(EnamelSettingsReceivedCallback *callback);

#endif