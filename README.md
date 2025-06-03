# Minishell-Project

 MiniSHELL is a simple custom shell written in C. It supports basic shell commands, custom prompts, background jobs, and piping.

 *Features*

Change prompt using: PS1=YourPrompt like PS1=Balu

No spaces allowed (e.g., PS1 = test is invalid)

Command Execution:

Runs external commands

Waits for command to finish before showing prompt again

Ignores empty commands

Special Variables:

echo $? – shows exit status of last command

echo $$ – shows shell PID

echo $SHELL – shows path to msh

Signal Handling:

Ctrl+C – sends SIGINT to foreground process

Ctrl+Z – sends SIGTSTP, stops process and shows its PID

Built-in Commands:

cd – change directory

pwd – print current directory

exit – close the shell

Background Jobs:

Use & to run a process in background (e.g., sleep 5 &)

jobs – show background jobs

bg – resume a stopped job in background

fg – bring a background job to foreground

Pipes:

Supports commands with pipes (e.g., ls | grep txt | wc -l)
