// Copyright (c) 2006-2018 Maxim Khizhinsky
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef CDSLIB_GC_DETAILS_RETIRED_PTR_H
#define CDSLIB_GC_DETAILS_RETIRED_PTR_H

#include <cds/details/defs.h>
#include <cds/details/static_functor.h>

//@cond
namespace cds { namespace gc {
    /// Common implementation details for any GC
    namespace details {

        /// Pointer to function to free (destruct and deallocate) retired pointer of specific type
        typedef void (* free_retired_ptr_func )( void * );

        /// Retired pointer
        /**
            Pointer to an object that is ready to delete.
        */
        struct retired_ptr {
            /// Pointer type
            typedef void *          pointer;

            union {
                pointer                 m_p;        ///< retired pointer
                uintptr_t               m_n;
            };
            free_retired_ptr_func   m_funcFree; ///< pointer to the destructor function

            /// Comparison of two retired pointers
            static bool less( const retired_ptr& p1, const retired_ptr& p2 ) noexcept
            {
                return p1.m_p < p2.m_p;
            }

            /// Default ctor initializes pointer to \p nullptr
            retired_ptr() noexcept
                : m_p( nullptr )
                , m_funcFree( nullptr )
            {}

            /// Ctor
            retired_ptr( pointer p, free_retired_ptr_func func ) noexcept
                : m_p( p )
                , m_funcFree( func )
            {}

            /// Typecasting ctor
            template <typename T>
            retired_ptr( T* p, free_retired_ptr_func func) noexcept
                : m_p( reinterpret_cast<pointer>(p))
                , m_funcFree( func )
            {}
/*
            template <typename T>
            retired_ptr( T * p, void (* pFreeFunc)(T *)) noexcept
                : m_p( reinterpret_cast<pointer>(p))
                , m_funcFree( reinterpret_cast< free_retired_ptr_func >( pFreeFunc ))
            {}
*/

            /// Assignment operator
            retired_ptr& operator =( retired_ptr const& s) noexcept
            {
                m_p = s.m_p;
                m_funcFree = s.m_funcFree;
                return *this;
            }

            /// Invokes destructor function for the pointer
            void free()
            {
                assert( m_funcFree );
                assert( m_p );
                m_funcFree( m_p );

                CDS_STRICT_DO( clear());
            }

            /// Checks if the retired pointer is not empty
            explicit operator bool() const noexcept
            {
                return m_p != nullptr;
            }

            /// Clears retired pointer without \p free() call
            void clear()
            {
                m_p = nullptr;
                m_funcFree = nullptr;
            }
        };

        static inline bool operator <( const retired_ptr& p1, const retired_ptr& p2 ) noexcept
        {
            return retired_ptr::less( p1, p2 );
        }

        static inline bool operator ==( const retired_ptr& p1, const retired_ptr& p2 ) noexcept
        {
            return p1.m_p == p2.m_p;
        }

        static inline bool operator !=( const retired_ptr& p1, const retired_ptr& p2 ) noexcept
        {
            return !(p1 == p2);
        }
    }  // namespace details

    template <typename Func, typename T>
    static inline cds::gc::details::retired_ptr make_retired_ptr( T * p )
    {
        return cds::gc::details::retired_ptr( p, cds::details::static_functor<Func, T>::call );
    }

}}   // namespace cds::gc
//@endcond

#endif // #ifndef CDSLIB_GC_DETAILS_RETIRED_PTR_H
