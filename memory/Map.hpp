#pragma once

#include <stdlib.h>
#include "types/Types.hpp"

namespace trafi
{

  namespace memory
  {


    class MapBadAllocation {

    };

    class MapException {

    };



    template<class K, class V>
    class Map
    {

    public:
      /**
        * Type fields
        */
      using pair_type     = types::Pair<K, V>;
      using pair_pointer  = pair_type*;
      using pair_cpointer = const pair_pointer;
      using size_type     = size_t;

    public:

      /**
        * Default starting allocation size
        */
      static constexpr size_type ALLOC_SIZE = 512;

    public:
      Map() = default;

      /**
        * Overwriting default starting buffer size
        */
      inline void set_allocsize( size_type size )
      {
        if ( size <= 0 )
          return; // throw size_exception

        _alloc_size = size;
      }


      /**
        * Get allocation size step
        */
      inline size_type alloc_size() const
      { return _alloc_size; }

      inline
      size_type buff_size() const
      { return end_buffer() - begin(); }

      inline size_type
      size() const {
        return end() - begin();
      }

      inline pair_cpointer begin() const
      { return _Pair_begin; }

      inline pair_cpointer end() const
      { return _Pair_pointer; }

      inline pair_cpointer end_buffer() const
      { return _Pair_end; }




    public:

      /**
        * Used by the user to read/write data from the map.
        * Generic use:
        * Map<K, V> _map;
        * Key k;
        * Val v;
        * _map[k] = v;
        *
        * Special case:
        * Map<std::string, int> _map;
        * _map["key"] = SOME_NUMBER;
        */
      V& operator[] (const K key)
      {
          V& _val = get(key);

          //std::cout << ((*(_Pair_pointer-1)).value.n) << std::endl;



      }



      /**
        *
        */
      ~Map()
      {
        // Deallocate memory

        auto __ptr = begin();

        for ( int i = 0; i < size(); ++i )
          __ptr[i].~Pair();

        free(__ptr);

        _Pair_begin   = nullptr;
        _Pair_end     = nullptr;
        _Pair_pointer = nullptr;
      }

    private:

      inline bool not_allocated() const
      { return buff_size() == 0; }

      /**
        *
        */
      void __move_buffers(
        pair_pointer out_begin,
        pair_pointer out_end
      )
      {

        auto _ptr = out_begin;

        const auto src_size = size();
        const auto out_size = out_end   - out_begin;



        /*
         * Nothing to move since the buffer
         * has been allocated for the first time
         */
        if ( not_allocated() )
        {
          std::cout << "Nothing to move" << std::endl;
          return;
        }

        if ( out_size < src_size )
          throw MapException{};

        auto _srcptr = begin();

        // to check this one
        for ( int i = 0; i < src_size; ++i )
        {
          _ptr[i] = _srcptr[i];
        }
      }

      /**
        * Check if we reached the end of the buffer size
        */
      bool needs_realloc() const
      {
        return end() == end_buffer();
      }

      /**
        * Allocate new buffer with a size of buff_size + allocation_step
        */
      void __allocate_mem()
      {
        size_type __next_buffsize = buff_size() + alloc_size();

        /*
         * using malloc to not call eventual constructors of type K|V
         */
        pair_pointer _pp = (pair_pointer) malloc(__next_buffsize * sizeof(pair_type));


        if ( !_pp )
          throw MapBadAllocation{};

        const auto __pp_begin = _pp;
        const auto __pp_end   = _pp + __next_buffsize;


        __move_buffers( __pp_begin, __pp_end );


        _Pair_begin = __pp_begin;
        _Pair_end   = __pp_end;

        if ( _Pair_pointer == nullptr )
          _Pair_pointer = _Pair_begin;
      }

      /**
        * Called to make sure we have a working buffer to
        * write data to or read from.
        */
      void __alloc()
      {
        if ( needs_realloc() )
        {
          __allocate_mem();
        }
      } // void __alloc();

      /**
        * internal alias of operator[]
        */
      V& get( const K key )
      {

        __alloc();

        std::cout << "size(): " << size() << std::endl;
        std::cout << "buff_size(): " << buff_size() << std::endl;


        for (auto _ptr = begin(); _ptr != end(); ++_ptr)
        {
          auto pair = *_ptr;

          /* operator== has to be defined in Key type. */
          if ( pair.key == key )
            return pair.value;
        }

        /*
         * if we are here, the key has not been found in our buffer
         */




        /*

        pair_pointer __p = begin();

        for ( int i = 0; i < size(); ++i )
        {
          const auto& _val = __p[i];

          // must have operator==() implemented
          // between keys' type
          if ( _val.key == key )
            return _val.value;
        }
        */

        // if we get here, we add new key to the pointer data

        // push( key );
      }


    private:
      // data fields
      pair_pointer _Pair_begin   = nullptr;
      pair_pointer _Pair_end     = nullptr;

      pair_pointer _Pair_pointer = nullptr;

      size_type     _alloc_size  = ALLOC_SIZE;


    }; // class Map

  } // namespace memory

} // namespace trafi
