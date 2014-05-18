/*
 *  Copyright (C) 2014 Cole Minor
 *  This file is part of elm-render-notes.
 *
 *  elm-render-notes is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  elm-render-notes is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#ifndef ELM_NOTE_RENDER_PLACES_H
#define ELM_NOTE_RENDER_PLACES_H

void places_initialize(void);
const char *places_get_long_name(const char *s);

/* short, medium */
#define X_PLACE_NAMES_FR(X) \
  X(aet, "Aeth Aelfan") \
  X(ani, "Anitora") \
  X(ari, "Arius") \
  X(cdp, "Cité du Port") \
  X(egr, "Égratia") \
  X(fen, "Fenegor") \
  X(gal, "Galein'th Aseyis") \
  X(gla, "Glakhmor") \
  X(hul, "Hulda") \
  X(ida, "Idaloran") \
  X(ill, "Illumen") \
  X(irl, "Iriliel") \
  X(irn, "Irinveron") \
  X(irs, "Irissadith") \
  X(isc, "Iscalrith") \
  X(kia, "Kial Kraw") \
  X(kil, "Kilaran") \
  X(kis, "Kilaran Sud") \
  X(kus, "Kusamura") \
  X(mel, "Melinis") \
  X(mor, "Morcraven") \
  X(nan, "Nargraw Nord") \
  X(nar, "Naralik") \
  X(nas, "Nargraw Sud") \
  X(niv, "Nivros-Um") \
  X(nor, "Nord-Thyl") \
  X(nuk, "Nukavuri") \
  X(oub, "Île des Oubliés") \
  X(pie, "Pierre Blanche") \
  X(sao, "Saonar Kraw") \
  X(sta, "Starenlith") \
  X(tah, "Taharaji") \
  X(tar, "Tarsengaard") \
  X(the, "Thelinor") \
  X(tra, "Trassian") \
  X(tre, "Trépont") \
  X(tyr, "Tyrnim") \
  X(yrs, "Yrsis") \
  X(zir, "Zirak-Inbar") \
  X(zor, "Zork'len")

#endif /* ELM_NOTE_RENDER_PLACES_H */
