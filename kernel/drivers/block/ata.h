#ifndef ATA_H
#define ATA_H

#include <klibc/types.h>

typedef struct identify_device_data_t
{
  struct
  {
    kuint16_t reserved1 : 1;
    kuint16_t retired3 : 1;
    kuint16_t response_incomplete : 1;
    kuint16_t retired2 : 3;
    kuint16_t fixed_device : 1;
    kuint16_t removable_media : 1;
    kuint16_t retired1 : 7;
    kuint16_t device_type : 1;
  } general_configuration;
  kuint16_t num_cylinders;
  kuint16_t specific_configuration;
  kuint16_t num_heads;
  kuint16_t retired1[2];
  kuint16_t num_sectors_per_track;
  kuint16_t vendor_unique1[3];
  char serial_number[20];
  kuint16_t retired2[2];
  kuint16_t obsolete1;
  char firmware_revision[8];
  char model_number[40];
  kuint8_t maximum_block_transfer;
  kuint8_t vendor_unique2;
  struct
  {
    kuint16_t feature_supported : 1;
    kuint16_t reserved : 15;
  } trusted_computing;
  struct
  {
    kuint8_t current_long_physical_sector_alignment : 2;
    kuint8_t reserved_byte49 : 6;
    kuint8_t dma_supported : 1;
    kuint8_t lba_supported : 1;
    kuint8_t iordy_disable : 1;
    kuint8_t iordy_supported : 1;
    kuint8_t reserved1 : 1;
    kuint8_t standby_timer_support : 1;
    kuint8_t reserved2 : 2;
    kuint16_t reserved_word50;
  } capabilities;
  kuint16_t obsolete_words51[2];
  kuint16_t translation_fields_valid : 3;
  kuint16_t reserved3 : 5;
  kuint16_t free_fall_control_sensitivity : 8;
  kuint16_t number_of_current_cylinders;
  kuint16_t number_of_current_heads;
  kuint16_t current_sectors_per_track;
  kuint32_t current_sector_capacity;
  kuint8_t current_multi_sector_setting;
  kuint8_t multi_sector_setting_valid : 1;
  kuint8_t reserved_byte59 : 3;
  kuint8_t sanitize_feature_supported : 1;
  kuint8_t crypto_scramble_ext_command_supported : 1;
  kuint8_t overwrite_ext_command_supported : 1;
  kuint8_t block_erase_ext_command_supported : 1;
  kuint32_t user_addressable_sectors;
  kuint16_t obsolete_word62;
  kuint16_t multi_word_dma_support : 8;
  kuint16_t multi_word_dma_active : 8;
  kuint16_t advanced_pio_modes : 8;
  kuint16_t reserved_byte64 : 8;
  kuint16_t minimum_mw_xfer_cycle_time;
  kuint16_t recommended_mw_xfer_cycle_time;
  kuint16_t minimum_pio_cycle_time;
  kuint16_t minimum_pio_cycle_time_iordy;

  struct
  {
    kuint16_t zoned_capabilities : 2;
    kuint16_t non_volatile_write_cache : 1;
    kuint16_t extended_user_addressable_sectors_supported : 1;
    kuint16_t device_encrypts_all_user_data : 1;
    kuint16_t read_zero_after_trim_supported : 1;
    kuint16_t optional28_bit_commands_supported : 1;
    kuint16_t ieee1667 : 1;
    kuint16_t download_microcode_dma_supported : 1;
    kuint16_t set_max_set_password_unlock_dma_supported : 1;
    kuint16_t write_buffer_dma_supported : 1;
    kuint16_t read_buffer_dma_supported : 1;
    kuint16_t device_config_identify_set_dma_supported : 1;
    kuint16_t lpsaerc_supported : 1;
    kuint16_t deterministic_read_after_trim_supported : 1;
    kuint16_t c_fast_spec_supported : 1;
  } additional_supported;

  kuint16_t reserved_words70[5];
  kuint16_t queue_depth : 5;
  kuint16_t reserved_word75 : 11;

  struct
  {
    kuint16_t reserved0 : 1;
    kuint16_t sata_gen1 : 1;
    kuint16_t sata_gen2 : 1;
    kuint16_t sata_gen3 : 1;
    kuint16_t reserved1 : 4;
    kuint16_t ncq : 1;
    kuint16_t hipm : 1;
    kuint16_t phy_events : 1;
    kuint16_t ncq_unload : 1;
    kuint16_t ncq_priority : 1;
    kuint16_t host_auto_ps : 1;
    kuint16_t device_auto_ps : 1;
    kuint16_t read_log_dma : 1;
    kuint16_t reserved2 : 1;
    kuint16_t current_speed : 3;
    kuint16_t ncq_streaming : 1;
    kuint16_t ncq_queue_mgmt : 1;
    kuint16_t ncq_receive_send : 1;
    kuint16_t devsl_pto_reduced_pwr_state : 1;
    kuint16_t reserved3 : 8;
  } serial_ata_capabilities;

  struct
  {
    kuint16_t reserved0 : 1;
    kuint16_t non_zero_offsets : 1;
    kuint16_t dma_setup_auto_activate : 1;
    kuint16_t dipm : 1;
    kuint16_t in_order_data : 1;
    kuint16_t hardware_feature_control : 1;
    kuint16_t software_settings_preservation : 1;
    kuint16_t ncq_autosense : 1;
    kuint16_t devslp : 1;
    kuint16_t hybrid_information : 1;
    kuint16_t reserved1 : 6;
  } serial_ata_features_supported;

  struct
  {
    kuint16_t reserved0 : 1;
    kuint16_t non_zero_offsets : 1;
    kuint16_t dma_setup_auto_activate : 1;
    kuint16_t dipm : 1;
    kuint16_t in_order_data : 1;
    kuint16_t hardware_feature_control : 1;
    kuint16_t software_settings_preservation : 1;
    kuint16_t device_auto_ps : 1;
    kuint16_t devslp : 1;
    kuint16_t hybrid_information : 1;
    kuint16_t reserved1 : 6;
  } serial_ata_features_enabled;

  kuint16_t major_revision;
  kuint16_t minor_revision;

  struct
  {
    kuint16_t smart_commands : 1;
    kuint16_t security_mode : 1;
    kuint16_t removable_media_feature : 1;
    kuint16_t power_management : 1;
    kuint16_t reserved1 : 1;
    kuint16_t write_cache : 1;
    kuint16_t look_ahead : 1;
    kuint16_t release_interrupt : 1;
    kuint16_t service_interrupt : 1;
    kuint16_t device_reset : 1;
    kuint16_t host_protected_area : 1;
    kuint16_t obsolete1 : 1;
    kuint16_t write_buffer : 1;
    kuint16_t read_buffer : 1;
    kuint16_t nop : 1;
    kuint16_t obsolete2 : 1;
    kuint16_t download_microcode : 1;
    kuint16_t dma_queued : 1;
    kuint16_t cfa : 1;
    kuint16_t advanced_pm : 1;
    kuint16_t msn : 1;
    kuint16_t power_up_in_standby : 1;
    kuint16_t manual_power_up : 1;
    kuint16_t reserved2 : 1;
    kuint16_t set_max : 1;
    kuint16_t acoustics : 1;
    kuint16_t big_lba : 1;
    kuint16_t device_config_overlay : 1;
    kuint16_t flush_cache : 1;
    kuint16_t flush_cache_ext : 1;
    kuint16_t word_valid83 : 2;
    kuint16_t smart_error_log : 1;
    kuint16_t smart_self_test : 1;
    kuint16_t media_serial_number : 1;
    kuint16_t media_card_pass_through : 1;
    kuint16_t streaming_feature : 1;
    kuint16_t gp_logging : 1;
    kuint16_t write_fua : 1;
    kuint16_t write_queued_fua : 1;
    kuint16_t wwn64_bit : 1;
    kuint16_t urg_read_stream : 1;
    kuint16_t urg_write_stream : 1;
    kuint16_t reserved_for_tech_report : 2;
    kuint16_t idle_with_unload_feature : 1;
    kuint16_t word_valid : 2;
  } command_set_support;

  struct
  {
    kuint16_t smart_commands : 1;
    kuint16_t security_mode : 1;
    kuint16_t removable_media_feature : 1;
    kuint16_t power_management : 1;
    kuint16_t reserved1 : 1;
    kuint16_t write_cache : 1;
    kuint16_t look_ahead : 1;
    kuint16_t release_interrupt : 1;
    kuint16_t service_interrupt : 1;
    kuint16_t device_reset : 1;
    kuint16_t host_protected_area : 1;
    kuint16_t obsolete1 : 1;
    kuint16_t write_buffer : 1;
    kuint16_t read_buffer : 1;
    kuint16_t nop : 1;
    kuint16_t obsolete2 : 1;
    kuint16_t download_microcode : 1;
    kuint16_t dma_queued : 1;
    kuint16_t cfa : 1;
    kuint16_t advanced_pm : 1;
    kuint16_t msn : 1;
    kuint16_t power_up_in_standby : 1;
    kuint16_t manual_power_up : 1;
    kuint16_t reserved2 : 1;
    kuint16_t set_max : 1;
    kuint16_t acoustics : 1;
    kuint16_t big_lba : 1;
    kuint16_t device_config_overlay : 1;
    kuint16_t flush_cache : 1;
    kuint16_t flush_cache_ext : 1;
    kuint16_t reserved3 : 1;
    kuint16_t words119_120_valid : 1;
    kuint16_t smart_error_log : 1;
    kuint16_t smart_self_test : 1;
    kuint16_t media_serial_number : 1;
    kuint16_t media_card_pass_through : 1;
    kuint16_t streaming_feature : 1;
    kuint16_t gp_logging : 1;
    kuint16_t write_fua : 1;
    kuint16_t write_queued_fua : 1;
    kuint16_t wwn64_bit : 1;
    kuint16_t urg_read_stream : 1;
    kuint16_t urg_write_stream : 1;
    kuint16_t reserved_for_tech_report : 2;
    kuint16_t idle_with_unload_feature : 1;
    kuint16_t reserved4 : 2;
  } command_set_active;

  kuint16_t ultra_dma_support : 8;
  kuint16_t ultra_dma_active : 8;

  struct
  {
    kuint16_t time_required : 15;
    kuint16_t extended_time_reported : 1;
  } normal_security_erase_unit;

  struct
  {
    kuint16_t time_required : 15;
    kuint16_t extended_time_reported : 1;
  } enhanced_security_erase_unit;

  kuint16_t current_apm_level : 8;
  kuint16_t reserved_word91 : 8;
  kuint16_t master_password_id;
  kuint16_t hardware_reset_result;
  kuint16_t current_acoustic_value : 8;
  kuint16_t recommended_acoustic_value : 8;
  kuint16_t stream_min_request_size;
  kuint16_t streaming_transfer_time_dma;
  kuint16_t streaming_access_latency_dmapio;
  kuint32_t streaming_perf_granularity;
  kuint32_t max48_bit_lba[2];
  kuint16_t streaming_transfer_time;
  kuint16_t dsm_cap;

  struct
  {
    kuint16_t logical_sectors_per_physical_sector : 4;
    kuint16_t reserved0 : 8;
    kuint16_t logical_sector_longer_than256_words : 1;
    kuint16_t multiple_logical_sectors_per_physical_sector : 1;
    kuint16_t reserved1 : 2;
  } physical_logical_sector_size;

  kuint16_t inter_seek_delay;
  kuint16_t world_wide_name[4];
  kuint16_t reserved_for_world_wide_name128[4];
  kuint16_t reserved_for_tlc_technical_report;
  kuint16_t words_per_logical_sector[2];

  struct
  {
    kuint16_t reserved_for_drq_technical_report : 1;
    kuint16_t write_read_verify : 1;
    kuint16_t write_uncorrectable_ext : 1;
    kuint16_t read_write_log_dma_ext : 1;
    kuint16_t download_microcode_mode3 : 1;
    kuint16_t freefall_control : 1;
    kuint16_t sense_data_reporting : 1;
    kuint16_t extended_power_conditions : 1;
    kuint16_t reserved0 : 6;
    kuint16_t word_valid : 2;
  } command_set_support_ext;

  struct
  {
    kuint16_t reserved_for_drq_technical_report : 1;
    kuint16_t write_read_verify : 1;
    kuint16_t write_uncorrectable_ext : 1;
    kuint16_t read_write_log_dma_ext : 1;
    kuint16_t download_microcode_mode3 : 1;
    kuint16_t freefall_control : 1;
    kuint16_t sense_data_reporting : 1;
    kuint16_t extended_power_conditions : 1;
    kuint16_t reserved0 : 6;
    kuint16_t reserved1 : 2;
  } command_set_active_ext;
  kuint16_t reserved_for_expanded_support_and_active[6];
  kuint16_t msn_support : 2;
  kuint16_t reserved_word127 : 14;
  struct
  {
    kuint16_t security_supported : 1;
    kuint16_t security_enabled : 1;
    kuint16_t security_locked : 1;
    kuint16_t security_frozen : 1;
    kuint16_t security_count_expired : 1;
    kuint16_t enhanced_security_erase_supported : 1;
    kuint16_t reserved0 : 2;
    kuint16_t security_level : 1;
    kuint16_t reserved1 : 7;
  } security_status;
  kuint16_t reserved_word129[31];
  struct
  {
    kuint16_t maximum_current_in_ma : 12;
    kuint16_t cfa_power_mode_1disabled : 1;
    kuint16_t cfa_power_mode1_required : 1;
    kuint16_t reserved0 : 1;
    kuint16_t word160_supported : 1;
  } cfa_power_mode1;
  kuint16_t reserved_for_cfa_word161[7];
  kuint16_t nominal_form_factor : 4;
  kuint16_t reserved_word168 : 12;
  struct
  {
    kuint16_t supports_trim : 1;
    kuint16_t reserved0 : 15;
  } data_set_management_feature;
  kuint16_t additional_product_id[4];
  kuint16_t reserved_for_cfa_word174[2];
  kuint16_t current_media_serial_number[30];
  struct
  {
    kuint16_t supported : 1;
    kuint16_t reserved0 : 1;
    kuint16_t write_same_supported : 1;
    kuint16_t error_recovery_control_supported : 1;
    kuint16_t feature_control_supported : 1;
    kuint16_t data_tables_supported : 1;
    kuint16_t reserved1 : 6;
    kuint16_t vendor_specific : 4;
  } sct_command_transport;
  kuint16_t reserved_word207[2];
  struct
  {
    kuint16_t alignment_of_logical_within_physical : 14;
    kuint16_t word209_supported : 1;
    kuint16_t reserved0 : 1;
  } block_alignment;
  kuint16_t write_read_verify_sector_count_mode3_only[2];
  kuint16_t write_read_verify_sector_count_mode2_only[2];
  struct
  {
    kuint16_t nv_cache_power_mode_enabled : 1;
    kuint16_t reserved0 : 3;
    kuint16_t nv_cache_feature_set_enabled : 1;
    kuint16_t reserved1 : 3;
    kuint16_t nv_cache_power_mode_version : 4;
    kuint16_t nv_cache_feature_set_version : 4;
  } nv_cache_capabilities;
  kuint16_t nv_cache_size_lsw;
  kuint16_t nv_cache_size_msw;
  kuint16_t nominal_media_rotation_rate;
  kuint16_t reserved_word218;
  struct
  {
    kuint8_t nv_cache_estimated_time_to_spin_up_in_seconds;
    kuint8_t reserved;
  } nv_cache_options;
  kuint16_t write_read_verify_sector_count_mode : 8;
  kuint16_t reserved_word220 : 8;
  kuint16_t reserved_word221;
  struct
  {
    kuint16_t major_version : 12;
    kuint16_t transport_type : 4;
  } transport_major_version;
  kuint16_t transport_minor_version;
  kuint16_t reserved_word224[6];
  kuint32_t extended_number_of_user_addressable_sectors[2];
  kuint16_t min_blocks_per_download_microcode_mode03;
  kuint16_t max_blocks_per_download_microcode_mode03;
  kuint16_t reserved_word236[19];
  kuint16_t signature : 8;
  kuint16_t check_sum : 8;
} __attribute__ ((packed)) identify_device_data_t;

void ata_init (void);
void ata_print_info (void);
void ata_read_sector (const kuint8_t bus, const kuint8_t drive,
                      const kuint32_t lba, kuint8_t *dst);

#endif
