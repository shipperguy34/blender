/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

/** \file
 * \ingroup freestyle
 */

#include "BPy_TVertex.h"

#include "../../BPy_Convert.h"
#include "../../BPy_Id.h"
#include "../../Interface1D/BPy_FEdge.h"
#include "../../Interface1D/BPy_ViewEdge.h"
#include "../BPy_SVertex.h"

#ifdef __cplusplus
extern "C" {
#endif

///////////////////////////////////////////////////////////////////////////////////////////

/*----------------------TVertex methods ----------------------------*/

PyDoc_STRVAR(TVertex_doc,
             "Class hierarchy: :class:`Interface0D` > :class:`ViewVertex` > :class:`TVertex`\n"
             "\n"
             "Class to define a T vertex, i.e. an intersection between two edges.\n"
             "It points towards two SVertex and four ViewEdges.  Among the\n"
             "ViewEdges, two are front and the other two are back.  Basically a\n"
             "front edge hides part of a back edge.  So, among the back edges, one\n"
             "is of invisibility N and the other of invisibility N+1.\n"
             "\n"
             ".. method:: __init__()\n"
             "\n"
             "   Default constructor.");

/* Note: No copy constructor in Python because the C++ copy constructor is 'protected'. */

static int TVertex_init(BPy_TVertex *self, PyObject *args, PyObject *kwds)
{
  static const char *kwlist[] = {nullptr};

  if (!PyArg_ParseTupleAndKeywords(args, kwds, "", (char **)kwlist)) {
    return -1;
  }
  self->tv = new TVertex();
  self->py_vv.vv = self->tv;
  self->py_vv.py_if0D.if0D = self->tv;
  self->py_vv.py_if0D.borrowed = false;
  return 0;
}

PyDoc_STRVAR(TVertex_get_svertex_doc,
             ".. method:: get_svertex(fedge)\n"
             "\n"
             "   Returns the SVertex (among the 2) belonging to the given FEdge.\n"
             "\n"
             "   :arg fedge: An FEdge object.\n"
             "   :type fedge: :class:`FEdge`\n"
             "   :return: The SVertex belonging to the given FEdge.\n"
             "   :rtype: :class:`SVertex`");

static PyObject *TVertex_get_svertex(BPy_TVertex *self, PyObject *args, PyObject *kwds)
{
  static const char *kwlist[] = {"fedge", nullptr};
  PyObject *py_fe;

  if (!PyArg_ParseTupleAndKeywords(args, kwds, "O!", (char **)kwlist, &FEdge_Type, &py_fe)) {
    return nullptr;
  }
  SVertex *sv = self->tv->getSVertex(((BPy_FEdge *)py_fe)->fe);
  if (sv) {
    return BPy_SVertex_from_SVertex(*sv);
  }
  Py_RETURN_NONE;
}

PyDoc_STRVAR(TVertex_get_mate_doc,
             ".. method:: get_mate(viewedge)\n"
             "\n"
             "   Returns the mate edge of the ViewEdge given as argument.  If the\n"
             "   ViewEdge is frontEdgeA, frontEdgeB is returned.  If the ViewEdge is\n"
             "   frontEdgeB, frontEdgeA is returned.  Same for back edges.\n"
             "\n"
             "   :arg viewedge: A ViewEdge object.\n"
             "   :type viewedge: :class:`ViewEdge`\n"
             "   :return: The mate edge of the given ViewEdge.\n"
             "   :rtype: :class:`ViewEdge`");

static PyObject *TVertex_get_mate(BPy_TVertex *self, PyObject *args, PyObject *kwds)
{
  static const char *kwlist[] = {"viewedge", nullptr};
  PyObject *py_ve;

  if (!PyArg_ParseTupleAndKeywords(args, kwds, "O!", (char **)kwlist, &ViewEdge_Type, &py_ve)) {
    return nullptr;
  }
  ViewEdge *ve = self->tv->mate(((BPy_ViewEdge *)py_ve)->ve);
  if (ve) {
    return BPy_ViewEdge_from_ViewEdge(*ve);
  }
  Py_RETURN_NONE;
}

static PyMethodDef BPy_TVertex_methods[] = {
    {"get_svertex",
     (PyCFunction)TVertex_get_svertex,
     METH_VARARGS | METH_KEYWORDS,
     TVertex_get_svertex_doc},
    {"get_mate",
     (PyCFunction)TVertex_get_mate,
     METH_VARARGS | METH_KEYWORDS,
     TVertex_get_mate_doc},
    {nullptr, nullptr, 0, nullptr},
};

/*----------------------TVertex get/setters ----------------------------*/

PyDoc_STRVAR(TVertex_front_svertex_doc,
             "The SVertex that is closer to the viewpoint.\n"
             "\n"
             ":type: :class:`SVertex`");

static PyObject *TVertex_front_svertex_get(BPy_TVertex *self, void *UNUSED(closure))
{
  SVertex *v = self->tv->frontSVertex();
  if (v) {
    return BPy_SVertex_from_SVertex(*v);
  }
  Py_RETURN_NONE;
}

static int TVertex_front_svertex_set(BPy_TVertex *self, PyObject *value, void *UNUSED(closure))
{
  if (!BPy_SVertex_Check(value)) {
    PyErr_SetString(PyExc_TypeError, "value must be an SVertex");
    return -1;
  }
  self->tv->setFrontSVertex(((BPy_SVertex *)value)->sv);
  return 0;
}

PyDoc_STRVAR(TVertex_back_svertex_doc,
             "The SVertex that is further away from the viewpoint.\n"
             "\n"
             ":type: :class:`SVertex`");

static PyObject *TVertex_back_svertex_get(BPy_TVertex *self, void *UNUSED(closure))
{
  SVertex *v = self->tv->backSVertex();
  if (v) {
    return BPy_SVertex_from_SVertex(*v);
  }
  Py_RETURN_NONE;
}

static int TVertex_back_svertex_set(BPy_TVertex *self, PyObject *value, void *UNUSED(closure))
{
  if (!BPy_SVertex_Check(value)) {
    PyErr_SetString(PyExc_TypeError, "value must be an SVertex");
    return -1;
  }
  self->tv->setBackSVertex(((BPy_SVertex *)value)->sv);
  return 0;
}

PyDoc_STRVAR(TVertex_id_doc,
             "The Id of this TVertex.\n"
             "\n"
             ":type: :class:`Id`");

static PyObject *TVertex_id_get(BPy_TVertex *self, void *UNUSED(closure))
{
  Id id(self->tv->getId());
  return BPy_Id_from_Id(id);  // return a copy
}

static int TVertex_id_set(BPy_TVertex *self, PyObject *value, void *UNUSED(closure))
{
  if (!BPy_Id_Check(value)) {
    PyErr_SetString(PyExc_TypeError, "value must be an Id");
    return -1;
  }
  self->tv->setId(*(((BPy_Id *)value)->id));
  return 0;
}

static PyGetSetDef BPy_TVertex_getseters[] = {
    {"front_svertex",
     (getter)TVertex_front_svertex_get,
     (setter)TVertex_front_svertex_set,
     TVertex_front_svertex_doc,
     nullptr},
    {"back_svertex",
     (getter)TVertex_back_svertex_get,
     (setter)TVertex_back_svertex_set,
     TVertex_back_svertex_doc,
     nullptr},
    {"id", (getter)TVertex_id_get, (setter)TVertex_id_set, TVertex_id_doc, nullptr},
    {nullptr, nullptr, nullptr, nullptr, nullptr} /* Sentinel */
};

/*-----------------------BPy_TVertex type definition ------------------------------*/
PyTypeObject TVertex_Type = {
    PyVarObject_HEAD_INIT(nullptr, 0) "TVertex", /* tp_name */
    sizeof(BPy_TVertex),                         /* tp_basicsize */
    0,                                           /* tp_itemsize */
    nullptr,                                     /* tp_dealloc */
    /* Incompatible with Python3.8+ (deprecated function).
     * NOLINTNEXTLINE: modernize-use-nullptr. */
    0,                                           /* tp_print */
    nullptr,                                     /* tp_getattr */
    nullptr,                                     /* tp_setattr */
    nullptr,                                     /* tp_reserved */
    nullptr,                                     /* tp_repr */
    nullptr,                                     /* tp_as_number */
    nullptr,                                     /* tp_as_sequence */
    nullptr,                                     /* tp_as_mapping */
    nullptr,                                     /* tp_hash  */
    nullptr,                                     /* tp_call */
    nullptr,                                     /* tp_str */
    nullptr,                                     /* tp_getattro */
    nullptr,                                     /* tp_setattro */
    nullptr,                                     /* tp_as_buffer */
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,    /* tp_flags */
    TVertex_doc,                                 /* tp_doc */
    nullptr,                                     /* tp_traverse */
    nullptr,                                     /* tp_clear */
    nullptr,                                     /* tp_richcompare */
    0,                                           /* tp_weaklistoffset */
    nullptr,                                     /* tp_iter */
    nullptr,                                     /* tp_iternext */
    BPy_TVertex_methods,                         /* tp_methods */
    nullptr,                                     /* tp_members */
    BPy_TVertex_getseters,                       /* tp_getset */
    &ViewVertex_Type,                            /* tp_base */
    nullptr,                                     /* tp_dict */
    nullptr,                                     /* tp_descr_get */
    nullptr,                                     /* tp_descr_set */
    0,                                           /* tp_dictoffset */
    (initproc)TVertex_init,                      /* tp_init */
    nullptr,                                     /* tp_alloc */
    nullptr,                                     /* tp_new */
};

///////////////////////////////////////////////////////////////////////////////////////////

#ifdef __cplusplus
}
#endif
