(***********************************************************************)
(*                                                                     *)
(*                           Objective Caml                            *)
(*                                                                     *)
(*            Fran�ois Pessaux, projet Cristal, INRIA Rocquencourt     *)
(*            Pierre Weis, projet Cristal, INRIA Rocquencourt          *)
(*            Jun Furuse, projet Cristal, INRIA Rocquencourt           *)
(*                                                                     *)
(*  Copyright 1999-2004,                                               *)
(*  Institut National de Recherche en Informatique et en Automatique.  *)
(*  Distributed only by permission.                                    *)
(*                                                                     *)
(***********************************************************************)

(* $Id: camlimages.ml.in,v 1.3.2.1 2010/05/13 13:14:47 furuse Exp $ *)

let version = "0.1";;

(* Supported libraries *)
let lib_gif = true;;
let lib_png = true;;
let lib_jpeg = true;;
let lib_tiff = true;;
let lib_freetype = true;;
let lib_ps = true;;
let lib_xpm = true;;

(* External files *)
let path_rgb_txt = ref "/etc/X11/rgb.txt";;
let path_gs = ref "/usr/bin/gs";;

(* They are written in ML, so always supported *)
let lib_ppm = true;;
let lib_bmp = true;;
let lib_xvthumb = true;;

(* Word size, used for the bitmap swapping memory management *)
let word_size = Sys.word_size / 8;;

