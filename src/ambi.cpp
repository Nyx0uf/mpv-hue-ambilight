#include <cstdlib>
#include "hue_controller.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "vf_dlopen.h"
#include "filterutils.h"
#include "global.h"

#include <libavcodec/avcodec.h>
#include <libswscale/swscale.h>
#include <libavutil/avutil.h>

#ifdef __cplusplus
}
#endif

#define ALLFORMATS \
    /*     format    bytes   xmul   ymul */ \
    FORMAT("rgb24"  ,    3,     1,     1) \
    FORMAT("yuv420p",    1,     2,     2)



typedef struct _nyx_ambi_struct {
	int frame_step;
	hue_controller_t* hue;
	struct SwsContext* sws_ctx;
	AVPicture picture;
} ambi_t;


#ifdef DEBUG
static void save_frame(const AVFrame* frame, const int width, const int height, const int frame_n);
#endif


static int nyx_ambi_init(struct vf_dlopen_context* ctx)
{
	ambi_t* ambi = (ambi_t*)ctx->priv;

	ambi->frame_step = 0;
	ambi->sws_ctx = sws_getContext(ctx->in_width, ctx->in_height, AV_PIX_FMT_YUV420P, ctx->in_width, ctx->in_height, AV_PIX_FMT_RGB24, 0, 0, 0, 0);
	avpicture_alloc(&ambi->picture, AV_PIX_FMT_RGB24, ctx->in_width, ctx->in_height);

	return 1;
}

void nyx_ambi_uninit(struct vf_dlopen_context* ctx)
{
	ambi_t* ambi = (ambi_t*)ctx->priv;

	delete ambi->hue;
	avpicture_free(&ambi->picture);
	sws_freeContext(ambi->sws_ctx);

	free(ambi);
}

static int nyx_ambi_init_put_image(struct vf_dlopen_context* ctx)
{
	ambi_t* ambi = (ambi_t*)ctx->priv;
	ambi->frame_step++;

	if (ambi->frame_step == 24)
	{
		AVPicture picture = ambi->picture;
		const uint8_t* data[3] = {ctx->inpic.plane[0], ctx->inpic.plane[1], ctx->inpic.plane[2]};
		int linesize[3] = {ctx->inpic.planestride[0], ctx->inpic.planestride[1], ctx->inpic.planestride[2]};
		sws_scale(ambi->sws_ctx, data, linesize, 0, ctx->in_height, picture.data, picture.linesize);

		ambi->hue->apply_dominant_color_from_buffer(picture.data[0], ctx->in_width, ctx->in_height);

		//save_frame((AVFrame*)&picture, ctx->in_width, ctx->in_height, ambi->nb);
		ambi->frame_step = 0;
	}

	// Copy input -> output
	// TODO: avoid this ?
	const int np = ctx->inpic.planes;
	for (int p = 0; p < np; ++p)
	{
		copy_plane(ctx->outpic->plane[p], ctx->outpic->planestride[p], ctx->inpic.plane[p], ctx->inpic.planestride[p], NYX_MIN(ctx->inpic.planestride[p], ctx->outpic->planestride[p]), ctx->inpic.planeheight[p]);
	}
	ctx->outpic->pts = ctx->inpic.pts;

	return 1;
}

/*
--vf=dlopen=ambi.dylib:HUE_IP:NUMBER_OF_LAMPS
1: IP of the Hue bridge
2: Number of lamps
*/
int vf_dlopen_getcontext(struct vf_dlopen_context* ctx, int argc __attribute__((unused)), const char** argv __attribute__((unused)))
{
	VF_DLOPEN_CHECK_VERSION(ctx);

	if (argc != 1)
	{
		NYX_ERRLOG("\nUsage:\n--vf=dlopen=ambi.dylib:HUE_IP\n\n");
		return -1;
	}
	const char* ip = argv[0];

	ambi_t* ambi = (ambi_t*)calloc(1, sizeof(ambi_t));
	ambi->hue = new hue_controller_t(ip, 3);

	static struct vf_dlopen_formatpair map[] = {
#define FORMAT(fmt,sz,xmul,ymul) {fmt, NULL},
        ALLFORMATS
#undef FORMAT
        {NULL, NULL}
	};
	ctx->priv = (void*)ambi;
	ctx->format_mapping = map;
	ctx->config = nyx_ambi_init;
	ctx->put_image = nyx_ambi_init_put_image;
	ctx->uninit = nyx_ambi_uninit;

	return 1;
}

#ifdef DEBUG
__attribute__((unused)) void save_frame(const AVFrame* frame, const int width, const int height, const int frame_n)
{
	FILE* fp = NULL;
	char filename[4096] = {0x00};

	// Open file
	sprintf(filename, "/Users/nyxouf/Desktop/_frame%d.ppm", frame_n);
	fp = fopen(filename, "wb");
	if (fp == NULL)
		return;

	// Write header
	fprintf(fp, "P6\n%d %d\n255\n", width, height);

	// Write pixel data
	for (int y = 0; y < height; y++)
		fwrite(frame->data[0] + y * frame->linesize[0], 1, width * 3, fp);

	// Close file
	fclose(fp);
}
#endif
