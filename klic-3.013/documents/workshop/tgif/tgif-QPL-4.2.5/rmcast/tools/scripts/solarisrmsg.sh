#/bin/bash

# This script removes everthing from your system message queue
# For further information about message queue and other IPC features
# look in the man pages

ipcs -q | grep $USER | awk '{print $2}' > ~/list.id

for i in `cat ~/list.id`
do
  ipcrm -q $i
done

rm -f ~/list.id


