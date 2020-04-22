#!/bin/sh
# Run this script inside yojimbo directory

fd -t f -x \
  sed -i '
  s/yojimbo_serialize_ack_relative/yojimbo_yojimbo_serialize_ack_relative/g
  s/yojimbo_serialize_address/yojimbo_yojimbo_serialize_address/g
  s/yojimbo_serialize_align/yojimbo_yojimbo_serialize_align/g
  s/yojimbo_serialize_bits/yojimbo_yojimbo_serialize_bits/g
  s/yojimbo_serialize_bool/yojimbo_yojimbo_serialize_bool/g
  s/yojimbo_serialize_bytes/yojimbo_yojimbo_serialize_bytes/g
  s/yojimbo_serialize_check/yojimbo_yojimbo_serialize_check/g
  s/yojimbo_serialize_double/yojimbo_yojimbo_serialize_double/g
  s/yojimbo_serialize_float/yojimbo_yojimbo_serialize_float/g
  s/yojimbo_serialize_int/yojimbo_yojimbo_serialize_int/g
  s/yojimbo_yojimbo_serialize_int_relative/yojimbo_yojimbo_yojimbo_serialize_int_relative/g
  s/yojimbo_serialize_object/yojimbo_yojimbo_serialize_object/g
  s/yojimbo_serialize_sequence_relative/yojimbo_yojimbo_serialize_sequence_relative/g
  s/yojimbo_serialize_string/yojimbo_yojimbo_serialize_string/g
  s/yojimbo_serialize_uint32/yojimbo_yojimbo_serialize_uint32/g
  s/yojimbo_serialize_uint64/yojimbo_yojimbo_serialize_uint64/g
  s/yojimbo_serialize_varint32/yojimbo_yojimbo_serialize_varint32/g
  s/yojimbo_serialize_varint64/yojimbo_yojimbo_serialize_varint64/g
  s/yojimbo_read_bits/yojimbo_yojimbo_read_bits/g
  s/yojimbo_read_int/yojimbo_yojimbo_read_int/g
  s/yojimbo_read_bool/yojimbo_yojimbo_read_bool/g
  s/yojimbo_read_float/yojimbo_yojimbo_read_float/g
  s/yojimbo_read_uint32/yojimbo_yojimbo_read_uint32/g
  s/yojimbo_read_uint64/yojimbo_yojimbo_read_uint64/g
  s/yojimbo_read_double/yojimbo_yojimbo_read_double/g
  s/yojimbo_read_bytes/yojimbo_yojimbo_read_bytes/g
  s/yojimbo_read_string/yojimbo_yojimbo_read_string/g
  s/yojimbo_read_align/yojimbo_yojimbo_read_align/g
  s/yojimbo_read_check/yojimbo_yojimbo_read_check/g
  s/yojimbo_read_object/yojimbo_yojimbo_read_object/g
  s/yojimbo_read_address/yojimbo_yojimbo_read_address/g
  s/yojimbo_yojimbo_read_int_relative/yojimbo_yojimbo_yojimbo_read_int_relative/g
  s/yojimbo_read_ack_relative/yojimbo_yojimbo_read_ack_relative/g
  s/yojimbo_read_sequence_relative/yojimbo_yojimbo_read_sequence_relative/g
  s/yojimbo_write_bits/yojimbo_yojimbo_write_bits/g
  s/yojimbo_write_int/yojimbo_yojimbo_write_int/g
  s/yojimbo_write_bool/yojimbo_yojimbo_write_bool/g
  s/yojimbo_write_float/yojimbo_yojimbo_write_float/g
  s/yojimbo_write_uint32/yojimbo_yojimbo_write_uint32/g
  s/yojimbo_write_uint64/yojimbo_yojimbo_write_uint64/g
  s/yojimbo_write_double/yojimbo_yojimbo_write_double/g
  s/yojimbo_write_bytes/yojimbo_yojimbo_write_bytes/g
  s/yojimbo_write_string/yojimbo_yojimbo_write_string/g
  s/yojimbo_write_align/yojimbo_yojimbo_write_align/g
  s/yojimbo_write_check/yojimbo_yojimbo_write_check/g
  s/yojimbo_write_object/yojimbo_yojimbo_write_object/g
  s/yojimbo_write_address/yojimbo_yojimbo_write_address/g
  s/yojimbo_yojimbo_write_int_relative/yojimbo_yojimbo_yojimbo_write_int_relative/g
  s/yojimbo_write_ack_relative/yojimbo_yojimbo_write_ack_relative/g
  s/yojimbo_write_sequence_relative/yojimbo_yojimbo_write_sequence_relative/g
  ' {}
