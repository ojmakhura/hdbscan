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

static hdbscan* cluster = NULL;
static double *dset = NULL;

typedef struct {
	PyObject_HEAD
	PyList *dataset;
	PyList *labels;
	uint minPoints, cols, rows;
} PyHdbscan;

static void PyHdbscan_dealloc(PyHdbscan* self){
	hdbscan_clean(cluster);
	free(dset);
    Py_XDECREF(self->labels);
    Py_TYPE(self)->tp_free((PyObject*)self);
}

static double* PyList_toArray(PyList* dataset, double* dset_p, uint rows, uint cols){
	if(!dset_p){
		dset_p = (double *)malloc(rows * cols * sizeof(double));
	}
#pragma omp parallel for
	for(long i = 0; i < rows; i++){
		PyObject* row = PyList_GetItem(dataset, i);
		
		for(long j = 0; j < cols; j++){
			PyObject* temp = PyList_GetItem(row, j);
			long idx = i * lenj + j;			
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
        self->dataset = NULL;
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
	PyList *dataset;
	uint minPoints;
    static char *kwlist[] = {"dataset", "minPoints", NULL};

    if (! PyArg_ParseTupleAndKeywords(args, kwds, "Oi", kwlist, &dataset, &minPoints,))
        return -1;
    
    if(minPoints < 3){
		return -1;
	} 
	
	self->labels = PyList_New();
	self->minPoints = minPoints;
    
    if (dataset) {
        tmp = self->dataset;
        Py_INCREF(dataset);
        self->dataset = dataset;
        Py_XDECREF(tmp);
    
		self->rows = PyList_Size(self->dataset);
		PyObject* row = PyList_GetItem(self->dataset, 0);
		self->cols = PyList_Size(row);
		dset = PyList_toArray(self->dataset, NULL, self->rows, self->cols);    
		cluster = hdbscan_init(cluster, self->minPoints, DATATYPE_DOUBLE);
		
		if(!cluster){
			return -1;
		}
	} else {
		return -1;
	}
	
    return 0;
}

static void PyHdbscan_run(PyHdbscan *self){
	/// TODO: check for errors
	
	int err = hdbscan_run(cluster, dset, self->rows, self->cols, TRUE);	
	for(uint i = 0; i < self->rows; i++){
		PyList_Append(self->labels, Py_BuildValue("i", cluster->labels[i]))
	}
}

static PyMethodDef PyHdbscan_methods[] = {
    {"run", (PyCFunction)PyHdbscan_run, METH_NOARGS, "Run the clustering algorithm and extract cluster labels"},
    {NULL}  /* Sentinel */
};

static PyMemberDef PyHdbscan_members[] = {
    {"dataset", T_OBJECT_EX, offsetof(PyHdbscan, dataset), 0, "dataset to be clustered"},
    {"labels", T_OBJECT_EX, offsetof(PyHdbscan, labels), 0, "Cluster labels"},
    {"minPoints", T_OBJECT_EX, offsetof(PyHdbscan, minPoints), 0, "Minimum number of point in a cluster"},
    {"rows", T_INT, offsetof(PyHdbscan, rows), 0, "number of data points"},
    {"cols", T_INT, offsetof(PyHdbscan, cols), 0, "The size of each data point"},
    {NULL}  /* Sentinel */
};

static PyTypeObject
PyHdbscanType = {
   PyObject_HEAD_INIT(NULL)
   0,                         /* ob_size */
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
   0,                         /* tp_new */
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
