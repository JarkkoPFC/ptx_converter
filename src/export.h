//============================================================================
// ptx_conv - PTX Texture Converter
//
// Copyright (c) 2022, Jarkko Lempiainen, Inc.
// All rights reserved.
//============================================================================

#ifndef PFC_PTX_EXPORT_H
#define PFC_PTX_EXPORT_H
//----------------------------------------------------------------------------


//============================================================================
// interface
//============================================================================
// external
#include "sxp_src/core/streams.h"

// new
enum {ptx_file_version=0x1100};  // v1.1
struct ptx_data;
bool export_ptx(pfc::bin_output_stream_base&, ptx_data&);
//----------------------------------------------------------------------------


//============================================================================
// e_ptx_flags
//============================================================================
enum e_ptx_flags
{
};
//----------------------------------------------------------------------------


//============================================================================
// e_ptx_type
//============================================================================
enum e_ptx_type
{
  ptxtype_none    =0x00,
  ptxtype_2d      =0x01,
  ptxtype_3d      =0x02,
  ptxtype_cube    =0x03,
  ptxtype_array2d =0x04
};
PFC_ENUM(e_ptx_type);
//----------------------------------------------------------------------------


//============================================================================
// e_ptx_format
//============================================================================
enum e_ptx_format
{
  ptxfmt_none          =0x00,
  // <8bpc formats
  ptxfmt_r3g3b2        =0x01,
  ptxfmt_r5g6b5        =0x02,
  ptxfmt_b5g6r5        =0x03,
  ptxfmt_r5g5b5a1      =0x04,
  ptxfmt_r4g4b4a4      =0x05,
  // 8bpc formats
  ptxfmt_r8            =0x10,
  ptxfmt_r8g8          =0x11,
  ptxfmt_r8g8b8        =0x12,
  ptxfmt_b8g8r8        =0x13,
  ptxfmt_r8g8b8a8      =0x14,
  ptxfmt_b8g8r8a8      =0x15,
  // 10-11bpc formats
  ptxfmt_r11g11b10     =0x20,
  ptxfmt_r11g11b10f    =0x21,
  ptxfmt_r10g10b10a2   =0x22,
  ptxfmt_r10g10b10a2f  =0x23,
  // 16bpc formats
  ptxfmt_r16           =0x30,
  ptxfmt_r16f          =0x31,
  ptxfmt_r16g16        =0x32,
  ptxfmt_r16g16f       =0x33,
  ptxfmt_r16g16b16     =0x34,
  ptxfmt_r16g16b16f    =0x35,
  ptxfmt_r16g16b16a16  =0x36,
  ptxfmt_r16g16b16a16f =0x37,
  // 32bpc formats
  ptxfmt_r32           =0x40,
  ptxfmt_r32f          =0x41,
  ptxfmt_r32g32        =0x42,
  ptxfmt_r32g32f       =0x43,
  ptxfmt_r32g32b32     =0x44,
  ptxfmt_r32g32b32f    =0x45,
  ptxfmt_r32g32b32a32  =0x46,
  ptxfmt_r32g32b32a32f =0x47,
};
PFC_ENUM(e_ptx_format);
//----------------------------------------------------------------------------


//============================================================================
// e_ptx_layout
//============================================================================
enum e_ptx_layout
{
  ptxlayout_linear =0x00,
};
PFC_ENUM(e_ptx_layout);
//----------------------------------------------------------------------------


//============================================================================
// ptx_data
//============================================================================
struct ptx_data
{
  pfc::owner_data pixel_data;
  uint16_t width, height, depth;
  uint8_t num_lods;
  uint8_t bpp;
  size_t num_pixels;
  size_t num_bytes;
  size_t file_size;
  e_ptx_format format;
  e_ptx_type type;
  e_ptx_layout layout;
};
//----------------------------------------------------------------------------

//============================================================================
#endif
