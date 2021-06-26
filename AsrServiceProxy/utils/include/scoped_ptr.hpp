// Scopers help you manage ownership of a pointer, helping you easily manage the
// a pointer within a scope, and automatically destroying the pointer at the
// end of a scope.  There are two main classes you will use, which correspond
// to the operators new/delete and new[]/delete[].
//
// Example usage (ScopedPtr):
//   {
//     ScopedPtr<Foo> foo(new Foo("wee"));
//   }  // foo goes out of scope, releasing the pointer with it.
//
//   {
//     ScopedPtr<Foo> foo;          // No pointer managed.
//     foo.reset(new Foo("wee"));    // Now a pointer is managed.
//     foo.reset(new Foo("wee2"));   // Foo("wee") was destroyed.
//     foo.reset(new Foo("wee3"));   // Foo("wee2") was destroyed.
//     foo->Method();                // Foo::Method() called.
//     foo.get()->Method();          // Foo::Method() called.
//     SomeFunc(foo.release());      // SomeFunc takes ownership, foo no longer
//                                   // manages a pointer.
//     foo.reset(new Foo("wee4"));   // foo manages a pointer again.
//     foo.reset();                  // Foo("wee4") destroyed, foo no longer
//                                   // manages a pointer.
//   }  // foo wasn't managing a pointer, so nothing was destroyed.
//
// Example usage (ScopedArray):
//   {
//     ScopedArray<Foo> foo(new Foo[100]);
//     foo.get()->Method();  // Foo::Method on the 0th element.
//     foo[10].Method();     // Foo::Method on the 10th element.
//   }

#ifndef UTILS_ScopedPtr_HPP
#define UTILS_ScopedPtr_HPP

// This is an implementation designed to match the anticipated future TR2
// implementation of the ScopedPtr class, and its closely-related brethren,
// ScopedArray, ScopedPtrMalloc.

#include "aip_common.h"

// A ScopedPtr<T> is like a T*, except that the destructor of ScopedPtr<T>
// automatically deletes the pointer it holds (if any).
// That is, ScopedPtr<T> owns the T object that it points to.
// Like a T*, a ScopedPtr<T> may hold either NULL or a pointer to a T object.
// Also like T*, ScopedPtr<T> is thread-compatible, and once you
// dereference it, you get the threadsafety guarantees of T.
//
// The size of a ScopedPtr is small:
// sizeof(ScopedPtr<C>) == sizeof(C*)

template <class C>
class ScopedPtr { 
public:

    // The element type
    typedef C element_type;

    // Constructor.  Defaults to initializing with NULL.
    // There is no way to create an uninitialized ScopedPtr.
    // The input parameter must be allocated with new.
    explicit ScopedPtr(C* p = NULL) : _ptr(p) { }

    // Destructor.  If there is a C object, delete it.
    // We don't need to test _ptr == NULL because C++ does that for us.
    ~ScopedPtr() {
        enum { TYPE_MUST_BE_COMPLETE = sizeof(C) };
        delete _ptr;
    }

    // Reset.  Deletes the current owned object, if any.
    // Then takes ownership of a new object, if given.
    // this->reset(this->get()) works.
    void reset(C* p = NULL) {
        if (p != _ptr) {
            enum { TYPE_MUST_BE_COMPLETE = sizeof(C) };
            delete _ptr;
            _ptr = p;
        }
    }

    // Accessors to get the owned object.
    // operator* and operator-> will assert() if there is no current object.
    C& operator*() const {
        assert(_ptr != NULL);
        return *_ptr;
    }
    C* operator->() const {
        assert(_ptr != NULL);
        return _ptr;
    }
    C* get() const {
        return _ptr;
    }

    // Comparison operators.
    // These return whether two ScopedPtr refer to the same object, not just to
    // two different but equal objects.
    bool operator==(C* p) const {
        return _ptr == p;
    }
    bool operator!=(C* p) const {
        return _ptr != p;
    }

    // Swap two scoped pointers.
    void swap(ScopedPtr& p2) {
        C* tmp = _ptr;
        _ptr = p2._ptr;
        p2._ptr = tmp;
    }

    // Release a pointer.
    // The return value is the current pointer held by this object.
    // If this object holds a NULL pointer, the return value is NULL.
    // After this operation, this object will hold a NULL pointer,
    // and will not own the object any more.
    C* release() {
        C* ret_val = _ptr;
        _ptr = NULL;
        return ret_val;
    }

private:
    C* _ptr;

    // Forbid comparison of ScopedPtr types.  If C2 != C, it totally doesn't
    // make sense, and if C2 == C, it still doesn't make sense because you should
    // never have the same object owned by two different ScopedPtrs.
    template <class C2> bool operator==(ScopedPtr<C2> const& p2) const;
    template <class C2> bool operator!=(ScopedPtr<C2> const& p2) const;

    // Disallow evil constructors
    ScopedPtr(const ScopedPtr&);
    void operator=(const ScopedPtr&);
};

// Free functions
template <class C>
void swap(ScopedPtr<C>& p1, ScopedPtr<C>& p2) {
    p1.swap(p2);
}

template <class C>
bool operator==(C* p1, const ScopedPtr<C>& p2) {
    return p1 == p2.get();
}

template <class C>
bool operator!=(C* p1, const ScopedPtr<C>& p2) {
    return p1 != p2.get();
}

// ScopedArray<C> is like ScopedPtr<C>, except that the caller must allocate
// with new [] and the destructor deletes objects with delete [].
//
// As with ScopedPtr<C>, a ScopedArray<C> either points to an object
// or is NULL.  A ScopedArray<C> owns the object that it points to.
// ScopedArray<T> is thread-compatible, and once you index into it,
// the returned objects have only the threadsafety guarantees of T.
//
// Size: sizeof(ScopedArray<C>) == sizeof(C*)
template <class C>
class ScopedArray {
public:

    // The element type
    typedef C element_type;

    // Constructor.  Defaults to intializing with NULL.
    // There is no way to create an uninitialized ScopedArray.
    // The input parameter must be allocated with new [].
    explicit ScopedArray(C* p = NULL) : _array(p) { }

    // Destructor.  If there is a C object, delete it.
    // We don't need to test _ptr == NULL because C++ does that for us.
    ~ScopedArray() {
        enum { TYPE_MUST_BE_COMPLETE = sizeof(C) };
        delete[] _array;
    }

    // Reset.  Deletes the current owned object, if any.
    // Then takes ownership of a new object, if given.
    // this->reset(this->get()) works.
    void reset(C* p = NULL) {
        if (p != _array) {
            enum { TYPE_MUST_BE_COMPLETE = sizeof(C) };
            delete[] _array;
            _array = p;
        }
    }

    // Get one element of the current object.
    // Will assert() if there is no current object, or index i is negative.
    C& operator[](std::ptrdiff_t i) const {
        assert(i >= 0);
        assert(_array != NULL);
        return _array[i];
    }

    // Get a pointer to the zeroth element of the current object.
    // If there is no current object, return NULL.
    C* get() const {
        return _array;
    }

    // Comparison operators.
    // These return whether two ScopedArray refer to the same object, not just to
    // two different but equal objects.
    bool operator==(C* p) const {
        return _array == p;
    }
    bool operator!=(C* p) const {
        return _array != p;
    }

    // Swap two scoped arrays.
    void swap(ScopedArray& p2) {
        C* tmp = _array;
        _array = p2._array;
        p2._array = tmp;
    }

    // Release an array.
    // The return value is the current pointer held by this object.
    // If this object holds a NULL pointer, the return value is NULL.
    // After this operation, this object will hold a NULL pointer,
    // and will not own the object any more.
    C* release() {
        C* ret_val = _array;
        _array = NULL;
        return ret_val;
    }

private:
    C* _array;

    // Forbid comparison of different ScopedArray types.
    template <class C2> bool operator==(ScopedArray<C2> const& p2) const;
    template <class C2> bool operator!=(ScopedArray<C2> const& p2) const;

    // Disallow evil constructors
    ScopedArray(const ScopedArray&);
    void operator=(const ScopedArray&);
};

// Free functions
template <class C>
void swap(ScopedArray<C>& p1, ScopedArray<C>& p2) {
    p1.swap(p2);
}

template <class C>
bool operator==(C* p1, const ScopedArray<C>& p2) {
    return p1 == p2.get();
}

template <class C>
bool operator!=(C* p1, const ScopedArray<C>& p2) {
    return p1 != p2.get();
}

// This class wraps the c library function free() in a class that can be
// passed as a template argument to ScopedPtrMalloc below.
class ScopedPtrMallocFree {
public:
    inline void operator()(void* x) const {
        free(x);
    }
};

// ScopedPtrMalloc<> is similar to ScopedPtr<>, but it accepts a
// second template argument, the functor used to free the object.

template<class C, class FreeProc = ScopedPtrMallocFree>
class ScopedPtrMalloc {  //
public:
    // Constructor.  Defaults to initializing with NULL.
    // There is no way to create an uninitialized ScopedPtr.
    // The input parameter must be allocated with an allocator that matches the
    // Free functor.  For the default Free functor, this is malloc, calloc, or
    // realloc.
    explicit ScopedPtrMalloc(C* p = NULL): _ptr(p) {}

    // Destructor.  If there is a C object, call the Free functor.
    ~ScopedPtrMalloc() {
        free_(_ptr);
    }

    // Reset.  Calls the Free functor on the current owned object, if any.
    // Then takes ownership of a new object, if given.
    // this->reset(this->get()) works.
    void reset(C* p = NULL) {
        if (_ptr != p) {
            free_(_ptr);
            _ptr = p;
        }
    }

    // Get the current object.
    // operator* and operator-> will cause an assert() failure if there is
    // no current object.
    C& operator*() const {
        assert(_ptr != NULL);
        return *_ptr;
    }

    C* operator->() const {
        assert(_ptr != NULL);
        return _ptr;
    }

    C* get() const {
        return _ptr;
    }

    // Comparison operators.
    // These return whether a ScopedPtrMalloc and a plain pointer refer
    // to the same object, not just to two different but equal objects.
    // For compatibility with the boost-derived implementation, these
    // take non-const arguments.
    bool operator==(C* p) const {
        return _ptr == p;
    }

    bool operator!=(C* p) const {
        return _ptr != p;
    }

    // Swap two scoped pointers.
    void swap(ScopedPtrMalloc & b) {
        C* tmp = b._ptr;
        b._ptr = _ptr;
        _ptr = tmp;
    }

    // Release a pointer.
    // The return value is the current pointer held by this object.
    // If this object holds a NULL pointer, the return value is NULL.
    // After this operation, this object will hold a NULL pointer,
    // and will not own the object any more.
    C* release() {
        C* tmp = _ptr;
        _ptr = NULL;
        return tmp;
    }

private:
    C* _ptr;

    // no reason to use these: each ScopedPtrMalloc should have its own object
    template <class C2, class GP>
    bool operator==(ScopedPtrMalloc<C2, GP> const& p) const;
    template <class C2, class GP>
    bool operator!=(ScopedPtrMalloc<C2, GP> const& p) const;

    static FreeProc const free_;

    // Disallow evil constructors
    ScopedPtrMalloc(const ScopedPtrMalloc&);
    void operator=(const ScopedPtrMalloc&);
};

template<class C, class FP>
FP const ScopedPtrMalloc<C, FP>::free_ = FP();

template<class C, class FP>
inline void swap(ScopedPtrMalloc<C, FP>& a, ScopedPtrMalloc<C, FP>& b) {
    a.swap(b);
}

template<class C, class FP>
inline bool operator==(C* p, const ScopedPtrMalloc<C, FP>& b) {
    return p == b.get();
}

template<class C, class FP>
inline bool operator!=(C* p, const ScopedPtrMalloc<C, FP>& b) {
    return p != b.get();
}

#endif  // UTILS_ScopedPtr_HPP
