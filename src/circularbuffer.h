/*
 * Tempo2 - Template for a circular (or ring) buffer
 *
 * Copyright (c) 2015, CSP Innovazione nelle ICT. All rights reserved.
 *
 * Authors:
 *   Marco Boeris Frusca <marco.boeris@csp.it>
 *
 */
//============================================================================
// Name        : circulatbuffer.h
// Author      : Marco Boeris Frusca
// Version     : 1.0
// Copyright   : GNU GENERAL PUBLIC LICENSE
// Description : Template for a circular buffer
//============================================================================
#ifndef _T2_CIRCULARBUFFER_H_
#define _T2_CIRCULARBUFFER_H_

#include <vector>
#include <iostream>
#include <iomanip>
#include <stdexcept>

/**
 * @brief The CircularBuffer class provides a template to implement a circular
 * (or ring) buffer.
 *
 * A ring showing, conceptually, a circular buffer. This visually shows that
 * the buffer has no real end and it can loop around the buffer. However, since
 * memory is never physically created as a ring, a linear representation is
 * generally used.
 *
 * A circular buffer, cyclic buffer or ring buffer is a data structure that uses a
 * single, fixed-size buffer as if it were connected end-to-end. This structure
 * lends itself easily to buffering data streams.
 *
 * The default behavior is overwrite older elements when you try to write a number
 * of elements greater then the buffer size.
 *
 */
template<typename T>
class CircularBuffer
{
    /**
     * @brief Increments an index in cyclic logic.
     *
     *
     * @param[out] index to increase.
     */
    void increment(unsigned int &index)
    {
      index++;
      index %= m_max_size;
    }

  public:
    //! Default size of the buffer
    static const unsigned int SIZE = 100;

    /**
     * @brief Create an empty buffer.
     *
     * Constructs an empty buffer. If you try to write a number of elements
     * greater then the buffer size, the default behavior is overwrite older
     * elements: \a overwriting \a property set to true.
     *
     */
    CircularBuffer() :
        m_head(0), m_tail(0), m_size(0), m_max_size(SIZE), m_overwrite(true)
    {
      m_buffer = std::vector<T>(m_max_size);
    }

    /**
     * @brief Create an empty buffer.
     *
     * Constructs an empty buffer. Using the flag \a ow is possible change the
     * default behavior of the write operation. If this flag set the \a overwriting
     * \a property set. If it is false then the write operation writes
     * elements until the buffer is full and then returns without overwrite the
     * older ones.
     *
     * @param[in] ow Value to set the \a overwriting \a property. If true, then older elements will be overwritten.
     *
     */
    CircularBuffer(bool ow) :
        m_head(0), m_tail(0), m_size(0), m_max_size(SIZE), m_overwrite(true)
    {
      m_buffer = std::vector<T>(m_max_size);
    }

    /**
     * @brief Create an empty buffer.
     *
     * Constructs an empty buffer of \a maxSize elements. Using the flag \a ow
     * is possible change the default behavior of the write operation (\a overwriting
     * \a property. If this flag is false then the  write operation writes elements
     * until the buffer is full, and then returns without overwrite the older ones.
     *
     * @param[in] maxSize Buffer size.
     * @param[in] ow Value to set the \a overwriting \a property. If true, then older elements will be overwritten.
     *
     */
    CircularBuffer(unsigned int maxSize, bool ow) :
        m_head(0), m_tail(0), m_size(0), m_max_size(maxSize), m_overwrite(true)
    {
      m_buffer = std::vector<T>(m_max_size);
    }

    /**
     * @brief Copy Construct
     *
     * @param[in] n source buffer object.
     *
     */
    CircularBuffer(const CircularBuffer& b)
    {
      m_head = b.m_head;
      m_tail = b.m_tail;
      m_size = b.m_size;
      m_max_size = b.m_max_size;
      m_overwrite = b.m_overwrite;
      m_buffer = b.m_buffer;
    }

    /**
     * @brief Destroys the circular buffer.
     *
     */
    ~CircularBuffer()
    {
      m_buffer.clear();
    }

    /**
     * @brief Assign content.
     *
     * @param[in] n source buffer object.
     *
     * @returns reference to CircularBuffer object.
     */
    CircularBuffer & operator=(const CircularBuffer &b)
    {
      m_head = b.m_head;
      m_tail = b.m_tail;
      m_size = b.m_size;
      m_max_size = b.m_max_size;
      m_overwrite = b.m_overwrite;
      m_buffer = b.m_buffer;

      return (*this);
    }

    /**
     * @brief Sets the \a overwriting \a property.
     *
     * This function sets the \a overwriting \a property. If this flag is
     * false then the write operation writes elements until the buffer is
     * full, and then returns without overwrite the older ones.
     * If it is true then older elements will be overwritten.
     *
     * @param[in] value Value to set the \a overwriting \a property.
     *
     */
    void setOverWrite(bool value)
    {
      m_overwrite = value;
    }

    /**
     * @brief Changes the size of the buffer.
     *
     * This function sets the size of the buffer. After this operation
     * buffer will be empty.
     * This function may throw exceptions on failure (bad_alloc is thrown
     * if the resize operation does not succeed).
     *
     * @param[in] n New buffer size, expressed in number of elements. Member type size_type is an unsigned integral type.
     */
    void resize(unsigned int n) throw (std::exception)
    {
      // Clear the buffer
      m_buffer.clear();

      // Resize the empty buffer
      try
      {
        m_buffer.resize(n);
        m_size = 0;
        m_head = 0;
        m_tail = 0;
        m_max_size = n;
      }
      catch (std::exception &e)
      {
        throw(e);
      }
    }

    /**
     * @brief Gets the current number of elements stored in the buffer.
     *
     * @returns The number of elements stored in the buffer.
     */
    unsigned int size()
    {
      return m_size;
    }

    /**
     * @brief Gets the capacity of the buffer: maximum number of elements storable into the
     * buffer.
     *
     * @returns The buffer capacity.
     */
    unsigned int capacity()
    {
      return m_max_size;
    }

    /**
     * @brief Returns true if the buffer is empty.
     *
     * @returns true if buffer is empty, false otherwise.
     */
    bool empty()
    {
      return m_size == 0;
    }

    /**
     * @brief Gets the first element of the buffer and deletes it from the buffer.
     *
     * This function gets the first element of the buffer and deletes it
     * from the buffer. If buffer is empty or there is an error, this
     * function returns false.
     *
     * @param[out] elt Reference to the element read from the buffer.
     *
     * @returns false if buffer is empty or there is an error; true otherwise.
     */
    bool pop(T& elt)
    {
      // Check if internal index are correct
      if (m_size > m_max_size)
      {
        return false;
      }

      if (m_size <= 0)
      {
        return false;
      }

      // Get the element of the buffer
      elt = m_buffer[m_head % m_max_size];

      // index
      increment(m_head);
      m_size--;

      return true;
    }

    /**
     * @brief Stores an element in the buffer.
     *
     * This function stores an element in the buffer. If the overwriting
     * property is disabled  (false) and buffer is full them the element
     * will not be stored and the function returns false.
     *
     * @param[in] Reference to the element to store in the buffer.
     *
     * @returns false if buffer element has not been stored or there is an error.
     */
    bool push(T& elt)
    {
      // Check if internal index are correct
      if (!m_overwrite && m_size >= m_max_size)
      {
        return false;
      }

      if (m_size < m_max_size)
      {
        // Store element
        m_buffer[m_tail % m_max_size] = elt;
        increment(m_tail);
        m_size++;

        return true;
      }

      return false;
    }

    /**
     * @brief Reads \a size element from the buffer and stores them into \a buf array.
     *
     * This function reads \a size element from the buffer and stores
     * them into \a buf array. It returns the number of element read.
     *
     * @param[out] buf Buffer where to save the elements read from the circular buffer.
     * @param[in] n Number of element to read from the circular buffer.
     *
     * @returns The number of elements read.
     */
    unsigned int read(T buf[], unsigned int n)
    {
      // Try to get elements n elements
      unsigned int i = 0;
      for (i = 0; i < n && m_size > 0; i++)
      {
        if (!pop(buf[i]))
        {
          break;
        }
      }
      return i;
    }

    /**
     * @brief  Writes n elements into the circular buffer.
     *
     * This function writes n elements into the circular buffer.
     * If \a overwriting \a property is  disable, when the buffer will be
     * full, this function will terminate and will return the number of
     * elements written in the buffer.
     *
     * @param[in] buf Buffer with the elements to store into the circular buffer.
     * @param[in] n Number of element to write into the circular buffer.
     *
     * @returns The number of elements written.
     */
    unsigned int write(T buf[], unsigned int n)
    {
      unsigned int i = 0;

      for (i = 0; i < n && m_size < m_max_size; i++)
      {
        // Try to store element
        if (!push(buf[i]))
        {
          break;
        }
      }

      return i;
    }

    /**
     * @brief Gets the element at position n in the buffer.
     *
     * This function gets the element at position n in the buffer.
     * The function automatically checks whether n is within the bounds of
     * valid elements in the buffer, throwing an out_of_range exception if
     * it is not (i.e., if n is greater or equal than its size).
     *
     * @param[in] n Position of an element in the container.
     *
     * @returns Element at the specified position in the container.
     */
    T& at(unsigned int n) throw (std::exception)
    {
      unsigned int index = (m_head + n) % m_max_size;
      return m_buffer.at(index);
    }

    /**
     * Gets the index of the first element
     *
     * #returns index of the first element
     */
    unsigned int head()
    {
      return m_head;
    }

    /**
     * Gets the index of the first element
     *
     * #returns index of the first element
     */
    unsigned int tail()
    {
      return m_tail;
    }

    /**
     * @brief Deletes \a n elements from the buffer.
     *
     * This function drops \a n elements from the buffer. If \a n is
     * greater than the buffer size this function returns the number
     * of elements deleted.
     *
     * @param[in] n Number of elements to delete.
     *
     * @returns The number of elements deleted.
     */
    unsigned int drop(unsigned int n)
    {
      if (n == 0)
      {
        return 0;
      }

      if (n > m_size)
      {
        n = m_size;
      }

      m_head = (m_head + n) % m_max_size;
      m_size -= n;

      return n;
    }

    /**
     * Diagnostic dump context for debugging on standard error. If debug flag is true
     * dump is more verbose.
     *
     * \param debug true to print more information.
     *
     */
    /**
     * @brief Dumps buffer on standard error.
     *
     * Diagnostic dump context for debugging on standard error. If debug
     * flag is true dump is more verbose.
     *
     * @param[in] debug true to print more information.
     *
     */
    void dump(bool debug = false)
    {
      if (debug)
      {
        std::cerr << std::left << std::setw(10) << std::setfill(' ') << "Size" << ": " << m_size
            << std::endl;
        std::cerr << std::left << std::setw(10) << std::setfill(' ') << "N elements" << ": "
            << m_size << std::endl;
        std::cerr << std::left << std::setw(10) << std::setfill(' ') << "Head" << ": " << m_head
            << std::endl;
        std::cerr << std::left << std::setw(10) << std::setfill(' ') << "Tail" << ": " << m_tail
            << std::endl;
        for (unsigned int i = 0; i < m_max_size; i++)
        {
          std::cerr << "\t[" << std::right << std::setw(3) << std::setfill(' ') << std::dec << i << "] :"
               << m_buffer[i] << std::endl;
        }
      }
      else
      {
        for (unsigned int i = 0; i < m_size; i++)
        {
          std::cerr << "\t[" << std::left << std::setw(3) << std::setfill(' ') << i << "] :"
              << m_buffer[(m_head + i) % m_max_size] << std::endl;
        }
      }
    }

  private:
    //! Index of the first element of the buffer
    unsigned int m_head;

    //! Index of the last element of the buffer
    unsigned int m_tail;

    //! Number of elements stored in the buffer
    unsigned int m_size;

    //! Maximum number of elements storable in the buffer
    unsigned int m_max_size;

    //! Overwriting property
    bool m_overwrite;

    std::vector<T> m_buffer;
};

#endif /* _T2_CIRCULARBUFFER_H_ */
