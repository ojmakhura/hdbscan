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
	hdbscan* scan;		/// Each Python object owns its own clustering state
} PyHdbscan;

/**
 * @brief Get the numpy type matching label_t
 * 
 * @return int 
 */
static int label_npy_type(void){
    if(sizeof(label_t) == sizeof(int)) {
        return NPY_INT;
    } else if (sizeof(label_t) == sizeof(long)) {
        return NPY_LONG;
    }
    return NPY_SHORT;
}

/**
 * @brief Get the PyArg format character matching index_t
 * 
 * @return const char* 
 */
static const char* index_format(void){
    if(sizeof(index_t) == sizeof(int)) {
        return "I";
    } else if(sizeof(index_t) == sizeof(long)) {
        return "k";
    }
    return "H";
}

/**
 * @brief Replace a PyObject member, releasing the old reference
 * 
 * @param member 
 * @param value A new reference, which is stolen
 */
static void replace_member(PyObject** member, PyObject* value){
    PyObject* old = *member;
    *member = value;
    Py_XDECREF(old);
}

/**
 * @brief Cleanup
 * 
 * @param self 
 */
static void PyHdbscan_dealloc(PyHdbscan* self){
    if(self->scan != NULL){
        hdbscan_destroy(self->scan);
        self->scan = NULL;
    }
    Py_CLEAR(self->labels);
    Py_CLEAR(self->clusterMap);
    Py_CLEAR(self->hierarchy);
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
		self->scan = NULL;
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
    index_t minPoints = 0;

    if (!PyArg_ParseTupleAndKeywords(args, kwds, index_format(), kwlist, &minPoints))
        return -1;
    
    if(minPoints < 2){
        PyErr_SetString(PyExc_ValueError, "minPoints must be at least 2.");
		return -1;
	} 

    if(self->scan != NULL){
        hdbscan_destroy(self->scan);
    }

    self->minPoints = minPoints;
    self->rows = 0;
    self->cols = 0;
    self->scan = hdbscan_init(NULL, self->minPoints);
    if(self->scan == NULL){
        PyErr_NoMemory();
        return -1;
    }
	
    return 0;
}

/**
 * @brief Build self->labels from the clustering result
 * 
 * @param self 
 * @return int 0 on success, -1 with a Python exception set on failure
 */
static int get_labels(PyHdbscan *self){
    label_t* labels = self->scan->clusterLabels;
    if(labels == NULL){
        PyErr_SetString(PyExc_RuntimeError, "No cluster labels available.");
        return -1;
    }

    PyObject* list = PyList_New((Py_ssize_t)self->rows);
    if(list == NULL){
        return -1;
    }

	for(index_t i = 0; i < self->rows; i++){
        PyObject* item = PyLong_FromUnsignedLong((unsigned long)labels[i]);
        if(item == NULL){
            Py_DECREF(list);
            return -1;
        }
		PyList_SET_ITEM(list, (Py_ssize_t)i, item);	/// Steals the reference
	}

    replace_member(&self->labels, list);
    return 0;
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

    if(self->scan == NULL){
        PyErr_SetString(PyExc_RuntimeError, "PyHdbscan has not been initialised.");
        return NULL;
    }
    
    // Convert anything array-like into an aligned, C-contiguous numpy array
    PyArrayObject* d_arr = (PyArrayObject*)PyArray_FROM_OF(dataset, NPY_ARRAY_IN_ARRAY);
    if(d_arr == NULL){
        return NULL;
    }

    int typenum = PyArray_TYPE(d_arr);
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
        Py_DECREF(d_arr);
        PyErr_SetString(PyExc_TypeError, "Unsupported datatype.");
        return NULL;
    }
	
    int nd = PyArray_NDIM(d_arr);
	npy_intp *dimensions = PyArray_DIMS(d_arr);
    npy_intp rows, cols;

    // Get numpy array dimensions
    if(nd == 1)
    {
        rows = dimensions[0];
        cols = 1;
    } else if(nd == 2) {
        rows = dimensions[0];
        cols = dimensions[1];
    } else {
        Py_DECREF(d_arr);
        PyErr_SetString(PyExc_ValueError, "The dataset must be a 1 or 2 dimensional array.");
        return NULL;
    }

    if(rows < 2 || cols < 1 || (npy_uintp)rows > (npy_uintp)((index_t)-1) || (npy_uintp)cols > (npy_uintp)((index_t)-1)){
        Py_DECREF(d_arr);
        PyErr_SetString(PyExc_ValueError, "The dataset has an unsupported shape.");
        return NULL;
    }

    self->rows = (index_t)rows;
    self->cols = (index_t)cols;
	
    void *dset = PyArray_DATA(d_arr); /// The contigous array
	int err = hdbscan_run(self->scan, dset, self->rows, self->cols, TRUE, datatype);
    Py_DECREF(d_arr);

    if(err == HDBSCAN_SUCCESS && get_labels(self) < 0){
        return NULL;
    }

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
    index_t minPoints = 0;
    if (!PyArg_ParseTuple(args, index_format(), &minPoints))
        return NULL;
    
    if(self->scan == NULL || self->scan->distanceFunction.distances == NULL){
        PyErr_SetString(PyExc_RuntimeError, "run() must succeed before rerun().");
        return NULL;
    }

    if(minPoints < 2 || minPoints > self->rows){
        PyErr_SetString(PyExc_ValueError, "minPoints must be between 2 and the number of rows.");
		return NULL;
	} 
	
    self->minPoints = minPoints;
	int err = hdbscan_rerun(self->scan, self->minPoints);

    if(err == HDBSCAN_SUCCESS && get_labels(self) < 0){
        return NULL;
    }
	
	return Py_BuildValue("i", err);
}

static PyObject* PyHdbscan_getClusterMap(PyHdbscan *self, PyObject *args) {
    int32_t begin, end;
    if (!PyArg_ParseTuple(args, "ii", &begin, &end))
        return NULL;

    if(self->scan == NULL || self->scan->clusterLabels == NULL){
        PyErr_SetString(PyExc_RuntimeError, "run() must succeed before getClusterMap().");
        return NULL;
    }

    if(begin < 0 || end <= begin || (index_t)end > self->rows){
        PyErr_SetString(PyExc_IndexError, "begin and end must satisfy 0 <= begin < end <= rows.");
        return NULL;
    }

    int tp = label_npy_type();
    hashtable* tmp = hdbscan_create_cluster_map(self->scan->clusterLabels, (index_t)begin, (index_t)end);
    if(tmp == NULL){
        return PyErr_NoMemory();
    }

    PyObject* clusterMap = PyDict_New();
    if(clusterMap == NULL){
        hdbscan_destroy_cluster_map(tmp);
        return NULL;
    }

    for(size_t i = 0; i < hashtable_size(tmp); i++) {
        label_t label = ((label_t *)tmp->keys->data)[i];
        ArrayList* lst = NULL;
        hashtable_lookup(tmp, &label, &lst);
        if(lst == NULL){
            continue;
        }

        // Copy the points so the array does not refer to memory freed below
        npy_intp dims[] = {(npy_intp)lst->size};
        PyObject* value = PyArray_SimpleNew(1, dims, tp);
        PyObject* key = PyLong_FromUnsignedLong((unsigned long)label);
        if(value == NULL || key == NULL || PyArray_ITEMSIZE((PyArrayObject*)value) != (npy_intp)lst->step){
            Py_XDECREF(value);
            Py_XDECREF(key);
            Py_DECREF(clusterMap);
            hdbscan_destroy_cluster_map(tmp);
            if(!PyErr_Occurred()){
                PyErr_SetString(PyExc_RuntimeError, "Could not build the cluster map.");
            }
            return NULL;
        }
        memcpy(PyArray_DATA((PyArrayObject*)value), lst->data, lst->size * lst->step);

        int set = PyDict_SetItem(clusterMap, key, value);
        Py_DECREF(key);
        Py_DECREF(value);
        if(set < 0){
            Py_DECREF(clusterMap);
            hdbscan_destroy_cluster_map(tmp);
            return NULL;
        }
    }
    hdbscan_destroy_cluster_map(tmp);

    replace_member(&self->clusterMap, clusterMap);
    Py_INCREF(self->clusterMap);
    return self->clusterMap;
}

static PyObject* PyHdbscan_getHierarchies(PyHdbscan *self, PyObject *args) {
    if(self->scan == NULL || self->scan->hierarchy == NULL){
        PyErr_SetString(PyExc_RuntimeError, "run() must succeed before getHierarchies().");
        return NULL;
    }

    int tp = label_npy_type();
    hashtable* hierarchy = self->scan->hierarchy;
    PyObject* hierarchyDict = PyDict_New();
    if(hierarchyDict == NULL){
        return NULL;
    }

    for(size_t i = 0; i < hashtable_size(hierarchy); i++) {
        int64_t level;
	    hierarchy_entry* data = NULL;
		set_value_at(hierarchy->keys, i, &level);
        hashtable_lookup(hierarchy, &level, &data);
        if(data == NULL || data->labels == NULL){
            continue;
        }

        // Copy the labels so Python never holds pointers into memory freed by rerun()
        npy_intp dims[] = {(npy_intp)self->scan->numPoints};
        PyObject* value = PyArray_SimpleNew(1, dims, tp);
        PyObject* key = PyLong_FromLongLong((long long)level);
        if(value == NULL || key == NULL){
            Py_XDECREF(value);
            Py_XDECREF(key);
            Py_DECREF(hierarchyDict);
            return NULL;
        }
        memcpy(PyArray_DATA((PyArrayObject*)value), data->labels, (size_t)self->scan->numPoints * sizeof(label_t));

        int set = PyDict_SetItem(hierarchyDict, key, value);
        Py_DECREF(key);
        Py_DECREF(value);
        if(set < 0){
            Py_DECREF(hierarchyDict);
            return NULL;
        }
    }

    replace_member(&self->hierarchy, hierarchyDict);
    return Py_BuildValue("i", 1);
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
    {"labels", T_OBJECT_EX, offsetof(PyHdbscan, labels), READONLY, "Cluster labels"},
    {"clusterMap", T_OBJECT_EX, offsetof(PyHdbscan, clusterMap), READONLY, "Dictionary of the clusters and the points"},
    {"hierarchy", T_OBJECT_EX, offsetof(PyHdbscan, hierarchy), READONLY, "Dictionary of the hierarchies"},
    {"minPoints", T_UINT, offsetof(PyHdbscan, minPoints), READONLY, "Minimum number of point in a cluster"},
    {"rows", T_UINT, offsetof(PyHdbscan, rows), READONLY, "number of data points"},
    {"cols", T_UINT, offsetof(PyHdbscan, cols), READONLY, "The size of each data point"},
    {NULL}  /* Sentinel */
};

/**
 * @brief The PyTypeObject definition
 * 
 */
static PyTypeObject PyHdbscanType = {
   PyVarObject_HEAD_INIT(NULL, 0)
   "PyHdbscan.PyHdbscan",     /* tp_name */
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
