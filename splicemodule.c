/*
 * py-splice
 *
 * A Python module interface to splice(2)
 *
 * move data bw two file descriptors without copying bw
 * kernal address space and user address space.
 *
 * It transfers up to len bytes of data from the file descriptor fd_in 
 * to the file descriptor fd_out, where one of the file descriptors must
 * refer to a pipe.
 *
 * Author:
 *      2018 Danish Prakash <danishprakash <at> outlook <dot> com>
 *
 * License:
 *      GNU General Public License
 */

#include "Python.h"
#include "structmember.h"
#include <fcntl.h>
#include <errno.h>
#include <string.h>

/* function declarations */
PyMODINIT_FUNC PyInit_splice();


static PyObject * method_splice(PyObject *self, PyObject *args) {
    int fd_in;
    int fd_out;
    size_t len;
    unsigned int flags = 0;
    ssize_t ret;

    if (!PyArg_ParseTuple(args, "iin|I:splice", &fd_in, &fd_out, &len, &flags)) {
        return NULL;
    }
    Py_BEGIN_ALLOW_THREADS
    while (len) {
        ret = splice(fd_in, NULL, fd_out, NULL, len, flags);
        if (ret == -1) {
            PyErr_Format(PyExc_OSError, "splice error: %s", strerror(errno));
            return NULL;
        }
        len -= ret;
    }
    Py_END_ALLOW_THREADS
    Py_RETURN_NONE;
}

/* structure used to describe method 
 * accessible in Python */ 
static PyMethodDef SpliceMethods[] = {
    {
        "splice", /* name of method accessible in Python */
        (PyCFunction)method_splice, /* pointer to the actual function */
        METH_VARARGS, /* flag bits indicating calling convention */
        "Python interace for splice(2) system call." /* docstring content */
    },
    {NULL, NULL, 0, NULL}
};

/* structure that holds information needed
 * to create a module object */ 
static struct PyModuleDef splicemodule = {
    PyModuleDef_HEAD_INIT,
    "splice", /* name of module */
    NULL, /* docstring for module */
    -1, /* no support for sub-interpretors */
    SpliceMethods /* pointer to table of module-level functions */
};

/* specifies PyObject * as return type,
 * declares any special linkage declarations
 * required by the platform */
PyMODINIT_FUNC
PyInit_splice(void)
{
    PyObject *module;
    module = PyModule_Create(&splicemodule);

/* define constants for splice(2) flags */
#ifdef SPLICE_F_MOVE
    PyModule_AddIntConstant(module, "SPLICE_F_MOVE", SPLICE_F_MOVE);
#endif
#ifdef SPLICE_F_MORE
    PyModule_AddIntConstant(module, "SPLICE_F_MORE", SPLICE_F_MORE);
#endif
#ifdef SPLICE_F_GIFT
    PyModule_AddIntConstant(module, "SPLICE_F_GIFT", SPLICE_F_GIFT);
#endif    
#ifdef SPLICE_F_NONBLOCK
    PyModule_AddIntConstant(module, "SPLICE_F_NONBLOCK", SPLICE_F_NONBLOCK);
#endif

    return module;
}
