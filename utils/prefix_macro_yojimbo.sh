#!/bin/sh
# Run this script inside yojimbo directory

fd -t f -x \
  sed -i '
  s/serialize_ack_relative/yojimbo_serialize_ack_relative/g
  s/serialize_address/yojimbo_serialize_address/g
  s/serialize_align/yojimbo_serialize_align/g
  s/serialize_bits/yojimbo_serialize_bits/g
  s/serialize_bool/yojimbo_serialize_bool/g
  s/serialize_bytes/yojimbo_serialize_bytes/g
  s/serialize_check/yojimbo_serialize_check/g
  s/serialize_double/yojimbo_serialize_double/g
  s/serialize_float/yojimbo_serialize_float/g
  s/serialize_int/yojimbo_serialize_int/g
  s/serialize_int_relative/yojimbo_serialize_int_relative/g
  s/serialize_object/yojimbo_serialize_object/g
  s/serialize_sequence_relative/yojimbo_serialize_sequence_relative/g
  s/serialize_string/yojimbo_serialize_string/g
  s/serialize_uint32/yojimbo_serialize_uint32/g
  s/serialize_uint64/yojimbo_serialize_uint64/g
  s/serialize_varint32/yojimbo_serialize_varint32/g
  s/serialize_varint64/yojimbo_serialize_varint64/g
  s/read_bits/yojimbo_read_bits/g
  s/read_int/yojimbo_read_int/g
  s/read_bool/yojimbo_read_bool/g
  s/read_float/yojimbo_read_float/g
  s/read_uint32/yojimbo_read_uint32/g
  s/read_uint64/yojimbo_read_uint64/g
  s/read_double/yojimbo_read_double/g
  s/read_bytes/yojimbo_read_bytes/g
  s/read_string/yojimbo_read_string/g
  s/read_align/yojimbo_read_align/g
  s/read_check/yojimbo_read_check/g
  s/read_object/yojimbo_read_object/g
  s/read_address/yojimbo_read_address/g
  s/read_int_relative/yojimbo_read_int_relative/g
  s/read_ack_relative/yojimbo_read_ack_relative/g
  s/read_sequence_relative/yojimbo_read_sequence_relative/g
  s/write_bits/yojimbo_write_bits/g
  s/write_int/yojimbo_write_int/g
  s/write_bool/yojimbo_write_bool/g
  s/write_float/yojimbo_write_float/g
  s/write_uint32/yojimbo_write_uint32/g
  s/write_uint64/yojimbo_write_uint64/g
  s/write_double/yojimbo_write_double/g
  s/write_bytes/yojimbo_write_bytes/g
  s/write_string/yojimbo_write_string/g
  s/write_align/yojimbo_write_align/g
  s/write_check/yojimbo_write_check/g
  s/write_object/yojimbo_write_object/g
  s/write_address/yojimbo_write_address/g
  s/write_int_relative/yojimbo_write_int_relative/g
  s/write_ack_relative/yojimbo_write_ack_relative/g
  s/write_sequence_relative/yojimbo_write_sequence_relative/g
  ' {}
