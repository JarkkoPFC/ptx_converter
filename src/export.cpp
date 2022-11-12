//============================================================================
// ptx_conv - PTX Texture Converter
//
// Copyright (c) 2022, Jarkko Lempiainen, Inc.
// All rights reserved.
//============================================================================

#include "export.h"
using namespace pfc;
//----------------------------------------------------------------------------


//============================================================================
// e_ptx_type
//============================================================================
#define PFC_ENUM_TYPE e_ptx_type
#define PFC_ENUM_PREFIX ptxtype_
#define PFC_ENUM_VALS PFC_ENUM_VAL(2d)\
                      PFC_ENUM_VAL(3d)\
                      PFC_ENUM_VAL(cube)\
                      PFC_ENUM_VAL(array2d)
#include "sxp_src/core/enum.inc"
//----------------------------------------------------------------------------


//============================================================================
// e_ptx_format
//============================================================================
#define PFC_ENUM_TYPE e_ptx_format
#define PFC_ENUM_PREFIX ptxfmt_
#define PFC_ENUM_VALS PFC_ENUM_VAL(r3g3b2)\
                      PFC_ENUM_VAL(r5g6b5)\
                      PFC_ENUM_VAL(b5g6r5)\
                      PFC_ENUM_VAL(r5g5b5a1)\
                      PFC_ENUM_VAL(r4g4b4a4)\
                      PFC_ENUM_VAL(r8)\
                      PFC_ENUM_VAL(r8g8)\
                      PFC_ENUM_VAL(r8g8b8)\
                      PFC_ENUM_VAL(b8g8r8)\
                      PFC_ENUM_VAL(r8g8b8a8)\
                      PFC_ENUM_VAL(b8g8r8a8)\
                      PFC_ENUM_VAL(r11g11b10)\
                      PFC_ENUM_VAL(r11g11b10f)\
                      PFC_ENUM_VAL(r10g10b10a2)\
                      PFC_ENUM_VAL(r10g10b10a2f)\
                      PFC_ENUM_VAL(r16)\
                      PFC_ENUM_VAL(r16f)\
                      PFC_ENUM_VAL(r16g16)\
                      PFC_ENUM_VAL(r16g16f)\
                      PFC_ENUM_VAL(r16g16b16)\
                      PFC_ENUM_VAL(r16g16b16f)\
                      PFC_ENUM_VAL(r16g16b16a16)\
                      PFC_ENUM_VAL(r16g16b16a16f)\
                      PFC_ENUM_VAL(r32)\
                      PFC_ENUM_VAL(r32f)\
                      PFC_ENUM_VAL(r32g32)\
                      PFC_ENUM_VAL(r32g32f)\
                      PFC_ENUM_VAL(r32g32b32)\
                      PFC_ENUM_VAL(r32g32b32f)\
                      PFC_ENUM_VAL(r32g32b32a32)\
                      PFC_ENUM_VAL(r32g32b32a32f)
#include "sxp_src/core/enum.inc"
//----------------------------------------------------------------------------


//============================================================================
// e_ptx_layout
//============================================================================
#define PFC_ENUM_TYPE e_ptx_layout
#define PFC_ENUM_PREFIX ptxlayout_
#define PFC_ENUM_VALS PFC_ENUM_VAL(linear)
#include "sxp_src/core/enum.inc"
//----------------------------------------------------------------------------


//============================================================================
// export_ptx
//============================================================================
bool export_ptx(bin_output_stream_base &fout_, ptx_data &ptx_)
{
  enum {ptx_header_size=32};
  uint16_t flags=0;
  uint32_t total_fsize=uint32_t(ptx_header_size+ptx_.num_bytes);
  ptx_.file_size=total_fsize;

  // log stats
  logf("      Type: %s\r\n", enum_string(ptx_.type));
  log( "      Size: ");
  switch(ptx_.type)
  {
    case ptxtype_2d: logf("%i x %i", ptx_.width, ptx_.height); break;
    case ptxtype_3d: logf("%i x %i x %i", ptx_.width, ptx_.height, ptx_.depth); break;
    case ptxtype_cube: logf("%i x %i", ptx_.width, ptx_.height); break;
    case ptxtype_array2d: logf("%i x %i, %i slices", ptx_.width, ptx_.height, ptx_.depth); break;
  }
  logf(", %i LODs\r\n", ptx_.num_lods);
  logf("    Format: %s (%ibpp)\r\n", enum_string(ptx_.format), ptx_.bpp);
  logf("    Layout: %s\r\n", enum_string(ptx_.layout));
  logf(" File size: %i bytes\r\n", total_fsize);

  // output file header
  fout_.write_bytes("ptex", 4); /*todo; support big-endian?*/
  fout_<<uint16_t(ptx_file_version);
  fout_<<uint16_t(flags);
  fout_<<uint32_t(total_fsize);
  fout_<<uint8_t(ptx_.type);
  fout_<<uint8_t(ptx_.format);
  fout_<<uint8_t(ptx_.layout);
  fout_<<uint8_t(ptx_.num_lods);
  fout_<<uint16_t(ptx_.width);
  fout_<<uint16_t(ptx_.height);
  fout_<<uint16_t(ptx_.depth);
  fout_<<uint16_t(0)<<uint16_t(0)<<uint16_t(0)<<uint16_t(0)<<uint16_t(0);
  PFC_ASSERT(fout_.pos()==ptx_header_size);
  fout_.write_bytes(ptx_.pixel_data.data, ptx_.num_bytes);
  PFC_ASSERT(fout_.pos()==total_fsize);
  return true;
}
//----------------------------------------------------------------------------
