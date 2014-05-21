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
<ol>
	<li>Download files using clone or extracting the zip</li>
	<li>Navigate to the directory you extracted the files to using the command line</li>
	<li>
		All files need the libfunctions.dll so we need to generate it first:
			<pre>./compileFunctions.sh libfunctions</pre>
		Or generate it with static functions at the same time:
			<pre>./compileFunctions.sh libfunctions doFunction</pre>
	</li>
	<li>If you only generated the libfunctions, you need also need to run the following command: <pre>./compileFunctions.sh doFunction</pre></li>
	<li>Generate dynamic libraries using the following command: <pre>./compileLibs.sh sendMessage alterStruct performAction</pre></li>
	<li>Create an executable file using: <pre>./compileApp.sh</pre></li>
</ol>

How do I use it?
================

**Windows:**

On windows you can do one of the following to start your websocket server:
<ul>
	<li>double-click run.exe</li>
	<li>Open a Command Prompt (Shift + Right Click in windows explorer and "Open command window here") and enter the following:<pre>run.exe</pre></li>
	<li>If you use a linux shell (for example: Cygwin), you can enter the following command in it:<pre>./run.exe</pre></li>
</ul>

**Linux / Unix**

I have never used a Linux / Unix operating system so I am not sure you they have a file explorer but I'll include that method just in case
<ul>
	<li>Using the command-line, change directories to where you created the websocket server then<pre>./run.sh</pre></li>
	<li>If you have a file explorer, navigate to folder where you created the websocket server then open run.sh</li>
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
