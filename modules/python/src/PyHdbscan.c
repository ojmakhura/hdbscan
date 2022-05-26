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

/**
 * @file PyHdbscan.c
 * @author Onalenna Junior Makhura (ojmakhura@roguesystems.co.bw)
 * @brief Implementation of the python 2 and 3 bindings.
 * @version 3.1.6
 * @date 2018-01-10
 * 
 * @copyright Copyright (c) 2018
 * 
 */
 
#if PY_MAJOR_VERSION >= 3
#define NPY_NO_DEPRECATED_API NPY_1_7_API_VERSION
#endif
#define NPY_NO_DEPRECATED_API NPY_1_7_API_VERSION

#include "hdbscan/hdbscan.h"
#include <numpy/arrayobject.h>
#include <Python.h>
#include "structmember.h"
#include <omp.h>

#define MOD_ERROR_VAL NULL
#define MOD_SUCCESS_VAL(val) val
#define MOD_INIT(name) PyMODINIT_FUNC PyInit_##name(void)
#define MOD_DEF(ob, name, doc, methods) \
        static struct PyModuleDef PyHdbscanNodule = { \
            PyModuleDef_HEAD_INIT, name, doc, -1, methods, }; \
        ob = PyModule_Create(&PyHdbscanNodule);

hdbscan* scan = NULL;

/**
 * @brief PyHdbscan object
 * 
 */
typedef struct {
	PyObject_HEAD
	PyObject* labels;
    PyObject* clusterMap;
    PyObject* hierarchy;
	index_t minPoints, cols, rows;
} PyHdbscan;

/**
 * @brief Cleanup
 * 
 * @param self 
 */
static void PyHdbscan_dealloc(PyHdbscan* self){
	hdbscan_clean(scan);
    Py_XDECREF(self->labels);
    Py_XDECREF(self->clusterMap);
    Py_XDECREF(self->hierarchy);
    Py_TYPE(self)->tp_free((PyObject*)self);
}

/**
 * @brief Create a new PyHdbscan object
 * 
 * @param type 
 * @param args 
 * @param kwds 
 * @return PyObject* 
 */
static PyObject *
PyHdbscan_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    PyHdbscan *self;

    self = (PyHdbscan *)type->tp_alloc(type, 0);
    if (self != NULL) {
        self->labels = NULL;
        self->clusterMap = NULL;
        self->hierarchy = NULL;
		self->minPoints = 0;
		self->rows = 0;
		self->cols = 0;
    }

    return (PyObject *)self;
}

/**
 * @brief 
 * 
 * @param self 
 * @param args 
 * @param kwds 
 * @return int 
 */
static int
PyHdbscan_init(PyHdbscan *self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"minPoints", NULL};

    char* c;
    if(sizeof(index_t) == sizeof(int)) {
        c = "I";
    } else if(sizeof(index_t) == sizeof(long)) {
        c = "k";
    } else {
        c ="H";
    }

    if (!PyArg_ParseTupleAndKeywords(args, kwds, c, kwlist, &self->minPoints))
        return -1;
    
    if(self->minPoints < 2){
		return -1;
	} 
	
    scan = hdbscan_init(NULL, self->minPoints);
	
    return 0;
}

/**
 * @brief Get the labels object
 * 
 * @param self 
 * @param labels 
 */
void get_labels(PyHdbscan *self, label_t *labels){
    char* c;
    if(sizeof(label_t) == sizeof(int)) {
        c = "I";
    } else if(sizeof(label_t) == sizeof(long)) {
        c = "k";
    } else {
        c ="H";
    }
    
    self->labels = PyList_New(0);

	for(uint i = 0; i < self->rows; i++){
		PyList_Append(self->labels, Py_BuildValue(c, labels[i]));
	}
}

/**
 * @brief 
 * 
 * @param self 
 * @param args 
 * @return PyObject* 
 */
static PyObject *PyHdbscan_run(PyHdbscan *self, PyObject *args){
	
	PyObject *dataset;
	    
    if (! PyArg_ParseTuple(args, "O", &dataset)){
        return NULL;
	}
    
    int typenum = PyArray_TYPE(dataset);
    uint datatype;

    /// Determine the datatype from the array
    if(typenum == NPY_DOUBLE)
    {
        datatype = H_DOUBLE;
    } 
    else if(typenum == NPY_FLOAT32)
    {
        datatype = H_FLOAT;
    }
    else if(typenum == NPY_INT32)
    {
        datatype = H_INT;
    }
    else if(typenum == NPY_LONG)
    {
        datatype = H_LONG;
    }
    else if(typenum == NPY_SHORT)
    {
        datatype = H_SHORT;
    }
    else{
        printf("Unsupported datatype.\n");
        return NULL;
    }

    Py_INCREF(dataset);    
    
    // Create a contigous array from dataset
    PyArrayObject* d_arr = (PyArrayObject*)PyArray_ContiguousFromAny(dataset, typenum, 0, 0);
	
    int nd = PyArray_NDIM(d_arr);
	npy_intp *dimensions = PyArray_DIMS(d_arr);

    // Get numpy array dimensions
    if(nd == 1)
    {
        self->cols = 1;
        self->rows = (uint)*dimensions;
    } else {
        self->rows = (uint)dimensions[0];
        self->cols = (uint)dimensions[1];
    }
	
    void *dset = PyArray_DATA(d_arr); /// The contigous array
	int err = hdbscan_run(scan, dset, self->rows, self->cols, TRUE, datatype);

    npy_intp dims[] = {self->rows, 1}; // Dimensions for the labels numpy array
    get_labels(self, scan->clusterLabels);

    Py_INCREF(self->labels);
    Py_XDECREF(dataset); // Release the dataset memory

	return Py_BuildValue("i", err);
}

/**
 * @brief 
 * 
 * @param self 
 * @param args 
 * @return PyObject* 
 */
static PyObject* PyHdbscan_rerun(PyHdbscan *self, PyObject *args) {
	Py_XDECREF(self->labels); 
    if (!PyArg_ParseTuple(args, "i", &self->minPoints))
        return NULL;
    
    if(self->minPoints < 2){
        printf("minPts must be greater than 2.\n");
		return NULL;
	} 
	
	int err = hdbscan_rerun(scan, self->minPoints);

    npy_intp dims[] = {self->rows, 1};
    enum NPY_TYPES tp = NPY_SHORT;

    if(sizeof(label_t) == sizeof(int)) {
        tp = NPY_INT;
    } else if (sizeof(label_t) == sizeof(long)) {
        tp = NPY_LONG;
    }

    get_labels(self, scan->clusterLabels);
    Py_INCREF(self->labels);
	
	return Py_BuildValue("i", err);
}

static PyObject* PyHdbscan_getClusterMap(PyHdbscan *self, PyObject *args) {
    Py_XDECREF(self->clusterMap); 
    int32_t begin, end;
    if (!PyArg_ParseTuple(args, "ii", &begin, &end))
        return NULL;

    enum NPY_TYPES tp = NPY_SHORT;

    if(sizeof(label_t) == sizeof(int)) {
        tp = NPY_INT;
    } else if (sizeof(label_t) == sizeof(long)) {
        tp = NPY_LONG;
    }

    int err = 1;
    hashtable* tmp = hdbscan_create_cluster_map(scan->clusterLabels, begin, end);
    self->clusterMap = PyDict_New();
    label_t label;

    for(size_t i = 0; i < tmp->size; i++) {
        label = ((label_t *)tmp->keys->data)[i];
        PyObject* key = Py_BuildValue("i", label);
        ArrayList* lst;
        hashtable_lookup(tmp, &label, &lst);
        npy_intp dims[] = {lst->size, 1};
        label_t* labels = lst->data;
        PyObject* value = PyArray_SimpleNewFromData(1, dims, tp, labels);
        PyDict_SetItem(self->clusterMap, key, value);
    }

    Py_INCREF(self->clusterMap);
    return self->clusterMap;
}

static PyObject* PyHdbscan_getHierarchies(PyHdbscan *self, PyObject *args) {
    Py_XDECREF(self->hierarchy); 
    enum NPY_TYPES tp = NPY_SHORT;

    if(sizeof(label_t) == sizeof(int)) {
        tp = NPY_INT;
    } else if (sizeof(label_t) == sizeof(long)) {
        tp = NPY_LONG;
    }

    int err = 1;
    self->hierarchy = PyDict_New();
    int64_t level;
	hierarchy_entry* data;

    for(size_t i = 0; i < hashtable_size(scan->hierarchy); i++) {
        
		set_value_at(scan->hierarchy->keys, i, &level);
        hashtable_lookup(scan->hierarchy, &level, &data);
        npy_intp dims[] = {scan->numPoints, 1};
        PyObject* value = PyArray_SimpleNewFromData(1, dims, tp, data->labels);
        PyObject* key = Py_BuildValue("k", level);
        PyDict_SetItem(self->hierarchy, key, value);
    }
    
    Py_INCREF(self->hierarchy);
    return Py_BuildValue("i", err);
}

/**
 * @brief PyHdbscan methods
 * 
 */
static PyMethodDef PyHdbscan_methods[] = {
    {"run", (PyCFunction)PyHdbscan_run, METH_VARARGS, "Run the clustering algorithm and extract cluster labels."},
    {"rerun", (PyCFunction)PyHdbscan_rerun, METH_VARARGS, "Extract clusters using old dataset and new minPoints."},
    {"getClusterMap", (PyCFunction)PyHdbscan_getClusterMap, METH_VARARGS, "Get a mapping of the cluster labels to the points."},
    {"getHierarchies", (PyCFunction)PyHdbscan_getHierarchies, METH_VARARGS, "Get the hierarchy data."},
    {NULL}  /* Sentinel */
};

/**
 * @brief PyHdbscan members
 * 
 */
static PyMemberDef PyHdbscan_members[] = {
    {"labels", T_OBJECT_EX, offsetof(PyHdbscan, labels), 0, "Cluster labels"},
    {"clusterMap", T_OBJECT_EX, offsetof(PyHdbscan, clusterMap), 0, "Dictionary of the clusters and the points"},
    {"hierarchy", T_OBJECT_EX, offsetof(PyHdbscan, hierarchy), 0, "Dictionary of the hierarchies"},
    {"minPoints", T_INT, offsetof(PyHdbscan, minPoints), 0, "Minimum number of point in a cluster"},
    {"rows", T_INT, offsetof(PyHdbscan, rows), 0, "number of data points"},
    {"cols", T_INT, offsetof(PyHdbscan, cols), 0, "The size of each data point"},
    {NULL}  /* Sentinel */
};

/**
 * @brief The PyTypeObject definition
 * 
 */
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

/**
 * @brief Construct a new mod init object
 * 
 */
MOD_INIT(PyHdbscan)
{
    PyObject *m;

    MOD_DEF(m, "PyHdbscan", "HDBSCAN clustering algorithm",
            NULL)

    if (m == NULL)
        return MOD_ERROR_VAL;

    if (PyType_Ready(&PyHdbscanType) < 0)
        return MOD_ERROR_VAL;

    Py_INCREF(&PyHdbscanType);
    PyModule_AddObject(m, "PyHdbscan", (PyObject *)&PyHdbscanType);
    import_array();
    return MOD_SUCCESS_VAL(m);

}
