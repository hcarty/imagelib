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

(* $Id: color.mli,v 1.1 2006/11/28 15:43:28 rousse Exp $ *)

(** Definition of colormaps, i.e. mappings from real RGB colors to
 integers. The integer corresponding to a color [c] is an index [i]
 into a vector of colors whose [i]th element is [c]. *)

exception Too_many_colors;;

type 'a map = { mutable max : int; mutable map : 'a array };;
(** This is copied in Images also *)
(** Colormap manipulation functions *)
val size : 'a map -> int;;
    (** Returns the size of a colormap. *)
val find_exact : 'a map -> 'a -> int;;
    (** Finds a color in the colormap and returns its color index.
       Raises exception [Not_found] if the color is not in the colormap. *)
val add_color : 'a map -> 'a -> int;;
    (** Add a new color into the given colormap and return its index.
       If the color is already in the colormap, it is not
       added again, and the corresponding color index is returned. *)
val add_colors : 'a map -> 'a list -> int list;;
    (** Add the list of new colors into the given colormap and return 
       their indices. If a color is already in the colormap, it is not
       added again, and the corresponding color index is returned. *)
val copy : 'a map -> 'a map;;
    (** Copy a colormap *)
(*
val find_nearest : 'a map -> 'a -> int;;
    (* [find_nearest m c] finds the color [co] that is the nearest to
    [c] color into the colormap [m].
    Returns the color index of [co] and the distance between [c]
    and [co]. *)
    (* this is defined inside each color model implementation *)
*)

module Rgb : sig
  type t = { mutable r : int; mutable g : int; mutable b : int }
  val square_distance : t -> t -> int
  val plus : t -> t -> t
  val minus : t -> t -> t
  val size : t map -> int
  val find_exact : t map -> t -> int
  val add_color : t map -> t -> int
  val add_colors : t map -> t list -> int list
  val find_nearest : t map -> t -> int
end;;

type rgb = Rgb.t = { mutable r : int; mutable g : int; mutable b : int };;
(* This is copied in Images also *)
 (* R(ed), G(reen), B(lue) representation of colors. *)

module Rgba : sig
  type t = { color : rgb; mutable alpha : int; } 
  val square_distance : t -> t -> int
  val plus : t -> t -> t
  val minus : t -> t -> t
  val merge : t -> t -> t
  val size : t map -> int
  val find_exact : t map -> t -> int
  val add_color : t map -> t -> int
  val add_colors : t map -> t list -> int list
  val find_nearest : t map -> t -> int
end;;

type rgba = Rgba.t = { color: rgb; mutable alpha : int; };;
 (** RGB with alpha (transparent) information *)

module Cmyk :
  sig
    type t = {mutable c : int; mutable m : int; mutable y : int;
	      mutable k : int } 
    val square_distance : t -> t -> int
    val plus : t -> t -> t
    val minus : t -> t -> t
    val size : t map -> int
    val find_exact : t map -> t -> int
    val add_color : t map -> t -> int
    val add_colors : t map -> t list -> int list
    val find_nearest : t map -> t -> int
  end;;

type cmyk = Cmyk.t =
  { mutable c : int; mutable m : int; mutable y : int; mutable k : int; };;
 (** Cyan Magenta Yellow blacK color model *)

(** Rgb specialized functions (for backward compatibility) *)
val rgb_square_distance : rgb -> rgb -> int;;
    (** Compute the distance between two colours. *)
val plus : rgb -> rgb -> rgb;;
val minus : rgb -> rgb -> rgb;;

val brightness : rgb -> int;;

(********************************************************* Color name parser *)

val color_parse : string -> rgb;;
val colormap_parse : string array -> rgb array * int;;
