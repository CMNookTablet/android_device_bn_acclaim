/*
 * Copyright (C) 2008 The Android Open Source Project
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

#ifndef CAMERA_METADATA_H
#define CAMERA_METADATA_H

#include <utils/Timers.h>

typedef struct camera_metadata_face {
    int32_t top;
    int32_t left;
    int32_t bottom;
    int32_t right;
} camera_metadata_face_t;

// Structure that handles the Camera Frame metadata
typedef struct camera_metadata
{
    // Used to store the information about frame
    // number in processing sequence (i.e preview)
    uint32_t frame_number;

    // Used to store the information about shot number
    // in a burst sequence.
    uint32_t shot_number;

    // Used to store analog gain information for
    // current frame. Metadata is represented as 100*EV.
    uint32_t analog_gain;

    // Used for storing analog gain information
    // requested by application for current frame. Metadata is represented as 100*EV.
    int32_t analog_gain_req;

    // Used for storing the analog gain
    // lower limit for current frame. Metadata is represented as 100*EV.
    uint32_t analog_gain_min;

    // Used for storing the analog gain
    // upper limit for current frame. Metadata is represented as 100*EV.
    uint32_t analog_gain_max;

    // Used for storing the analog gain
    // deviation after flicker reduction for current frame. Metadata is represented as 100*EV.
    int32_t analog_gain_dev;

    // Used for storing analog gain error for
    // current frame. Represents the difference between requested value and actual value.
    uint32_t analog_gain_error;

    // Used for storing the exposure time for current frame.
    // Metadata is represented in us.
    uint32_t exposure_time;

    // Used for storing the exposure time requested by
    // application for current frame. Metadata is represented in us.
    int32_t exposure_time_req;

    // Used for storing the exposure time lower limit for
    // current frame. Metadata is represented in us.
    uint32_t exposure_time_min;

    // Used for storing the exposure time upper limit for
    // current frame. Metadata is represented in us.
    uint32_t exposure_time_max;

    // Used for storing the exposure time
    // deviation after flicker reduction for current frame. Metadata is represented in us.
    int32_t exposure_time_dev;

    // Used for storing the time difference between
    // requested exposure time and actual exposure time.
    int32_t exposure_time_error;

    // Used for storing the current total exposure
    // compensation requested by application for current frame.  Metadata is represented as 100*EV.
    int32_t exposure_compensation_req;

    // Used for storing current total exposure
    // deviation for current frame.  Metadata is represented as 100*EV.
    int32_t exposure_dev;

    // Represents the timestamp in terms of a reference clock.
    nsecs_t timestamp;

    // Represents the temperature of current scene in Kelvin
    uint32_t awb_temp;

    // Represent gains applied to each RGGB color channel.
    uint32_t gain_r;
    uint32_t gain_gr;
    uint32_t gain_gb;
    uint32_t gain_b;

    // Represent offsets applied to each RGGB color channel.
    uint32_t offset_r;
    uint32_t offset_gr;
    uint32_t offset_gb;
    uint32_t offset_b;

    // Offset to a lens shading correction table.  The table consists of an
    // N by M array of elements.  Each element has 4 integer values
    // ranging from 0 to 1000, corresponding to a multiplier for
    // each of the Bayer color filter channels (R, Gr, Gb, B).
    // Correction is performed on pixels in a Bayer image by interpolating
    // the corresponding color filter channel in the array, and then
    // multiplying by (value/1000).
    uint32_t lsc_table_offset;

    // Stores the size of the lens shading correction table
    size_t lsc_table_size;

    // Indicates whether LSC table is applied or not
    uint32_t lsc_table_applied;

    // The number of detected faces in the frame.
    int32_t number_of_faces;

    // Offset to an 'camera_metadata_face' array of the detected faces.
    //  The length is number_of_faces.
    uint32_t faces_offset;

    void *handle;
} camera_metadata_t;

#endif //CAMERA_METADATA_H

