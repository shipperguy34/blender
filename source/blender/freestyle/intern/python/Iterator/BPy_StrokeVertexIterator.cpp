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

#include "BPy_StrokeVertexIterator.h"

#include "../BPy_Convert.h"
#include "../Interface1D/BPy_Stroke.h"
#include "BPy_Interface0DIterator.h"

#ifdef __cplusplus
extern "C" {
#endif

///////////////////////////////////////////////////////////////////////////////////////////

//------------------------INSTANCE METHODS ----------------------------------

PyDoc_STRVAR(StrokeVertexIterator_doc,
             "Class hierarchy: :class:`Iterator` > :class:`StrokeVertexIterator`\n"
             "\n"
             "Class defining an iterator designed to iterate over the\n"
             ":class:`StrokeVertex` of a :class:`Stroke`.  An instance of a\n"
             "StrokeVertexIterator can be obtained from a Stroke by calling\n"
             "iter(), stroke_vertices_begin() or stroke_vertices_begin().  It is iterating\n"
             "over the same vertices as an :class:`Interface0DIterator`.  The difference\n"
             "resides in the object access: an Interface0DIterator only allows\n"
             "access to an Interface0D while one might need to access the\n"
             "specialized StrokeVertex type.  In this case, one should use a\n"
             "StrokeVertexIterator.  To call functions of the UnaryFuntion0D type,\n"
             "a StrokeVertexIterator can be converted to an Interface0DIterator by\n"
             "by calling Interface0DIterator(it).\n"
             "\n"
             ".. method:: __init__()\n"
             "            __init__(brother)\n"
             "\n"
             "   Creates a :class:`StrokeVertexIterator` using either the\n"
             "   default constructor or the copy constructor.\n"
             "\n"
             "   :arg brother: A StrokeVertexIterator object.\n"
             "   :type brother: :class:`StrokeVertexIterator`");

static int StrokeVertexIterator_init(BPy_StrokeVertexIterator *self,
                                     PyObject *args,
                                     PyObject *kwds)
{
  static const char *kwlist_1[] = {"brother", nullptr};
  static const char *kwlist_2[] = {"stroke", nullptr};
  PyObject *brother = nullptr, *stroke = nullptr;

  if (PyArg_ParseTupleAndKeywords(
          args, kwds, "O!", (char **)kwlist_1, &StrokeVertexIterator_Type, &brother)) {
    self->sv_it = new StrokeInternal::StrokeVertexIterator(
        *(((BPy_StrokeVertexIterator *)brother)->sv_it));
    self->reversed = ((BPy_StrokeVertexIterator *)brother)->reversed;
    self->at_start = ((BPy_StrokeVertexIterator *)brother)->at_start;
  }

  else if ((void)PyErr_Clear(),
           PyArg_ParseTupleAndKeywords(
               args, kwds, "|O!", (char **)kwlist_2, &Stroke_Type, &stroke)) {
    if (!stroke) {
      self->sv_it = new StrokeInternal::StrokeVertexIterator();
    }
    else {
      self->sv_it = new StrokeInternal::StrokeVertexIterator(
          ((BPy_Stroke *)stroke)->s->strokeVerticesBegin());
    }
    self->reversed = false;
    self->at_start = true;
  }
  else {
    PyErr_SetString(PyExc_TypeError, "argument 1 must be StrokeVertexIterator or Stroke");
    return -1;
  }
  self->py_it.it = self->sv_it;
  return 0;
}

static PyObject *StrokeVertexIterator_iter(BPy_StrokeVertexIterator *self)
{
  Py_INCREF(self);
  self->at_start = true;
  return (PyObject *)self;
}

static PyObject *StrokeVertexIterator_iternext(BPy_StrokeVertexIterator *self)
{
  /* Because Freestyle iterators for which it.isEnd() holds true have no valid object
   * (they point to the past-the-end element and can't be dereferenced), we have to check
   * iterators for validity.
   * Additionally, the at_start attribute is used to keep Freestyle iterator objects
   * and Python for loops in sync. */

  if (self->reversed) {
    if (self->sv_it->isBegin()) {
      PyErr_SetNone(PyExc_StopIteration);
      return nullptr;
    }
    self->sv_it->decrement();
  }
  else {
    /* If sv_it.isEnd() is true, the iterator can't be incremented. */
    if (self->sv_it->isEnd()) {
      PyErr_SetNone(PyExc_StopIteration);
      return nullptr;
    }
    /* If at the start of the iterator, only return the object
     * and don't increment, to keep for-loops in sync */
    if (self->at_start) {
      self->at_start = false;
    }
    /* If sv_it.atLast() is true, the iterator is currently pointing to the final valid element.
     * Incrementing it further would lead to a state that the iterator can't be dereferenced. */
    else if (self->sv_it->atLast()) {
      PyErr_SetNone(PyExc_StopIteration);
      return nullptr;
    }
    else {
      self->sv_it->increment();
    }
  }
  StrokeVertex *sv = self->sv_it->operator->();
  return BPy_StrokeVertex_from_StrokeVertex(*sv);
}

/*----------------------StrokeVertexIterator methods ----------------------------*/

PyDoc_STRVAR(StrokeVertexIterator_incremented_doc,
             ".. method:: incremented()\n"
             "\n"
             "   Returns a copy of an incremented StrokeVertexIterator.\n"
             "\n"
             "   :return: A StrokeVertexIterator pointing the next StrokeVertex.\n"
             "   :rtype: :class:`StrokeVertexIterator`");

static PyObject *StrokeVertexIterator_incremented(BPy_StrokeVertexIterator *self)
{
  if (self->sv_it->isEnd()) {
    PyErr_SetString(PyExc_RuntimeError, "cannot increment any more");
    return nullptr;
  }
  StrokeInternal::StrokeVertexIterator copy(*self->sv_it);
  copy.increment();
  return BPy_StrokeVertexIterator_from_StrokeVertexIterator(copy, self->reversed);
}

PyDoc_STRVAR(StrokeVertexIterator_decremented_doc,
             ".. method:: decremented()\n"
             "\n"
             "   Returns a copy of a decremented StrokeVertexIterator.\n"
             "\n"
             "   :return: A StrokeVertexIterator pointing the previous StrokeVertex.\n"
             "   :rtype: :class:`StrokeVertexIterator`");

static PyObject *StrokeVertexIterator_decremented(BPy_StrokeVertexIterator *self)
{
  if (self->sv_it->isBegin()) {
    PyErr_SetString(PyExc_RuntimeError, "cannot decrement any more");
    return nullptr;
  }
  StrokeInternal::StrokeVertexIterator copy(*self->sv_it);
  copy.decrement();
  return BPy_StrokeVertexIterator_from_StrokeVertexIterator(copy, self->reversed);
}

PyDoc_STRVAR(StrokeVertexIterator_reversed_doc,
             ".. method:: reversed()\n"
             "\n"
             "   Returns a StrokeVertexIterator that traverses stroke vertices in the\n"
             "   reversed order.\n"
             "\n"
             "   :return: A StrokeVertexIterator traversing stroke vertices backward.\n"
             "   :rtype: :class:`StrokeVertexIterator`");

static PyObject *StrokeVertexIterator_reversed(BPy_StrokeVertexIterator *self)
{
  return BPy_StrokeVertexIterator_from_StrokeVertexIterator(*self->sv_it, !self->reversed);
}

static PyMethodDef BPy_StrokeVertexIterator_methods[] = {
    {"incremented",
     (PyCFunction)StrokeVertexIterator_incremented,
     METH_NOARGS,
     StrokeVertexIterator_incremented_doc},
    {"decremented",
     (PyCFunction)StrokeVertexIterator_decremented,
     METH_NOARGS,
     StrokeVertexIterator_decremented_doc},
    {"reversed",
     (PyCFunction)StrokeVertexIterator_reversed,
     METH_NOARGS,
     StrokeVertexIterator_reversed_doc},
    {nullptr, nullptr, 0, nullptr},
};

/*----------------------StrokeVertexIterator get/setters ----------------------------*/

PyDoc_STRVAR(StrokeVertexIterator_object_doc,
             "The StrokeVertex object currently pointed to by this iterator.\n"
             "\n"
             ":type: :class:`StrokeVertex`");

static PyObject *StrokeVertexIterator_object_get(BPy_StrokeVertexIterator *self,
                                                 void *UNUSED(closure))
{
  if (self->sv_it->isEnd()) {
    PyErr_SetString(PyExc_RuntimeError, "iteration has stopped");
    return nullptr;
  }
  StrokeVertex *sv = self->sv_it->operator->();
  if (sv) {
    return BPy_StrokeVertex_from_StrokeVertex(*sv);
  }
  Py_RETURN_NONE;
}

PyDoc_STRVAR(StrokeVertexIterator_t_doc,
             "The curvilinear abscissa of the current point.\n"
             "\n"
             ":type: float");

static PyObject *StrokeVertexIterator_t_get(BPy_StrokeVertexIterator *self, void *UNUSED(closure))
{
  return PyFloat_FromDouble(self->sv_it->t());
}

PyDoc_STRVAR(StrokeVertexIterator_u_doc,
             "The point parameter at the current point in the stroke (0 <= u <= 1).\n"
             "\n"
             ":type: float");

static PyObject *StrokeVertexIterator_u_get(BPy_StrokeVertexIterator *self, void *UNUSED(closure))
{
  return PyFloat_FromDouble(self->sv_it->u());
}

PyDoc_STRVAR(StrokeVertexIterator_at_last_doc,
             "True if the iterator points to the last valid element.\n"
             "For its counterpart (pointing to the first valid element), use it.is_begin.\n"
             "\n"
             ":type: bool");

static PyObject *StrokeVertexIterator_at_last_get(BPy_StrokeVertexIterator *self)
{
  return PyBool_from_bool(self->sv_it->atLast());
}

static PyGetSetDef BPy_StrokeVertexIterator_getseters[] = {
    {"object",
     (getter)StrokeVertexIterator_object_get,
     (setter) nullptr,
     StrokeVertexIterator_object_doc,
     nullptr},
    {"t",
     (getter)StrokeVertexIterator_t_get,
     (setter) nullptr,
     StrokeVertexIterator_t_doc,
     nullptr},
    {"u",
     (getter)StrokeVertexIterator_u_get,
     (setter) nullptr,
     StrokeVertexIterator_u_doc,
     nullptr},
    {"at_last",
     (getter)StrokeVertexIterator_at_last_get,
     (setter) nullptr,
     StrokeVertexIterator_at_last_doc,
     nullptr},
    {nullptr, nullptr, nullptr, nullptr, nullptr} /* Sentinel */
};

/*-----------------------BPy_StrokeVertexIterator type definition ------------------------------*/

PyTypeObject StrokeVertexIterator_Type = {
    PyVarObject_HEAD_INIT(nullptr, 0) "StrokeVertexIterator", /* tp_name */
    sizeof(BPy_StrokeVertexIterator),                         /* tp_basicsize */
    0,                                                        /* tp_itemsize */
    nullptr,                                                  /* tp_dealloc */
    /* Incompatible with Python3.8+ (deprecated function).
     * NOLINTNEXTLINE: modernize-use-nullptr. */
    0,                                                        /* tp_print */
    nullptr,                                                  /* tp_getattr */
    nullptr,                                                  /* tp_setattr */
    nullptr,                                                  /* tp_reserved */
    nullptr,                                                  /* tp_repr */
    nullptr,                                                  /* tp_as_number */
    nullptr,                                                  /* tp_as_sequence */
    nullptr,                                                  /* tp_as_mapping */
    nullptr,                                                  /* tp_hash  */
    nullptr,                                                  /* tp_call */
    nullptr,                                                  /* tp_str */
    nullptr,                                                  /* tp_getattro */
    nullptr,                                                  /* tp_setattro */
    nullptr,                                                  /* tp_as_buffer */
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,                 /* tp_flags */
    StrokeVertexIterator_doc,                                 /* tp_doc */
    nullptr,                                                  /* tp_traverse */
    nullptr,                                                  /* tp_clear */
    nullptr,                                                  /* tp_richcompare */
    0,                                                        /* tp_weaklistoffset */
    (getiterfunc)StrokeVertexIterator_iter,                   /* tp_iter */
    (iternextfunc)StrokeVertexIterator_iternext,              /* tp_iternext */
    BPy_StrokeVertexIterator_methods,                         /* tp_methods */
    nullptr,                                                  /* tp_members */
    BPy_StrokeVertexIterator_getseters,                       /* tp_getset */
    &Iterator_Type,                                           /* tp_base */
    nullptr,                                                  /* tp_dict */
    nullptr,                                                  /* tp_descr_get */
    nullptr,                                                  /* tp_descr_set */
    0,                                                        /* tp_dictoffset */
    (initproc)StrokeVertexIterator_init,                      /* tp_init */
    nullptr,                                                  /* tp_alloc */
    nullptr,                                                  /* tp_new */
};

///////////////////////////////////////////////////////////////////////////////////////////

#ifdef __cplusplus
}
#endif
