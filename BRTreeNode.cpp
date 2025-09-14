#include "BRTreeNode.h"


template<class TypeToSort, class ...Types>
TNode<TypeToSort, Types...>::TNode(const TypeToSort& dataToSort, const Types&... otherData) {

	_dataToSort = dataToSort;
	_dataTuple = std::make_tuple(otherData...);
}

template<class TypeToSort, class ...Types>
__forceinline int TNode<TypeToSort, Types...>::howManyChilds() const {
	return (childs[LEFT] != nullptr) + (childs[RIGHT] != nullptr);
}

template<class TypeToSort, class ...Types>
__forceinline bool TNode<TypeToSort, Types...>::hasChilds() const {
	return howManyChilds() != 0;
}


template<class TypeToSort, class ...Types>
__forceinline std::tuple<Types...> TNode<TypeToSort, Types...>::getTuple() {
	return _dataTuple;
}

template<class TypeToSort, class ...Types>
__forceinline TypeToSort TNode<TypeToSort, Types...>::getData() {
	return _dataToSort;
}

template<class TypeToSort, class ...Types>
__forceinline void TNode<TypeToSort, Types...>::setData(const TypeToSort& data) {
	_dataToSort = data;
}

template<class TypeToSort, class ...Types>
__forceinline TNode<TypeToSort, Types...>* TNode<TypeToSort, Types...>::getSibling(const TNode<TypeToSort, Types...>* node) const {
	return childs[!getNodePosition(node)];
}

template<class TypeToSort, class ...Types>
__forceinline NodePosition TNode<TypeToSort, Types...>::getNodePosition(const TNode<TypeToSort, Types...>* node) const {
	if (childs[LEFT] != nullptr && node == childs[LEFT])
		return LEFT;
	return RIGHT;
}

template<class TypeToSort, class ...Types>
__forceinline bool TNode<TypeToSort, Types...>::operator==(const TNode<TypeToSort, Types...>* node) {
	return _dataToSort == node._dataToSort;
}

template<class TypeToSort, class ...Types>
__forceinline bool TNode<TypeToSort, Types...>::operator>(const TNode<TypeToSort, Types...>* node) {
	return _dataToSort > node._dataToSort;
}

template<class TypeToSort, class ...Types>
__forceinline bool TNode<TypeToSort, Types...>::operator<(const TNode<TypeToSort, Types...>* node) {
	return _dataToSort < node._dataToSort;
}

template<class TypeToSort, class ...Types>
__forceinline bool TNode<TypeToSort, Types...>::operator>=(const TNode<TypeToSort, Types...>* node)
{
	return _dataToSort >= node._dataToSort;
}

template<class TypeToSort, class ...Types>
__forceinline bool TNode<TypeToSort, Types...>::operator<=(const TNode<TypeToSort, Types...>* node)
{
	return _dataToSort <= node._dataToSort;
}

template<class TypeToSort, class ...Types>
__forceinline bool TNode<TypeToSort, Types...>::operator==(const TypeToSort& data) {
	return _dataToSort == data;
}

template<class TypeToSort, class ...Types>
__forceinline bool TNode<TypeToSort, Types...>::operator>(const TypeToSort& data) {
	return _dataToSort > data;
}

template<class TypeToSort, class ...Types>
__forceinline bool TNode<TypeToSort, Types...>::operator<(const TypeToSort& data) {
	return _dataToSort < data;
}

template<class TypeToSort, class ...Types>
__forceinline bool TNode<TypeToSort, Types...>::operator>=(const TypeToSort& data) {
	return _dataToSort >= data;
}

template<class TypeToSort, class ...Types>
__forceinline bool TNode<TypeToSort, Types...>::operator<=(const TypeToSort& data) {
	return _dataToSort <= data;
}