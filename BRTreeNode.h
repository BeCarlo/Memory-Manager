#ifndef BRTREE_NODE_H
#define BRTREE_NODE_H

#include <tuple>

enum NodeColor : bool {
	BLACK,
	RED
};

enum NodePosition : bool {
	LEFT,
	RIGHT
};

template<typename TypeToSort, class... Types>
class TNode {
protected:
	TypeToSort _dataToSort;
	std::tuple<Types...> _dataTuple;

public:
	NodeColor color = RED;

	TNode<TypeToSort, Types...>* parent = nullptr;
	TNode<TypeToSort, Types...>* childs[2] = { nullptr, nullptr };

	TNode(const TypeToSort& dataToSort, const Types&... otherData);

	__forceinline int howManyChilds() const;
	__forceinline bool hasChilds() const;

	__forceinline std::tuple<Types...> getTuple();

	__forceinline TypeToSort getData();
	__forceinline void setData(const TypeToSort& data);

	__forceinline TNode<TypeToSort, Types...>* getSibling(const TNode<TypeToSort, Types...>* node) const;
	__forceinline NodePosition getNodePosition(const TNode<TypeToSort, Types...>* node) const;

	__forceinline bool operator ==(const TNode<TypeToSort, Types...>* node);
	__forceinline bool operator >(const TNode<TypeToSort, Types...>* node);
	__forceinline bool operator <(const TNode<TypeToSort, Types...>* node);
	__forceinline bool operator >=(const TNode<TypeToSort, Types...>* node);
	__forceinline bool operator <=(const TNode<TypeToSort, Types...>* node);

	__forceinline bool operator ==(const TypeToSort& data);
	__forceinline bool operator >(const TypeToSort& data);
	__forceinline bool operator <(const TypeToSort& data);
	__forceinline bool operator >=(const TypeToSort& data);
	__forceinline bool operator <=(const TypeToSort& data);
};

#endif // !BRTREE_NODE_H