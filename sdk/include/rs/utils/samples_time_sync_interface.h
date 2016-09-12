// License: Apache 2.0. See LICENSE file in root directory.
// Copyright(c) 2016 Intel Corporation. All Rights Reserved.

#include "rs_sdk.h"
#include "rs/utils/cyclic_array.h"

namespace rs
{
    namespace utils
    {
        class samples_time_sync_interface : public rs::core::release_interface
        {
        public:
            /**
            @brief Create and initialize the sync utility - register streams and motions that are required to be synced.
            @param[in]  streams_fps               Array of fps values for every stream needed to be registered. Zero value streams are not registered.
            @param[in]  motions_fps               Array of fps values for every motion needed to be registered. Zero value motions are not registered.
            @param[in]  max_input_latency         The maximum latency in millisecinds that is allowed to be when receiving two frames from
                                                  different streams with same timestamp. Defines the number of frames to be stored in sync
                                                  utility. Increasing this value will cause a larger number of buffered images.
            */
            static samples_time_sync_interface *
            create_instance(int streams_fps[static_cast<int>(rs::core::stream_type::max)],
                            int motions_fps[static_cast<int>(rs::core::motion_type::max)],
                            const char * device_name
                            );

            static samples_time_sync_interface *
            create_instance(int streams_fps[static_cast<int>(rs::core::stream_type::max)],
                            int motions_fps[static_cast<int>(rs::core::motion_type::max)],
                            const char * device_name,
                            unsigned int max_input_latency
                            );

            static samples_time_sync_interface *
            create_instance(int streams_fps[static_cast<int>(rs::core::stream_type::max)],
                            int motions_fps[static_cast<int>(rs::core::motion_type::max)],
                            const char * device_name,
                            unsigned int max_input_latency,
                            unsigned int not_matched_frames_buffer_size);


            /**
            @brief inserts the new image to the sync utility. Returns true if the correlated sample was found.
            @param[in]  new_image                 New image.
            @param[out] sample_set                Correlated sample containing correlated images and/or motions. May be empty.
            @return                               true if the match was found
            */
            virtual bool insert(rs::core::image_interface * new_image, rs::core::correlated_sample_set& sample_set)= 0;

            /**
            @brief inserts the new motion to the sync utility. Returns true if the correlated sample was found.
            @param[in]  new_motion                New motion.
            @param[out] sample_set                Correlated sample containing correlated images and/or motions. May be empty.
            @return                               true if the match was found            */
            virtual bool insert(rs::core::motion_sample& new_motion, rs::core::correlated_sample_set& sample_set) = 0;


            /**
            @brief Puts the first unmatched frame of stream_type to the location specified by second parameter.
                   Returns true if there are more unmatched frames of this stream_type available.
            @param[out]  not_matched_frame         Smart_ptr to put unmatched frame to
            @return                               true if more unmatched frames available      */
            virtual bool get_not_matched_frame(rs::core::stream_type stream_type, rs::core::image_interface ** not_matched_frame) = 0;

            /**
            @brief Removes all the frames from the internal lists.
            @return void
            */
            virtual void flush() = 0;


            virtual ~samples_time_sync_interface() {};

        };

    }
}
