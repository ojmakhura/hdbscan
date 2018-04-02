/*
 * PyHdbscan.c
 * 
 * Copyright 2018 Onalenna Junior Makhura
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation files
 * (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT.  IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "hdbscan/hdbscan.h"
#include <Python.h>
#include "structmember.h"

hdbscan* scan = NULL;

typedef struct {
	PyObject_HEAD
	PyObject *labels;
	uint minPoints, cols, rows;
} PyHdbscan;

static void PyHdbscan_dealloc(PyHdbscan* self){
	hdbscan_clean(scan);
    Py_XDECREF(self->labels);
    Py_TYPE(self)->tp_free((PyObject*)self);
}

static double* PyList_toArray(PyObject* dataset, double* dset_p, uint rows, uint cols){
	if(!dset_p){
		dset_p = (double *)malloc(rows * cols * sizeof(double));
	}
#pragma omp parallel for
	for(long i = 0; i < rows; i++){
		PyObject* row = PyList_GetItem(dataset, i);
		
		for(long j = 0; j < cols; j++){
			PyObject* temp = PyList_GetItem(row, j);
			long idx = i * cols + j;			
			dset_p[idx] = PyFloat_AsDouble(temp);
		}
	}
	
	return dset_p;
}

static PyObject *
PyHdbscan_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    PyHdbscan *self;

    self = (PyHdbscan *)type->tp_alloc(type, 0);
    if (self != NULL) {
        self->labels = NULL;
		self->minPoints = 0;
		self->rows = 0;
		self->cols = 0;
    }

    return (PyObject *)self;
}

static int
PyHdbscan_init(PyHdbscan *self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"minPoints", NULL};

    if (! PyArg_ParseTupleAndKeywords(args, kwds, "i", kwlist, &self->minPoints))
        return -1;
    
    if(self->minPoints < 3){
		return -1;
	} 
	
    scan = hdbscan_init(NULL, self->minPoints, DATATYPE_DOUBLE);
	
    return 0;
}

void get_labels(PyHdbscan *self, int32_t *labels){
	for(uint i = 0; i < self->rows; i++){
		PyList_Append(self->labels, Py_BuildValue("i", labels[i]));
	}
}

static PyObject *PyHdbscan_run(PyHdbscan *self, PyObject *args){
	/// TODO: check for errors
	PyObject *dataset;
    //static char *kwlist[] = {"dataset", NULL};
	double *dset = NULL;
    if (! PyArg_ParseTuple(args, "O", &dataset)){
        return NULL;
	}
    
    if (dataset) {		
        Py_INCREF(dataset);    
		self->rows = PyList_Size(dataset);
		PyObject* row = PyList_GetItem(dataset, 0);
		self->cols = PyList_Size(row);
		dset = PyList_toArray(dataset, NULL, self->rows, self->cols);
		self->labels = PyList_New(0);
		Py_INCREF(self->labels);
		Py_XDECREF(dataset); 
		if(!scan){
			return NULL;
		}
	} else {
		return NULL;
	}
	
	int err = hdbscan_run(scan, dset, self->rows, self->cols, TRUE);	
	get_labels(self, scan->clusterLabels);
	free(dset);
	return Py_BuildValue("i", err);
}
static PyObject *PyHdbscan_rerun(PyHdbscan *self, PyObject *args){
	Py_XDECREF(self->labels); 
    if (!PyArg_ParseTuple(args, "i", &self->minPoints))
        return NULL;
    
    if(self->minPoints < 3){
		return NULL;
	} 
	
	int err = hdbscan_rerun(scan, self->minPoints);
	self->labels = PyList_New(0);
	Py_INCREF(self->labels);
	get_labels(self, scan->clusterLabels);
	
	return Py_BuildValue("i", err);
}

static PyMethodDef PyHdbscan_methods[] = {
    {"run", (PyCFunction)PyHdbscan_run, METH_VARARGS, "Run the clustering algorithm and extract cluster labels"},
    {"rerun", (PyCFunction)PyHdbscan_rerun, METH_VARARGS, "Extract clusters using old dataset and new minPoints"},
    {NULL}  /* Sentinel */
};

static PyMemberDef PyHdbscan_members[] = {
    {"labels", T_OBJECT_EX, offsetof(PyHdbscan, labels), 0, "Cluster labels"},
    {"minPoints", T_INT, offsetof(PyHdbscan, minPoints), 0, "Minimum number of point in a cluster"},
    {"rows", T_INT, offsetof(PyHdbscan, rows), 0, "number of data points"},
    {"cols", T_INT, offsetof(PyHdbscan, cols), 0, "The size of each data point"},
    {NULL}  /* Sentinel */
};

static PyTypeObject PyHdbscanType = {
   PyVarObject_HEAD_INIT(NULL, 0)
   "PyHdbscan.PyHdbscan",               /* tp_name */
   sizeof(PyHdbscan),         /* tp_basicsize */
   0,                         /* tp_itemsize */
   (destructor)PyHdbscan_dealloc, /* tp_dealloc */
   0,                         /* tp_print */
   0,                         /* tp_getattr */
   0,                         /* tp_setattr */
   0,                         /* tp_compare */
   0,                         /* tp_repr */
   0,                         /* tp_as_number */
   0,                         /* tp_as_sequence */
   0,                         /* tp_as_mapping */
   0,                         /* tp_hash */
   0,                         /* tp_call */
   0,                         /* tp_str */
   0,                         /* tp_getattro */
   0,                         /* tp_setattro */
   0,                         /* tp_as_buffer */
   Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /* tp_flags*/
   "CountDict object",        /* tp_doc */
   0,                         /* tp_traverse */
   0,                         /* tp_clear */
   0,                         /* tp_richcompare */
   0,                         /* tp_weaklistoffset */
   0,                         /* tp_iter */
   0,                         /* tp_iternext */
   PyHdbscan_methods,         /* tp_methods */
   PyHdbscan_members,         /* tp_members */
   0,                         /* tp_getset */
   0,                         /* tp_base */
   0,                         /* tp_dict */
   0,                         /* tp_descr_get */
   0,                         /* tp_descr_set */
   0,                         /* tp_dictoffset */
   (initproc)PyHdbscan_init,  /* tp_init */
   0,                         /* tp_alloc */
   PyHdbscan_new,             /* tp_new */
};

static PyModuleDef PyHdbscanmodule = {	
    PyModuleDef_HEAD_INIT,
    "PyHdbscan",
    "HDBSCAn clustering algorithm",
    -1,
    NULL, NULL, NULL, NULL, NULL
};


PyMODINIT_FUNC
PyInit_PyHdbscan(void)
{
    PyObject* m;

    if (PyType_Ready(&PyHdbscanType) < 0)
        return NULL;

    m = PyModule_Create(&PyHdbscanmodule);
    if (m == NULL)
        return NULL;

    Py_INCREF(&PyHdbscanType);
    PyModule_AddObject(m, "PyHdbscan", (PyObject *)&PyHdbscanType);
    return m;
}
