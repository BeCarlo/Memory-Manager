#ifndef BLACK_RED_TREE_H
#define BLACK_RED_TREE_H


#include "BRTreeNode.h"

typedef NodePosition BRTRotationDirection;

template<typename TypeToSort, class... Types>
class TRedBlackTree {
	TNode<TypeToSort, Types...>* _root = nullptr;

public:
	TRedBlackTree() = default;
	TRedBlackTree(const TypeToSort& dataToSort, const Types&... otherData);
	~TRedBlackTree();

	__forceinline TNode<TypeToSort, Types...>* insert(const TypeToSort& dataToSort, const Types&... otherData);
	__forceinline TNode<TypeToSort, Types...>* search(const TypeToSort& data);
	__forceinline std::tuple<Types...> remove(const TypeToSort data);

private:
	__forceinline void rotation(TNode<TypeToSort, Types...>* node, const BRTRotationDirection rotation);

	__forceinline void fixInsertViolation(const TNode<TypeToSort, Types...>* node);
	__forceinline void fixRemoveViolation(const TNode<TypeToSort, Types...>* node);

	__forceinline void deleteAll(TNode<TypeToSort, Types...>* node);
};

#endif // !BLACK_RED_TREE_H