#pragma once

template<typename DataType>
class LinkedPtrListNode
{
public:
	LinkedPtrListNode<DataType>* m_next;
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
			m_next->~LinkedPtrListNode();
			free(m_next);
		}
	}
};

//linked list that store ptr (DataType*)
template<typename DataType>
class LinkedPtrList
{
private:
	LinkedPtrListNode<DataType>* m_head;
public:

	DataType* Get(size_t index);
	size_t Size();
	void PushFront(DataType* data);
	void SortingPush(DataType* data, bool ascending = true);
	void Delete(LinkedPtrListNode<DataType>* node);
	LinkedPtrListNode<DataType>* Head() { return m_head; }
	void Initialize() { m_head = nullptr; }
	void Destroy(bool deleteMeta) { m_head->Destroy(deleteMeta); }
};

template<typename DataType>
void LinkedPtrList<DataType>::SortingPush(DataType* data, bool ascending /*= true*/)
{
	LinkedPtrListNode<DataType>* newNode = (LinkedPtrListNode<DataType>*)malloc(sizeof(LinkedPtrListNode<DataType>));
	newNode->Initialize();
 	newNode->m_data = data;

	if (!m_head)
	{
		m_head = newNode;
		return;
	}

	LinkedPtrListNode<DataType>* prev = nullptr;
	LinkedPtrListNode<DataType>* current = m_head;
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
size_t LinkedPtrList<DataType>::Size()
{
	size_t size = 0;
	LinkedPtrListNode<DataType>* current = m_head;
	while (current)
	{
		size++;
		current = current->m_next;
	}
	return size;
}

template<typename DataType>
void LinkedPtrList<DataType>::PushFront(DataType* data)
{
	LinkedPtrListNode<DataType>* newNode = (LinkedPtrListNode<DataType>*)malloc(sizeof(LinkedPtrListNode<DataType>));
	newNode->Initialize();
	newNode->m_next = m_head;
	newNode->m_data = data;
	m_head = newNode;
}

template<typename DataType>
void LinkedPtrList<DataType>::Delete(LinkedPtrListNode<DataType>* node)
{
	if (!m_head)
		return;
	if (node == m_head)
	{
		LinkedPtrListNode<DataType>* temp = m_head;
		m_head = m_head->m_next;
		temp->Free();
		free(temp);
		return;
	}

	LinkedPtrListNode<DataType>* prev = m_head;
	LinkedPtrListNode<DataType>* current = prev->m_next;
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
DataType* LinkedPtrList<DataType>::Get(size_t index)
{
	int count = 0;
	LinkedPtrListNode<DataType>* current = m_head;
	while (count < index || current)
	{
		current = current->m_next;
		count++;
	}
	if (current)
		return current->m_data;
	return nullptr;
}


