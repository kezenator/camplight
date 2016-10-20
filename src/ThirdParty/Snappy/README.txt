To create the snappy folder:

1) The downloaded ZIP file was extracted
2) The required .h and .cc files were copied out
3) snappy-stubs-public.h.in was renamed to .h and
   fixed up for out platform - since we don't use automake.
   The version was fixed a 1.1.3 by hand
4) The following pragma was added to snappy.cc
   to prevent warnings in MSVC
