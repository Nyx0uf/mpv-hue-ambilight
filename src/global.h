#ifndef __NYX_GLOBAL_H__
#define __NYX_GLOBAL_H__


/* Logging macros */
#ifdef DEBUG
#define NYX_DLOG(...) fprintf(stdout, __VA_ARGS__)
#else
#define NYX_DLOG(...) ((void)0)
#define NDEBUG
#endif /* DEBUG */
#define NYX_ERRLOG(...) fprintf(stderr, __VA_ARGS__)

/* Returns the lower value */
#define NYX_MIN(A, B) ((A) < (B) ? (A) : (B))
/* Returns the higher value */
#define NYX_MAX(A, B) ((A) > (B) ? (A) : (B))
/* Clamp */
#define NYX_CLAMP(VAL, MIN, MAX) (((VAL) < (MIN)) ? (MIN) : (((VAL) > (MAX)) ? (MAX) : (VAL)))
/* Swap */
#define NYX_SWAP(__TYPE, A, B) do { __TYPE SWAP_TMP = B; B = A; A = SWAP_TMP; } while (0)

/* Not a Number check */
#define NYX_IS_NAN(NUM) (((double)NUM) != ((double)NUM))


/* Use FFMPEG by default */
//#define NYX_USE_FFMPEG 1

//#ifndef NYX_USE_FFMPEG
/* Use OpenCL */
//#define NYX_USE_OPENCL 1
//#endif /* NYX_USE_FFMPEG */

#endif /* __NYX_GLOBAL_H__ */
