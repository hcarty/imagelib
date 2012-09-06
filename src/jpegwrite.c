/***********************************************************************/
/*                                                                     */
/*                           Objective Caml                            */
/*                                                                     */
/*            François Pessaux, projet Cristal, INRIA Rocquencourt     */
/*            Pierre Weis, projet Cristal, INRIA Rocquencourt          */
/*            Jun Furuse, projet Cristal, INRIA Rocquencourt           */
/*                                                                     */
/*  Copyright 1999,2000                                                */
/*  Institut National de Recherche en Informatique et en Automatique.  */
/*  Distributed only by permission.                                    */
/*                                                                     */
/***********************************************************************/
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <caml/mlvalues.h>
#include <caml/alloc.h>
#include <caml/memory.h>
#include <caml/fail.h>

#include <stdio.h>
#include <setjmp.h>

/*
 * Include file for users of JPEG library.
 * You will need to have included system headers that define at least
 * the typedefs FILE and size_t before you can include jpeglib.h.
 * (stdio.h is sufficient on ANSI-conforming systems.)
 * You may also wish to include "jerror.h".
 */

#include "jpeglib.h"

/*
 * Sample routine for JPEG compression.  We assume that the target file name
 * and a compression quality factor are passed in.
 */

extern char jpg_error_message[JMSG_LENGTH_MAX];
struct my_error_mgr {
  struct jpeg_error_mgr pub;	/* "public" fields */

  jmp_buf setjmp_buffer;	/* for return to caller */
};
typedef struct my_error_mgr * my_error_ptr;
extern void my_error_exit (j_common_ptr);

GLOBAL(value)
write_JPEG_file (value file,
                 value buffer,
		 value width, 
		 value height, 
		 value qual)
{
  JSAMPLE *image_buffer;
  int image_height;
  int image_width;
  char *filename;
  int quality;

  /* This struct contains the JPEG compression parameters and pointers to
   * working space (which is allocated as needed by the JPEG library).
   * It is possible to have several such structures, representing multiple
   * compression/decompression processes, in existence at once.  We refer
   * to any one struct (and its associated working data) as a "JPEG object".
   */
  struct jpeg_compress_struct cinfo;
  /* This struct represents a JPEG error handler.  It is declared separately
   * because applications often want to supply a specialized error handler
   * (see the second half of this file for an example).  But here we just
   * take the easy way out and use the standard error handler, which will
   * print a message on stderr and call exit() if compression fails.
   * Note that this struct must live as long as the main JPEG parameter
   * struct, to avoid dangling-pointer problems.
   */
  struct jpeg_error_mgr jerr;
  /* More stuff */
  FILE * outfile;		/* target file */
  JSAMPROW row_pointer[1];	/* pointer to JSAMPLE row[s] */
  int row_stride;		/* physical row width in image buffer */

  image_buffer = String_val(buffer);
  image_width = Int_val(width);
  image_height = Int_val(height);
  filename = String_val(file);
  quality= Int_val(qual);

  /* Step 1: allocate and initialize JPEG compression object */

  /* We have to set up the error handler first, in case the initialization
   * step fails.  (Unlikely, but it could happen if you are out of memory.)
   * This routine fills in the contents of struct jerr, and returns jerr's
   * address which we place into the link field in cinfo.
   */
  cinfo.err = jpeg_std_error(&jerr);
  /* Now we can initialize the JPEG compression object. */
  jpeg_create_compress(&cinfo);

  /* Step 2: specify data destination (eg, a file) */
  /* Note: steps 2 and 3 can be done in either order. */

  /* Here we use the library-supplied code to send compressed data to a
   * stdio stream.  You can also write your own code to do something else.
   * VERY IMPORTANT: use "b" option to fopen() if you are on a machine that
   * requires it in order to write binary files.
   */
  if ((outfile = fopen(filename, "wb")) == NULL) {
    failwith ("can't open %s\n");
  }
  jpeg_stdio_dest(&cinfo, outfile);

  /* Step 3: set parameters for compression */

  /* First we supply a description of the input image.
   * Four fields of the cinfo struct must be filled in:
   */
  cinfo.image_width = image_width; 	/* image width and height, in pixels */
  cinfo.image_height = image_height;
  cinfo.input_components = 3;		/* # of color components per pixel */
  cinfo.in_color_space = JCS_RGB; 	/* colorspace of input image */
  /* Now use the library's routine to set default compression parameters.
   * (You must set at least cinfo.in_color_space before calling this,
   * since the defaults depend on the source color space.)
   */
  jpeg_set_defaults(&cinfo);
  /* Now you can set any non-default parameters you wish to.
   * Here we just illustrate the use of quality (quantization table) scaling:
   */
  jpeg_set_quality(&cinfo, quality, TRUE /* limit to baseline-JPEG values */);

  /* Step 4: Start compressor */

  /* TRUE ensures that we will write a complete interchange-JPEG file.
   * Pass TRUE unless you are very sure of what you're doing.
   */
  jpeg_start_compress(&cinfo, TRUE);

  /* Step 5: while (scan lines remain to be written) */
  /*           jpeg_write_scanlines(...); */

  /* Here we use the library's state variable cinfo.next_scanline as the
   * loop counter, so that we don't have to keep track ourselves.
   * To keep things simple, we pass one scanline per call; you can pass
   * more if you wish, though.
   */
  row_stride = image_width * 3;	/* JSAMPLEs per row in image_buffer */

  while (cinfo.next_scanline < cinfo.image_height) {
    /* jpeg_write_scanlines expects an array of pointers to scanlines.
     * Here the array is only one element long, but you could pass
     * more than one scanline at a time if that's more convenient.
     */
    row_pointer[0] = & image_buffer[cinfo.next_scanline * row_stride];
    (void) jpeg_write_scanlines(&cinfo, row_pointer, 1);
  }

  /* Step 6: Finish compression */

  jpeg_finish_compress(&cinfo);
  /* After finish_compress, we can close the output file. */
  fclose(outfile);

  /* Step 7: release JPEG compression object */

  /* This is an important step since it will release a good deal of memory. */
  jpeg_destroy_compress(&cinfo);

  /* And we're done! */
  return Val_unit;
}


/*
 * SOME FINE POINTS:
 *
 * In the above loop, we ignored the return value of jpeg_write_scanlines,
 * which is the number of scanlines actually written.  We could get away
 * with this because we were only relying on the value of cinfo.next_scanline,
 * which will be incremented correctly.  If you maintain additional loop
 * variables then you should be careful to increment them properly.
 * Actually, for output to a stdio stream you needn't worry, because
 * then jpeg_write_scanlines will write all the lines passed (or else exit
 * with a fatal error).  Partial writes can only occur if you use a data
 * destination module that can demand suspension of the compressor.
 * (If you don't know what that's for, you don't need it.)
 *
 * If the compressor requires full-image buffers (for entropy-coding
 * optimization or a multi-scan JPEG file), it will create temporary
 * files for anything that doesn't fit within the maximum-memory setting.
 * (Note that temp files are NOT needed if you use the default parameters.)
 * On some systems you may need to set up a signal handler to ensure that
 * temporary files are deleted if the program is interrupted.  See libjpeg.doc.
 *
 * Scanlines MUST be supplied in top-to-bottom order if you want your JPEG
 * files to be compatible with everyone else's.  If you cannot readily read
 * your data in that order, you'll need an intermediate array to hold the
 * image.  See rdtarga.c or rdbmp.c for examples of handling bottom-to-top
 * source data using the JPEG code's internal virtual-array mechanisms.
 */

value open_jpeg_file_for_write_colorspace( name, width, height, qual, colorspace )
     value name;
     value width;
     value height;
     value qual;
     J_COLOR_SPACE colorspace;
{
  char *filename;
  int image_height;
  int image_width;
  int quality;

  struct jpeg_compress_struct* cinfop;
  struct my_error_mgr *jerrp;
  /* More stuff */
  FILE * outfile;		/* source file */
  value res;

  image_width= Int_val( width );
  image_height= Int_val( height );
  filename= String_val( name );
  quality= Int_val(qual);
 
  if ((outfile = fopen(filename, "wb")) == NULL) {
    failwith("failed to open jpeg file");
  }

  cinfop = malloc(sizeof (struct jpeg_compress_struct));
  jerrp = malloc(sizeof (struct my_error_mgr));
  cinfop->err = jpeg_std_error(&jerrp->pub);
  jerrp->pub.error_exit = my_error_exit;

  if (setjmp(jerrp->setjmp_buffer)) {
    /* If we get here, the JPEG code has signaled an error.
     * We need to clean up the JPEG object, close the input file, and return.
     */
    jpeg_destroy_compress(cinfop);
    free(jerrp);
    fclose(outfile);
    failwith(jpg_error_message);
  }

  jpeg_create_compress(cinfop);
  jpeg_stdio_dest(cinfop, outfile);

  cinfop->image_width= image_width; 
  cinfop->image_height= image_height; 
  cinfop->input_components = (colorspace == JCS_RGB ? 3 : 4);
  cinfop->in_color_space= colorspace /* JCS_RGB or JCS_CMYK */;
  jpeg_set_defaults(cinfop);
  jpeg_set_quality(cinfop, quality, TRUE);
  jpeg_start_compress(cinfop, TRUE);

  { 
    res = alloc_tuple(3);
    Field(res, 0) = (value)cinfop;
    Field(res, 1) = (value)outfile;
    Field(res, 2) = (value)jerrp;
  }
  /*
  fprintf(stderr, "cinfop= %d outfile= %d %d %d \n", cinfop, infile, cinfop->output_scanline, cinfop->output_height); 
  fflush(stderr);
  */
  return res;
}

value open_jpeg_file_for_write( name, width, height, qual )
     value name;
     value width;
     value height;
     value qual;
{
  return
    open_jpeg_file_for_write_colorspace( name, width, height, qual, JCS_RGB );
}

value open_jpeg_file_for_write_cmyk( name, width, height, qual )
     value name;
     value width;
     value height;
     value qual;
{
  return
    open_jpeg_file_for_write_colorspace( name, width, height, qual, JCS_CMYK );
}

value write_jpeg_scanline( jpegh, buf )
value jpegh, buf;
{
  struct jpeg_compress_struct *cinfop;
  JSAMPROW row[1];

  cinfop = (struct jpeg_compress_struct *) Field( jpegh, 0 );

  row[0] = String_val( buf );

  jpeg_write_scanlines( cinfop, row, 1 );
  return Val_unit;
}

value close_jpeg_file_for_write( jpegh )
     value jpegh;
{
  struct jpeg_compress_struct *cinfop;
  struct my_error_mgr *jerrp;
  FILE *outfile;

#ifdef DEBUG_JPEG
  fprintf(stderr, "closing\n");
  fflush(stderr);
#endif

  cinfop = (struct jpeg_compress_struct *) Field( jpegh, 0 );
  outfile = (FILE *) Field( jpegh, 1 );
  jerrp = (struct my_error_mgr *) Field( jpegh, 2 );

#ifdef DEBUG_JPEG
  fprintf(stderr, "cinfop= %d outfile= %d %d %d \n", cinfop, outfile, cinfop->next_scanline, cinfop->image_height); 
#endif

   if( cinfop->next_scanline >= cinfop->image_height ){ 
#ifdef DEBUG_JPEG
     fprintf(stderr, "finish\n");
     fflush(stderr);
#endif
     jpeg_finish_compress( cinfop );
   }
#ifdef DEBUG_JPEG
  fprintf(stderr, "destroy\n");
  fflush(stderr);
#endif
  jpeg_destroy_compress( cinfop ); 
  
  free(cinfop);
  free(jerrp);
#ifdef DEBUG_JPEG
  fprintf(stderr, "file close\n");
  fflush(stderr);
#endif
  fclose(outfile);

  return Val_unit;
}
