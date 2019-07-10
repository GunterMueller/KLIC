:- module main.
main :- unix:unix([read_open(string#"iotest.kl1", InRes)])@lower_priority,
	unix:unix([stdout(OutRes)]),
	check_streams(InRes, OutRes).

check_streams(normal(In0), normal(Out0)) :-
	In0=[getc(X)|In],
	copy(X,In,Out0).
otherwise.
check_streams(_, _).

copy(X,In,Out) :- X=:= -1 | In=[], Out=[].
copy(X,In,Out) :- X=\= -1 |
    Out=[putc(X),fflush(Ack)|OutT], In=[getc(Y)|InT],
    copy(Ack, Y, InT, OutT).

copy(0, Y, InT, OutT) :-
    copy(Y,InT,OutT).
copy(-1, _, InT, OutT) :- InT=[fclose(Iack)], OutT=[fclose(Oack)],
	wait_ack(Iack, Oack).

wait_ack(I, O) :- wait(I), wait(O) | true.


