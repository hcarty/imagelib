(***********************************************************************)
(*                                                                     *)
(*                           Objective Caml                            *)
(*                                                                     *)
(*            François Pessaux, projet Cristal, INRIA Rocquencourt     *)
(*            Pierre Weis, projet Cristal, INRIA Rocquencourt          *)
(*            Jun Furuse, projet Cristal, INRIA Rocquencourt           *)
(*                                                                     *)
(*  Copyright 1999-2004,                                               *)
(*  Institut National de Recherche en Informatique et en Automatique.  *)
(*  Distributed only by permission.                                    *)
(*                                                                     *)
(***********************************************************************)

(* $Id: bitmap.mli,v 1.5 2009/07/04 03:39:28 furuse Exp $*)
 
(* Bitmaps used in images.
   Bitmaps are partitioned into blocks. Usually only one block is 
   allocated for one image, but for huge images, they needs more... *)

val debug : bool ref;;

(* see Swap to set swap file directory *)

val maximum_live : int ref;;
val maximum_block_size : int ref;;
  (* Configuration parameters for image swapping.

     You can specify the maximum size of live data by setting [maximum_live]
     in words. If the size of live data in heap exceeds [maximum_live], then
     Camlimages library tries to escape part of image buffer into "swap" 
     files. If swapped data are required, they are read back into memory.
     This swapping is automatically done by the camlimages library.
     If [maximum_live] is 0, image swapping is disabled.

     Swapped images are separated into block shaped "partitions". 
     [maximum_block_size] is a maximum size of each partition, also in 
     words. This parameter may affect the swapping performance. There is
     no theory (yet) how we should specify it. The author of the library
     propose to have (!maximum_live / 10). If it is larger, each swapping 
     becomes slower. If smaller, more swappings will occur. Too large and 
     too small maximum_block_size, both may make the program slower.

     If you use image swapping, you need to explicitly call [destroy]
     function of each image format (Rgb24.destroy, image#destroy, etc...)
     to free the memory and swap files of the needless images.

     The defaults are both 0. (i.e. swapping is disabled ) *)

module Block : sig
  type t = {
    width : int;
    height : int;
    x : int;
    y : int;
    dump : string;
  }
end

module type Bitdepth = sig
  val bytes_per_pixel : int
end;;

module Make(B:Bitdepth) : sig
  type t;;
  (* Bitmap type *)

  val create : int -> int -> string option -> t
    (* [create width height initopt] creates a bitmap of size
       [width] x [height]. You can set [initopt] the value to 
       fill the bitmap *)

  val create_with : int -> int -> string -> t
    (* [create_with width height initdata] creates a bitmap whose
        initial data is [initdata]. *)

  val create_with_scanlines : int -> int -> string array -> t

  val destroy : t -> unit
    (* Destroy bitmaps *)

  val access : t -> int -> int -> string * int

  val get_strip : t -> int -> int -> int -> string
  val set_strip : t -> int -> int -> int -> string -> unit
    (* Strip access
       Here, "strip" means a rectangle region with height 1.
  	 [get_strip t x y w] returns the string reprensentation of strip of [t]
       at (x, y) - (x + w - 1, y).
  	 [set_strip t x y w str] write [str] to the strip of [t]
       at (x, y) - (x + w - 1, y).
    *)
 
  val get_scanline : t -> int -> string
  val set_scanline : t -> int -> string -> unit
    (* Scanline access 
  	 [get_scanline t y] returns the string representation of the scanline
       of [t] at [y].
  	 [set_scanline t y str] writes [str] to the scanline of [t] at [y].
    *)

  (* only for one row *)      
  val get_scanline_ptr : t -> (int -> (string * int) * int) option

  val dump : t -> string
    (* Create a string representation of a bitmap. It may easily raise
       an exception Out_of_memory for large images. *)

  val copy : t -> t

  val sub : t -> int -> int -> int -> int -> t
    (* [sub src x y width height] returns sub-bitmap of [src],
       at (x, y) - (x + width - 1, y + height - 1). *)

  val blit : t -> int -> int -> t -> int -> int -> int -> int -> unit
    (* [blit src sx sy dst dx dy width height] copies the rectangle
       region of [src] at
       (sx, sy) - (sx + width - 1, sy + height - 1)
       to [dst], at
       (dx, dy) - (dx + width - 1, dy + height - 1) *)

  val blocks : t -> int * int
    (* returns number of blocks in row and column *)  

  val dump_block : t -> int -> int -> Block.t
end;;
