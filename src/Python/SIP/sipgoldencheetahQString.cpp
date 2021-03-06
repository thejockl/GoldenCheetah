/*
 * Interface wrapper code.
 *
 * Generated by SIP 4.19.8
 */

#include "sipAPIgoldencheetah.h"

#line 28 "goldencheetah.sip"
#include <qstring.h>
#line 12 "./sipgoldencheetahQString.cpp"



extern "C" {static void assign_QString(void *, SIP_SSIZE_T, void *);}
static void assign_QString(void *sipDst, SIP_SSIZE_T sipDstIdx, void *sipSrc)
{
    reinterpret_cast< ::QString *>(sipDst)[sipDstIdx] = *reinterpret_cast< ::QString *>(sipSrc);
}


extern "C" {static void *array_QString(SIP_SSIZE_T);}
static void *array_QString(SIP_SSIZE_T sipNrElem)
{
    return new  ::QString[sipNrElem];
}


extern "C" {static void *copy_QString(const void *, SIP_SSIZE_T);}
static void *copy_QString(const void *sipSrc, SIP_SSIZE_T sipSrcIdx)
{
    return new  ::QString(reinterpret_cast<const  ::QString *>(sipSrc)[sipSrcIdx]);
}


/* Call the mapped type's destructor. */
extern "C" {static void release_QString(void *, int);}
static void release_QString(void *ptr, int)
{
    delete reinterpret_cast< ::QString *>(ptr);
}



extern "C" {static int convertTo_QString(PyObject *, void **, int *, PyObject *);}
static int convertTo_QString(PyObject *sipPy,void **sipCppPtrV,int *sipIsErr,PyObject *)
{
     ::QString **sipCppPtr = reinterpret_cast< ::QString **>(sipCppPtrV);

#line 32 "goldencheetah.sip"
    if (sipIsErr == NULL) return PyUnicode_Check(sipPy);
    if (sipPy == Py_None) {
        *sipCppPtr = new QString();
        return 1;
    }
    if (PyUnicode_Check(sipPy)) {
        Py_ssize_t size;
        wchar_t *string = PyUnicode_AsWideCharString(sipPy, &size);
        if (string) {
            if (size) *sipCppPtr = new QString(QString::fromWCharArray(string));
            else *sipCppPtr = new QString();
        }
        return 1;
    }
    return 0;
#line 67 "./sipgoldencheetahQString.cpp"
}


extern "C" {static PyObject *convertFrom_QString(void *, PyObject *);}
static PyObject *convertFrom_QString(void *sipCppV, PyObject *)
{
    ::QString *sipCpp = reinterpret_cast< ::QString *>(sipCppV);

#line 49 "goldencheetah.sip"
    return PyUnicode_FromString(sipCpp->toUtf8().data());
#line 78 "./sipgoldencheetahQString.cpp"
}


sipMappedTypeDef sipTypeDef_goldencheetah_QString = {
    {
        -1,
        0,
        0,
        SIP_TYPE_ALLOW_NONE|SIP_TYPE_MAPPED,
        sipNameNr_QString,     /* QString */
        {0},
        0
    },
    {
        -1,
        {0, 0, 1},
        0, 0,
        0, 0,
        0, 0,
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
    },
    assign_QString,
    array_QString,
    copy_QString,
    release_QString,
    convertTo_QString,
    convertFrom_QString
};
