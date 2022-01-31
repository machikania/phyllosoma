' This script will change enter codes to CRLF in all files in current folder
' Run this script by cscript.exe

option explicit
dim fso
set fso = createObject("Scripting.FileSystemObject")

explorerDir(".\")

msgbox "done"

sub explorerDir(dir)
	dim file,dirobj,text,newtext,line
	' Create new folder to make new files
	if false=fso.folderExists("crlf\" & dir) then fso.createFolder("crlf\" & dir)
	' Construct an object
	set dirobj=fso.getFolder(dir)
	' Check all files in the directory
	for each file in dirobj.files
		set text=fso.OpenTextFile(dir & file.name)
		' Check if CRLF exists
		If 0=InStr(text.ReadAll(), vbCrLf) Then
			' This file doesn't contain CRLF
			WScript.Echo dir & file.name
			set text=fso.OpenTextFile(dir & file.name)
			set newtext=fso.CreateTextFile("crlf\" & dir & file.name,true)
			do until text.AtEndOfStream
				line=text.ReadLine
				newtext.WriteLine line
			loop
		end if
	next
	' Pick up all sub folders
	for each file in dirobj.subfolders
		' Exclude git folder and crlf folder
		if ".git"<>file.name and "crlf"<>file.name then explorerDir(dir & file.name & "\")
	next
end sub
