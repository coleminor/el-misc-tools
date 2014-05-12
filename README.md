el-misc-tools
=============
Miscellaneous scripts and programs for viewing, creating and
modifying files for the online game Eternal Lands, and the
French version Landes Eternelles.

elm2pov.pl
----------
This perl script uses several perl modules to read an
Eternal Lands game map file, write a scene definition
file, convert and downsize needed texture images with
ImageMagick, and render a final tab map image using
POV-Ray.

elm2mask.pl
-----------
Based on elm2pov.pl, this script uses povray to generate
a black and white PNG image mask of all points intersected
by a chosen set of map meshes.

mask2rects
----------
A program to find rectangular covers for regions in plane
specified by an image mask.

rects2def
---------
Transforms a list of bounding rectangles into sections of
an Eternal Lands server-side map def file.

el-copy-map.py
--------------
Copies rectangular regions from one map to another.

elm-remove-bad-lights.py
------------------------
Removes or just finds all invalid lights in an map file.

elmhdr
------
A small and fast program to display map file headers.

elm-render-notes
----------------
Creates an overlay image of fancy map symbols from a list
of notes.
