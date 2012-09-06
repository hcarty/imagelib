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

/* $Id: jpegread.c,v 1.5 2009/10/16 16:08:33 weis Exp $ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <caml/mlvalues.h>
#include <caml/alloc.h>
#include <caml/memory.h>
#include <caml/fail.h>

#include "oversized.h"

#include <stdio.h>
#include <string.h>

/*
 * Include file for users of JPEG library.
 * You will need to have included system headers that define at least
 * the typedefs FILE and size_t before you can include jpeglib.h.
 * (stdio.h is sufficient on ANSI-conforming systems.)
 * You may also wish to include "jerror.h".
 */

#include "jpeglib.h"

/*
 * <setjmp.h> is used for the optional error recovery mechanism shown in
 * the second part of the example.
 */

#include <setjmp.h>

char jpg_error_message[JMSG_LENGTH_MAX];
 
struct my_error_mgr {
  struct jpeg_error_mgr pub;	/* "public" fields */

  jmp_buf setjmp_buffer;	/* for return to caller */
};

typedef struct my_error_mgr * my_error_ptr;

void
my_error_exit (j_common_ptr cinfo)
{
  /* cinfo->err really points to a my_error_mgr struct, so coerce pointer */
  my_error_ptr myerr = (my_error_ptr) cinfo->err;

  /* Always display the message. */
  /* We could postpone this until after returning, if we chose. */
  fprintf(stderr,"setting message\n");
  (*cinfo->err->format_message) (cinfo, jpg_error_message );

  /* Return control to the setjmp point */
  longjmp(myerr->setjmp_buffer, 1);
}

GLOBAL(value)
read_JPEG_file (value name)
{
  CAMLparam1(name);
  CAMLlocal1(res);

  char *filename;
  /* This struct contains the JPEG decompression parameters and pointers to
   * working space (which is allocated as needed by the JPEG library).
   */
  struct jpeg_decompress_struct cinfo;
  /* We use our private extension JPEG error handler.
   * Note that this struct must live as long as the main JPEG parameter
   * struct, to avoid dangling-pointer problems.
   */
  struct my_error_mgr jerr;
  /* More stuff */
  FILE * infile;		/* source file */
  JSAMPARRAY buffer;		/* Output row buffer */
  int row_stride;		/* physical row width in output buffer */
  int i;

  filename= String_val( name );

  /* In this example we want to open the input file before doing anything else,
   * so that the setjmp() error recovery below can assume the file is open.
   * VERY IMPORTANT: use "b" option to fopen() if you are on a machine that
   * requires it in order to read binary files.
   */

  if ((infile = fopen(filename, "rb")) == NULL) {
    failwith("failed to open jpeg file");
  }

  /* Step 1: allocate and initialize JPEG decompression object */

  /* We set up the normal JPEG error routines, then override error_exit. */
  cinfo.err = jpeg_std_error(&jerr.pub);
  jerr.pub.error_exit = my_error_exit;

  /* Establish the setjmp return context for my_error_exit to use. */
  if (setjmp(jerr.setjmp_buffer)) {
    /* If we get here, the JPEG code has signaled an error.
     * We need to clean up the JPEG object, close the input file, and return.
     */
    fprintf(stderr, "Exiting...");
    jpeg_destroy_decompress(&cinfo);
    fclose(infile);
    exit(-1);
    failwith(jpg_error_message);
  }
  /* Now we can initialize the JPEG decompression object. */
  jpeg_create_decompress(&cinfo);

  /* Step 2: specify data source (eg, a file) */

  jpeg_stdio_src(&cinfo, infile);

  /* Step 3: read file parameters with jpeg_read_header() */

  (void) jpeg_read_header(&cinfo, TRUE);

  /* We can ignore the return value from jpeg_read_header since
   *   (a) suspension is not possible with the stdio data source, and
   *   (b) we passed TRUE to reject a tables-only JPEG file as an error.
   * See libjpeg.doc for more info.
   */

  /* Step 4: set parameters for decompression */

  /* In this example, we don't need to change any of the defaults set by
   * jpeg_read_header(), so we do nothing here.
   */

  cinfo.out_color_space = JCS_RGB;

  /* Step 5: Start decompressor */

  (void) jpeg_start_decompress(&cinfo);
  /* We can ignore the return value since suspension is not possible
   * with the stdio data source.
   */

  /* We may need to do some setup of our own at this point before reading
   * the data.  After jpeg_start_decompress() we have the correct scaled
   * output image dimensions available, as well as the output colormap
   * if we asked for color quantization.
   * In this example, we need to make an output work buffer of the right size.
   */ 
  /* JSAMPLEs per row in output buffer */

  if( oversized(cinfo.output_width, cinfo.output_components) ){
    jpeg_destroy_decompress(&cinfo);
    fclose(infile);
    failwith_oversized("jpeg");
  }

  row_stride = cinfo.output_width * cinfo.output_components;

  /* Make a one-row-high sample array that will go away when done with image */
  buffer = (*cinfo.mem->alloc_sarray)
		((j_common_ptr) &cinfo, JPOOL_IMAGE, row_stride, 
		 cinfo.output_height );

  /* Step 6: while (scan lines remain to be read) */
  /*           jpeg_read_scanlines(...); */

  /* Here we use the library's state variable cinfo.output_scanline as the
   * loop counter, so that we don't have to keep track ourselves.
   */
  while (cinfo.output_scanline < cinfo.output_height) {
    /* jpeg_read_scanlines expects an array of pointers to scanlines.
     * Here the array is only one element long, but you could ask for
     * more than one scanline at a time if that's more convenient.
     */
    jpeg_read_scanlines(&cinfo, buffer + cinfo.output_scanline, 1); 
  }

  if( oversized(row_stride, cinfo.output_height) ){
    jpeg_destroy_decompress(&cinfo);
    fclose(infile);
    failwith_oversized("jpeg");
  }

  {
    CAMLlocalN(r,3);
    r[0] = Val_int(cinfo.output_width);
    r[1] = Val_int(cinfo.output_height);
    r[2] = alloc_string ( row_stride * cinfo.output_height );
    for(i=0; i<cinfo.output_height; i++){
      memcpy( String_val(r[2]) + i * row_stride, 
	       buffer[i], row_stride);
    }
    res = alloc_tuple(3);
    for(i=0; i<3; i++) Field(res, i) = r[i];
  }

  /* Step 7: Finish decompression */

  (void) jpeg_finish_decompress(&cinfo);
  /* We can ignore the return value since suspension is not possible
   * with the stdio data source.
   */

  /* Step 8: Release JPEG decompression object */

  /* This is an important step since it will release a good deal of memory. */
  jpeg_destroy_decompress(&cinfo);

  /* After finish_decompress, we can close the input file.
   * Here we postpone it until after no more JPEG errors are possible,
   * so as to simplify the setjmp error logic above.  (Actually, I don't
   * think that jpeg_destroy can do an error exit, but why assume anything...)
   */
  fclose(infile);

  /* At this point you may want to check to see whether any corrupt-data
   * warnings occurred (test whether jerr.pub.num_warnings is nonzero).
   */

  /* And we're done! */
  CAMLreturn(res);
}

value jpeg_set_scale_denom( jpegh, denom )
     value jpegh;
     value denom;
{
  CAMLparam2(jpegh,denom);
  struct jpeg_decompress_struct *cinfop;

  cinfop = (struct jpeg_decompress_struct *) Field ( jpegh, 0 );
  cinfop->scale_num = 1;
  cinfop->scale_denom = Int_val( denom );
  CAMLreturn(Val_unit);
}

value open_jpeg_file_for_read( name )
     value name;
{
  CAMLparam1(name);
  CAMLlocal1(res);

  char *filename;
  /* This struct contains the JPEG decompression parameters and pointers to
   * working space (which is allocated as needed by the JPEG library).
   */
  struct jpeg_decompress_struct* cinfop;
  /* We use our private extension JPEG error handler.
   * Note that this struct must live as long as the main JPEG parameter
   * struct, to avoid dangling-pointer problems.
   */
  struct my_error_mgr *jerrp;
  /* More stuff */
  FILE * infile;		/* source file */
  int i;

  filename= String_val( name );

  if ((infile = fopen(filename, "rb")) == NULL) {
    failwith("failed to open jpeg file");
  }

  cinfop = malloc(sizeof (struct jpeg_decompress_struct));
  jerrp = malloc(sizeof (struct my_error_mgr));

  /* In this example we want to open the input file before doing anything else,
   * so that the setjmp() error recovery below can assume the file is open.
   * VERY IMPORTANT: use "b" option to fopen() if you are on a machine that
   * requires it in order to read binary files.
   */


  /* Step 1: allocate and initialize JPEG decompression object */

  /* We set up the normal JPEG error routines, then override error_exit. */
  cinfop->err = jpeg_std_error(&jerrp->pub);
  jerrp->pub.error_exit = my_error_exit;
  /* Establish the setjmp return context for my_error_exit to use. */
  if (setjmp(jerrp->setjmp_buffer)) {
    /* If we get here, the JPEG code has signaled an error.
     * We need to clean up the JPEG object, close the input file, and return.
     */
    jpeg_destroy_decompress(cinfop);
    free(jerrp);
    fclose(infile);
    failwith(jpg_error_message);
  }
  /* Now we can initialize the JPEG decompression object. */
  jpeg_create_decompress(cinfop);

  /* Step 2: specify data source (eg, a file) */

  jpeg_stdio_src(cinfop, infile);

  /* Step 3: read file parameters with jpeg_read_header() */

  (void) jpeg_read_header(cinfop, TRUE);

  { 
    CAMLlocalN(r,3);
    r[0] = Val_int(cinfop->image_width);
    r[1] = Val_int(cinfop->image_height);
    r[2] = alloc_tuple(3);
    Field(r[2], 0) = (value)cinfop;
    Field(r[2], 1) = (value)infile;
    Field(r[2], 2) = (value)jerrp;
    res = alloc_tuple(3);
    for(i=0; i<3; i++) Field(res, i) = r[i];
  }
  CAMLreturn(res);
}

value open_jpeg_file_for_read_start( jpegh )
     value jpegh;
{
  CAMLparam1(jpegh);
  CAMLlocal1(res);
  struct jpeg_decompress_struct* cinfop;
  struct my_error_mgr *jerrp;
  FILE *infile;
  int i;

  cinfop = (struct jpeg_decompress_struct *) Field( jpegh, 0 );
  infile = (FILE *) Field( jpegh, 1 );
  jerrp = (struct my_error_mgr *) Field( jpegh, 2 );

  /* We can ignore the return value from jpeg_read_header since
   *   (a) suspension is not possible with the stdio data source, and
   *   (b) we passed TRUE to reject a tables-only JPEG file as an error.
   * See libjpeg.doc for more info.
   */

  /* Step 4: set parameters for decompression */

  /* In this example, we don't need to change any of the defaults set by
   * jpeg_read_header(), so we do nothing here.
   */

  cinfop->out_color_space = JCS_RGB;

  /* Step 5: Start decompressor */

  (void) jpeg_start_decompress(cinfop);
  /* We can ignore the return value since suspension is not possible
   * with the stdio data source.
   */

  /* We may need to do some setup of our own at this point before reading
   * the data.  After jpeg_start_decompress() we have the correct scaled
   * output image dimensions available, as well as the output colormap
   * if we asked for color quantization.
   * In this example, we need to make an output work buffer of the right size.
   */ 
  /* JSAMPLEs per row in output buffer */

  /* row_stride = cinfop->output_width * cinfop->output_components; */

  { 
    CAMLlocalN(r,3);
    // CR jfuruse: integer overflow
    r[0] = Val_int(cinfop->output_width);
    r[1] = Val_int(cinfop->output_height);
    r[2] = alloc_tuple(3);
    Field(r[2], 0) = (value)cinfop;
    Field(r[2], 1) = (value)infile;
    Field(r[2], 2) = (value)jerrp;
    res = alloc_tuple(3);
    for(i=0; i<3; i++) Field(res, i) = r[i];
  }
#ifdef DEBUG_JPEG
  fprintf(stderr, "cinfop= %d infile= %d %d %d \n", cinfop, infile, cinfop->output_scanline, cinfop->output_height); 
  fflush(stderr);
#endif
  CAMLreturn(res);
}

value read_jpeg_scanline( jpegh, buf, offset )
value jpegh, offset, buf;
{
  CAMLparam3(jpegh,offset,buf);
  struct jpeg_decompress_struct *cinfop;
  JSAMPROW row[1];

  cinfop = (struct jpeg_decompress_struct *) Field( jpegh, 0 );
  row[0] = String_val( buf ) + Int_val( offset );
  jpeg_read_scanlines( cinfop, row, 1 );

  CAMLreturn(Val_unit);
}

/* no boundary checks */
value read_jpeg_scanlines( value jpegh, value buf, value offset, value lines )
{
  CAMLparam4(jpegh,offset,buf,lines);
  struct jpeg_decompress_struct *cinfop;
  JSAMPROW row[1];
  int clines = Int_val(lines);
  int i;
  row[0] = String_val(buf) + Int_val(offset);
  cinfop = (struct jpeg_decompress_struct *) Field( jpegh, 0 );
  // width is NOT image_width since we may have scale_denom <> 1
  int scanline_bytes = cinfop->output_width * 3; // no padding (size 3 is fixed? )
  for(i=0; i<clines; i++){
    jpeg_read_scanlines( cinfop, row, 1 );
    row[0] += scanline_bytes;
  }
  CAMLreturn0;
}

value close_jpeg_file_for_read( jpegh )
     value jpegh;
{
  CAMLparam1(jpegh);

  struct jpeg_decompress_struct *cinfop;
  struct my_error_mgr *jerrp;
  FILE *infile;

#ifdef DEBUG_JPEG
  fprintf(stderr, "closing\n");
  fflush(stderr);
#endif

  cinfop = (struct jpeg_decompress_struct *) Field( jpegh, 0 );
  infile = (FILE *) Field( jpegh, 1 );
  jerrp = (struct my_error_mgr *) Field( jpegh, 2 );

#ifdef DEBUG_JPEG
  fprintf(stderr, "cinfop= %d infile= %d %d %d \n", cinfop, infile, cinfop->output_scanline, cinfop->output_height); 
#endif

  if( cinfop->output_scanline >= cinfop->output_height ){ 
#ifdef DEBUG_JPEG
    fprintf(stderr, "finish\n");
    fflush(stderr);
#endif
    jpeg_finish_decompress( cinfop );
  }
#ifdef DEBUG_JPEG
  fprintf(stderr, "destroy\n");
  fflush(stderr);
#endif
  jpeg_destroy_decompress( cinfop ); 
  
  free(cinfop);
  free(jerrp);
#ifdef DEBUG_JPEG
  fprintf(stderr, "file close\n");
  fflush(stderr);
#endif
  fclose(infile);

  CAMLreturn(Val_unit);
}
