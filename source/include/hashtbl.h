/*!
 * @file hashtbl.h
 * @brief Implementation of a hash table.
 *
 * @author Selan
 */

#ifndef _HASHTBL_H_
#define _HASHTBL_H_

#include <memory>           // unique_ptr
#include <iostream>         // cout, endl, ostream
#include <forward_list>     // forward_list
#include <algorithm>        // copy, find_if, for_each
#include <cmath>            // sqrt
#include <iterator>         // std::begin(), std::end()
#include <initializer_list>
#include <utility>          // std::pair
#include <vector>           // vector

namespace ac // Associative container
{
    /*!
     * Struct representing a hash table element.
     *
     * @tparam KeyType The key type.
     * @tparam DataType The data type.
     */
	template<class KeyType, class DataType>
	struct HashEntry {
        KeyType m_key;   //! Data key
        DataType m_data; //! The data

        // Regular constructor.
        HashEntry( KeyType kt_, DataType dt_ ) : m_key{kt_} , m_data{dt_}
        {/*Empty*/}

        friend std::ostream & operator<<( std::ostream & os_, const HashEntry & he_ ) {
            // os_ << "{" << he_.m_key << "," << he_.m_data << "}";
            os_ << he_.m_data;
            return os_;
        }
    };

    /*! 
     * This class implements an STL hash table.
     *
     * @tparam KeyType The key type.
     * @tparam DataType The data type.
     * @tparam KeyHash A function that reads a key and returns an unsigned integer.
     * @tparam KeyEqual  A function that compares two keys.
     */
	template< class KeyType,
		      class DataType,
		      class KeyHash = std::hash< KeyType >,
		      class KeyEqual = std::equal_to< KeyType > >
	class HashTbl {
        public:
            // Aliases
            using entry_type = HashEntry<KeyType,DataType>;
            using list_type = std::forward_list< entry_type >;
            using size_type = std::size_t;

            /// Constructors
            explicit HashTbl( size_type table_sz_ = DEFAULT_SIZE );
            HashTbl( const HashTbl& );
            HashTbl( const std::initializer_list< entry_type > & );
            
            /// Overloaded operators
            HashTbl& operator=( const HashTbl& );
            HashTbl& operator=( const std::initializer_list< entry_type > & );

            /// Destructor
            virtual ~HashTbl();

            /// Class methods
            bool insert( const KeyType &, const DataType &  );
            bool retrieve( const KeyType &, DataType & ) const;
            bool erase( const KeyType & );
            void clear();
            bool empty() const;
            inline size_type size() const { return m_count; };
            DataType& at( const KeyType& );
            DataType& operator[]( const KeyType& );
            size_type count( const KeyType& ) const;
            float max_load_factor() const { return m_max_load_factor; };
            void max_load_factor(float mlf) { m_max_load_factor = mlf; };

            /// Friend functions
            friend std::ostream & operator<<( std::ostream & os_, const HashTbl & ht_ ) {
                for ( size_type i = 0; i < ht_.m_size; ++i )
                {
                    auto &it = ht_.m_table[ i ];
                    os_ << "[" << i << "]-> ";
                    if ( not it.empty() )
                    {
                        os_ << "\n";
                        for ( auto hashed : it )
                            os_ << hashed << "\n";
                    } 
                    else
                    {
                        os_ << "\"Empty\"\n";
                    }
                }

                return os_;
            }

        private:
            /// Private methods
            static bool is_prime( size_type );
            static size_type find_next_prime( size_type );
            void rehash( void );

        private:
            size_type m_size;           //!< Table size.
            size_type m_count;          //!< Number of elements in the table.
            float m_max_load_factor;    //!< Load factor, i.e. ratio between m_count and m_size.
            std::unique_ptr< std::forward_list< entry_type > [] > m_table;
            static const short DEFAULT_SIZE = 11;
    };

} // MyHashTable
#include "hashtbl.inl"
#endif
