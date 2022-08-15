/*!
 * @file hashtbl.inl
 * @brief Implementation of the HashTbl class methods.
 *
 * @author Fernanda Lustosa
 * @author Lucas Bazante
 */

#include "hashtbl.h"

namespace ac {

    /// CONSTRUCTORS

    // Size constructor.
    /*!
     * This constructor allocates a new table with a size equal to the next prime number after sz.
     *
     * @tparam KeyType The key type.
     * @tparam DataType The data type.
     * @tparam KeyHash A function that reads a key and returns an unsigned integer.
     * @tparam KeyEqual  A function that compares two keys.
     *
     * @param sz The minimun size of the new table.
     */
	template< typename KeyType, typename DataType, typename KeyHash, typename KeyEqual >
	HashTbl<KeyType,DataType,KeyHash,KeyEqual>::HashTbl( size_type sz )
	{
        m_size = find_next_prime( sz );
        m_count = 0;
        m_table = std::make_unique< std::forward_list< entry_type > [] >( m_size );
	}

    // Copy constructor.
    /*!
     * This constructor creates a new table like the one provided.
     *
     * @tparam KeyType The key type.
     * @tparam DataType The data type.
     * @tparam KeyHash A function that reads a key and returns an unsigned integer.
     * @tparam KeyEqual  A function that compares two keys.
     *
     * @param source Hash table to be copied.
     */
	template< typename KeyType, typename DataType, typename KeyHash, typename KeyEqual >
	HashTbl<KeyType,DataType,KeyHash,KeyEqual>::HashTbl( const HashTbl& source )
	{
        m_size = source.m_size;
        m_count = source.m_count;
        max_load_factor( source.max_load_factor( ) );
        m_table = std::make_unique< std::forward_list< entry_type > [] >( m_size );

        for ( size_type i = 0; i < m_size; ++i )
        {
            if ( not source.m_table[i].empty() )
                m_table[i] = source.m_table[i];
        }
	}

    // Initializer constructor
    /*!
     * This constructor creates a hash table with the values from the initializer list.
     *
     * @tparam KeyType The key type.
     * @tparam DataType The data type.
     * @tparam KeyHash A function that reads a key and returns an unsigned integer.
     * @tparam KeyEqual  A function that compares two keys.
     *
     * @param ilist List of values.
     */
	template< typename KeyType, typename DataType, typename KeyHash, typename KeyEqual >
	HashTbl<KeyType,DataType,KeyHash,KeyEqual>::HashTbl( const std::initializer_list<entry_type>& ilist )
    {
        m_size = find_next_prime( ilist.size() * 2 ); // double the size for a good ratio
        m_count = 0;
        
        m_table.reset( nullptr ); // if there was already something
        m_table = std::make_unique< std::forward_list< entry_type> [] >( m_size );
        
        for ( auto en : ilist )
            insert( en.m_key, en.m_data );
    }

    /// OVERLOADED OPERATORS

    // Assignment operator.
    /*!
     * This operator assigns values from one hash table to another.
     *
     * @tparam KeyType The key type.
     * @tparam DataType The data type.
     * @tparam KeyHash A function that reads a key and returns an unsigned integer.
     * @tparam KeyEqual  A function that compares two keys.
     *
     * @param clone The hash table to be cloned.
     *
     * @return A reference to the modified hash table.
     */
	template< typename KeyType, typename DataType, typename KeyHash, typename KeyEqual >
	HashTbl<KeyType,DataType,KeyHash,KeyEqual>&
    HashTbl<KeyType,DataType,KeyHash,KeyEqual>::operator=( const HashTbl& clone )
    {
        m_size = clone.m_size;
        m_count = clone.m_count;
        max_load_factor( clone.max_load_factor( ) );
        m_table = std::make_unique< std::forward_list< entry_type > [] >( m_size );

        for ( size_type i = 0; i < m_size; ++i )
        {
            if ( not clone.m_table[i].empty() )
                m_table[i] = clone.m_table[i];
        }

        return *this;
    }

    // Assignment initializer list.
    /*!
     * This operator assigns values from a initializer list to a hash table.
     *
     * @tparam KeyType The key type.
     * @tparam DataType The data type.
     * @tparam KeyHash A function that reads a key and returns an unsigned integer.
     * @tparam KeyEqual  A function that compares two keys.
     *
     * @param ilist List of values.
     *
     * @return A reference to the modified hash table.
     */
	template< typename KeyType, typename DataType, typename KeyHash, typename KeyEqual >
	HashTbl<KeyType,DataType,KeyHash,KeyEqual>&
    HashTbl<KeyType,DataType,KeyHash,KeyEqual>::operator=( const std::initializer_list< entry_type >& ilist )
    {
        m_size = find_next_prime( ilist.size() * 2 ); // double the size for a good ratio
        m_count = 0;

        m_table.reset( nullptr );
        m_table = std::make_unique< std::forward_list< entry_type > [] >( m_size );

        for ( auto en : ilist )
            insert( en.m_key, en.m_data );

        return *this;
    }

    /// DESTRUCTOR 

    // Class destructor.
	template< typename KeyType, typename DataType, typename KeyHash, typename KeyEqual >
	HashTbl<KeyType,DataType,KeyHash,KeyEqual>::~HashTbl( )
	{
        m_size = 0, m_count = 0;
        max_load_factor( 0 );
        m_table.reset( nullptr ); // resets unique_ptr to a nullptr state, freeing its memmory
	}

    /// CLASS METHODS
    
    // Inserts data into the hash table according to the associated key.
    /*!
     * Inserts the new entry if the key does not exist and updates the data otherwise.
     *
     * @tparam KeyType The key type.
     * @tparam DataType The data type.
     * @tparam KeyHash A function that reads a key and returns an unsigned integer.
     * @tparam KeyEqual  A function that compares two keys.
     *
     * @param key_ Key associated with data.
     * @param new_data_ New data to be inserted/updated.
     *
     * @return True if the insertion was successful; False if the key already existed.
     */
    template< typename KeyType, typename DataType, typename KeyHash, typename KeyEqual >
	bool HashTbl<KeyType,DataType,KeyHash,KeyEqual>::insert( const KeyType & key_, const DataType & new_data_ )
    {
        KeyHash hashf;
        KeyEqual eq;
        auto & which = m_table[ hashf( key_ ) % m_size ];

        auto item = std::find_if( std::begin( which ), std::end( which ), [ & ]( entry_type en ){ return eq( en.m_key, key_ ); } ); 
        if ( item != std::end( which ) )
        {
            item->m_data = new_data_;
            return false;
        }

        entry_type entry( key_, new_data_ );
        which.push_front( entry );
        
        max_load_factor( ( float ) ++m_count / m_size );

        if ( max_load_factor( ) > 1.0 )
            rehash( );

        return true;
    }
	
    // Clears the data table.
    /*!
     * Erases all memory associated with table collision lists.
     *
     * @tparam KeyType The key type.
     * @tparam DataType The data type.
     * @tparam KeyHash A function that reads a key and returns an unsigned integer.
     * @tparam KeyEqual  A function that compares two keys.
     */
    template <typename KeyType, typename DataType, typename KeyHash, typename KeyEqual>
    void HashTbl<KeyType, DataType, KeyHash, KeyEqual>::clear()
    {
        m_count = 0;
        for ( size_type i = 0; i < m_size; i++ )
            if ( not m_table[i].empty( ) )
                m_table[i].clear( );
    }

    // Checks if the table has elements.
    /*!
     * Tests whether the table is empty.
     *
     * @tparam KeyType The key type.
     * @tparam DataType The data type.
     * @tparam KeyHash A function that reads a key and returns an unsigned integer.
     * @tparam KeyEqual  A function that compares two keys.
     *
     * @return True the table is empty, False otherwise.
     */
    template< typename KeyType, typename DataType, typename KeyHash, typename KeyEqual >
    bool HashTbl<KeyType, DataType, KeyHash, KeyEqual>::empty() const
    {
        return ( m_count == 0 );
    }

    // Retrieves data from the table.
    /*! 
     * Retrieves a data item from the table, based on the key associated with the data. 
     * If the data cannot be found, false is returned; otherwise, true is returned instead.
     *  
     * @tparam KeyType The key type.
     * @tparam DataType The data type.
     * @tparam KeyHash A function that reads a key and returns an unsigned integer.
     * @tparam KeyEqual  A function that compares two keys.
     *
     * @param key_ Data key to search for in the table.
     * @param data_item_ Data record to be filled in when data item is found.
     * 
     * @return True if the data item is found; False, otherwise.
     */
	template< typename KeyType, typename DataType, typename KeyHash, typename KeyEqual >
    bool HashTbl<KeyType, DataType, KeyHash, KeyEqual>::retrieve( const KeyType & key_, DataType & data_item_ ) const
    {
        KeyHash hashf;
        KeyEqual eq;
        auto which = m_table[ hashf( key_ ) % m_size ];
        
        auto item = std::find_if( std::begin( which ), std::end( which ), [ & ]( entry_type en ){ return eq( en.m_key, key_ ); } );  
        
        if ( item != std::end( which ) ) 
        {
            data_item_ = item->m_data;
            return true;   
        }

        return false; // This is just a stub. Reaplace it accordinly.
    }

    // Rearranges the hash table to match the load factor.
    /*!
     * This function creates a new hash table, bigger than the other one, 
     * and redistributes all its values according to the new hash function.
     *
     * @tparam KeyType The key type.
     * @tparam DataType The data type.
     * @tparam KeyHash A function that reads a key and returns an unsigned integer.
     * @tparam KeyEqual  A function that compares two keys.
     */
    template <typename KeyType, typename DataType, typename KeyHash, typename KeyEqual>
    void HashTbl<KeyType, DataType, KeyHash, KeyEqual>::rehash( void )
    {
        std::vector< std::forward_list< entry_type > > copy;

        for ( size_type i = 0; i < m_size; ++i )
            if ( not m_table[i].empty( ) )
                copy.push_back( m_table[i] );

        m_size = find_next_prime( 2 * m_size ), m_count = 0;
        m_table.reset( nullptr );
        m_table = std::make_unique< std::forward_list< entry_type > [] >( m_size );

        for ( auto fl : copy )
            for ( auto en : fl )
                insert( en.m_key, en.m_data );
    }

    // Erase element from the hash table.
    /*!
     * This function removes the element at the given position.
     *
     * @tparam KeyType The key type.
     * @tparam DataType The data type.
     * @tparam KeyHash A function that reads a key and returns an unsigned integer.
     * @tparam KeyEqual  A function that compares two keys.
     *
     * @param key_ Key of element to be removed.
     *
     * @return True if the key was found; False otherwise.
     */
	template< typename KeyType, typename DataType, typename KeyHash, typename KeyEqual >
    bool HashTbl< KeyType, DataType, KeyHash, KeyEqual >::erase( const KeyType & key_ )
    {
        KeyHash hashf;
        KeyEqual eq;
        auto & which = m_table[ hashf( key_ ) % m_size ];
        
        if ( std::find_if( std::begin( which ), std::end( which ), [ & ]( entry_type en ){ return eq( en.m_key, key_ ); } ) != std::end( which ) )
        {
            which.remove_if( [ & ]( entry_type en ){ return eq( en.m_key, key_ ); } );
            --m_count;
            return true;
        }

        return false;
    }

    // Check if number is prime.
    /*!
     * This function tests if given number is prime.
     *
     * @tparam KeyType The key type.
     * @tparam DataType The data type.
     * @tparam KeyHash A function that reads a key and returns an unsigned integer.
     * @tparam KeyEqual  A function that compares two keys.
     *
     * @param n_ Number to be checked.
     *
     * @return True if it's prime; False otherwise.
     */
    template< typename KeyType, typename DataType, typename KeyHash, typename KeyEqual >
    bool HashTbl< KeyType, DataType, KeyHash, KeyEqual >::is_prime( size_type n_ )
    {
        if ( n_ == 2 or n_ == 3 )
            return true;
        if ( n_ % 2 == 0 or n_ % 3 == 0 )
            return false;

        size_type divisor = 6;
        while ( divisor * divisor - 2 * divisor + 1 <= n_ )
        {
            if ( n_ % ( divisor - 1 ) == 0 )
                return false;
            if ( n_ % ( divisor + 1 ) == 0 )
                return false;
            divisor += 6;
        }

        return true;
    }

    // Find the next prime.
    /*!
     * This function looks for the next prime after the given number.
     *
     * @tparam KeyType The key type.
     * @tparam DataType The data type.
     * @tparam KeyHash A function that reads a key and returns an unsigned integer.
     * @tparam KeyEqual  A function that compares two keys.
     *
     * @param n_ Number where the search should start.
     *
     * @return The next prime number.
     */
	template< typename KeyType, typename DataType, typename KeyHash, typename KeyEqual >
    std::size_t HashTbl<KeyType,DataType,KeyHash,KeyEqual>::find_next_prime( size_type  n_ )
    {
       while ( not is_prime( ++n_ ) );
       return n_;
    }

    // Count the elements in a list.
    /*!
     * This function counts the number of elements in the list at given position.
     *
     * @tparam KeyType The key type.
     * @tparam DataType The data type.
     * @tparam KeyHash A function that reads a key and returns an unsigned integer.
     * @tparam KeyEqual  A function that compares two keys.
     *
     * @param key_ Index of the list in the hash table.
     *
     * @return Number of elements in the list.
     */
	template< typename KeyType, typename DataType, typename KeyHash, typename KeyEqual >
    typename HashTbl< KeyType, DataType, KeyHash, KeyEqual >::size_type
    HashTbl< KeyType, DataType, KeyHash, KeyEqual >::count( const KeyType & key_ ) const
    {
        KeyHash hashf;
        auto & which = m_table[ hashf( key_ ) % m_size ];

        return std::distance( std::begin( which ), std::end( which ) ); // Stub
    }

    // Reference to the element at given position.
    /*!
     * This function finds the data associated with a certain key, if it doesn't exist, an exception is thrown.
     *
     * @tparam KeyType The key type.
     * @tparam DataType The data type.
     * @tparam KeyHash A function that reads a key and returns an unsigned integer.
     * @tparam KeyEqual  A function that compares two keys.
     *
     * @param key_ Key to wanted element.
     *
     * @return Data associated with the key.
     */
	template< typename KeyType, typename DataType, typename KeyHash, typename KeyEqual >
    DataType& HashTbl<KeyType, DataType, KeyHash, KeyEqual>::at( const KeyType & key_ )
    {
        KeyHash hashf;
        KeyEqual eq;
        auto & which = m_table[ hashf( key_ ) % m_size ];

        auto item = std::find_if( std::begin( which ), std::end( which ), [ & ]( entry_type en ){ return eq( en.m_key, key_ ); } );
        if ( item != which.end() )
            return item->m_data;

        throw std::out_of_range( "Not present" ); // Stub
    }

    // Accesses the element associated with the key or inserts a new element.
    /*!
     * Returns a reference to the data associated with the given key if it exists. 
     * If the key is not in the table, the method performs the insert.
     * 
     * @tparam KeyType The key type.
     * @tparam DataType The data type.
     * @tparam KeyHash A function that reads a key and returns an unsigned integer.
     * @tparam KeyEqual  A function that compares two keys.
     *
     * @param key_ Key possibly associated with an element in the table.
     *
     * @return A reference to the data associated with the key.
     */
	template< typename KeyType, typename DataType, typename KeyHash, typename KeyEqual >
    DataType& HashTbl<KeyType, DataType, KeyHash, KeyEqual>::operator[]( const KeyType & key_ )
    {
        KeyHash hashf;
        KeyEqual eq;
        auto & which = m_table[ hashf( key_ ) % m_size ];
 
        auto item = std::find_if( std::begin( which ), std::end( which ), [ & ]( entry_type en ){ return eq( en.m_key, key_ ); } );
        if ( item != which.end() )
            return item->m_data;

        ++m_count;
        max_load_factor( ( float ) m_count / m_size );
        entry_type entry( key_, DataType{ } ); // a default constructor 
        which.push_front( entry );
        return which.front( ).m_data;
    }
} // Namespace ac.
