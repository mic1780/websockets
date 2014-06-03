Websockets
==========

This project is for a websocket server in C

Current Requirements
====================
<ol>
	<li>GNU GCC compiler</li>
	<li>Access to a command-line that has BASH</li>
</ol>

<hr>
Some of the code must be changed due to this program being developed for use from a specific directory.
This will be changed over time while I make the files more dynamic.
I eventually plan to get rid of the PHP files and replace them with a C version of database connection.
In the meantime feel free to use and alter this project however you see fit.

Getting Started
===============
<h1>Simple setup (using compile defaults)</h1>
<ol>
	<li>Download files using clone or extracting the zip</li>
	<li>Navigate to the directory you extracted the files to using the command line</li>
	<li>
		To build the application (assuming nothing has been compiled yet) simply run the following command:
			<pre>./compile.sh -b</pre>
	</li>
		To run your newly created server run the following command:
			<pre>./compile.sh -r</pre>
	</li>
	<li>If you want to see the list of available console commands for the server type in "help" after the server has started.</li>
	<li>Enjoy your new websocket server</li>
</ol>

<h1>Advanced Setup</h1>
<ol>
	<li>Download files using clone or extracting the zip</li>
	<li>Navigate to the directory you extracted the files to using the command line</li>
	<li>
		You can run the following command to compile everything for you.
			<pre>./compile.sh -b</pre>
		If you did not run the above command then continue to the next step.
	</li>
	<li>
		All files need the libfunctions library so we need to generate it first:
			<pre>./compile.sh -f</pre>
		Or if you have your own global functions in seperate files you can generate them with libfunctions at the same time:
			<pre>./compile.sh -f libfunctions [your global function files]</pre>
	</li>
	<li>
		Once you have generated your libfunctions library, next you will need to generate your dynamic function libraries:
			<pre>./compile.sh -l</pre>
		This will compile the default dynamic libraries. This commands is the same as doing the following:
			<pre>./compile.sh -l sendMessage alterStruct performAction callFunction</pre>
	</li>
	<li>
		Finally, you need to actually create the main executable. This can be done using the following command:
			<pre>./compile.sh -a</pre>
		This gives the compiled application the default name websocket.exe on Windows or websocket.out on UNIX.
	</li>
	<li>
		Now you can run your new application:
			<pre>./compile.sh -r</pre>
		This will run your server application using this default name websocket.exe on Windows or websocket.out on UNIX
	</li>
	<li>Enjoy your new websocket server</li>
</ol>

If you want to know what compile.sh does you can run it with the -h flag to bring up help

How do I use it?
================

**Windows:**

On windows you can do one of the following to start your websocket server (for websocket.exe only):
<ul>
	<li>double-click run.exe</li>
	<li>Open a Command Prompt (Shift + Right Click in windows explorer and "Open command window here") and enter the following:<pre>run.exe</pre></li>
	<li>If you use a linux shell (for example: Cygwin), you can enter the following command in it:<pre>./run.exe</pre></li>
</ul>

**Linux / Unix**

<ul>
	<li>Using the command-line, change directories to where you created the websocket server then<pre>./run.out</pre></li>
	<li>If you have a file explorer, navigate to folder where you created the websocket server then open run.out (untested)</li>
</ul>

Questions?
==========

If you have any question or have found any bugs with this application please let
me know on GitHub or crate an issue with what happened and how I can reproduce the problem.

Want to help?
=============

If you are interested in helping me improve this program feel free to make changes and tell me
about what youu did and why it would help the server be more efficient, fast, or more dynamic
(dynamic be how can I keep the server online while I make revisions / changes to the dynamic libraries).
I am still trying to figure out how this should be used and I am trying to include all potential uses
of this program (like IM servers, chat rooms, notification pusher/puller, live updates, etc.).
Any help / ideas are welcome.
