/*****************************************************************************/
/*
 * Digital Subband Video 2
 *   DSV-2
 *
 *     -
 *    =--     2024 EMMIR
 *   ==---  Envel Graphics
 *  ===----
 *
 *   GitHub : https://github.com/LMP88959
 *   YouTube: https://www.youtube.com/@EMMIR_KC/videos
 *   Discord: https://discord.com/invite/hdYctSmyQJ
 */
/*****************************************************************************/

#ifndef _DSV_ENCODER_H_
#define _DSV_ENCODER_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "dsv_internal.h"

#define DSV_ENCODER_VERSION 1

#define DSV_GOP_INTRA 0
#define DSV_GOP_INF   INT_MAX

#define DSV_ENC_NUM_BUFS  0x03 /* mask */
#define DSV_ENC_FINISHED  0x04

#define DSV_MIN_EFFORT 0
#define DSV_MAX_EFFORT 10

#define DSV_RATE_CONTROL_CRF  0 /* constant rate factor */
#define DSV_RATE_CONTROL_ABR  1 /* one pass average bitrate */

#define DSV_MAX_PYRAMID_LEVELS 5

#define DSV_RC_QUAL_SCALE 4
#define DSV_RC_QUAL_MAX ((DSV_MAX_QUALITY * DSV_RC_QUAL_SCALE))
#define DSV_USER_QUAL_TO_RC_QUAL(user) ((user) * DSV_RC_QUAL_SCALE)
#define DSV_MAX_QUALITY (100)
#define DSV_QUALITY_PERCENT(pct) (pct)

typedef struct _DSV_ENCDATA {
    int refcount;

    DSV_FNUM fnum;
    DSV_FRAME *padded_frame;
    DSV_FRAME *pyramid[DSV_MAX_PYRAMID_LEVELS];
    DSV_FRAME *residual;
    DSV_FRAME *prediction;
    DSV_FRAME *recon_frame;

    DSV_PARAMS params;

    int quant;

    struct _DSV_ENCDATA *refdata;

    DSV_MV *final_mvs;
} DSV_ENCDATA;

typedef struct {
    int quality; /* user configurable, 0...DSV_MAX_QUALITY  */

    int effort; /* encoder effort. DSV_MIN_EFFORT...DSV_MAX_EFFORT */

    int gop; /* GOP (Group of Pictures) length */

    int do_scd; /* scene change detection */
    int do_temporal_aq; /* toggle temporal adaptive quantization for I frames */
    int do_psy; /* enable psychovisual optimizations */

    /* threshold for skip block determination. -1 = disable
    Larger value = more likely to mark it as skipped */
    int skip_block_thresh;

    int block_size_override_x;
    int block_size_override_y;

    int variable_i_interval; /* intra frame insertions reset GOP counter */
    /* rate control */
    int rc_mode; /* rate control mode */

    /* approximate average bitrate desired */
    unsigned bitrate;
    /* for ABR */
    int max_q_step;
    int min_quality; /* 0...DSV_MAX_QUALITY */
    int max_quality; /* 0...DSV_MAX_QUALITY */
    int min_I_frame_quality; /* 0...DSV_MAX_QUALITY */

    int intra_pct_thresh; /* 0-100% */
    int scene_change_delta;
    unsigned stable_refresh; /* # frames after which stability accum resets */
    int pyramid_levels;

    /* used internally */
    unsigned rc_qual;
    /* bpf = bytes per frame */
#define DSV_BPF_RESET 256 /* # frames after which average bpf resets */
    unsigned bpf_total;
    unsigned bpf_reset;
    int bpf_avg;
    int total_P_frame_q;
    int avg_P_frame_q;

    DSV_FNUM next_fnum;
    DSV_ENCDATA *ref;
    DSV_META vidmeta;
    int prev_link;
    int force_metadata;

    struct DSV_STAB_ACC {
        signed x : 16;
        signed y : 16;
    } *stability;
    unsigned refresh_ctr;
    uint8_t *blockdata;

    DSV_FNUM prev_gop;
} DSV_ENCODER;

extern void dsv_enc_init(DSV_ENCODER *enc);
extern void dsv_enc_free(DSV_ENCODER *enc);
extern void dsv_enc_set_metadata(DSV_ENCODER *enc, DSV_META *md);
extern void dsv_enc_force_metadata(DSV_ENCODER *enc);

extern void dsv_enc_start(DSV_ENCODER *enc);

/* returns number of buffers available in bufs ptr */
extern int dsv_enc(DSV_ENCODER *enc, DSV_FRAME *frame, DSV_BUF *bufs);
extern void dsv_enc_end_of_stream(DSV_ENCODER *enc, DSV_BUF *bufs);

/* used internally */
typedef struct {
    DSV_PARAMS *params;
    DSV_FRAME *src[DSV_MAX_PYRAMID_LEVELS + 1];
    DSV_FRAME *ref[DSV_MAX_PYRAMID_LEVELS + 1];
    DSV_FRAME *recon;
    DSV_MV *mvf[DSV_MAX_PYRAMID_LEVELS + 1];
    DSV_ENCODER *enc;
} DSV_HME;

extern int dsv_hme(DSV_HME *hme, int *scene_change_blocks);

#ifdef __cplusplus
}
#endif

#endif
