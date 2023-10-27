#!/bin/sh

TOADDR=klic-report@klic.icot.or.jp

ask_yes_or_no () {
    set_var=$2;
    do_ask=true;
    if [ -n "$set_var" ]; then eval "default=\$$set_var"; do_ask=$def_ask;
    else default=""; fi
    if [ -z "$default" ]; then default=$1; do_ask=true; fi
    if [ "$set_var" = USE_DEFAULT ]; then set_var=''; do_ask=$def_ask; fi
    echo $n " (yes or no)"$c
    while true ; do
	echo $n " [$default] "$c
	if $do_ask ; then read ans; else echo $default; ans=$default; fi
	if test -z "$ans" ; then ans=$default; fi
	if [ -n "$set_var" ]; then echo "$set_var='$ans'" >> config.tmp; fi
	case $ans in
	yes) return 0;;
	no) return 1;;
	*) echo $n "Please input yes or no"$c;;
	esac
	do_ask=true
    done
}

concat_string () {
   if [ x"$1" = x ]; then
     eval $3="$2"
   else
     if [ x"$2" = x ]; then
       eval $3="$1"
     else
       eval $3=\"$1 $2\"
     fi
  fi
}

include_file () {
  source=$1
  dest=$2

  if [ ! -f $source ]; then
    echo "-------------------- $1 is not found --------------------" >>$dest
    return 0
  else
    echo "-------------------- $1 begin --------------------" >>$dest
    cat $source >>$dest
    echo "-------------------- $1 end --------------------" >>$dest
    return 1
  fi
}

if ( echo "test line\c"; echo " ") | grep c >/dev/null 2>/dev/null ; then
    n='-n'
    c=''
else
    n=''
    c='\c'
fi

MAILLOG=mail.log

cat <<GAZONK
This program checks the test results and send following files
to klic-report@klic.icot.or.jp:

	config.sh
	include/klic/config.h

GAZONK

echo $n "Can I send the above files to klic-report? "$c

if ask_yes_or_no yes; then
   :
else
   echo "Good bye!"
   exit 1
fi

cat <<GAZONK

If this program fails to send the mail, please send the contents of
the file mail.log to klic-report@klic.icot.or.jp by hands.

GAZONK

if [ x$USER = x ]; then
   USER=`whoami`
fi

cat <<GAZONK >$MAILLOG
From: $USER
To: $TOADDR
Subject: KLIC installation report

GAZONK

echo $n 'Platform: ' >>$MAILLOG

uname -a  >>$MAILLOG

echo >>$MAILLOG

IMP=0

if [ -f tests.log ]; then
   IMP=1
   cat tests.log >>$MAILLOG
fi

if [ -f disttests.log ]; then
   IMP=1
   cat disttests.log >>$MAILLOG
fi

if [ -f shmtests.log ]; then
   IMP=1
   cat shmtests.log >>$MAILLOG
fi

if [ $IMP -eq 0 ]; then
   echo "No implemetation was installed." >>$MAILLOG
fi

include_file config.sh $MAILLOG
include_file include/klic/config.h $MAILLOG


for sm in /usr/lib /usr/bin /usr/sbin /bin /sbin; do
   if [ -x $sm/sendmail ]; then
      SENDMAIL=$sm/sendmail
   fi
done

if [ x$SENDMAIL = x ]; then
   echo 'sendmail program not found.'
   echo 'Please send mail.log to klic-report@klic.icot.or.jp by hand.'
   echo 'Thanks in advance.'
else
   echo $n 'Sending mail...'

   if $SENDMAIL $TOADDR <$MAILLOG; then
      echo 'done.'
      echo 'Thanks for your coorperation.'
   else
      echo 'Failed to send mail.'
      echo 'Please send mail.log to klic-report@klic.icot.or.jp by hand.'
      echo 'Thanks in advance.'
   fi
fi

exit 0
