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

// -----------------------------------------------------
// Getter for 'AppBackgroundV2'
const char* enamel_get_AppBackgroundV2();
// -----------------------------------------------------

void enamel_init();

void enamel_deinit();

typedef void* EventHandle;
typedef void(EnamelSettingsReceivedHandler)(void* context);

EventHandle enamel_settings_received_subscribe(EnamelSettingsReceivedHandler *handler, void *context);
void enamel_settings_received_unsubscribe(EventHandle handle);

#endif