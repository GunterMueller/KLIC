%
% Author:      William Chia-Wei Cheng (bill.cheng@acm.org)
%
% Copyright (C) 2001-2009, William Chia-Wei Cheng.
%
% This file may be distributed under the terms of the Q Public License
% as defined by Trolltech AS of Norway and appearing in the file
% LICENSE.QPL included in the packaging of this file.
%
% THIS FILE IS PROVIDED AS IS WITH NO WARRANTY OF ANY KIND, INCLUDING
% THE WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR
% PURPOSE.  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL,
% INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING
% FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT,
% NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION
% WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
%
% @(#)$Header: /mm2/home/cvs/bc-src/tgif/frontend.pl,v 1.6 2011/05/16 16:26:57 william Exp $
%

% The main predicate exported is:  interface/9.

:- abolish(foreign_file/2).
:- abolish(foreign/3).

foreign_file('frontend11.o', ['MainLoop','Animate','UpdAttrVal']).

foreign('MainLoop', c,
	interface(+string, +string, -string, -string, -string, -string,
	-string, -string, -string)).
foreign('Animate', c,
	interface_animate(+string, +string, +string, +string, -string)).
foreign('UpdAttrVal', c,
	interface_updattr(+string, +string, +string, +string, -string)).

:- load_foreign_files(['frontend11.o'],
	['-lXext','-lX11','-ldl','-lm','-lsocket','-lnsl','-lgen']),
	abolish(foreign_file/2),
	abolish(foreign/3).
