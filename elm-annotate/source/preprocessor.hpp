//
//  Copyright (C) 2014 Cole Minor
//  This file is part of elm-annotate
//
//  elm-annotate is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  elm-annotate is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
#ifndef ELM_ANNOTATE_PREPROCESSOR_HPP
#define ELM_ANNOTATE_PREPROCESSOR_HPP

#define _PP_JOIN(a, b) a##b
#define PP_JOIN(a, b) _PP_JOIN(a, b)

#define PP_ID(n) PP_JOIN(PP_JOIN(_, n), __LINE__)

#define PP_REPEAT(n, X) PP_JOIN(_PP_REPEAT, n)(X)
#define _PP_REPEAT1(X) X(0)
#define _PP_REPEAT2(X) _PP_REPEAT1(X) X(1)
#define _PP_REPEAT3(X) _PP_REPEAT2(X) X(2)
#define _PP_REPEAT4(X) _PP_REPEAT3(X) X(3)
#define _PP_REPEAT5(X) _PP_REPEAT4(X) X(4)
#define _PP_REPEAT6(X) _PP_REPEAT5(X) X(5)
#define _PP_REPEAT7(X) _PP_REPEAT6(X) X(6)
#define _PP_REPEAT8(X) _PP_REPEAT7(X) X(7)
#define _PP_REPEAT9(X) _PP_REPEAT8(X) X(8)
#define _PP_REPEAT10(X) _PP_REPEAT9(X) X(9)
#define _PP_REPEAT11(X) _PP_REPEAT10(X) X(10)
#define _PP_REPEAT12(X) _PP_REPEAT11(X) X(11)
#define _PP_REPEAT13(X) _PP_REPEAT12(X) X(12)
#define _PP_REPEAT14(X) _PP_REPEAT13(X) X(13)
#define _PP_REPEAT15(X) _PP_REPEAT14(X) X(14)
#define _PP_REPEAT16(X) _PP_REPEAT15(X) X(15)

#define PP_LIST(n, X) PP_JOIN(_PP_LIST, n)(X)
#define _PP_LIST1(X) X(0)
#define _PP_LIST2(X) _PP_LIST1(X), X(1)
#define _PP_LIST3(X) _PP_LIST2(X), X(2)
#define _PP_LIST4(X) _PP_LIST3(X), X(3)
#define _PP_LIST5(X) _PP_LIST4(X), X(4)
#define _PP_LIST6(X) _PP_LIST5(X), X(5)
#define _PP_LIST7(X) _PP_LIST6(X), X(6)
#define _PP_LIST8(X) _PP_LIST7(X), X(7)
#define _PP_LIST9(X) _PP_LIST8(X), X(8)
#define _PP_LIST10(X) _PP_LIST9(X), X(9)
#define _PP_LIST11(X) _PP_LIST10(X), X(10)
#define _PP_LIST12(X) _PP_LIST11(X), X(11)
#define _PP_LIST13(X) _PP_LIST12(X), X(12)
#define _PP_LIST14(X) _PP_LIST13(X), X(13)
#define _PP_LIST15(X) _PP_LIST14(X), X(14)
#define _PP_LIST16(X) _PP_LIST15(X), X(15)

#endif // ELM_ANNOTATE_PREPROCESSOR_HPP
