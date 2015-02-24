#include <cstdlib>
#include "hue_controller.h"
#include "img_formats.h"


#ifdef NYX_USE_OPENCL
#include "cl_global.h" 
#endif /* NYX_USE_OPENCL */


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "vf_dlopen.h"
#include "filterutils.h"

#ifdef __cplusplus
}
#endif /* __cplusplus */


#define ALLFORMATS \
    /*     format    bytes   xmul   ymul */ \
    FORMAT("rgb24"  ,    3,     1,     1) \
    FORMAT("yuv420p",    1,     2,     2)


#define NYX_MAX_WIDTH 1280


typedef struct _nyx_ambi_struct {
	int frame_step;
	hue_controller_t* hue;
	img_format_t* img;
} ambi_t;


static int nyx_ambi_init(struct vf_dlopen_context* ctx)
{
#ifdef NYX_USE_FFMPEG
	av_log_set_level(AV_LOG_QUIET);
#endif /* NYX_USE_FFMPEG */

#ifdef NYX_USE_OPENCL
	nyx_cl_init();
#endif /* NYX_USE_OPENCL */

	ambi_t* ambi = (ambi_t*)ctx->priv;

	ambi->frame_step = 0;

	ambi->img = new img_format_t(ctx->in_width, ctx->in_height);

	return 1;
}

void nyx_ambi_uninit(struct vf_dlopen_context* ctx)
{
	ambi_t* ambi = (ambi_t*)ctx->priv;

	delete ambi->hue;
	delete ambi->img;

	free(ambi);

#ifdef NYX_USE_OPENCL
	nyx_cl_uninit();
#endif /* NYX_USE_OPENCL */
}

static int nyx_ambi_init_put_image(struct vf_dlopen_context* ctx)
{
	ambi_t* ambi = (ambi_t*)ctx->priv;

	//NYX_DLOG("\nplanewidth=%d planeheight=%d planestride=%d\n", ctx->inpic.planewidth[0], ctx->inpic.planeheight[0], ctx->inpic.planestride[0]);
	//NYX_DLOG("planewidth=%d planeheight=%d planestride=%d\n", ctx->inpic.planewidth[1], ctx->inpic.planeheight[1], ctx->inpic.planestride[1]);
	//NYX_DLOG("planewidth=%d planeheight=%d planestride=%d\n\n", ctx->inpic.planewidth[2], ctx->inpic.planeheight[2], ctx->inpic.planestride[2]);

	if (++ambi->frame_step == 24)
	{
		uint8_t* rgba = ambi->img->yuv420p_to_rgba(ctx->inpic.plane[0], ctx->inpic.planestride[0], ctx->inpic.plane[1], ctx->inpic.planestride[1], ctx->inpic.plane[2], ctx->inpic.planestride[2], ctx->inpic.planewidth[0], ctx->inpic.planeheight[0]);

		ambi->hue->apply_dominant_color_from_buffer(rgba, ctx->in_width, ctx->in_height);
		free(rgba);

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
*/
int vf_dlopen_getcontext(struct vf_dlopen_context* ctx, int argc, const char** argv)
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
