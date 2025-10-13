#ifndef BRTREE_NODE_H
#define BRTREE_NODE_H

#include <cassert>

enum NodeColor {
	BLACK,
	RED
};

enum NodePosition {
	LEFT,
	RIGHT
};



template<typename TypeToSort, typename OtherType>
class TNode {
protected:
	TypeToSort _dataToSort;
	OtherType _otherData;

public:
	NodeColor color = RED;

	TNode<TypeToSort, OtherType>* parent = nullptr;
	TNode<TypeToSort, OtherType>* childs[2] = { nullptr, nullptr };

	TNode(const TypeToSort& dataToSort, const OtherType& otherData);
	~TNode();

	__forceinline int howManyChilds() const;
	__forceinline bool hasChilds() const;

	__forceinline OtherType getOtherData();
	__forceinline OtherType* getOtherDataPointer();

	__forceinline TypeToSort getData();
	__forceinline void setData(const TypeToSort& data);

	__forceinline TNode<TypeToSort, OtherType>* getSibling(const TNode<TypeToSort, OtherType>* node) const;
	__forceinline NodePosition getNodePosition(const TNode<TypeToSort, OtherType>* node) const;

	__forceinline bool operator ==(const TNode<TypeToSort, OtherType>* node);
	__forceinline bool operator >(const TNode<TypeToSort, OtherType>* node);
	__forceinline bool operator <(const TNode<TypeToSort, OtherType>* node);
	__forceinline bool operator >=(const TNode<TypeToSort, OtherType>* node);
	__forceinline bool operator <=(const TNode<TypeToSort, OtherType>* node);

	__forceinline bool operator ==(const TypeToSort& data);
	__forceinline bool operator >(const TypeToSort& data);
	__forceinline bool operator <(const TypeToSort& data);
	__forceinline bool operator >=(const TypeToSort& data);
	__forceinline bool operator <=(const TypeToSort& data);
};

template<typename TypeToSort, typename OtherType> __forceinline static bool operator==(const TypeToSort& data, TNode<TypeToSort, OtherType>* node);
template<typename TypeToSort, typename OtherType> __forceinline static bool operator>(const TypeToSort& data, TNode<TypeToSort, OtherType>* node);
template<typename TypeToSort, typename OtherType> __forceinline static bool operator<(const TypeToSort& data, TNode<TypeToSort, OtherType>* node);
template<typename TypeToSort, typename OtherType> __forceinline static bool operator>=(const TypeToSort& data, TNode<TypeToSort, OtherType>* node);
template<typename TypeToSort, typename OtherType> __forceinline static bool operator<=(const TypeToSort& data, TNode<TypeToSort, OtherType>* node);

//IMPLEMENTATION

template<typename TypeToSort, class OtherType>
TNode<TypeToSort, OtherType>::TNode(const TypeToSort& dataToSort, const OtherType& otherData) {

	_dataToSort = dataToSort;
	_otherData = otherData;
}

template<typename TypeToSort, typename OtherType>
inline TNode<TypeToSort, OtherType>::~TNode() = default;


template<typename TypeToSort, typename OtherType>
__forceinline int TNode<TypeToSort, OtherType>::howManyChilds() const {
	return (childs[LEFT] != nullptr) + (childs[RIGHT] != nullptr);
}

template<typename TypeToSort, typename OtherType>
__forceinline bool TNode<TypeToSort, OtherType>::hasChilds() const {
	return howManyChilds() != 0;
}


template<typename TypeToSort, typename OtherType>
__forceinline OtherType TNode<TypeToSort, OtherType>::getOtherData() {
	return _otherData;
}

template<typename TypeToSort, typename OtherType>
__forceinline OtherType* TNode<TypeToSort, OtherType>::getOtherDataPointer() {
	return &_otherData;
}

template<typename TypeToSort, typename OtherType>
__forceinline TypeToSort TNode<TypeToSort, OtherType>::getData() {
	return _dataToSort;
}

template<typename TypeToSort, typename OtherType>
__forceinline void TNode<TypeToSort, OtherType>::setData(const TypeToSort& data) {
	_dataToSort = data;
}

template<typename TypeToSort, typename OtherType>
__forceinline TNode<TypeToSort, OtherType>* TNode<TypeToSort, OtherType>::getSibling(const TNode<TypeToSort, OtherType>* node) const {
	return childs[!getNodePosition(node)];
}

template<typename TypeToSort, typename OtherType>
__forceinline NodePosition TNode<TypeToSort, OtherType>::getNodePosition(const TNode<TypeToSort, OtherType>* node) const {
	return (NodePosition)(childs[RIGHT] != nullptr && node == childs[RIGHT]);
}

template<typename TypeToSort, typename OtherType>
__forceinline bool TNode<TypeToSort, OtherType>::operator==(const TNode<TypeToSort, OtherType>* node) {
	return _dataToSort == node->_dataToSort;
}

template<typename TypeToSort, typename OtherType>
__forceinline bool TNode<TypeToSort, OtherType>::operator>(const TNode<TypeToSort, OtherType>* node) {
	return _dataToSort > node->_dataToSort;
}

template<typename TypeToSort, typename OtherType>
__forceinline bool TNode<TypeToSort, OtherType>::operator<(const TNode<TypeToSort, OtherType>* node) {
	return _dataToSort < node->_dataToSort;
}

template<typename TypeToSort, typename OtherType>
__forceinline bool TNode<TypeToSort, OtherType>::operator>=(const TNode<TypeToSort, OtherType>* node)
{
	return _dataToSort >= node->_dataToSort;
}

template<typename TypeToSort, typename OtherType>
__forceinline bool TNode<TypeToSort, OtherType>::operator<=(const TNode<TypeToSort, OtherType>* node)
{
	return _dataToSort <= node->_dataToSort;
}

template<typename TypeToSort, typename OtherType>
__forceinline bool TNode<TypeToSort, OtherType>::operator==(const TypeToSort& data) {
	return _dataToSort == data;
}

template<typename TypeToSort, typename OtherType>
__forceinline bool TNode<TypeToSort, OtherType>::operator>(const TypeToSort& data) {
	return _dataToSort > data;
}

template<typename TypeToSort, typename OtherType>
__forceinline bool TNode<TypeToSort, OtherType>::operator<(const TypeToSort& data) {
	return _dataToSort < data;
}

template<typename TypeToSort, typename OtherType>
__forceinline bool TNode<TypeToSort, OtherType>::operator>=(const TypeToSort& data) {
	return _dataToSort >= data;
}

template<typename TypeToSort, typename OtherType>
__forceinline bool TNode<TypeToSort, OtherType>::operator<=(const TypeToSort& data) {
	return _dataToSort <= data;
}

template<typename TypeToSort, typename OtherType>
inline bool operator==(const TypeToSort& data, TNode<TypeToSort, OtherType>* node) {
	return node == data;
}

template<typename TypeToSort, typename OtherType>
inline bool operator>(const TypeToSort& data, TNode<TypeToSort, OtherType>* node)
{
	return data > node->getData();
}

template<typename TypeToSort, typename OtherType>
inline bool operator<(const TypeToSort& data, TNode<TypeToSort, OtherType>* node)
{
	return data < node->getData();
}
template<typename TypeToSort, typename OtherType>
inline bool operator>=(const TypeToSort& data, TNode<TypeToSort, OtherType>* node)
{
	return data >= node->getData();
}
template<typename TypeToSort, typename OtherType>
inline bool operator<=(const TypeToSort& data, TNode<TypeToSort, OtherType>* node)
{
	return data <= node->getData();
}

#endif // !BRTREE_NODE_H