#define N_IMPLEMENTS nPythonServer
//--------------------------------------------------------------------
//  npythonrun.cc -- Command processing
//  Created by Jason Asbahr, 2001
//  Derived from ntclrun.cc by Andre Weissflog
//  Updates by Tivonenko Ivan (aka Dark Dragon), 2003
//--------------------------------------------------------------------

#include <stdlib.h>
#include <stdio.h>

#include "kernel/nfileserver2.h"
#include "python/npythonserver.h"

//--------------------------------------------------------------------
//  Prompt()
//--------------------------------------------------------------------
char *nPythonServer::Prompt(char *buf, int size)
{
	char buffer[1024];
	const char* result;
	buffer[0] = 0;

	this->Run("psel()", Py_eval_input, result);

	if (result)
		strcat(buffer, result);

	strcat(buffer, ">");
	n_strncpy2(buf, buffer, size);
	return buf;
}


//--------------------------------------------------------------------
//  Run(const char *cmd_str, const char *& result)
//  Wraps Run() with mode
//--------------------------------------------------------------------

bool nPythonServer::Run(const char *cmd_str, const char*& result)
{
	return this->Run(cmd_str, Py_single_input, result);
}

PyObject *NArg2PyObject(nArg *a) 
{
	switch (a->GetType()) {
		case nArg::ARGTYPE_VOID:
			return Py_None;
		case nArg::ARGTYPE_INT:
			return PyInt_FromLong(a->GetI());
		case nArg::ARGTYPE_FLOAT:
			return PyFloat_FromDouble(a->GetF());			
		case nArg::ARGTYPE_STRING:
			return PyString_FromString(a->GetS());
		case nArg::ARGTYPE_USTRING:
			{
				const wchar* s = a->GetU();
				int sz = s ? wcslen(s) : 0;
				return PyUnicode_FromUnicode((const Py_UNICODE*)(s ? s : L":null:"), sz);
			}			
		case nArg::ARGTYPE_BOOL:
			if (a->GetB())
				return PyInt_FromLong(1);
			else
				return PyInt_FromLong(0);
		case nArg::ARGTYPE_OBJECT:
			return PyCObject_FromVoidPtr(a->GetO(), NULL);
		case nArg::ARGTYPE_CODE:
			return PyString_FromString(a->GetC());
	}
	return NULL;
}

//--------------------------------------------------------------------
//  Run(const char *cmd_str, const char *& result)
//  Executes a given string as Python code
//--------------------------------------------------------------------

bool nPythonServer::Run(const char *cmd_str, int mode, const char*& result)
{
	// buffer was to small :(
	char buffer[1<<12];
	PyObject *module_dict;
	PyObject *main_module_dict;
	PyObject *output = NULL;
	bool     retval = true;

	result = NULL;

	module_dict = PyModule_GetDict(this->nmodule);
	main_module_dict = PyModule_GetDict(this->main_module);

	if (strlen(cmd_str) > 0) {
		output = PyRun_String((char *)cmd_str, mode, main_module_dict, module_dict);
	}
	else {
		retval = true;
	}

	// Process output

	if (output) {
		if (output == Py_None) {
			Py_DECREF(output);
			strcpy(&buffer[0], "");   // TODO: More elegant solution here
			result = (const char*)&buffer[0];
			retval = true;
		}
		else {
			// Copy the string, because it will be freed when output is decref'ed.
			PyObject* temp;
			temp = PyObject_Str(output);
			strcpy(&buffer[0], PyString_AsString(temp));
			Py_DECREF(temp);
			Py_DECREF(output);
			result = (const char*)&buffer[0];
			retval = true;
		}
	}
	else {
		// There was an exception.
		PyObject* exception = NULL;
		exception = PyErr_Occurred();
		if (exception) {
			PyObject* temp;
			temp = PyObject_Str(exception);
			strcpy(&buffer[0], PyString_AsString(temp));
			result = (const char*)&buffer[0];
			Py_DECREF(temp);
			PyErr_Print();
		}
		retval = false;
	}

	return retval;
}


//--------------------------------------------------------------------
//  RunScript()
//  Executes the contents of a file as Python code
//--------------------------------------------------------------------

bool nPythonServer::RunScript(const char *fname, const char*& result)
{	
	char buffer[1024];	
	int success = 0;
	result = NULL;

	// Initialize Python commands in __main__
	// Note, this could be done via the Python C API, but it would
	//       require many more lines.  :-)
	PyRun_SimpleString("from npython import *");

	// Open file
	nFileServer2* file2 = kernelServer->GetFileServer2();
	stl_string path;
	file2->ManglePath(fname, path);
	nFile* file = file2->NewFileObject();

	if (file->Open(path.c_str(), "r")) 
	{
		// We only opened this to be able to report the error ourselves.
		// So, go on, close it now.
		file->Close();

		// We go through this madness of using execfile() because under
		// Windows, the FILE *fp is different between Debug and Release
		// builds.  Because of this, PyRun_SimpleFile() doesn't work
		// if you try to force a debug build of Nebula to use the Release
		// build of Python.  Instead of requiring everyone who has
		// a debug build of Nebula to build Python from source, we're using
		// this work around here.
		stl_string exec_str = "execfile('";
		exec_str += path.c_str();
		exec_str += "')";		
		
		success = PyRun_SimpleString(&exec_str[0]);		

		if (success == -1) 
		{
			// interpreter exits due to an exception
			strcpy(&buffer[0], "Problem running script!");
			if(PyErr_Occurred())
				PyErr_Print();
			result = (const char*)&buffer[0];
			return false;
		}
	}
	else {
		n_delete file;
		n_printf("Error unable to open file %s", fname);
		return false;
	}
	n_delete file;
	return true;
}


//--------------------------------------------------------------------
//  Trigger()
//
//  Process events gathered by Python.  This function checks to see if
//  a callback has been registered by the Python layer, and if so, it
//  calls it.
//
//  TODO: Add time delta calculation and pass the delta as an argument
//--------------------------------------------------------------------

bool nPythonServer::Trigger(void)
{
	PyObject *result = NULL;
	PyObject *delta  = NULL;

	if (PyCallable_Check(this->callback)) {
		result = PyEval_CallObject(this->callback, delta);
		Py_XDECREF(delta);

		if (result == NULL)
			// TODO: Report a traceback
			return false;
		else
			Py_XDECREF(result);

	}

	// Let's see what the rest of the trigger system has to say
	return nScriptServer::Trigger();
}

//--------------------------------------------------------------------
//  EOF
//--------------------------------------------------------------------

