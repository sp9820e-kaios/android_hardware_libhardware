/*
 * Copyright (C) 2012 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef ANDROID_INCLUDE_HARDWARE_POWER_H
#define ANDROID_INCLUDE_HARDWARE_POWER_H

#include <stdint.h>
#include <sys/cdefs.h>
#include <sys/types.h>

#include <hardware/hardware.h>

__BEGIN_DECLS

#define POWER_MODULE_API_VERSION_0_1  HARDWARE_MODULE_API_VERSION(0, 1)
#define POWER_MODULE_API_VERSION_0_2  HARDWARE_MODULE_API_VERSION(0, 2)
#define POWER_MODULE_API_VERSION_0_3  HARDWARE_MODULE_API_VERSION(0, 3)

/**
 * The id of this module
 */
#define POWER_HARDWARE_MODULE_ID "power"

/*
 * Power hint identifiers passed to (*powerHint)
 */

typedef enum {
    POWER_HINT_VSYNC = 0x00000001,
    POWER_HINT_INTERACTION = 0x00000002,
    /* DO NOT USE POWER_HINT_VIDEO_ENCODE/_DECODE!  They will be removed in
     * KLP.
     */
    POWER_HINT_VIDEO_ENCODE = 0x00000003,
    POWER_HINT_VIDEO_DECODE = 0x00000004,
    POWER_HINT_LOW_POWER = 0x00000005,
    POWER_HINT_SUSTAINED_PERFORMANCE = 0x00000006,
    POWER_HINT_VR_MODE = 0x00000007,
    POWER_HINT_LAUNCH = 0x00000008,
    POWER_HINT_DISABLE_TOUCH = 0x00000009,

    // Sprd extention
    POWER_HINT_VENDOR = 0x00000010,
    /*POWER_HINT_VENDOR_DDR = 0x7f000001,
    POWER_HINT_VENDOR_CAMERA_PREVIEW = 0x7f000002,
    POWER_HINT_VENDOR_VIDEO_SYNC = 0x7f000003,
    POWER_HINT_VENDOR_VIDEO_60FPS = 0x7f000004,
    POWER_HINT_VENDOR_MP4_PLAYBACK = 0x7f000005,
    POWER_HINT_VENDOR_CAMERA_RECORD = 0x7f000006,
    POWER_HINT_VENDOR_SCREENOF_MP3_PLAYBACK = 0x7f000007,
    POWER_HINT_VENDOR_VEDIO_1080P = 0x7f000008,
    POWER_HINT_VENDOR_CAMERA_PERFORMANCE = 0x7f000009,
    POWER_HINT_VENDOR_MAX,*/

    /* For power save mode */
    POWER_HINT_VENDOR_MODE_NORMAL = 0x7fff0000,
    POWER_HINT_VENDOR_MODE_LOW_POWER = 0x7fff0001,
    POWER_HINT_VENDOR_MODE_POWER_SAVE = 0x7fff0002,
    POWER_HINT_VENDOR_MODE_ULTRA_POWER_SAVE = 0x7fff0003,
    POWER_HINT_VENDOR_MODE_PERFORMANCE = 0x7fff0004,
    POWER_HINT_VENDOR_MODE_MAX,

    POWER_HINT_VENDOR_SCREEN_ON = 0x7fff0030,
    POWER_HINT_VENDOR_SCREEN_OFF = 0x7fff0031,
} power_hint_t;

// Must be kept in sync with definitions in PowerManagerInternal.java
enum {
    POWER_HINT_VENDOR_DDR = 1,
    //POWER_HINT_VENDOR_CAMERA_PREVIEW = 2,
    POWER_HINT_VENDOR_VIDEO_SYNC = 3,
    POWER_HINT_VENDOR_VIDEO_60FPS = 4,
    POWER_HINT_VENDOR_MP4_PLAYBACK = 5,
    //POWER_HINT_VENDOR_CAMERA_RECORD = 6,
    POWER_HINT_VENDOR_SCREENOF_MP3_PLAYBACK = 7,
    POWER_HINT_VENDOR_VEDIO_1080P = 8,
    POWER_HINT_VENDOR_CAMERA_PERFORMANCE = 9,
    POWER_HINT_VENDOR_PERFORMANCE_GTS = 10,
    POWER_HINT_VENDOR_PERFORMANCE_CTS = 11,
    //POWER_HINT_VENDOR_CAMERA_HDR = 12,
    POWER_HINT_VENDOR_CAMERA_HIGH_PERFORMANCE = 12,
    POWER_HINT_VENDOR_PERFORMANCE = 13,
    POWER_HINT_VENDOR_CAMERA_LOW_POWER = 14,
    POWER_HINT_VENDOR_RADIO_CALL = 15,
    POWER_HINT_VENDOR_MAX,
};

typedef enum {
    POWER_FEATURE_DOUBLE_TAP_TO_WAKE = 0x00000001
} feature_t;

/**
 * Every hardware module must have a data structure named HAL_MODULE_INFO_SYM
 * and the fields of this data structure must begin with hw_module_t
 * followed by module specific information.
 */
typedef struct power_module {
    struct hw_module_t common;

    /*
     * (*init)() performs power management setup actions at runtime
     * startup, such as to set default cpufreq parameters.  This is
     * called only by the Power HAL instance loaded by
     * PowerManagerService.
     */
    void (*init)(struct power_module *module);

    /*
     * (*setInteractive)() performs power management actions upon the
     * system entering interactive state (that is, the system is awake
     * and ready for interaction, often with UI devices such as
     * display and touchscreen enabled) or non-interactive state (the
     * system appears asleep, display usually turned off).  The
     * non-interactive state is usually entered after a period of
     * inactivity, in order to conserve battery power during
     * such inactive periods.
     *
     * Typical actions are to turn on or off devices and adjust
     * cpufreq parameters.  This function may also call the
     * appropriate interfaces to allow the kernel to suspend the
     * system to low-power sleep state when entering non-interactive
     * state, and to disallow low-power suspend when the system is in
     * interactive state.  When low-power suspend state is allowed, the
     * kernel may suspend the system whenever no wakelocks are held.
     *
     * on is non-zero when the system is transitioning to an
     * interactive / awake state, and zero when transitioning to a
     * non-interactive / asleep state.
     *
     * This function is called to enter non-interactive state after
     * turning off the screen (if present), and called to enter
     * interactive state prior to turning on the screen.
     */
    void (*setInteractive)(struct power_module *module, int on);

    /*
     * (*powerHint) is called to pass hints on power requirements, which
     * may result in adjustment of power/performance parameters of the
     * cpufreq governor and other controls.  The possible hints are:
     *
     * POWER_HINT_VSYNC
     *
     *     Foreground app has started or stopped requesting a VSYNC pulse
     *     from SurfaceFlinger.  If the app has started requesting VSYNC
     *     then CPU and GPU load is expected soon, and it may be appropriate
     *     to raise speeds of CPU, memory bus, etc.  The data parameter is
     *     non-zero to indicate VSYNC pulse is now requested, or zero for
     *     VSYNC pulse no longer requested.
     *
     * POWER_HINT_INTERACTION
     *
     *     User is interacting with the device, for example, touchscreen
     *     events are incoming.  CPU and GPU load may be expected soon,
     *     and it may be appropriate to raise speeds of CPU, memory bus,
     *     etc.  The data parameter is unused.
     *
     * POWER_HINT_LOW_POWER
     *
     *     Low power mode is activated or deactivated. Low power mode
     *     is intended to save battery at the cost of performance. The data
     *     parameter is non-zero when low power mode is activated, and zero
     *     when deactivated.
     *
     * A particular platform may choose to ignore any hint.
     *
     * availability: version 0.2
     *
     */
    void (*powerHint)(struct power_module *module, power_hint_t hint,
                      void *data);

    /*
     * (*setFeature) is called to turn on or off a particular feature
     * depending on the state parameter. The possible features are:
     *
     * FEATURE_DOUBLE_TAP_TO_WAKE
     *
     *    Enabling/Disabling this feature will allow/disallow the system
     *    to wake up by tapping the screen twice.
     *
     * availability: version 0.3
     *
     */
    void (*setFeature)(struct power_module *module, feature_t feature, int state);

} power_module_t;


__END_DECLS

#endif  // ANDROID_INCLUDE_HARDWARE_POWER_H
