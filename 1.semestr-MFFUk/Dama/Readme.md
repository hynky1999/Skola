Programmer documentation is in form of docstrings in python.
I have used sphinx to extract it to html. If you want to make it again
you have to commentout call of startGame() at the end of main.py.

User documentation can be found in doctring of module main.py, it should be pretty clear
how to use this program

There are only two testing input but it should showcase all of the features.
First testing input is called input.txt. It will play normal game with white side winning.
Second testing input is called input-error.txt. It will input wrong commands for showcase of errors.

If you will test it with input.txt last print of checkboard will be displayed incorrectly.
It is because when user inputs input he press enter thus new line is inserted.
When input is redirected to program there is no enter pressed thus no new line.
