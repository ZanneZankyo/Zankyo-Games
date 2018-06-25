#pragma once

#include "criticalsection.hpp"

template<typename DataType>
class ThreadSafeLinkedPtrListNode
{
public:
	ThreadSafeLinkedPtrListNode<DataType>* m_next;
	DataType* m_data;
public:
	DataType* Data() { return m_data; }
	void Initialize() { m_next = nullptr; m_data = nullptr; }
	void Free() { free(m_data); }
	void Destroy(bool deleteMeta = true) {
		if(deleteMeta)
			free(m_data);
		if (m_next)
		{
			m_next->~ThreadSafeLinkedPtrListNode();
			free(m_next);
		}
	}
};

//linked list that store ptr (DataType*)
template<typename DataType>
class ThreadSafeLinkedPtrList
{
private:
	ThreadSafeLinkedPtrListNode<DataType>* m_head;
	CriticalSection m_lock;
public:

	DataType* Get(size_t index);
	size_t Size() const;
	void PushFront(DataType* data);
	void SortingPush(DataType* data, bool ascending = true);
	void Delete(ThreadSafeLinkedPtrListNode<DataType>* node);
	ThreadSafeLinkedPtrListNode<DataType>* Head() { SCOPE_LOCK(m_lock); return m_head; }
	void Initialize() { m_lock.Initialize(); m_head = nullptr; }
	void Destroy(bool deleteMeta = true) 
	{ 
		m_lock.~CriticalSection(); 
		if(m_head)
			m_head->Destroy(deleteMeta); 
	}
};

template<typename DataType>
void ThreadSafeLinkedPtrList<DataType>::SortingPush(DataType* data, bool ascending /*= true*/)
{
	SCOPE_LOCK(m_lock);
	ThreadSafeLinkedPtrListNode<DataType>* newNode = (ThreadSafeLinkedPtrListNode<DataType>*)malloc(sizeof(ThreadSafeLinkedPtrListNode<DataType>));
	newNode->Initialize();
 	newNode->m_data = data;

	if (!m_head)
	{
		m_head = newNode;
		return;
	}

	ThreadSafeLinkedPtrListNode<DataType>* prev = nullptr;
	ThreadSafeLinkedPtrListNode<DataType>* current = m_head;
	bool isPushed = false;
	while (current)
	{
		bool nodeFound = false;
		if (ascending && *(newNode->Data()) < *(current->Data()))
			nodeFound = true;
		else if (!ascending && !(*(newNode->Data()) < *(current->Data())))
			nodeFound = true;

		if (nodeFound)
		{
			if (prev)
				prev->m_next = newNode;
			else
				m_head = newNode;
			newNode->m_next = current;
			
			isPushed = true;
			break;
		}

		prev = current;
		current = current->m_next;
	}
	if (!isPushed)
		prev->m_next = newNode;
}

template<typename DataType>
size_t ThreadSafeLinkedPtrList<DataType>::Size() const
{
	size_t size = 0;
	ThreadSafeLinkedPtrListNode<DataType>* current = m_head;
	while (current)
	{
		size++;
		current = current->m_next;
	}
	return size;
}

template<typename DataType>
void ThreadSafeLinkedPtrList<DataType>::PushFront(DataType* data)
{
	SCOPE_LOCK(m_lock);
	ThreadSafeLinkedPtrListNode<DataType>* newNode = (ThreadSafeLinkedPtrListNode<DataType>*)malloc(sizeof(ThreadSafeLinkedPtrListNode<DataType>));
	newNode->Initialize();
	newNode->m_next = m_head;
	newNode->m_data = data;
	m_head = newNode;
}

template<typename DataType>
void ThreadSafeLinkedPtrList<DataType>::Delete(ThreadSafeLinkedPtrListNode<DataType>* node)
{
	SCOPE_LOCK(m_lock);
	if (!m_head)
		return;
	if (node == m_head)
	{
		ThreadSafeLinkedPtrListNode<DataType>* temp = m_head;
		m_head = m_head->m_next;
		temp->Free();
		free(temp);
		return;
	}

	ThreadSafeLinkedPtrListNode<DataType>* prev = m_head;
	ThreadSafeLinkedPtrListNode<DataType>* current = prev->m_next;
	while (current)
	{
		if (current == node)
			break;
		prev = current;
		current = current->m_next;
	}
	if (!current)
		return;
	prev->m_next = current->m_next;
	//free(current->m_data);
	current->Free();
	free(current);
}

template<typename DataType>
DataType* ThreadSafeLinkedPtrList<DataType>::Get(size_t index)
{
	SCOPE_LOCK(m_lock);
	int count = 0;
	ThreadSafeLinkedPtrListNode<DataType>* current = m_head;
	while (count < index || current)
	{
		current = current->m_next;
		count++;
	}
	if (current)
		return current->m_data;
	return nullptr;
}


