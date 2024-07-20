/*  <src/databases/databases.hpp>
    
    Defines the homogenous database model used throughtout the program. */

/*  Assignment part for @HexagonalUniverse.
    
    Last update: 20/07/2024. */


#ifndef _DATABASES_HPP_INCLUDED_
#define _DATABASES_HPP_INCLUDED_

/*  Troubleshooting references:
	
    . Class's definition linkage error: 
		https://stackoverflow.com/questions/1353973/c-template-linking-error 
	
		(Template implementations must be done entirely in header-files...)
*/


#include "../headers/commons.h"
#include <stdio.h.> // FILE ds

// IO & STL
#include <iostream> // cin & cout
#include <list>
#include <functional> 


/*	TODO: (Overall description...) 
*
*
*/

// Specifies the page-size of the database in bytes.
// Default: 64 [kB].
constexpr size_t database_paging_buffer_size = 65536ULL;

typedef unsigned char byte;


/*	Base Declaration
 *	================ */

/*  A template class representing a homogeneous database manager. 
    The homogeneous data are its elements, and they're specified by their index. 
    
    The database is associated with a headed file-stream tracking the indexes. */
template <typename ElementType> class Database {
private:
	const size_t page_elements_qtt = database_paging_buffer_size / sizeof(ElementType);
	// byte page_buffer[database_paging_buffer_size]; // NOT YET IMPLEMENTED.

    const char * filename;          // The database's (main) stream filename.
    bool init_succeeded;            // Tracks if the database's initialization was successful.
    
    // Attempts initializing (from empty) the file-stream, overwriting it.
    bool overwrite_stream(void);    

    // Attempts nulling "item_qtt" from stream's header.
    virtual bool reset_database(void);      

protected:
    // The database's file-stream.
    FILE * stream;

    /*  Stream-header attributes : keeps overall information of the stream. 
        As child classes derives from this one, other information shall be aggregated
        with the standard "item_qtt". stream_header_size shall be updated accordingly. */
    id_t item_qtt;  // How many items does is the database holding.

    // Tracks the size of the class stream header, in bytes.
    // Constant once the object is initialized.
    size_t stream_header_size;

    // Attempts opening the stream. Sets init_succeeded to false in case of failing.
    bool initialize_stream(void);

    // Closes the file-stream.
    inline void finalize_stream(void) { fclose(stream); stream = nullptr; }; 
    
    // Reads the internal stream-header structure.
    virtual bool retrieve_stream_header(void);    

    // Writes the internal stream-header structure.
    virtual bool update_stream_header(void) const;

    /*  Reads a single element in the homogeneous database data-space.
        The element is specified by its index; the element is written by reference.
        Returns success; fails in case of IO sequencing. In case of fail, the stream
        pointer state is undeterminated. */
    inline bool read_element(id_t index, ElementType * const _DstItem) const {
        return (fseek(stream, stream_header_size + index * sizeof(ElementType), SEEK_SET) == 0) && 
            (fread(_DstItem, sizeof(ElementType), 1, stream) > 0);
    }

    /*  Writes a single element in the homogeneous database data-space.
        The element is specified by its index; the element is written by reference.
        Returns success; fails in case of IO sequencing. In case of fail, the stream
        pointer state is undeterminated. */
    inline bool write_element(id_t index, const ElementType * const _SrcItem) const {
        return (fseek(stream, stream_header_size + index * sizeof(ElementType), SEEK_SET) == 0) &&
            (fwrite(_SrcItem, sizeof(ElementType), 1, stream) > 0);
    }

    /*  Database Representation
        ----------------------- */
    
    virtual inline void fprint_element(FILE * _OutputStream, const ElementType * _Element) { fprintf(_OutputStream, "NONE%p", _Element); };

    /*  Represents the database onto an output stream, sectioned inclusively, from a start to an end
        - (_From) and (_To) respectively.
        Case for what (_To = 0) actually will means that the entire database is target of printing.
        Prints it on its entirety, and on stdout, by default. */
    bool print_database(FILE * _OutputStream = stdout, size_t _From = 0, size_t _To = 0);


    std::list<ElementType> list_filter(std::function<bool(const ElementType &)> check, size_t _From = 0, size_t _To = ((size_t) - 1));

    Database(const char * filename, size_t stream_header_size);
public:
    Database(const char * filename);
    ~Database(void);

    /*  Returns if the database initialization was successfully done. 
        In case of fail, the object shall not be used. */
    inline bool could_initialize(void) const { return init_succeeded; }
};


/*  Base Implementation
 *	=================== */

template <typename ElementType> inline bool
Database<ElementType>::overwrite_stream(void) { return ((stream = fopen(filename, "w+b")) != nullptr); }

template <typename ElementType> inline bool
Database<ElementType>::reset_database(void) {
	item_qtt = 0;
	return update_stream_header();
}

template <typename ElementType> inline bool
Database<ElementType>::initialize_stream(void)
{
	stream = nullptr;
	if (((stream = fopen(filename, "r+b")) == nullptr) && (! overwrite_stream())) {
		init_succeeded = false;
		std::cerr << "Couldn't initialize the database's stream properly." << std::endl;
		return false;
	}
	return true;
}

template <typename ElementType>
Database<ElementType>::Database(const char * filename, size_t stream_header_size) : filename(filename), stream_header_size(stream_header_size)
{
	std::cout << "Number of elements per page: " << page_elements_qtt << std::endl;
	init_succeeded = true;
	item_qtt = 0;

	if (! initialize_stream())
		return;
}

template <typename ElementType>
Database<ElementType>::Database(const char * filename) : Database(filename, sizeof(item_qtt)) {
	if (! init_succeeded) return;

	/*	If reading the stream-header fails, then probably the database is empty.
		In that case, it is reset. */
	if (! retrieve_stream_header())
	{
		// Checking if the stream is actually empty.
		if ((! fseek(stream, 0, SEEK_END)) && ftell(stream) == 0)
			reset_database();
	}
}

template <typename ElementType>
Database<ElementType>::~Database(void) {
	if (stream == nullptr)	return;

	if (! update_stream_header())
		std::cerr << "Stream header couldn't be written at <Database> class object." << std::endl;

	finalize_stream();
}

template <typename ElementType>
bool Database<ElementType>::retrieve_stream_header(void) {
	struct {
		id_t item_qtt = 0;
	} _stream_header;

	rewind(stream);
	if (fread(&_stream_header, sizeof(_stream_header), 1, stream) < 1)
		return false;

	item_qtt = _stream_header.item_qtt;
	return true;
}

template <typename ElementType>
bool Database<ElementType>::update_stream_header(void) const {
	struct {
		id_t item_qtt = 0;
	} _stream_header;
	_stream_header.item_qtt = item_qtt;

	rewind(stream);
	return fwrite(&_stream_header, stream_header_size, 1, stream) > 0;
}

/*	Database Representation
 *	----------------------- */

#include <cstdlib>

template <typename ElementType>
bool Database<ElementType>::print_database(FILE * _OutputStream, size_t _From, size_t _To)
{
	// _To = 0 will force printing the entire database.
	if ((_To == 0) && (item_qtt > 0))
		_To = item_qtt;

	ElementType element_buffer;

	if (fseek(stream, stream_header_size, SEEK_SET) != 0)
		return false;

	size_t iterator = _From;
	while ((iterator <= _To) && read_element(iterator ++, &element_buffer)) {
		// not safe inside the loop yet*

		fprintf(_OutputStream, "[%03llu] ", iterator - 1);
		fprint_element(_OutputStream, &element_buffer);
		fprintf(_OutputStream, "\n");
	}

	return iterator == (_To + 1);
}

template <typename ElementType>
std::list<ElementType> Database<ElementType>::list_filter(std::function<bool(const ElementType &)> check, size_t _From, size_t _To)
{
	std::list<ElementType> elements;

	if (_To == ((size_t) -1))
	{
		if (item_qtt == 0) // empty database case
			return elements;

		_To = item_qtt - 1;
	}

	ElementType element_buffer;

	for (size_t i = _From; i <= _To; ++ i)
	{
		if (! read_element(i, & element_buffer))
		{
			fprintf(stderr, "[%s] Couldn't read #%llu element.\n", __func__, (unsigned long long) i);
			continue;
		}

		if (check(element_buffer))
			elements.push_back(element_buffer);
	}

	return elements;
}


#endif // _DATABASES_HPP_INCLUDED_
