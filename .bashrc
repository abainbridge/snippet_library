#    < Set window title > <- Yellow ->     <- Green -->  <Normal >
PS1='\[\033]0;\u@\h\007\] \[\e[1;33m\]\n\t \[\e[1;32m\]\w\[\e[0m\]\n\$ '
 
# Update the BASH history file after each command, so if the sessions crashes/is
# terminated, we get to keep the history
shopt -s histappend
PROMPT_COMMAND="history -a;$PROMPT_COMMAND"
 
PROMPT_COMMAND="pwd > ~/.lastdir;$PROMPT_COMMAND"
cd `cat ~/.lastdir`
 
shopt -s checkwinsize
shopt -s no_empty_cmd_completion
shopt -s nocaseglob         # Make the * wildcard case insensitive
export HISTCONTROL=ignoreboth:erasedups
 
alias ls='ls -oAF --color --group-directories-first'
