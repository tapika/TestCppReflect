#pragma once

//
// If you're using multithreading, please make sure that two threads are not accessing 
// SmartPtr<> pointers which are cross linked.
//
template <class T>
class SmartPtr
{
public:
    SmartPtr() : ptr( nullptr ), next( nullptr )
    {
    }

    SmartPtr( T* pt ) : ptr( pt ), next( nullptr )
    {
    }

    //
    //  Certain std containers require const, but we don't need one - in implementation wise we don't care, since it just works.
    //
    SmartPtr( const SmartPtr<T>& sp ) : ptr( nullptr ), next( nullptr )
    {
        operator=(sp);
    }

    ~SmartPtr()
    {
        release();
    }

    // Reference to pointer - assumed to be filled out by user.
    T*& refptr()
    {
        release();
        return ptr;
    }

    T& operator*()
    {
        return *ptr;
    }

    // Pointer itself, assumed to be used.
    T* get()
    {
        return ptr;
    }

    T* operator->() const
    {
        return ptr;
    }

    /*
    // Don't override this, since STL container can copy pointers without linking pointer ownership.
    operator T*()
    {
        return ptr;
    }
    */

    T* operator=( T* _ptr )
    {
        release();
        ptr = _ptr;
        return ptr;
    }

    //
    //  const is required (operator=, copy ctor) for some template containers like VidLib::Array
    //
    SmartPtr<T>& operator=( const SmartPtr<T>& sp )
    {
        release();
        ptr = sp.ptr;

        if ( ptr )      // If we have valid pointer, share ownership.
        {

            if( sp.next == nullptr )
            {
                next = &sp;
                sp.next = this;
            } else {
                const SmartPtr<T>* it = &sp;

                while( it->next != &sp )
                    it = it->next;

                next = &sp;
                it->next = this;
            }
        }

        return *this;
    }

    void release()
    {
        if ( !ptr )
            return;

        if( _releaseOwnership() )
            // Single user.
            delete ptr;

        ptr = nullptr;
    }

    //
    //  Does not perform ptr deletion, only releases ptr from this class ownership.
    //  Returns allocation back to user.
    //
    T* detach()
    {
        _releaseOwnership();
        T* ret = ptr;
        ptr = nullptr;
        return ret;
    }

    bool _releaseOwnership()
    {
        // Shared ownership.
        if( next != nullptr )
        {
            // Remove myself from shared pointer list.
            const SmartPtr<T>* it = next;

            while( it->next != this )
                it = it->next;

            if( it == it->next->next )
                it->next = nullptr;
            else
                it->next = next;

            next = nullptr;
            ptr = nullptr;
            return false;
        }

        return true;
    }

    mutable T* ptr;                     // pointer to object
    mutable const SmartPtr<T>* next;    // nullptr if pointer is not shared with anyone, 
                                        // otherwise cyclic linked list of all SmartPtr referencing that pointer.
};

#ifdef TEST_SMARTPTR
#include <stdio.h>              //printf

class One
{
public:
    One()
    {
        printf( "construct One\n" );
    }

    ~One()
    {
        printf( "destruct One\n" );
    }
};

void testX( One*& pp )
{
    pp = nullptr;
}

void TestSmartPtr( void )
{
    printf( "Test 1:\n" );
    {
        SmartPtr<One> pOne( new One );
        SmartPtr<One> pTwo = pOne;
        SmartPtr<One> pThree = pTwo;
    }

    printf( "Test 2:\n" );
    {
        SmartPtr<One> pOne( new One );
    }

    printf( "Test 3:\n" );
    {
        SmartPtr<One> pOne( new One );
    }

    printf( "Test 4:\n" );
    {
        SmartPtr<One> pOne( new One );
        SmartPtr<One> pTwo( pOne );
    }

    printf( "Test 5:\n" );
    {
        SmartPtr<One> pOne( new One );
        SmartPtr<One> pTwo( pOne );

        pOne = new One;
    }

    printf( "Test 6:\n" );
    {
        SmartPtr<One> pOne( new One );
        SmartPtr<One> pTwo( pOne );

        testX( pTwo.refptr() );
    }

    printf( "Test 8:\n" );
    {
        SmartPtr<One> pOne( new One );
        SmartPtr<One> pTwo( pOne );

        pOne = new One;
        pTwo = new One;
    }

    printf( "Test 8:\n" );
    {
        SmartPtr<One> pOne( new One );
        SmartPtr<One> pTwo( pOne );

        pOne = nullptr;
        pTwo = nullptr;
    }

}

void main( void )
{
    TestSmartPtr();
}
#endif

