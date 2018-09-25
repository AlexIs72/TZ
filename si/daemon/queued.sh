#!/bin/sh


DAEMON=queued
PID_FILE=/tmp/${DAEMON}.pid


stop() {
	[ -f ${PID_FILE} ] && {
		pid=`cat ${PID_FILE}`
		kill $pid
		rm -f ${PID_FILE}
	} || true
}

start() {
	./${DAEMON}
	pid_file=`pidof ${DAEMON}`
	[ ! -z "$pid_file" ] && {
		echo $pid_file > ${PID_FILE}
		echo "Daemon started"
	}
}



case "$1" in
	start)
		start
	;;
	stop)
		stop
	;;
	restart)
		stop
		start
	;;
	*)
		echo "Usage: $0 <start|stop|restart>"
	;;
esac
	