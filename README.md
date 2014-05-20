websockets
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
	<li>Generate the functions library using the following command: <pre>./compileFunctions.sh doFunction performAction</pre></li>
	<li>Generate dynamic libraries using the following command: <pre>./compileLibs.sh sendMessage alterStruct</pre></li>
	<li>Create an executable file using: <pre>./compileApp.sh</pre></li>
	<li>Run the newly created websocket program</li>
</ol>