/*  <src/databases/databases.hpp>
    
    Defines the homogenous database model used throughtout the program. */

/*	Last update: 08/08/2024. */


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


#if ! defined(DEBUG_DATABASE_STATE)
#	define DEBUG_DATABASE_STATE	false
#endif // ! defined(DEBUG_DATABASE_STATE)


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

    c_filepath filename;			// The database's (main) stream filename.
    bool init_succeeded = false;	// Tracks if the database's initialization was successful.
    
    bool overwrite_stream(void);	// Attempts initializing (from empty) the file-stream, overwriting it.
    virtual bool reset_database(void);	// Attempts nulling "item_qtt" from stream's header.

protected:
	// The database's file-stream.
    FILE * stream = nullptr;	

    /*  Stream-header attributes: keeps overall information of the stream. 
        As further classes derives from this one, other information shall be aggregated
        with the standard "item_qtt". stream_header_size will be updated accordingly. */
    Id_t item_qtt = 0;  // How many items does is the database holding.

    // Tracks the size of the class stream header, in bytes.
    // Constant once the object is initialized.
    size_t stream_header_size;

    bool initialize_stream(void);	// Attempts opening the stream. Sets init_succeeded to false in case of failing.
	inline void finalize_stream(void) { fclose(stream); stream = nullptr; }	// Closes the file-stream.
    virtual bool retrieve_stream_header(void);		// Reads the internal stream-header structure.
    virtual bool update_stream_header(void) const;	// Writes the internal stream-header structure.

	/*	Searching, retrieving and update
		-------------------------------- */

    /*  Reads a single element in the homogeneous database data-space.
        The element is specified by its index; the element is written by reference.
        Returns success; fails in case of IO sequencing. In case of fail, the stream
        pointer state is undeterminated. */
	inline bool read_element(Id_t index, ElementType * const _DstItem) const {
        return (fseek(stream, stream_header_size + index * sizeof(ElementType), SEEK_SET) == 0) && 
            (fread(_DstItem, sizeof(ElementType), 1, stream) > 0);
    }

    /*  Writes a single element in the homogeneous database data-space.
        The element is specified by its index; the element is written by reference.
        Returns success; fails in case of IO sequencing. In case of fail, the stream
        pointer state is undeterminated. */
    inline bool write_element(Id_t index, const ElementType * const _SrcItem) const {
        return (fseek(stream, stream_header_size + index * sizeof(ElementType), SEEK_SET) == 0) &&
            (fwrite(_SrcItem, sizeof(ElementType), 1, stream) > 0);
    }

	inline size_t read_elements(Id_t from_index, size_t n, ElementType * const _DstBuffer) const {
		if (fseek(stream, stream_header_size + from_index * sizeof(ElementType), SEEK_SET) != 0)
			return 0;
		return fread(_DstBuffer, sizeof(ElementType), n, stream);
	}

	int64_t fetch_element(std::function<bool(const ElementType &)> match, 
		ElementType & return_element,
		size_t _From = 0, size_t _To = 0) const
	{
		if (_To == 0)	_To = item_qtt;
		else _To = _To + 1; // inclusive -> exclusive interval...

		ElementType element_buffer;
		size_t iterator = _From;
		while ((iterator < _To) && read_element(iterator, &element_buffer))
		{
			if (match(element_buffer))
			{
				return_element = element_buffer;
				return (int64_t) iterator;
			}
			iterator ++;
		}
		return -1;
	}

    /*  Database Representation
        ----------------------- */
    
    virtual inline void fprint_element(FILE * _OutputStream, const ElementType * _Element) const { 
		fprintf(_OutputStream, "NADA%p", _Element); 
	}

    /*  Represents the database onto an output stream, sectioned inclusively, from a start to an end
        - (_From) and (_To) respectively.
        Case for what (_To = 0) actually will means that the entire database is target of printing.
        Prints it on its entirety, and on stdout, by default. */
    bool print_database(FILE * _OutputStream = stdout, size_t _From = 0, size_t _To = 0);

	/*	Traverses a database's section, and separates the elements which are checked by a predicate into a std::list object. */
    std::list<ElementType> list_filter(std::function<bool(const ElementType &)> check, size_t _From = 0, size_t _To = ((size_t) - 1)) const;

    Database(const char * filename, size_t stream_header_size);

public:
    Database(const char * filename);
    ~Database(void);
	
	/*	A separated initializer for the database.
		It is made necessary, as there is polymorphic derived behavior
		that can't be initialized on the constructor. */
	// bool initialize(void);

	/*	Returns the cardinality of the database in elements. */
	size_t get_size(void) const { return item_qtt; }

	size_t print_database_filtered(std::function<bool(const ElementType &)> check, FILE * _OutputStream = stdout, size_t _From = 0, size_t _To = 0);

    /*  Returns if the database initialization was successfully done. 
        In case of fail, the object shall not be used. */
    inline bool could_initialize(void) const { return init_succeeded; }

	/*	Attemps saving the database's state. 
		For that, closes and re-opens the file-stream. 
		Returns could_initialize(); that in turn tells whether the procedure was successful. */
	bool save_state(void) {
		fclose(stream);

		Database::initialize_stream();
		return Database::could_initialize();
	}
};


/*  Base Implementation
 *	=================== */

template <typename ElementType> inline bool
Database<ElementType>::overwrite_stream(void) { return ((stream = fopen(filename, "w+b")) != nullptr); }

template <typename ElementType> inline bool
Database<ElementType>::reset_database(void) {
	item_qtt = 0;
	return Database::update_stream_header();
}

template <typename ElementType> inline bool
Database<ElementType>::initialize_stream(void)
{
	stream = nullptr;
	if (((stream = fopen(filename, "r+b")) == nullptr) && (! Database::overwrite_stream())) {
		init_succeeded = false;
		std::cerr << "N�o foi poss�vel inicializar o fluxo do <DataBase> corretamente." << std::endl;
		return false;
	}
	return true;
}

template <typename ElementType>
Database<ElementType>::Database(const char * filename, size_t stream_header_size) : filename(filename), stream_header_size(stream_header_size)
{
	std::cout << "N�mero de elementos por p�gina: " << page_elements_qtt << std::endl;
	init_succeeded = true;
	item_qtt = 0;

	if (! Database::initialize_stream())
		return;
}

template <typename ElementType>
Database<ElementType>::Database(const char * filename) : Database(filename, sizeof(item_qtt)) {
	if (! init_succeeded) 
		return;

	/*	If reading the stream-header fails, then probably the database is empty.
		In that case, it is reset. */
	if (! Database::retrieve_stream_header())
	{
		// Checking if the stream is actually empty.
		if ((! fseek(stream, 0, SEEK_END)) && ftell(stream) == 0)
			Database::reset_database();
	}
}

template <typename ElementType>
Database<ElementType>::~Database(void) {
	if (stream == nullptr)	return;

	if (! Database::update_stream_header())
		std::cerr << "O cabe�alho do stream n�o p�de ser gravado no objeto de classe <Database>." << std::endl;

	Database::finalize_stream();
}

template <typename ElementType>
bool Database<ElementType>::retrieve_stream_header(void) {
	struct {
		Id_t item_qtt = 0;
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
		Id_t item_qtt = 0;
	} _stream_header;
	_stream_header.item_qtt = item_qtt;

	rewind(stream);
	return fwrite(&_stream_header, stream_header_size, 1, stream) > 0;
}

/*	Database Representation
 *	----------------------- */

#include <cstdlib>

template <typename ElementType>
size_t Database<ElementType>::print_database_filtered(std::function<bool(const ElementType &)> check, FILE * _OutputStream, size_t _From, size_t _To)
{
	if ((_To == 0) && (item_qtt > 0))
		_To = item_qtt;

	ElementType element_buffer;

	if (fseek(stream, stream_header_size, SEEK_SET) != 0)
		return false;

	printf("ftell: %lld\n", (long long) ftell(stream));

	size_t iterator = _From;
	while ((iterator <= _To) && read_element(iterator ++, &element_buffer))
	{
		if (! check(element_buffer))
			continue;

		fprintf(_OutputStream, "[%03llu] ", iterator - 1);
		fprint_element(_OutputStream, &element_buffer);
		fprintf(_OutputStream, "\n");
	}

	return iterator;
}

template <typename ElementType>
bool Database<ElementType>::print_database(FILE * _OutputStream, size_t _From, size_t _To)
{
	return print_database_filtered([](const ElementType &) { return true; }, _OutputStream, _From, _To) == (_To + 1);
}

template <typename ElementType>
std::list<ElementType> Database<ElementType>::list_filter(std::function<bool(const ElementType &)> check, size_t _From, size_t _To) const
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
			fprintf(stderr, "[%s] N�o pode ler o #%llu elemento.\n", __func__, (unsigned long long) i);
			continue;
		}

		if (check(element_buffer))
			elements.push_back(element_buffer);
	}

	return elements;
}


#endif // _DATABASES_HPP_INCLUDED_
