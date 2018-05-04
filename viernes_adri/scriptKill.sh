ps -ef | grep main | awk '{ print "kill -9 "$2 >> "aux.sh"}'
sh aux.sh
rm aux.sh

ipcs
ipcrm -a
ipcs
