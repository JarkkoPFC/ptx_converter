//============================================================================
// ptx_conv - PTX Texture Converter
//
// Copyright (c) 2022, Jarkko Lempiainen, Inc.
// All rights reserved.
//============================================================================

#include "export.h"
#include "sxp_src/core_engine/texture.h"
#include "sxp_src/core/fsys/fsys.h"
#include "sxp_src/core/main.h"
using namespace pfc;
//----------------------------------------------------------------------------


//============================================================================
// config
//============================================================================
static const char *s_tool_name="PTX Texture Converter v0.2";
static const char *s_copyright_message="Copyright (c) 2022, Jarkko Lempiainen, Inc. All rights reserved.";
static const char *s_usage_message="Usage: ptx_conv [options] -i <input> -o <output.ptx> -f <format>  (-h for help)";
static const char *s_conversion_fail_msg="Conversion failed!\r\n";
static const char *s_conversion_success_msg="Done!\r\n";
//----------------------------------------------------------------------------


//============================================================================
// bcd16_version_str
//============================================================================
stack_str8 bcd16_version_str(uint16_t version_)
{
  stack_str8 s;
  s.format("%i.", version_>>12);
  version_<<=4;
  do
  {
    s.push_back_format("%x", (version_>>12));
    version_<<=4;
  } while(version_);
  return s;
}
//----------------------------------------------------------------------------


//============================================================================
// str_strip_quotes
//============================================================================
void str_strip_quotes(heap_str &s_)
{
  s_.resize(str_strip_outer_chars(s_.c_str(), '\"'));
}
//----------------------------------------------------------------------------


//============================================================================
// e_ptx_output_type
//============================================================================
enum e_ptx_output_type
{
  ptxouttype_bin,
  ptxouttype_hex,
  ptxouttype_hexd,
};
//----------------------------------------------------------------------------


//============================================================================
// command_arguments
//============================================================================
struct command_arguments
{
  command_arguments()
  {
    output_format=ptxfmt_none;
    ptx_output_type=ptxouttype_bin;
    suppress_copyright=false;
  }
  //----

  heap_str input_file;
  heap_str friendly_input_file;
  heap_str output_file;
  e_ptx_format output_format;
  heap_str friendly_output_file;
  e_ptx_output_type ptx_output_type;
  bool suppress_copyright;
};
//----

bool parse_command_arguments(command_arguments &ca_, const char **args_, unsigned num_args_)
{
  // parse arguments
  heap_str error_msg;
  for(unsigned arg_idx=0; arg_idx<num_args_; ++arg_idx)
  {
    // check compiler for option
    if(args_[arg_idx][0]=='-')
    {
      // switch to proper argument handling
      const char *carg=args_[arg_idx];
      size_t arg_size=str_size(carg);
      switch(to_lower(carg[1]))
      {
        // help
        case 'h':
        {
          if(arg_size==2)
          {
            // output help
            logf("%s (PTX v%s)\r\n" // tool name, desc & version
                 "%s\r\n"     // copyright
                 "\r\n"
                 "%s\r\n"     // usage
                 "\r\n"
                 "Options:\r\n"
                 "  -i <infile>   Input texture file (see below for supported formats)\r\n"
                 "  -o <outfile>  Output file (ptx format)\r\n"
                 "  -f <format>   Target texture format (see below)\r\n"
                 "  -hex          Output data as comma separated byte ASCII hex codes\r\n"
                 "  -hexd         Output data as comma separated dword ASCII hex codes\r\n"
                 "\r\n"
                 "  -h            Print this screen\n"
                 "  -c            Suppress copyright message\r\n"
                 "\r\n"
                 "  <infile>:\r\n"
                 "    jpg/jpeg: RGB, 8bpc, lossy\r\n"
                 "         png: RGB/RGBA, 8/16/32bpc, lossless\r\n"
                 "         tga: RGB/RGBA, 8/15/16/24/32bpp, lossless\r\n"
                 "    tif/tiff: RGB/RGBA, 8/16bpc, lossless/lossy\r\n"
                 "        webp: RGB/RGBA, 8bpc, lossless/lossy\r\n"
                 "         dds: RGB/RGBA/BC, 4-128bpp, lossless/lossy\r\n"
                 "         psd: RGB/RGBA, 8/16/32bpc, lossless\r\n"
                 "         jp2: RGB/RGBA, 4-10bpc, lossless/lossy\r\n"
                 "\r\n"
                 "  <format>:\r\n"
                 "    <8bpc formats:\r\n"
                 "      r3g3b2, r5g6b5, b5g6r5, r5g5b5a1, r4g4b4a4\r\n"
                 "    8bpc formats:\r\n"
                 "      r8, r8g8, r8g8b8, b8g8r8, r8g8b8a8, b8g8r8a8\r\n"
                 "    10-11bpc formats:\r\n"
                 "      r11g11b10, r11g11b10f, r10g10b10a2, r10g10b10a2f\r\n"
                 "    16bpc formats:\r\n"
                 "      r16, r16f, r16g16, r16g16f, r16g16b16, r16g16b16f,\r\n"
                 "      r16g16b16a16, r16g16b16a16f\r\n"
                 "    32bpc formats:\r\n"
                 "      r32, r32f, r32g32, r32g32f, r32g32b32, r32g32b32f,\r\n"
                 "      r32g32b32a32, r32g32b32a32f\r\n",
                 s_tool_name, bcd16_version_str(ptx_file_version).c_str(),
                 s_copyright_message,
                 s_usage_message);
            return false;
          }
          else if(str_eq(carg, "-hex"))
            ca_.ptx_output_type=ptxouttype_hex;
          else if(str_eq(carg, "-hexd"))
            ca_.ptx_output_type=ptxouttype_hexd;
        }

        case 'i':
        {
          if(arg_size==2 && arg_idx<num_args_-1)
          {
            ca_.input_file=args_[++arg_idx];
            str_strip_quotes(ca_.input_file);
            ca_.friendly_input_file=get_filename(ca_.input_file.c_str());
            str_lower(ca_.friendly_input_file.c_str());
          }
        } break;

        // output file
        case 'o':
        {
          if(arg_size==2 && arg_idx<num_args_-1)
          {
            ca_.output_file=args_[++arg_idx];
            str_strip_quotes(ca_.output_file);
            ca_.friendly_output_file=get_filename(ca_.output_file.c_str());
            str_lower(ca_.friendly_output_file.c_str());
          }
        } break;

        // format
        case 'f':
        {
          if(arg_size==2 && arg_idx<num_args_-1)
          {
            heap_str output_format=args_[++arg_idx];
            str_lower(output_format.c_str());
            if(!enum_value(ca_.output_format, output_format.c_str()))
              error_msg.push_back_format("error: Unknown output format \"%s\"\r\n", output_format.c_str());
          }
        } break;

        // suppress copyright text
        case 'c':
        {
          if(arg_size==2)
            ca_.suppress_copyright=true;
        } break;
      }
    }
  }

  // check for help string and copyright message output
  if(!ca_.suppress_copyright)
  {
    logf("%s (PTX v%s)\r\n", s_tool_name, bcd16_version_str(ptx_file_version).c_str());
    logf("%s\r\n\r\n", s_copyright_message);
  }
  if(error_msg.size())
  {
    error(error_msg.c_str());
    return false;
  }
  if(!ca_.input_file.size() || !ca_.output_file.size() || ca_.output_format==ptxfmt_none)
  {
    log(s_usage_message);
    log("\r\n");
    return false;
  }

  return true;
}
//----------------------------------------------------------------------------


//============================================================================
// create_ptx_texture
//============================================================================
#define PFC_TEXFORMAT_LIST \
  PFC_TEXFORMAT(r3g3b2,           8, 1, texfmttype_rgba,          a8r8g8b8,    0,  3,    3,  3,    6,  2,    0,  0,   0x0123)\
  PFC_TEXFORMAT(r5g6b5,          16, 1, texfmttype_rgba,          a8r8g8b8,    0,  5,    5,  6,   11,  5,    0,  0,   0x0123)\
  PFC_TEXFORMAT(b5g6r5,          16, 1, texfmttype_rgba,          a8r8g8b8,   11,  5,    5,  6,    0,  5,    0,  0,   0x0321)\
  PFC_TEXFORMAT(r5g5b5a1,        16, 1, texfmttype_rgba,          a8r8g8b8,    0,  5,    5,  5,   10,  5,   15,  1,   0x1234)\
  PFC_TEXFORMAT(r4g4b4a4,        16, 1, texfmttype_rgba,          a8r8g8b8,    0,  4,    4,  4,    8,  4,   12,  4,   0x1234)\
  PFC_TEXFORMAT(r8,               8, 1, texfmttype_rgba,          a8r8g8b8,    0,  8,    0,  0,    0,  0,    0,  0,   0x0001)\
  PFC_TEXFORMAT(r8g8,            16, 1, texfmttype_rgba,          a8r8g8b8,    0,  8,    8,  8,    0,  0,    0,  0,   0x0012)\
  PFC_TEXFORMAT(r8g8b8,          24, 1, texfmttype_rgba,          a8r8g8b8,    0,  8,    8,  8,   16,  8,    0,  0,   0x0123)\
  PFC_TEXFORMAT(b8g8r8,          24, 1, texfmttype_rgba,          a8r8g8b8,   16,  8,    8,  8,    0,  8,    0,  0,   0x0321)\
  PFC_TEXFORMAT(r8g8b8a8,        32, 1, texfmttype_rgba,          a8r8g8b8,    0,  8,    8,  8,   16,  8,   24,  8,   0x1234)\
  PFC_TEXFORMAT(b8g8r8a8,        32, 1, texfmttype_rgba,          a8r8g8b8,   16,  8,    8,  8,    0,  8,   24,  8,   0x3214)\
  PFC_TEXFORMAT(r11g11b10,       32, 1, texfmttype_rgba,      a16b16g16r16,    0, 11,   11, 11,   22, 10,    0,  0,   0x0123)\
/*  PFC_TEXFORMAT(r11g11b10f,      32, 1, ???????????????,     a32b32g32r32f,    0, 11,   11, 11,   22, 10,    0,  0,   0x0123)*/ \
  PFC_TEXFORMAT(r10g10b10a2,     32, 1, texfmttype_rgba,      a16b16g16r16,    0, 10,   10, 10,   20, 10,   30,  2,   0x0123)\
/*  PFC_TEXFORMAT(r10g10b10a2f,    32, 1, ???????????????,     a32b32g32r32f,    0, 10,   10, 10,   20, 10,   30,  2,   0x0123)*/ \
  PFC_TEXFORMAT(r16,             16, 1, texfmttype_rgba,      a16b16g16r16,    0, 16,    0,  0,    0,  0,    0,  0,   0x0001)\
  PFC_TEXFORMAT(r16f,            16, 1, texfmttype_rgba16f,  a32b32g32r32f,    0, 16,    0,  0,    0,  0,    0,  0,   0x0001)\
  PFC_TEXFORMAT(r16g16,          32, 1, texfmttype_rgba,      a16b16g16r16,    0, 16,   16, 16,    0,  0,    0,  0,   0x0012)\
  PFC_TEXFORMAT(r16g16f,         32, 1, texfmttype_rgba16f,  a32b32g32r32f,    0, 16,   16, 16,    0,  0,    0,  0,   0x0012)\
  PFC_TEXFORMAT(r16g16b16,       48, 1, texfmttype_rgba,      a16b16g16r16,    0, 16,   16, 16,   32, 16,    0,  0,   0x0123)\
  PFC_TEXFORMAT(r16g16b16f,      48, 1, texfmttype_rgba16f,  a32b32g32r32f,    0, 16,   16, 16,   32, 16,    0,  0,   0x0123)\
  PFC_TEXFORMAT(r16g16b16a16,    64, 1, texfmttype_rgba,      a16b16g16r16,    0, 16,   16, 16,   32, 16,   48, 16,   0x1234)\
  PFC_TEXFORMAT(r16g16b16a16f,   64, 1, texfmttype_rgba16f,  a32b32g32r32f,    0, 16,   16, 16,   32, 16,   48, 16,   0x1234)\
  PFC_TEXFORMAT(r32,             32, 1, texfmttype_rgba,      a32b32g32r32,    0, 32,    0,  0,    0,  0,    0,  0,   0x0001)\
  PFC_TEXFORMAT(r32f,            32, 1, texfmttype_rgba32f,  a32b32g32r32f,    0, 32,    0,  0,    0,  0,    0,  0,   0x0001)\
  PFC_TEXFORMAT(r32g32,          64, 1, texfmttype_rgba,      a32b32g32r32,    0, 32,   32, 32,    0,  0,    0,  0,   0x0012)\
  PFC_TEXFORMAT(r32g32f,         64, 1, texfmttype_rgba32f,  a32b32g32r32f,    0, 32,   32, 32,    0,  0,    0,  0,   0x0012)\
  PFC_TEXFORMAT(r32g32b32,       96, 1, texfmttype_rgba,      a32b32g32r32,    0, 32,   32, 32,   64, 32,    0,  0,   0x0123)\
  PFC_TEXFORMAT(r32g32b32f,      96, 1, texfmttype_rgba32f,  a32b32g32r32f,    0, 32,   32, 32,   64, 32,    0,  0,   0x0123)\
  PFC_TEXFORMAT(r32g32b32a32,   128, 1, texfmttype_rgba,      a32b32g32r32,    0, 32,   32, 32,   64, 32,   96, 32,   0x1234)\
  PFC_TEXFORMAT(r32g32b32a32f,  128, 1, texfmttype_rgba32f,  a32b32g32r32f,    0, 32,   32, 32,   64, 32,   96, 32,   0x1234)
//----------------------------------------------------------------------------

template<e_ptx_format format> struct ptx_format_cfg;
#define PFC_TEXFORMAT(name__, bpp__, block_size__, type__, conv_format__, rmask_pos__, rmask_size__, gmask_pos__, gmask_size__, bmask_pos__, bmask_size__, amask_pos__, amask_size__, rgba_order__)\
template<>\
struct ptx_format_cfg<ptxfmt_##name__>\
{\
  enum {bpp=bpp__,\
        block_size=block_size__,\
        type=type__,\
        conv_format=texfmt_##conv_format__,\
        rgba_order=rgba_order__,\
        rmask_pos=rmask_pos__, rmask_size=rmask_size__>0?rmask_size__:0,\
        gmask_pos=gmask_pos__, gmask_size=gmask_size__>0?gmask_size__:0,\
        bmask_pos=bmask_pos__, bmask_size=bmask_size__>0?bmask_size__:0,\
        amask_pos=amask_pos__, amask_size=amask_size__>0?amask_size__:0};\
};
PFC_TEXFORMAT_LIST
#undef PFC_TEXFORMAT
//----------------------------------------------------------------------------

template<e_texture_format format> struct conv_format_cfg;
#define PFC_TEXFORMAT(name__, bpp__, block_size__, type__, conv_format__, rmask_pos__, rmask_size__, gmask_pos__, gmask_size__, bmask_pos__, bmask_size__, amask_pos__, amask_size__, rgba_order__)\
template<>\
struct conv_format_cfg<texfmt_##name__>\
{\
  enum {bpp=bpp__,\
        block_size=block_size__,\
        type=type__,\
        conv_format=texfmt_##conv_format__,\
        rgba_order=rgba_order__,\
        rmask_pos=rmask_pos__, rmask_size=rmask_size__>0?rmask_size__:0,\
        gmask_pos=gmask_pos__, gmask_size=gmask_size__>0?gmask_size__:0,\
        bmask_pos=bmask_pos__, bmask_size=bmask_size__>0?bmask_size__:0,\
        amask_pos=amask_pos__, amask_size=amask_size__>0?amask_size__:0};\
};
//----

PFC_TEXFORMAT(a8r8g8b8,        32, 1, texfmttype_rgba,          a8r8g8b8,   16,  8,    8,  8,    0,  8,   24,  8,   0x4123) \
PFC_TEXFORMAT(a16b16g16r16,    64, 1, texfmttype_rgba,      a32b32g32r32,    0, 16,   16, 16,   32, 16,   48, 16,   0x4321) \
PFC_TEXFORMAT(a16b16g16r16f,   64, 1, texfmttype_rgba16f,  a32b32g32r32f,    0, 16,   16, 16,   32, 16,   48, 16,   0x4321) \
PFC_TEXFORMAT(a32b32g32r32,   128, 1, texfmttype_rgba,      a32b32g32r32,    0, 32,   32, 32,   64, 32,   96, 32,   0x4321) \
PFC_TEXFORMAT(a32b32g32r32f,  128, 1, texfmttype_rgba32f,  a32b32g32r32f,    0, 32,   32, 32,   64, 32,   96, 32,   0x4321)
#undef PFC_TEXFORMAT
//----------------------------------------------------------------------------

template<e_ptx_format dst_fmt>
void create_ptx_texture(texture_loader &tl_, ptx_data &ptx_)
{
  // setup ptx
  typedef ptx_format_cfg<dst_fmt> dst_format_cfg;
  ptx_.width=(uint16_t)tl_.width();
  ptx_.height=(uint16_t)tl_.height();
  ptx_.depth=(uint16_t)tl_.depth();
  ptx_.num_lods=1;
  ptx_.bpp=dst_format_cfg::bpp;
  ptx_.num_pixels=ptx_.width*ptx_.height;
  ptx_.num_bytes=(ptx_.num_pixels*ptx_.bpp)/8;
  ptx_.format=dst_fmt;
  ptx_.type=ptxtype_2d;
  ptx_.layout=ptxlayout_linear;
  ptx_.pixel_data=PFC_MEM_ALLOC(ptx_.num_bytes);

  // load image in the requested format
  enum {conv_format=dst_format_cfg::conv_format};
  typedef conv_format_cfg<e_texture_format(conv_format)> tmp_format_cfg;
  e_texture_format tmp_fmt=e_texture_format(conv_format);
  owner_data tmp_img=PFC_MEM_ALLOC(ptx_.num_pixels*texfmt_bpp(tmp_fmt));
  tl_.set_target_format(tmp_fmt);
  tl_.load_layer(tmp_img.data);
  convert_rgba<dst_format_cfg, tmp_format_cfg>(ptx_.pixel_data.data, tmp_img.data, unsigned(ptx_.num_pixels));
}
//----------------------------------------------------------------------------


//============================================================================
// main
//============================================================================
PFC_MAIN(const char *args_[], unsigned num_args_)
{
  // parse command line arguments
  command_arguments ca;
  if(!parse_command_arguments(ca, args_, num_args_))
    return -1;
  owner_ref<file_system_base> fsys=create_default_file_system(true);

  // try open the input file
  owner_ptr<bin_input_stream_base> fin=fsys->open_read(ca.input_file.c_str(), 0, fopencheck_none);
  if(!fin.data)
  {
    errorf("> Error: Unable to read file \"%s\"\r\n", ca.input_file.c_str());
    errorf(s_conversion_fail_msg);
    return -1;
  }

  // read texture
  texture_loader tl(*fin);
  ptx_data ptx;
  switch(ca.output_format)
  {
    #define PFC_TEXFORMAT(name__, bpp__, block_size__, type__, conv_format__, rmask_pos__, rmask_size__, gmask_pos__, gmask_size__, bmask_pos__, bmask_size__, amask_pos__, amask_size__, rgba_order__)\
      case ptxfmt_##name__: create_ptx_texture<ptxfmt_##name__>(tl, ptx); break;
    PFC_TEXFORMAT_LIST
    #undef PFC_TEXFORMAT
  }

  // open ptx file for writing
  owner_ptr<bin_output_stream_base> fout=fsys->open_write(ca.output_file.c_str());
  if(!fout.data)
  {
    errorf("> Error: Unable to write ptx file \"%s\"\r\n", ca.output_file.c_str());
    errorf(s_conversion_fail_msg);
    return -1;
  }

  logf("> Exporting PTX file \"%s\"...\r\n", ca.friendly_output_file.c_str());
  switch(ca.ptx_output_type)
  {
    // export binary file
    case ptxouttype_bin:
    {
      if(!export_ptx(*fout, ptx))
      {
        errorf(s_conversion_fail_msg);
        return -1;
      }
    } break;

    // export hex ASCII file
    case ptxouttype_hex:
    case ptxouttype_hexd:
    {
      // export data to container
      array<uint8_t> ptx_data;
      {
        container_output_stream<array<uint8_t> > cout(ptx_data);
        if(!export_ptx(cout, ptx))
        {
          errorf(s_conversion_fail_msg);
          return -1;
        }
      }

      // output stats
      stack_str32 size_str;
      switch(ptx.type)
      {
        case ptxtype_2d: size_str.format("%i x %i", ptx.width, ptx.height); break;
        case ptxtype_3d: size_str.format("%i x %i x %i", ptx.width, ptx.height, ptx.depth); break;
        case ptxtype_cube: size_str.format("%i x %i", ptx.width, ptx.height); break;
        case ptxtype_array2d: size_str.format("%i x %i, %i slices", ptx.width, ptx.height, ptx.depth); break;
      }
      text_output_stream ts(*fout);
      ts<<"// Texture file: "<<ca.friendly_input_file.c_str()<<"\r\n"
        <<"//         Type: "<<enum_string(ptx.type)<<"\r\n"
        <<"//         Size: "<<size_str.c_str()<<", "<<ptx.num_lods<<" LODs \r\n"
        <<"//       Format: "<<enum_string(ptx.format)<<" ("<<ptx.bpp<<"bpp)\r\n"
        <<"//       Layout: "<<enum_string(ptx.layout)<<"\r\n"
        <<"//    File size: "<<ptx.file_size<<"\r\n"
        <<"//    Converter: "<<s_tool_name<<" (PTX v"<<bcd16_version_str(ptx_file_version).c_str()<<")\r\n";

      // export the ASCII file
      if(ca.ptx_output_type==ptxouttype_hexd)
      {
        // write data as dword hex codes
        stack_str32 strbuf;
        ptx_data.insert_back((0-ptx_data.size())&3, uint8_t(0));
        size_t dwords_left=ptx_data.size()/4;
        const uint32_t *dwords=(const uint32_t*)ptx_data.data();
        while(dwords_left)
        {
          size_t num_dwords=min<size_t>(dwords_left, 128);
          for(unsigned i=0; i<num_dwords; ++i)
          {
            strbuf.format("0x%08x, ", dwords[i]);
            *fout.data<<strbuf.c_str();
          }
          *fout.data<<"\r\n";
          dwords+=num_dwords;
          dwords_left-=num_dwords;
        }
      }
      else
      {
        // write data as byte hex codes
        stack_str32 strbuf;
        size_t data_left=ptx_data.size();
        const uint8_t *bytes=ptx_data.data();
        while(data_left)
        {
          size_t num_bytes=min<usize_t>(data_left, 256);
          for(unsigned i=0; i<num_bytes; ++i)
          {
            strbuf.format("0x%02x, ", bytes[i]);
            *fout.data<<strbuf.c_str();
          }
          *fout.data<<"\r\n";
          bytes+=num_bytes;
          data_left-=num_bytes;
        }
      }
    } break;
  }
  log(s_conversion_success_msg);
  return 0;
}
//----------------------------------------------------------------------------
