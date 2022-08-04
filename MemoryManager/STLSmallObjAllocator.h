#pragma once
#include <iostream>
#include <limits>

#include "SmallObjAllocator.h"

namespace small_object_allocator
{

    template<typename T>
    class STLSmallObjectTraits {
    public:
        //    convert an STLSmallObjectTraits<T> to STLSmallObjectTraits<U>
        template<typename U>
        struct rebind {
            typedef STLSmallObjectTraits<U> other;
        };

    public:
        inline explicit STLSmallObjectTraits() {}
        inline ~STLSmallObjectTraits() {}
        template <typename U>
        inline explicit STLSmallObjectTraits(STLSmallObjectTraits<U> const&) {}

        inline T* address(T& r) { return &r; }
        inline T const* address(T const& r) { return &r; }

        inline void construct(T* p, const T& t) { new(p) T(t); }
        inline void destroy(T* p) { p->~T(); }
    };




    template<typename T, std::size_t numBlocks = 64>
    class STLSmallObjectAllocPolicy {
    public:
        //    typedefs
        typedef T value_type;
        typedef value_type* pointer;
        typedef const value_type* const_pointer;
        typedef value_type& reference;
        typedef const value_type& const_reference;
        typedef std::size_t size_type;
        typedef std::ptrdiff_t difference_type;

    public:
        //    convert an STLSmallObjectAllocPolicy<T> to STLSmallObjectAllocPolicy<U>
        template<typename U>
        struct rebind {
            typedef STLSmallObjectAllocPolicy<U, numBlocks> other;
        };

    public:
        inline explicit STLSmallObjectAllocPolicy() {}
        inline ~STLSmallObjectAllocPolicy() {}
        inline explicit STLSmallObjectAllocPolicy(STLSmallObjectAllocPolicy const&) {}
        template <typename T2, std::size_t N2>
        inline explicit STLSmallObjectAllocPolicy(STLSmallObjectAllocPolicy<T2, N2> const&) {}

        //    memory allocation
        inline pointer allocate(size_type cnt, const void* = 0)
    	{
            return reinterpret_cast<T*>(allocator_.Allocate(sizeof(T) * cnt));
        }
        inline void deallocate(pointer p, size_type cnt)
    	{
            allocator_.Deallocate(p, sizeof(T) * cnt);
        }

        //    size
        inline size_type max_size() const
    	{
            return std::numeric_limits<size_type>::max() / sizeof(T);
        }

    private:
        // the actual allocator
        static inline SmallObjAllocator allocator_ = SmallObjAllocator(numBlocks * sizeof(T), sizeof(T));
    };    


    //    determines if memory from another allocator
    //    can be deallocated from this one
    template<typename T, std::size_t N>
    inline bool operator==(STLSmallObjectAllocPolicy<T, N> const&, STLSmallObjectAllocPolicy<T, N> const&)
	{
        return true;
    }

    template<typename T, std::size_t N, typename T2, std::size_t N2>
    inline bool operator==(STLSmallObjectAllocPolicy<T, N> const&, STLSmallObjectAllocPolicy<T2, N2> const&)
	{
        return false;
    }

    template<typename T, std::size_t N, typename OtherAllocator>
    inline bool operator==(STLSmallObjectAllocPolicy<T, N> const&, OtherAllocator const&)
	{
        return false;
    }



    template<typename T,
			 typename Policy = STLSmallObjectAllocPolicy<T>,
			 typename Traits = STLSmallObjectTraits<T> >
        class STLSmallObjAllocator : public Policy, public Traits {
        private:
            typedef Policy AllocationPolicy;
            typedef Traits TTraits;

        public:
            typedef typename AllocationPolicy::size_type size_type;
            typedef typename AllocationPolicy::difference_type difference_type;
            typedef typename AllocationPolicy::pointer pointer;
            typedef typename AllocationPolicy::const_pointer const_pointer;
            typedef typename AllocationPolicy::reference reference;
            typedef typename AllocationPolicy::const_reference const_reference;
            typedef typename AllocationPolicy::value_type value_type;

        public:
            template<typename U>
            struct rebind {
                //for some reason i can't pass the policy and traits through... 
                typedef STLSmallObjAllocator<U> other;
            };

        public:
            inline explicit STLSmallObjAllocator() {}
            inline ~STLSmallObjAllocator() {}
            inline STLSmallObjAllocator(STLSmallObjAllocator const& rhs) :Traits(rhs), Policy(rhs) {}
            template <typename U>
            inline STLSmallObjAllocator(STLSmallObjAllocator<U> const&) {}
            template <typename U, typename P, typename T2>
            inline STLSmallObjAllocator(STLSmallObjAllocator<U, P, T2> const& rhs) : Traits(rhs), Policy(rhs) {}
    };    

    // determines if memory from another
    // allocator can be deallocated from this one
    template<typename T, typename P, typename Tr>
    inline bool operator==(STLSmallObjAllocator<T, P, Tr> const& lhs, STLSmallObjAllocator<T, P, Tr> const& rhs)
	{
        return operator==(static_cast<P&>(lhs), static_cast<P&>(rhs));
    }


    template<typename T, typename P, typename Tr, typename T2, typename P2, typename Tr2>
    inline bool operator==(STLSmallObjAllocator<T, P, Tr> const& lhs, STLSmallObjAllocator<T2, P2, Tr2> const& rhs)
	{
        return operator==(static_cast<P&>(lhs), static_cast<P2&>(rhs));
    }

    template<typename T, typename P, typename Tr, typename OtherAllocator>
    inline bool operator==(STLSmallObjAllocator<T, P, Tr> const& lhs, OtherAllocator const& rhs)
	{
        return operator==(static_cast<P&>(lhs), rhs);
    }

    template<typename T, typename P, typename Tr>
    inline bool operator!=(STLSmallObjAllocator<T, P, Tr> const& lhs, STLSmallObjAllocator<T, P, Tr> const& rhs)
	{
        return !operator==(lhs, rhs);
    }

    template<typename T, typename P, typename Tr, typename T2, typename P2, typename Tr2>
    inline bool operator!=(STLSmallObjAllocator<T, P, Tr> const& lhs, STLSmallObjAllocator<T2, P2, Tr2> const& rhs)
	{
        return !operator==(lhs, rhs);
    }

    template<typename T, typename P, typename Tr, typename OtherAllocator>
	inline bool operator!=(STLSmallObjAllocator<T, P, Tr> const& lhs, OtherAllocator const& rhs)
	{
        return !operator==(lhs, rhs);
    }

}
