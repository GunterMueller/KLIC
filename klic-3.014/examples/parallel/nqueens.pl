% N-queens in Prolog, with output unification moved to the end

queens([], Y0, Y) :- /*L0*/ Y = Y0.
queens([A | D], Y, Z) :-
    select([A | D], U, V) /*L1*/, qsafe(U, Y, 1), /*L3*/ queens(V, [U | Y], Z).

% select(original-list, selected-element, rest)
select([A | D], A, D).
select([A | D], U, V0) :- select(D, U, V) /*L2*/, V0 = [A | V].

% qsafe(new-row, old-row-list, column-distance)
qsafe(U, [P | Q], V) :- P+V=\=U, P-V=\=U, V1 is V+1, qsafe(U, Q, V1).
qsafe(U, [],      V).
