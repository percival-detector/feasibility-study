#!/bin/bash

cd $HOME

TMUX_SESSION_STEM=pdaq

#CONFIG_FILE=ttm_8rx8tx_parallel.config
CONFIG_FILE=ttm_8rx8tx_rr.config

launch() 
{
	echo "Launching $MODE session..."
	tmux start-server
	echo "Checking session..."
	tmux has-session -t $SESSION 

	if [ "$?" -eq 1 ]; then
	
		echo "Need to start tmux session $SESSION"
		
		tmux new-session -d -s $SESSION -n $WINDOW  'ssh -t te7aegnode01'
		tmux split-window -h -p 50 -t $SESSION:0    'ssh -t te7aegnode05'
		tmux split-window -v -p 50 -t $SESSION:0.0  'ssh -t te7aegnode03'
		tmux split-window -v -p 50 -t $SESSION:0.1  'ssh -t te7aegnode07'
		tmux split-window -v -p 50 -t $SESSION:0.0  'ssh -t te7aegnode02'
		tmux split-window -v -p 50 -t $SESSION:0.1  'ssh -t te7aegnode06'
		tmux split-window -v -p 50 -t $SESSION:0.2  'ssh -t te7aegnode04'
		tmux split-window -v -p 50 -t $SESSION:0.3  'ssh -t te7aegnode08'
	 
	else
	  echo "tmux session $SESSION already running"
	fi
	
	tmux attach-session -t $SESSION
}

start()
{
	echo "Starting $MODE processes"
	
	tmux has-session -t $SESSION
	
	if [ "$?" -eq 0 ]; then
		tmux send-keys -t $SESSION:0.0 "/usr/local/bin/ttm_wrapper $CONFIG_FILE $LOGCONFIG_FILE ${NODE_PREFIX}1" C-m
		tmux send-keys -t $SESSION:0.4 "/usr/local/bin/ttm_wrapper $CONFIG_FILE $LOGCONFIG_FILE ${NODE_PREFIX}2" C-m
		tmux send-keys -t $SESSION:0.2 "/usr/local/bin/ttm_wrapper $CONFIG_FILE $LOGCONFIG_FILE ${NODE_PREFIX}3" C-m
		tmux send-keys -t $SESSION:0.6 "/usr/local/bin/ttm_wrapper $CONFIG_FILE $LOGCONFIG_FILE ${NODE_PREFIX}4" C-m
		tmux send-keys -t $SESSION:0.1 "/usr/local/bin/ttm_wrapper $CONFIG_FILE $LOGCONFIG_FILE ${NODE_PREFIX}5" C-m
		tmux send-keys -t $SESSION:0.5 "/usr/local/bin/ttm_wrapper $CONFIG_FILE $LOGCONFIG_FILE ${NODE_PREFIX}6" C-m
		tmux send-keys -t $SESSION:0.3 "/usr/local/bin/ttm_wrapper $CONFIG_FILE $LOGCONFIG_FILE ${NODE_PREFIX}7" C-m
		tmux send-keys -t $SESSION:0.7 "/usr/local/bin/ttm_wrapper $CONFIG_FILE $LOGCONFIG_FILE ${NODE_PREFIX}8" C-m
	else
		echo "There is no $MODE session running, start one with the launch command"
	fi
}

stop()
{
    echo "Stopping $MODE processes"
	tmux send-keys -t $SESSION:0.0 C-c
	tmux send-keys -t $SESSION:0.4 C-c
	tmux send-keys -t $SESSION:0.2 C-c
	tmux send-keys -t $SESSION:0.6 C-c
	tmux send-keys -t $SESSION:0.1 C-c
	tmux send-keys -t $SESSION:0.5 C-c
	tmux send-keys -t $SESSION:0.3 C-c
	tmux send-keys -t $SESSION:0.7 C-c
}

kill()
{
    echo "Killing $MODE session"
	tmux kill-session -t $SESSION
}

usage()
{
	name=`basename $0`
	echo "usage: $name [rx|tx] [launch|start|stop|kill]"
}

if [[ $# -ne 2 ]]; then
	usage
	exit 1
fi

case $1 in
  'rx')
  	MODE=rx
  	NODE_PREFIX='1'
  	;;
  'tx')
  	MODE=tx
  	NODE_PREFIX=''
	;;
  *)
    usage
    exit 1
    ;;
esac
  	
SESSION=${TMUX_SESSION_STEM}_${MODE}
WINDOW=${MODE}
LOGCONFIG_FILE="Log4CxxConfig_${MODE}.config"

case $2 in
  'launch')
    launch
    ;;
  'start')
    start
	;;
  'stop')
    stop
	;;
  'kill')
    kill
    ;;
  *)
 	usage
    exit 1
    ;;
esac

exit 0
