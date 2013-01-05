/****************************************************************************
 ****************************************************************************
 ***
 ***   This header was automatically generated from a Linux kernel header
 ***   of the same name, to make information necessary for userspace to
 ***   call into the kernel available to libc.  It contains only constants,
 ***   structures, and macros generated from the original header, and thus,
 ***   contains no copyrightable information.
 ***
 ***   To edit the content of this header, modify the corresponding
 ***   source file (e.g. under external/kernel-headers/original/) then
 ***   run bionic/libc/kernel/tools/update_all.py
 ***
 ***   Any manual change here will be lost the next time this script will
 ***   be run. You've been warned!
 ***
 ****************************************************************************
 ****************************************************************************/
#ifndef _LINUX_DSSCOMP_H
#define _LINUX_DSSCOMP_H
enum omap_plane {
 OMAP_DSS_GFX = 0,
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
 OMAP_DSS_VIDEO1 = 1,
 OMAP_DSS_VIDEO2 = 2,
 OMAP_DSS_VIDEO3 = 3,
 OMAP_DSS_WB = 4,
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
};
enum omap_channel {
 OMAP_DSS_CHANNEL_LCD = 0,
 OMAP_DSS_CHANNEL_DIGIT = 1,
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
 OMAP_DSS_CHANNEL_LCD2 = 2,
};
enum omap_color_mode {
 OMAP_DSS_COLOR_CLUT1 = 1 << 0,
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
 OMAP_DSS_COLOR_CLUT2 = 1 << 1,
 OMAP_DSS_COLOR_CLUT4 = 1 << 2,
 OMAP_DSS_COLOR_CLUT8 = 1 << 3,
 OMAP_DSS_COLOR_RGB12U = 1 << 4,
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
 OMAP_DSS_COLOR_ARGB16 = 1 << 5,
 OMAP_DSS_COLOR_RGB16 = 1 << 6,
 OMAP_DSS_COLOR_RGB24U = 1 << 7,
 OMAP_DSS_COLOR_RGB24P = 1 << 8,
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
 OMAP_DSS_COLOR_YUV2 = 1 << 9,
 OMAP_DSS_COLOR_UYVY = 1 << 10,
 OMAP_DSS_COLOR_ARGB32 = 1 << 11,
 OMAP_DSS_COLOR_RGBA32 = 1 << 12,
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
 OMAP_DSS_COLOR_RGBX24 = 1 << 13,
 OMAP_DSS_COLOR_RGBX32 = 1 << 13,
 OMAP_DSS_COLOR_NV12 = 1 << 14,
 OMAP_DSS_COLOR_RGBA16 = 1 << 15,
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
 OMAP_DSS_COLOR_RGBX12 = 1 << 16,
 OMAP_DSS_COLOR_RGBX16 = 1 << 16,
 OMAP_DSS_COLOR_ARGB16_1555 = 1 << 17,
 OMAP_DSS_COLOR_XRGB15 = 1 << 18,
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
 OMAP_DSS_COLOR_XRGB16_1555 = 1 << 18,
};
enum omap_writeback_source {
 OMAP_WB_LCD1 = 0,
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
 OMAP_WB_TV = 1,
 OMAP_WB_LCD2 = 2,
 OMAP_WB_GFX = 3,
 OMAP_WB_VID1 = 4,
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
 OMAP_WB_VID2 = 5,
 OMAP_WB_VID3 = 6,
};
enum omap_writeback_mode {
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
 OMAP_WB_CAPTURE_MODE = 0,
 OMAP_WB_MEM2MEM_MODE = 1,
};
enum omap_dss_trans_key_type {
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
 OMAP_DSS_COLOR_KEY_GFX_DST = 0,
 OMAP_DSS_COLOR_KEY_VID_SRC = 1,
};
enum omap_dss_display_state {
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
 OMAP_DSS_DISPLAY_DISABLED = 0,
 OMAP_DSS_DISPLAY_ACTIVE,
 OMAP_DSS_DISPLAY_SUSPENDED,
 OMAP_DSS_DISPLAY_TRANSITION,
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
};
struct omap_video_timings {
 __u16 x_res;
 __u16 y_res;
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
 __u32 pixel_clock;
 __u16 hsw;
 __u16 hfp;
 __u16 hbp;
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
 __u16 vsw;
 __u16 vfp;
 __u16 vbp;
};
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
struct omap_dss_cconv_coefs {
 __s16 ry, rcr, rcb;
 __s16 gy, gcr, gcb;
 __s16 by, bcr, bcb;
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
 __u16 full_range;
} __aligned(4);
struct omap_dss_cpr_coefs {
 __s16 rr, rg, rb;
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
 __s16 gr, gg, gb;
 __s16 br, bg, bb;
};
enum s3d_disp_type {
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
 S3D_DISP_NONE = 0,
 S3D_DISP_FRAME_SEQ,
 S3D_DISP_ROW_IL,
 S3D_DISP_COL_IL,
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
 S3D_DISP_PIX_IL,
 S3D_DISP_CHECKB,
 S3D_DISP_OVERUNDER,
 S3D_DISP_SIDEBYSIDE,
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
};
enum s3d_disp_sub_sampling {
 S3D_DISP_SUB_SAMPLE_NONE = 0,
 S3D_DISP_SUB_SAMPLE_V,
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
 S3D_DISP_SUB_SAMPLE_H,
};
enum s3d_disp_order {
 S3D_DISP_ORDER_L = 0,
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
 S3D_DISP_ORDER_R = 1,
};
enum s3d_disp_view {
 S3D_DISP_VIEW_L = 0,
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
 S3D_DISP_VIEW_R,
};
struct s3d_disp_info {
 enum s3d_disp_type type;
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
 enum s3d_disp_sub_sampling sub_samp;
 enum s3d_disp_order order;
 unsigned int gap;
};
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
struct dsscomp_videomode {
 const char *name;
 __u32 refresh;
 __u32 xres;
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
 __u32 yres;
 __u32 pixclock;
 __u32 left_margin;
 __u32 right_margin;
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
 __u32 upper_margin;
 __u32 lower_margin;
 __u32 hsync_len;
 __u32 vsync_len;
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
 __u32 sync;
 __u32 vmode;
 __u32 flag;
};
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
enum omap_dss_ilace_mode {
 OMAP_DSS_ILACE = (1 << 0),
 OMAP_DSS_ILACE_SEQ = (1 << 1),
 OMAP_DSS_ILACE_SWAP = (1 << 2),
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
 OMAP_DSS_ILACE_NONE = 0,
 OMAP_DSS_ILACE_IL_TB = OMAP_DSS_ILACE,
 OMAP_DSS_ILACE_IL_BT = OMAP_DSS_ILACE | OMAP_DSS_ILACE_SWAP,
 OMAP_DSS_ILACE_SEQ_TB = OMAP_DSS_ILACE_IL_TB | OMAP_DSS_ILACE_SEQ,
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
 OMAP_DSS_ILACE_SEQ_BT = OMAP_DSS_ILACE_IL_BT | OMAP_DSS_ILACE_SEQ,
};
struct dss2_vc1_range_map_info {
 __u8 enable;
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
 __u8 range_y;
 __u8 range_uv;
} __aligned(4);
struct dss2_rect_t {
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
 __s32 x;
 __s32 y;
 __u32 w;
 __u32 h;
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
} __aligned(4);
struct dss2_decim {
 __u8 min_x;
 __u8 max_x;
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
 __u8 min_y;
 __u8 max_y;
} __aligned(4);
struct dss2_ovl_cfg {
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
 __u16 width;
 __u16 height;
 __u32 stride;
 enum omap_color_mode color_mode;
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
 __u8 pre_mult_alpha;
 __u8 global_alpha;
 __u8 rotation;
 __u8 mirror;
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
 enum omap_dss_ilace_mode ilace;
 struct dss2_rect_t win;
 struct dss2_rect_t crop;
 struct dss2_decim decim;
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
 struct omap_dss_cconv_coefs cconv;
 struct dss2_vc1_range_map_info vc1;
 __u8 wb_source;
 enum omap_writeback_mode wb_mode;
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
 __u8 ix;
 __u8 zorder;
 __u8 enabled;
 __u8 zonly;
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
 __u8 mgr_ix;
 __u8 force_1d;
 __u8 mflag_en;
} __aligned(4);
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
enum omapdss_buffer_type {
 OMAP_DSS_BUFTYPE_SDMA,
 OMAP_DSS_BUFTYPE_TILER_8BIT,
 OMAP_DSS_BUFTYPE_TILER_16BIT,
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
 OMAP_DSS_BUFTYPE_TILER_32BIT,
 OMAP_DSS_BUFTYPE_TILER_PAGE,
};
enum omapdss_buffer_addressing_type {
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
 OMAP_DSS_BUFADDR_DIRECT,
 OMAP_DSS_BUFADDR_BYTYPE,
 OMAP_DSS_BUFADDR_ION,
 OMAP_DSS_BUFADDR_GRALLOC,
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
 OMAP_DSS_BUFADDR_OVL_IX,
 OMAP_DSS_BUFADDR_LAYER_IX,
 OMAP_DSS_BUFADDR_FB,
};
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
struct dss2_ovl_info {
 struct dss2_ovl_cfg cfg;
 enum omapdss_buffer_addressing_type addressing;
 union {
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
 struct {
 void *address;
 void *uv_address;
 };
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
 struct {
 enum omapdss_buffer_type ba_type;
 enum omapdss_buffer_type uv_type;
 };
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
 struct {
 __u32 ba;
 __u32 uv;
 };
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
 };
};
struct dss2_mgr_info {
 __u32 ix;
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
 __u32 default_color;
 enum omap_dss_trans_key_type trans_key_type;
 __u32 trans_key;
 struct omap_dss_cpr_coefs cpr_coefs;
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
 __u8 trans_enabled;
 __u8 interlaced;
 __u8 alpha_blending;
 __u8 cpr_enabled;
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
 __u8 swap_rb;
} __aligned(4);
enum dsscomp_setup_mode {
 DSSCOMP_SETUP_MODE_APPLY = (1 << 0),
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
 DSSCOMP_SETUP_MODE_DISPLAY = (1 << 1),
 DSSCOMP_SETUP_MODE_CAPTURE = (1 << 2),
 DSSCOMP_SETUP_APPLY = DSSCOMP_SETUP_MODE_APPLY,
 DSSCOMP_SETUP_DISPLAY =
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
 DSSCOMP_SETUP_MODE_APPLY | DSSCOMP_SETUP_MODE_DISPLAY,
 DSSCOMP_SETUP_CAPTURE =
 DSSCOMP_SETUP_MODE_APPLY | DSSCOMP_SETUP_MODE_CAPTURE,
 DSSCOMP_SETUP_DISPLAY_CAPTURE =
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
 DSSCOMP_SETUP_DISPLAY | DSSCOMP_SETUP_CAPTURE,
};
struct dsscomp_setup_mgr_data {
 __u32 sync_id;
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
 struct dss2_rect_t win;
 enum dsscomp_setup_mode mode;
 __u16 num_ovls;
 __u16 get_sync_obj;
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
 struct dss2_mgr_info mgr;
 struct dss2_ovl_info ovls[0];
};
struct dsscomp_check_ovl_data {
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
 enum dsscomp_setup_mode mode;
 struct dss2_mgr_info mgr;
 struct dss2_ovl_info ovl;
};
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
struct dsscomp_setup_dispc_data {
 __u32 sync_id;
 enum dsscomp_setup_mode mode;
 __u16 num_ovls;
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
 __u16 num_mgrs;
 __u16 get_sync_obj;
 struct dss2_mgr_info mgrs[3];
 struct dss2_ovl_info ovls[5];
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
};
struct dsscomp_wb_copy_data {
 struct dss2_ovl_info ovl, wb;
};
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
struct dsscomp_display_info {
 __u32 ix;
 __u32 overlays_available;
 __u32 overlays_owned;
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
 enum omap_channel channel;
 enum omap_dss_display_state state;
 __u8 enabled;
 struct omap_video_timings timings;
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
 struct s3d_disp_info s3d_info;
 struct dss2_mgr_info mgr;
 __u16 width_in_mm;
 __u16 height_in_mm;
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
 __u32 modedb_len;
 struct dsscomp_videomode modedb[];
};
struct dsscomp_setup_display_data {
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
 __u32 ix;
 struct dsscomp_videomode mode;
};
enum dsscomp_wait_phase {
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
 DSSCOMP_WAIT_PROGRAMMED = 1,
 DSSCOMP_WAIT_DISPLAYED,
 DSSCOMP_WAIT_RELEASED,
};
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
struct dsscomp_wait_data {
 __u32 timeout_us;
 enum dsscomp_wait_phase phase;
};
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
enum dsscomp_fbmem_type {
 DSSCOMP_FBMEM_TILER2D = 0,
 DSSCOMP_FBMEM_VRAM = 1,
};
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
struct dsscomp_platform_info {
 __u8 max_xdecim_2d;
 __u8 max_ydecim_2d;
 __u8 max_xdecim_1d;
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
 __u8 max_ydecim_1d;
 __u32 fclk;
 __u8 min_width;
 __u16 max_width;
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
 __u16 max_height;
 __u8 max_downscale;
 __u16 integer_scale_ratio_limit;
 __u32 tiler1d_slot_size;
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
 enum dsscomp_fbmem_type fbmem_type;
};
#define DSSCIOC_SETUP_MGR _IOW('O', 128, struct dsscomp_setup_mgr_data)
#define DSSCIOC_CHECK_OVL _IOWR('O', 129, struct dsscomp_check_ovl_data)
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
#define DSSCIOC_WB_COPY _IOW('O', 130, struct dsscomp_wb_copy_data)
#define DSSCIOC_QUERY_DISPLAY _IOWR('O', 131, struct dsscomp_display_info)
#define DSSCIOC_WAIT _IOW('O', 132, struct dsscomp_wait_data)
#define DSSCIOC_SETUP_DISPC _IOW('O', 133, struct dsscomp_setup_dispc_data)
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
#define DSSCIOC_SETUP_DISPLAY   _IOW('O', 134, struct dsscomp_setup_display_data)
#define DSSCIOC_QUERY_PLATFORM _IOR('O', 135, struct dsscomp_platform_info)
#endif
