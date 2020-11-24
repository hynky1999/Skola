# .bashrc

# Source global definitions
if [ -f /etc/bashrc ]; then
	. /etc/bashrc
fi

# User specific environment
if ! [[ "$PATH" =~ "$HOME/.local/bin:$HOME/bin:" ]]
then
    PATH="$HOME/.local/bin:$HOME/bin:$PATH"
fi
export PATH

# Uncomment the following line if you don't like systemctl's auto-paging feature:
# export SYSTEMD_PAGER=
export PS1="\u@\h:\w\\$ "
alias vi='vimx'
alias vim='vimx'

# User specific aliases and functions
GITHUB_ACCESS_TOKEN=7257fcf358116c86c4f6a982484d8269aaacb82a
