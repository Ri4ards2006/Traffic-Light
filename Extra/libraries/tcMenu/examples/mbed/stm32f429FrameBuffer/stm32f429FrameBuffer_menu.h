/*
    The code in this file uses open source libraries provided by thecoderscorner

    DO NOT EDIT THIS FILE, IT WILL BE GENERATED EVERY TIME YOU USE THE UI DESIGNER
    INSTEAD EITHER PUT CODE IN YOUR SKETCH OR CREATE ANOTHER SOURCE FILE.

    All the variables you may need access to are marked extern in this file for easy
    use elsewhere.
 */

#ifndef MENU_GENERATED_CODE_H
#define MENU_GENERATED_CODE_H

#include <mbed.h>
#include <tcMenu.h>

#include <tcUnicodeHelper.h>
#include "BspUserSettings.h"
#include "tcMenuStChromaArt.h"
#include <graphics/MenuTouchScreenEncoder.h>
#include <extras/DrawableTouchCalibrator.h>
#include <RuntimeMenuItem.h>
#include <EditableLargeNumberMenuItem.h>
#include <ScrollChoiceMenuItem.h>
#include <IoAbstraction.h>
#include <EepromItemStorage.h>
#include <mbed/HalStm32EepromAbstraction.h>

// variables we declare that you may need to access
extern const PROGMEM ConnectorLocalInfo applicationInfo;
extern StChromaArtDrawable Drawable;
extern GraphicsDeviceRenderer renderer;
extern StBspTouchInterrogator touchInterrogator;
extern MenuTouchScreenManager touchScreen;
extern tcextras::IoaTouchScreenCalibrator touchCalibrator;
extern const UnicodeFont OpenSansCyrillicLatin18[];
extern const GFXfont RobotoMedium24;

// Any externals needed by IO expanders, EEPROMs etc


// Global Menu Item exports
extern AnalogMenuItem menuUnicodeLevel;
extern EnumMenuItem menuUnicodeChoice;
extern BackMenuItem menuBackUnicode;
extern SubMenuItem menuUnicode;
extern ActionMenuItem menuDialogs;
extern BooleanMenuItem menuSamplesBoolCheck;
extern EditableLargeNumberMenuItem menuSamplesLgePos;
extern Rgb32MenuItem menuSamplesRGB;
extern TimeFormattedMenuItem menuSamplesTime;
extern TextMenuItem menuSamplesText;
extern AnalogMenuItem menuBeltSpeed;
extern EnumMenuItem menuBeltStatus;
extern BackMenuItem menuBackSamples;
extern SubMenuItem menuSamples;
extern BooleanMenuItem menuConnectivityEnableUSB;
extern BackMenuItem menuBackConnectivity;
extern SubMenuItem menuConnectivity;
extern AnyMenuInfo minfoSettingsDashboard;
extern ActionMenuItem menuSettingsDashboard;
extern ActionMenuItem menuSettingsCalibrateNow;
extern BooleanMenuItem menuSettingsTSCalibration;
extern TimeFormattedMenuItem menuSettingsRunDuration;
extern AnalogMenuItem menuSettingsTargetSpeed;
extern BackMenuItem menuBackSettings;
extern SubMenuItem menuSettings;
extern AnalogMenuItem menuConsumption;
extern AnalogMenuItem menuACLine;
extern BooleanMenuItem menuPower;

// Provide a wrapper to get hold of the root menu item and export setupMenu
inline MenuItem& rootMenuItem() { return menuPower; }
void setupMenu();

// Callback functions must always include CALLBACK_FUNCTION after the return type
#define CALLBACK_FUNCTION

void CALLBACK_FUNCTION onCalibrateScreen(int id);
void CALLBACK_FUNCTION onPresentDialog(int id);
void CALLBACK_FUNCTION onShowDash(int id);
void CALLBACK_FUNCTION onTargetChanged(int id);
void CALLBACK_FUNCTION onTouchCalibration(int id);

#endif // MENU_GENERATED_CODE_H
