#ifndef AISDI_LINEAR_VECTOR_H
#define AISDI_LINEAR_VECTOR_H

#include <cstddef>
#include <initializer_list>
#include <stdexcept>

namespace aisdi
{

template <typename Type>
class Vector
{
public:
  using difference_type = std::ptrdiff_t;
  using size_type = std::size_t;
  using value_type = Type;
  using pointer = Type*;
  using reference = Type&;
  using const_pointer = const Type*;
  using const_reference = const Type&;

  class ConstIterator;
  class Iterator;
  using iterator = Iterator;
  using const_iterator = ConstIterator;

    size_type current_size;
    size_type capacity;
    pointer buffer;


  Vector()
  {
    current_size = 0;
    capacity = 128;
    buffer = new value_type[capacity];
  }

  Vector(std::initializer_list<Type> l)
  {
    current_size = 0;
    capacity = l.size();
    buffer = new value_type[capacity];
    for(auto iter = l.begin(); iter!=l.end(); iter++)
      append(*iter);
  }

  Vector(const Vector& other)
  {
    current_size = 0;
    capacity = other.capacity;
    buffer = new value_type[capacity];
    for(auto iter = other.begin(); iter!=other.end(); iter++)
        append(*iter);
  }

  Vector(Vector&& other)
  {
      buffer = other.buffer;
      current_size = other.current_size;
      capacity = other.capacity;
      other.capacity = 0;
      other.current_size = 0;
      other.buffer = nullptr;
  }

  ~Vector()
  {
      delete[] buffer;
  }

  Vector& operator=(const Vector& other)
  {
      if(buffer == other.buffer)
          return *this;
      delete[] buffer;
      capacity = other.capacity;
      current_size = 0;
      buffer = new value_type[capacity];
      for(auto iter = other.begin(); iter!=other.end(); iter++)
          append(*iter);
      return *this;
  }

  Vector& operator=(Vector&& other)
  {
      if(this == &other)
          return *this;
      capacity = other.capacity;
      current_size = other.current_size;
      delete[] buffer;
      buffer = other.buffer;
      other.buffer = nullptr;
      return *this;
  }

  void reallocate()
  {
      capacity*=2;
      pointer temp = new value_type[capacity];
      for(unsigned int iter = 0; iter<current_size; iter++)
          temp[iter] = std::move(buffer[iter]);
      delete[] buffer;
      buffer = temp;
  }
  bool isEmpty() const
  {
    return !current_size;
  }

  size_type getSize() const
  {
    return current_size;
  }

  void append(const Type& item)
  {
      current_size++;
      if(current_size==capacity)
          reallocate();
      buffer[current_size-1] = item;
  }

  void prepend(const Type& item)
  {
    current_size++;
    if(current_size==capacity)
        reallocate();
    for(auto iter = current_size-1; iter!=0; iter--)
        buffer[iter] = buffer[iter-1];
    buffer[0] = item;
  }

  void insert(const const_iterator& insertPosition, const Type& item)
  {
    if(insertPosition.index == 0)
        prepend(item);
    else if(insertPosition.index == current_size + 1)
        append(item);
    else
    {
        current_size++;
        if(current_size == capacity)
            reallocate();
        pointer temp = new value_type[capacity];
        for(unsigned int iter = 0; iter != current_size; iter++)
        {
            if (insertPosition.index == iter)
            {
                temp[iter] = item;
                break;
            }
            else
                temp[iter] = buffer[iter];
        }
        for(auto iter = insertPosition.index+1; iter!=current_size; iter++)
            temp[iter] = buffer[iter-1];
        delete[] buffer;
        buffer = temp;
    }
  }

  Type popFirst()
  {
    if(isEmpty())
        throw std::logic_error("No  elements");

    auto temp = buffer[0];
    for(unsigned int iter = 0; iter != current_size; iter++)
        buffer[iter] = buffer[iter+1];
    current_size--;
    return temp;
  }

  Type popLast()
  {
      if(isEmpty())
          throw std::logic_error("No  elements");
      return buffer[--current_size];
  }

  void erase(const const_iterator& possition)
  {
      if(isEmpty() || possition == end())
          throw std::out_of_range("Out of range");
      current_size--;
      for(auto iter = possition.index; iter!=current_size; iter++)
        buffer[iter] = buffer[iter+1];
  }

  void erase(const const_iterator& firstIncluded, const const_iterator& lastExcluded)
  {
      if (lastExcluded.index <= firstIncluded.index)
          return;
      if (firstIncluded.index >=  current_size || current_size == 0)
          throw std::out_of_range("Out of range");
      pointer temp = new value_type[capacity];
      for (unsigned int i = 0; i < firstIncluded.index; ++i)
          temp[i] = buffer[i];
      for (unsigned int i = lastExcluded.index; i <  current_size; ++i)
         temp[i - lastExcluded.index + firstIncluded.index] = buffer[i];
      delete[] buffer;
      this->buffer = temp;
      this->current_size -= (lastExcluded.index - firstIncluded.index);
  }

  iterator begin()
  {
      if(current_size==0)
          return end();
      else
          return Iterator(ConstIterator(this, 0));
  }

  iterator end()
  {
    return Iterator(ConstIterator(this, current_size));
  }

  const_iterator cbegin() const
  {
    return ConstIterator(this, 0);
  }

  const_iterator cend() const
  {
    return ConstIterator(this, current_size);
  }

  const_iterator begin() const
  {
    return cbegin();
  }

  const_iterator end() const
  {
    return cend();
  }
};

template <typename Type>
class Vector<Type>::ConstIterator
{
public:
  using iterator_category = std::bidirectional_iterator_tag;
  using value_type = typename Vector::value_type;
  using difference_type = typename Vector::difference_type;
  using pointer = typename Vector::const_pointer;
  using reference = typename Vector::const_reference;

  unsigned int index;
  const Vector<value_type > *ptr;

  explicit ConstIterator(const Vector<value_type> *pointer, unsigned int indx)
  {
      ptr = pointer;
      index = indx;
  }

  reference operator*() const
  {
      if(index == ptr -> current_size)
          throw std::out_of_range("Not in vector's area");
      return ptr->buffer[index];
  }

  ConstIterator& operator++()
  {

    if(index  >= ptr->current_size)
        throw std::out_of_range("Not in vector's area");
    ++index;
    return *this;
  }

  ConstIterator operator++(int)
  {
    auto temp = *this;
    operator++();
    return temp;
  }

  ConstIterator& operator--()
  {
    if(index==0)
        throw std::out_of_range("Not in vector's area");
    index--;
    return *this;
  }

  ConstIterator operator--(int)
  {
    if(index==0)
        throw std::out_of_range("Not in vector's area");
    auto temp = *this;
    index--;
    return temp;
  }

  ConstIterator operator+(difference_type d) const
  {
    if(index + (unsigned int)d > (unsigned int)ptr->current_size)
        throw std::out_of_range("Not in vector's area");
    ConstIterator temp(this->ptr, index+d);
    return temp;
  }

  ConstIterator operator-(difference_type d) const
  {
    if(index-d < 0)
        throw std::out_of_range("Not in vector's area");
    ConstIterator temp(this->ptr, index-d);
    return temp;
  }

  bool operator==(const ConstIterator& other) const
  {
      return (index == other.index && ptr == other.ptr);
  }

  bool operator!=(const ConstIterator& other) const
  {
      return !(index == other.index && ptr == other.ptr);
  }
};

template <typename Type>
class Vector<Type>::Iterator : public Vector<Type>::ConstIterator
{
public:
  using pointer = typename Vector::pointer;
  using reference = typename Vector::reference;

  explicit Iterator()
  {}

  Iterator(const ConstIterator& other)
    : ConstIterator(other)
  {}

  Iterator& operator++()
  {
    ConstIterator::operator++();
    return *this;
  }

  Iterator operator++(int)
  {
    auto result = *this;
    ConstIterator::operator++();
    return result;
  }

  Iterator& operator--()
  {
    ConstIterator::operator--();
    return *this;
  }

  Iterator operator--(int)
  {
    auto result = *this;
    ConstIterator::operator--();
    return result;
  }

  Iterator operator+(difference_type d) const
  {
    return ConstIterator::operator+(d);
  }

  Iterator operator-(difference_type d) const
  {
    return ConstIterator::operator-(d);
  }

  reference operator*() const
  {
    // ugly cast, yet reduces code duplication.
    return const_cast<reference>(ConstIterator::operator*());
  }
};

}

#endif // AISDI_LINEAR_VECTOR_H
