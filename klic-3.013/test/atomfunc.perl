## ---------------------------------------------------------- 
##   (C)1993,1994,1995 Institute for New Generation Computer Technology 
##       (Read COPYRIGHT for detailed information.) 
##   (C)1996, 1997, 1998, 1999 Japan Information Processing Development Center
##       (Read COPYRIGHT-JIPDEC for detailed information.)
##-----------------------------------------------------------
sub decode {
    local($encode)=@_;
    local($decode);
    for (;;) {
	if ($encode eq "") {
	    $decode =~ s/([\'\\])/\\\1/g;
	    return $decode;
	} elsif ($encode =~ /^([a-zA-Z0-9]+)/) {
	    $decode .= $1;
	    $encode =~ s/^[a-zA-Z0-9]+//;
	} elsif ($encode =~ /^__/) {
	    $decode .= "_";
	    $encode =~ s/^__//;
	} elsif ($encode =~ /^_([0-9A-F]{2})/) {
	    $decode .= sprintf("%c", hex($1));
	    $encode =~ s/^_[0-9A-F]{2}//;
	} else {
	    die;
	}
    }
}
print ":- module main.\n";
$var = "AAA";
while (<>) {
    if (/^atom_(.*)$/) {
	push(@atom, &decode($1));
    } elsif (/^functor_(.*)_(\d+)$/) {
	($2 == 0) && next;
	push(@arity, $2);
	push(@funct, &decode($1));
    }
}
print "main:-\n",
  " X=['",join("',\n\t'", @atom),"'],\n",
  " Y=[\"",join("\",\n\t\"", @atom),"\"],\n";
# make funct

for ($i = 0; $i < @arity; ++$i) {
    @arg = ();
    for ($a = 0; $a < $arity[$i]; ++$a) {
	push(@arg, 0);
    }
    push(@functor, "'$funct[$i]'(".join(",", @arg).")");
}
print
  "	A=[",join(",\n\t", @arity),"],\n",
  "	P=[\'",join("\',\n\t\'", @funct),"\'],\n",
  "	F=[",join(",\n\t", @functor),"],\n",
  " comp(X,Y),\n",
  " comp(F,P,A).\n";


print "
comp([],[]).
comp([A|B],[C|D]) :- atom_table:get_atom_string(A,C), comp(B,D).

comp([],[],[]).
comp([A|B],[C|D],[E|F]) :- functor(A,C,E) | comp(B,D,F).

";

    

