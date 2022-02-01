export PYTHONSTARTUP=~/.pythonstartup

#    < Set window title > <- Yellow ->     <- Green -->  <Normal >
PS1='\[\033]0;\u@\h\007\] \[\e[1;33m\]\n\t \[\e[1;32m\]\w\[\e[0m\]\n\$ '
 
# Update the BASH history file after each command, so if the sessions crashes/is
# terminated, we get to keep the history
shopt -s histappend
PROMPT_COMMAND="history -a;$PROMPT_COMMAND"
 
PROMPT_COMMAND="pwd > ~/.lastdir;$PROMPT_COMMAND"
cd `cat ~/.lastdir`
 
shopt -s checkwinsize
shopt -s no_empty_cmd_completion # Without this, pressing tab with an empty command line searches PATH for everything. This takes ages.
shopt -s nocaseglob         # Make the * wildcard case insensitive
export HISTCONTROL=ignoreboth:erasedups
 
alias ls="ls -oAF --color --group-directories-first --block-size=\'1" # The funky block-size causes thousands to be separated by commas.

HISTFILESIZE=9000
HISTSIZE=8000
HISTIGNORE='ls:bg:fg:history'
HISTTIMEFORMAT='%F %T '

bind "set show-all-if-ambiguous on"
bind "set completion-ignore-case on"

python2 -c "import colorsys, hashlib; hue=int(hashlib.md5(\"$HOSTNAME\").hexdigest()[:2], 16)/255.0; c=colorsys.hsv_to_rgb(hue, 1, 0.3); print'\033]11;#%xf%xf%xf\a'%(int(c[0]*11),int(c[1]*9),int(c[2]*15))"
