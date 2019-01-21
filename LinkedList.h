#ifndef AISDI_LINEAR_LINKEDLIST_H
#define AISDI_LINEAR_LINKEDLIST_H

#include <cstddef>
#include <initializer_list>
#include <stdexcept>

namespace aisdi
{

    template <typename Type>
    class LinkedList
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

        class Node
        {
        public:
            value_type value;
            Node *prev;
            Node *next;
        };

        Node *head;
        Node *tail;
        size_type  size;

        LinkedList()
        {
            size = 0;
            head = tail = new Node;
        }

        LinkedList(std::initializer_list<Type> l)
        {
            head = tail = new Node;
            size = 0;
            for(auto iter = l.begin(); iter!=l.end(); iter++)
                append(*iter);
        }

        LinkedList(const LinkedList& other)
        {
            head = tail = new Node;
            size = 0;
            for(auto iter = other.begin(); iter!=other.end(); iter++)
                append(*iter);
        }

        LinkedList(LinkedList&& other)
        {
            head = other.head;
            tail = other.tail;
            size = other.size;
            other.tail = other.head = nullptr;
            other.size = 0;
        }

        ~LinkedList()
        {}

        LinkedList& operator=(const LinkedList& other)
        {
            if(this == &other)
                return *this;
            head = tail = new Node;
            size = 0;
            for(auto iter = other.begin(); iter!=other.end(); iter++)
                append(*iter);
            return *this;
        }

        LinkedList& operator=(LinkedList&& other)
        {
            erase(begin(), end());
            head = other.head;
            tail = other.tail;
            size = other.size;
            other.tail = other.head = nullptr;
            other.size = 0;
            return *this;
        }

        bool isEmpty() const
        {
            return !size;
        }

        size_type getSize() const
        {
            return size;
        }

        void append(const Type& item)
        {
            Node *new_element = new Node;
            new_element->value = item;
            if(size == 0)
            {
                head = new_element;
                head->next = tail;
                tail->prev = new_element;
            }
            else
            {
                tail->prev->next = new_element;
                new_element->prev = tail->prev;
                tail->prev = new_element;
                new_element->next = tail;
            }
            size++;
        }

        void prepend(const Type& item)
        {
            if(!getSize())
                append(item);
            else
            {
                Node *new_element = new Node;
                new_element->value = item;
                head->prev = new_element;
                new_element->next = head;
                head = new_element;
                size++;
            }
        }

        void insert(const const_iterator& insertPosition, const Type& item)
        {
            if(insertPosition == end())
                append(item);
            else if(insertPosition == begin())
                prepend(item);
            else
            {
                Node* new_item = new Node;
                new_item->value = item;
                new_item->next = insertPosition.element_pointer->prev->next;
                new_item->prev = insertPosition.element_pointer->next->prev;
                insertPosition.element_pointer->prev->next = new_item;
                insertPosition.element_pointer->next->prev = new_item;
                size++;
            }
        }

        Type popFirst()
        {
            if(!getSize())
                throw std::logic_error("Cannot popFirst from empty collection");
            Type to_return = head->value;
            erase(begin());
            return to_return;
        }

        Type popLast()
        {
            if(!getSize())
                throw std::logic_error("Cannot popLast from empty collection");
            Type to_return = tail->prev->value;
            erase(end()-1);
            return to_return;
        }

        void erase(const const_iterator& possition)
        {
            if(getSize() == 0 || possition.element_pointer == tail)
                throw std::out_of_range("Cannot erase anything from empty collection");
            else if(possition == begin())
            {
                ConstIterator temp(this, head);
                head = head->next;
                head->prev = nullptr;
                delete(temp.element_pointer);
            }
            else
            {
                ConstIterator temp(this, possition.element_pointer);
                possition.element_pointer->prev->next = possition.element_pointer->next;
                possition.element_pointer->next->prev = possition.element_pointer->prev;
                delete(temp.element_pointer);
            }
            size--;
        }

        void erase(const const_iterator& firstIncluded, const const_iterator& lastExcluded)
        {
            for(auto iter = firstIncluded; iter!=lastExcluded; iter++)
                erase(iter);
        }

        iterator begin()
        {
            return iterator(cbegin());
        }

        iterator end()
        {
            return iterator(cend());
        }

        const_iterator cbegin() const
        {
            return ConstIterator(this, head);
        }

        const_iterator cend() const
        {
           return ConstIterator(this, tail);
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
    class LinkedList<Type>::ConstIterator
    {
    public:
        using iterator_category = std::bidirectional_iterator_tag;
        using value_type = typename LinkedList::value_type;
        using difference_type = typename LinkedList::difference_type;
        using pointer = typename LinkedList::const_pointer;
        using reference = typename LinkedList::const_reference;

        const LinkedList *list_pointer;
        Node *element_pointer;

        explicit ConstIterator( const LinkedList *l, Node *e)
        {
            list_pointer = l;
            element_pointer = e;
        }

        reference operator*() const
        {
            if(element_pointer == list_pointer->tail)
                throw std::out_of_range("Dereferecing incorrectly");
            return element_pointer->value;
        }

        ConstIterator& operator++()
        {
            if(element_pointer == list_pointer->tail)
                throw std::out_of_range("Out of range ++/");
            element_pointer = element_pointer->next;
            return *this;
        }

        ConstIterator operator++(int)
        {
            ConstIterator temp(list_pointer, element_pointer);
            operator++();
            return temp;
        }

        ConstIterator& operator--()
        {
            if(element_pointer == list_pointer->head)
                throw std::out_of_range("Out of range --/");
            element_pointer = element_pointer->prev;
            return *this;
        }

        ConstIterator operator--(int)
        {
            ConstIterator temp(list_pointer, element_pointer);
            operator--();
            return temp;
        }

        ConstIterator operator+(difference_type d) const
        {
            ConstIterator temp(list_pointer, element_pointer);
            for (auto iter = d; iter > 0; iter--)
                ++temp;
            return temp;
        }

        ConstIterator operator-(difference_type d) const
        {
            ConstIterator temp(list_pointer, element_pointer);
            for(auto iter = d; iter>0; iter--)
                --temp;
            return temp;
        }

        bool operator==(const ConstIterator& other) const
        {
            return (element_pointer == other.element_pointer && list_pointer == other.list_pointer);
        }

        bool operator!=(const ConstIterator& other) const
        {
            return !(element_pointer == other.element_pointer && list_pointer == other.list_pointer);
        }
    };

    template <typename Type>
    class LinkedList<Type>::Iterator : public LinkedList<Type>::ConstIterator
    {
    public:
        using pointer = typename LinkedList::pointer;
        using reference = typename LinkedList::reference;

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

#endif // AISDI_LINEAR_LINKEDLIST_H
